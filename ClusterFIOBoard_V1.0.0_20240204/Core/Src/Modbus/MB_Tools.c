/*
 * MB_Tools.c
 *
 *  Created on: Jan 24, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "MB_Tools.h"



/**
  * @brief combines 2 separate bytes into a single 2 byte variable
  */
uint16_t MBTOOL_CombBytesToU16(uint8_t highByte, uint8_t lowByte){
	return (((highByte&0x00FF)<<8U) | (uint16_t)lowByte);
}

/**
  * @brief combines 2 separate bytes into a single 2 byte variable
  */
void MBTOOL_SplitU16ToBytes(uint8_t *highByte, uint8_t *lowByte, uint16_t u16){
	*highByte = (uint8_t)((u16>>8)&0xFF);	// MS byte
	*lowByte = (uint8_t)(u16&0x00FF);		// LS byte
}
