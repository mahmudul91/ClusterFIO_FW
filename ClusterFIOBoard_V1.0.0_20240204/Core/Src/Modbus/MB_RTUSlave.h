/*
 * MB_RTUSlave.h
 *
 *  Created on: Oct 25, 2022
 *      Author: wsrra
 */

#ifndef SRC_MODBUS_MB_RTUSLAVE_H_
#define SRC_MODBUS_MB_RTUSLAVE_H_

#include "stdio.h"
#include "MB_Config.h"
#include "MB_Custom.h"

// for the Modbus slave properties
typedef struct MB_RTUSlave
{
	uint8_t slave_address;							// 1 byte, slave address of this device
	uint8_t *rxBuffer;		// PDU (application data unit) buffer for the request
	uint8_t *txBuffer;		// PDU buffer for the response
	uint8_t rxBufferSize;			// received adu buffer size
	uint8_t txBufferSize;			//response adu buffer size
	MB_Custom custom;
}MB_RTUSlave;
void MBRS_Init(MB_RTUSlave *mbSlave);

MB_RTUSlave *MBRS_GetInstance(void);
void MBRS_SetInstance(MB_RTUSlave *mbSlave);
uint8_t MBRS_GetSlaveAddress(void);
uint8_t MBRS_CheckSAnCRC(MB_RTUSlave *mbSlave);//uint8_t *rxBuffer, uint8_t size);	// to set h=the received PDU from the Modbus master
uint8_t MBRS_processRequest(MB_RTUSlave *mbSlave);								// to process the request
//uint8_t MBRS_crcCheck(uint8_t *buff, uint8_t size);											// to check the CRC

void MBRS_PostProcess(MB_RTUSlave *mbSlave, uint8_t error);



#endif /* SRC_MODBUS_MB_RTUSLAVE_H_ */
