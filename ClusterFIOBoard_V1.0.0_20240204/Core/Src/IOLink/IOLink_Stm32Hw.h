/*
 * IOLink_Stm32Hw.h
 *
 *  Created on: Jan 29, 2024
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_STM32HW_H_
#define SRC_IOLINK_IOLINK_STM32HW_H_

#include "l6360.h"

HAL_StatusTypeDef IOL_STM32_UartInit( L6360_IOLink_hw_t* l6360, uint8_t portNum, uint32_t baudrate );
HAL_StatusTypeDef IOL_STM32_UartInitDMA( L6360_IOLink_hw_t* l6360);
void  IOL_STM32_GpioInit(L6360_IOLink_hw_t* l6360);
#endif /* SRC_IOLINK_IOLINK_STM32HW_H_ */
