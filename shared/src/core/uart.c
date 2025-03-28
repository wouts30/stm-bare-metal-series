#include "core/uart.h"
#include "core/ring-buffer.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#define BAUDRATE (115200)
// 11520 bytes per second with 115200 buadrate
// 115.19 bytes per 10 ms at 115200 baud round up to 128 (powers of two)
#define RING_BUFFER_SIZE (128)

static ring_buffer_t rx_buffer = {0U};
static uint8_t data_buffer[RING_BUFFER_SIZE] = {0U};

void usart1_isr(void){
    const bool overrun_occurred = usart_get_flag(USART1, USART_FLAG_ORE) == 1;
    const bool received_data = usart_get_flag(USART1, USART_FLAG_RXNE) == 1;

    if (received_data || overrun_occurred){
        if ( ring_buffer_write(&rx_buffer, (uint8_t)usart_recv(USART1)) ){
            //handle failure if buffer overflow will occur and all data would will be lost
        }
    }
}

void uart_setup(void){
    ring_buffer_setup(&rx_buffer, data_buffer, RING_BUFFER_SIZE);

    rcc_periph_clock_enable(RCC_USART1);

    usart_set_mode(USART1, USART_MODE_TX_RX);

    usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
    usart_set_databits(USART1, 8);
    usart_set_stopbits(USART1, USART_STOPBITS_1);
    usart_set_parity(USART1, USART_PARITY_NONE);
    usart_set_baudrate(USART1, BAUDRATE);

    usart_enable_rx_interrupt(USART1);
    nvic_enable_irq(NVIC_USART1_IRQ);

    usart_enable(USART1);
}

void uart_write(uint8_t * data, const uint32_t lenght){
    for (uint32_t i = 0; i < lenght; i++){
        uart_write_byte((uint16_t)data[i]);
    }
}

void uart_write_byte(uint8_t data){
    usart_send_blocking(USART1, (uint16_t)data);
}

bool uart_data_available(void){
    return !ring_buffer_empty(&rx_buffer);
}

uint32_t uart_read(uint8_t * data, const uint32_t length){
    for (uint8_t read_bytes = 0; read_bytes < length; read_bytes++){
        if ( ring_buffer_read(&rx_buffer, &data[read_bytes]) ){
            return read_bytes;
        }
    }
    return length;

}

uint8_t uart_read_byte(void){
    uint8_t data = 0;
    (void) uart_read(&data, 1);
    return data;
}
