/*
 * MB_CRC.h
 *
 *  Created on: Jan 25, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_CRC_H_
#define INC_MB_CRC_H_
#include <stdint.h>


unsigned short MB_CalcCRC16 (volatile unsigned char *puchMsg, unsigned short usDataLen );
uint8_t MB_CRCCheck(uint8_t *buff, uint8_t size);
#endif /* INC_MB_CRC_H_ */
