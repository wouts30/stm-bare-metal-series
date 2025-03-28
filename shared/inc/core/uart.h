#ifndef SHARED_INC_CORE_UART_H_
#define SHARED_INC_CORE_UART_H_

#include "common-defines.h"

void uart_setup(void);
void uart_write(uint8_t * data, const uint32_t lenght);
void uart_write_byte(uint8_t data);
bool uart_data_available(void);
uint32_t uart_read(uint8_t * data, const uint32_t length);
uint8_t uart_read_byte(void);


#endif /* SHARED_INC_CORE_UART_H_ */