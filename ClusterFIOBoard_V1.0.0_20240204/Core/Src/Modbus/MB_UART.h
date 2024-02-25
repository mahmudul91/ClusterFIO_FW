/*
 * MB_UART.h
 *
 *  Created on: Jul 7, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_UART_H_
#define SRC_MODBUS_MB_UART_H_

#include "main.h"



uint8_t MB_UartInit(UART_HandleTypeDef *UART, USART_TypeDef *Instance, uint32_t baudRate);
uint8_t MB_UartInitDMA( DMA_HandleTypeDef *dma_uart_rx, DMA_HandleTypeDef *dma_uart_tx);
HAL_StatusTypeDef MB_UartSendDMA(uint8_t *buffer, uint16_t size);
HAL_StatusTypeDef MB_UartReceiveDMA(uint8_t *buffer, uint16_t size);
#endif /* SRC_MODBUS_MB_UART_H_ */
