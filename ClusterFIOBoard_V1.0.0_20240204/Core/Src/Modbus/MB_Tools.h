/*
 * MB_Tools.h
 *
 *  Created on: Jan 24, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_TOOLS_H_
#define SRC_MODBUS_MB_TOOLS_H_
#include "stdint.h"


uint16_t MBTOOL_CombBytesToU16(uint8_t highByte, uint8_t lowByte);
void MBTOOL_SplitU16ToBytes(uint8_t *highByte, uint8_t *lowByte, uint16_t u16);

#endif /* SRC_MODBUS_MB_TOOLS_H_ */
