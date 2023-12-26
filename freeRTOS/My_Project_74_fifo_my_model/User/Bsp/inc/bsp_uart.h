#ifndef __BSP_UART_H
#define __BSP_UART_H

#include <stdint.h>

extern uint8_t Rx_Buff[53];

int Bsp_Uart_Init(void);
void Bsp_Uart_Transmit(uint8_t *Tx_Buff, uint16_t size);
void Bsp_Uart_Rx(uint8_t *Rx_Buff, uint16_t size);

#endif
