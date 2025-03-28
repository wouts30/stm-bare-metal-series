#include "core/ring-buffer.h"

void ring_buffer_setup(ring_buffer_t * rb, uint8_t * data_buffer, uint32_t size){
    rb->buffer = data_buffer;
    rb->read_index = 0;
    rb->write_index = 0;
    //if buffer is even power of two then mask is size - 1
    // if 8 then mask is 7 111
    rb->mask = size - 1; 
}

bool ring_buffer_empty(ring_buffer_t * rb){
    return (rb->read_index == rb->write_index);
}


//statagies to do differently
//-> if if (next_write_index == local_read_index) dont move any of the pointers this means newest data is lost 
//-> if if (next_write_index == local_read_index) move both of the pointers forwards this means oldest data is overwritten 
// not recommended to modify both pointers in a single function
bool ring_buffer_write(ring_buffer_t * rb, uint8_t byte){
    //local copy is more stable than the pointers in the case of an interrupt during this function
    uint32_t local_read_index = rb->read_index;
    uint32_t local_write_index = rb->write_index;
    uint32_t next_write_index = (local_write_index + 1) & rb->mask;
    //check if the next is the same as the read this mean buffer is full now and we have 
    //created the condition that the buffer is empty even though it is full and we will 
    //not be able to access the data 
    if (next_write_index == local_read_index){
        return false;
    }

    rb->buffer[local_write_index] = byte;
    //5 = 4+1(0b101) & 7(0b111)
    //0 = 7+1(0b1000) & 7(0b0111)
    rb->write_index = next_write_index;
    // rb->read_index = local_read_index;
    return true;
}

bool ring_buffer_read(ring_buffer_t * rb, uint8_t * byte){
    //local copy is more stable than the pointers in the case of an interrupt during this function
    uint32_t local_read_index = rb->read_index;
    uint32_t local_write_index = rb->write_index;
    //buffer is seen as empty
    if (local_read_index == local_write_index){
        return false;
    }
    *byte = rb->buffer[local_read_index];
    //5 = 4+1(0b101) & 7(0b111)
    //0 = 7+1(0b1000) & 7(0b0111)
    //very important to store in local_read first 
    //cause a bug when interrupt was received while this function was running
    // help keep stable version would skip some chars when alphabet was entered
    local_read_index = (local_read_index + 1) & rb->mask;
    rb->read_index = local_read_index;
    // rb->write_index = local_write_index;
    return true;
}
