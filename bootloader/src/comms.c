#include "comms.h"
#include "core/uart.h"
#include "core/crc.h"
#include "assert.h"

#define PACKET_BUFFER_LENGTH (8)

typedef enum comms_state_t {
    CommsState_Length,
    CommsState_Data,
    CommsState_CRC,
} comms_state_t;

static comms_state_t comms_state = CommsState_Length;
static uint8_t data_byte_count = 0;

static comms_packet_t temp_packet = {.length = 0, .data = {0}, .crc = 0};  
static comms_packet_t retx_packet = {.length = 0, .data = {0}, .crc = 0}; //retransmit packet
static comms_packet_t ack_packet = {.length = 0, .data = {0}, .crc = 0};
static comms_packet_t last_tx_packet = {.length = 0, .data = {0}, .crc = 0};

static comms_packet_t packet_buffer[PACKET_BUFFER_LENGTH];
static uint32_t packet_read_index = 0;
static uint32_t packet_write_index = 0;
static uint32_t packet_buffer_mask =  PACKET_BUFFER_LENGTH - 1;

static bool comms_is_single_byte_packet(const comms_packet_t * packet, const uint8_t byte){
    if ( packet->length != 1){
        return false;
    }
     
    if ( packet->data[0] != byte){
        return false;
    }

    for (uint8_t i = 1; i < PACKET_DATA_LENGTH; i++){
        if ( packet->data[i] != 0xFF){
            return false;
        }
    }
    return true;
}

static void comms_packet_copy(const comms_packet_t * source, comms_packet_t * dest){
    dest->length = source->length;
    for (uint8_t i = 0; i < PACKET_DATA_LENGTH; i++){
        dest->data[i] = source->data[i];
    }
    dest->crc = source->crc;
}


void comms_setup(void){
    retx_packet.length = PACKET_RETX_DATA0;
    retx_packet.data[0] = PACKET_RETX_DATA0;
    for (uint8_t i = 1; i < PACKET_DATA_LENGTH; i++){
        retx_packet.data[i] = 0xFF;
    }
    retx_packet.crc = comms_copute_crc(&retx_packet);

    ack_packet.length = PACKET_ACK_DATA0;
    ack_packet.data[0] = PACKET_ACK_DATA0;
    for (uint8_t i = 1; i < PACKET_DATA_LENGTH; i++){
        ack_packet.data[i] = 0xFF;
    }
    ack_packet.crc = comms_copute_crc(&ack_packet);
}

void comms_update(void){
    while (uart_data_available()){
        switch (comms_state)
        {
            case CommsState_Length:{
                temp_packet.length = uart_read_byte();
                comms_state = CommsState_Data;
            } break;

            case CommsState_Data:{
                //look into reading multiple bytes simultaneously
                temp_packet.data[data_byte_count] = uart_read_byte();
                data_byte_count++;
                if (data_byte_count == PACKET_DATA_LENGTH){
                    data_byte_count = 0;
                    comms_state = CommsState_CRC;
                }
            } break;

            case CommsState_CRC:{
                temp_packet.crc = uart_read_byte();
                // packet wil not have padding due to all fields begin aligned and same size
                if (temp_packet.crc != comms_copute_crc(&temp_packet)){
                    comms_write(&retx_packet);
                    comms_state = CommsState_Length;
                    break;
                }
                if ( comms_is_single_byte_packet(&temp_packet, PACKET_RETX_DATA0) ){
                    comms_write(&last_tx_packet);
                    comms_state = CommsState_Length;
                    break;
                }
                if ( comms_is_single_byte_packet(&temp_packet, PACKET_ACK_DATA0) ){
                    comms_state = CommsState_Length;
                    break;
                }

                uint32_t next_write_index = (packet_write_index + 1) & packet_buffer_mask;
                // TODO: test this 
                // assert(next_write_index != packet_read_index); 
                if ( next_write_index == packet_read_index){
                    __asm__("BKPT #0"); //when we hit this we will get a breakpoint here 
                }

                comms_packet_copy(&temp_packet, &packet_buffer[packet_write_index]);
                packet_write_index = (packet_write_index + 1) & packet_buffer_mask;
                comms_write(&ack_packet);
                comms_state = CommsState_Length;
                 
            } break;

            default:{
                comms_state = CommsState_Length;
            } break;
        }
    }
}

bool comms_packets_available(void){
    return (packet_read_index != packet_write_index);
}

void comms_write(comms_packet_t * packet){
    uart_write((uint8_t *)packet, PACKET_LENGTH);
    comms_packet_copy(packet, &last_tx_packet);
}

void comms_read(comms_packet_t * packet){
    comms_packet_copy(&packet_buffer[packet_read_index], packet);
    packet_read_index = (packet_read_index + 1) & packet_buffer_mask;
}

uint8_t comms_copute_crc(comms_packet_t * packet){
    return crc8((uint8_t *)packet, PACKET_LENGTH - PACKET_CRC_BYTES);
}