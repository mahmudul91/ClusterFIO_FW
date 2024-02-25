/*
 * w25Qxx_Interface.h
 *
 *  Created on: Feb 21, 2024
 *      Author: wsrra
 */

#ifndef SRC_W25QXX_INTERFACE_H_
#define SRC_W25QXX_INTERFACE_H_

#include "w25qxx.h"


uint8_t W25Q_Init(w25qxx_type_t type, w25qxx_interface_t interface, w25qxx_bool_t dual_quad_spi_enable);
w25qxx_handle_t* W25Q_GetInstance(void);

uint8_t W25Q_SPIWrite(uint8_t addr, uint8_t *buf, uint16_t len);
uint8_t W25Q_SPIRead(uint8_t addr, uint8_t *buf, uint16_t len);
#endif /* SRC_W25QXX_INTERFACE_H_ */
