/*
 * MB_Gateway.c
 *
 *  Created on: Jan 24, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "MB_Gateway.h"
#include "MB_Serial.h"
#include "MB_TCP.h"
#include "MB_Tools.h"
#include "TimeStamp.h"

extern TimeStamp timStamp;

/**
  * @brief Initialize the modbus gateway
  * @param none
  * @retval none
  */
static MB_Gateway *gw;

void MBG_Init(MB_Gateway *gway){
	gw = gway;
	gw->clientId = 0;
	gw->clientKey =  0 ;
	gw->rplyTimer.enable = 0;
	gw->rplyTimer.timer = 0;
	gw->tcpHeader.transId = 0;
	gw->tcpHeader.protocolId = 0;
	gw->tcpHeader.length = 0;
	gw->tcpHeader.unitId = 0;
	gw->isBusy = 0;
	gw->isReqSent = 0;
}

void MBG_Clear(void){

	gw->clientId = 0;
	gw->clientKey =  0 ;
	gw->rplyTimer.enable = 0;
	gw->rplyTimer.timer = 0;
	gw->tcpHeader.transId = 0;
	gw->tcpHeader.protocolId = 0;
	gw->tcpHeader.length = 0;
	gw->tcpHeader.unitId = 0;
}

MB_Gateway *MBG_GetInstance(void){
	return gw;
}

void MBG_SetInstance(MB_Gateway *gway){
	 gw = gway;
}

MBTCP_Header *MBG_GetTCPHeader(void){
	return &gw->tcpHeader;
}

void MBG_SetTCPHeader(MBTCP_Header *header){
	gw->tcpHeader = *header;
}

uint32_t MBG_GetTCPClientId(void){
	return gw->clientId;
}

void MBG_SetTCPClientId(uint32_t id){
	gw->clientId = id;
}

uint32_t MBG_GetTCPClientKey(void){
	return gw->clientKey;
}

void MBG_SetTCPClientKey(uint32_t key){
	gw->clientKey = key;
}


/**
  * @brief Convert TCP protocol to RTU protocol
  * @param rtuBuffer: pointer to the rtu buffer, max size up to 256 bytes
  * @param tcpmBuffer: pointer to the tcp buffer,
  * @retval error status: 0 = Error, 1 = ok
  */
uint16_t MBG_ConvertTCP2RTU(uint8_t *rtuBuffer, uint8_t *tcpmBuffer, uint16_t tcpBufferSize){
	uint8_t startPos = MBTCP_INDX_UNIT_ID;		// start position from where bytes to be copied to the rtu buffer from tcp buffer
	tcpBufferSize = tcpBufferSize-startPos;
	if(tcpBufferSize > 0 && tcpBufferSize <= MBS_RTU_PDU_MAX_SIZE){
		memcpy(rtuBuffer, &tcpmBuffer[startPos], tcpBufferSize);		//CRC: 0XCDC5
		MBTOOL_SplitU16ToBytes(&rtuBuffer[tcpBufferSize+1], &rtuBuffer[tcpBufferSize], MB_CalcCRC16(rtuBuffer, tcpBufferSize));
		return tcpBufferSize+2;		// crc is 2 byte hence 2 is added to the buffer size
	}else{
		return MB_ERROR;
	}
}


/**
  * @brief Convert RTU protocol to TCP protocol
  * @param rtuBuffer: pointer to the rtu buffer, max size up to 256 bytes
  * @param tcpmBuffer: pointer to the rtu buffer,
  * @retval error status: 0 = Error, 1 = ok
  */
uint16_t MBG_ConvertRTU2TCP(uint8_t *tcpmBuffer, uint8_t *rtuBuffer, uint16_t rtuBufferSize){
//	MBTOOL_SplitU16ToBytes(&tcpmBuffer[MBTCP_INDX_LENGTH_HI], &tcpmBuffer[MBTCP_INDX_LENGTH_LO], rtuBufferSize);	// set the length of the rtu packet
	uint8_t startPos = MBTCP_INDX_UNIT_ID;		// start position from where bytes to be copied to the rtu buffer from tcp buffer
	if(rtuBufferSize > 2 && rtuBufferSize <= MBS_RTU_PDU_MAX_SIZE){
		memcpy(&tcpmBuffer[startPos], rtuBuffer, rtuBufferSize-2);
		return rtuBufferSize-2;
	}else{
		return MB_ERROR;
	}
}

void MBG_StartTimeout(MB_ReplyTimer *rplyTimer){
	rplyTimer->timer = TS_GetUS(&timStamp);
	rplyTimer->enable = 1;
}

void MBG_StopTimeout(MB_ReplyTimer *rplyTimer){
	rplyTimer->timer = 0;
	rplyTimer->enable = 0;
}

void MBG_ResetTimeout(MB_ReplyTimer *rplyTimer){
	rplyTimer->timer = TS_GetUS(&timStamp);

}

uint8_t MBG_CheckTimeout(MB_ReplyTimer *rplyTimer){
	if(!rplyTimer->enable) return MB_ERROR;

	if((uint64_t)fabsl((long double)(TS_GetUS(&timStamp)-rplyTimer->timer)) >= (uint64_t)rplyTimer->timeout){
//		gWay->isTimeoutStarted = 0;
		return MB_OK;

	}
	return MB_ERROR;
}


void MBG_SetTimeout(MB_ReplyTimer *rplyTimer, uint32_t timeout){
	rplyTimer->timeout= timeout;

}

//void MBG_SetSlaveTimeout(MB_Gateway *gWay, uint32_t timeout){
//	gWay->slvTimeout= timeout;
//
//}
//
//uint32_t MBG_GetSlaveTimeout(MB_Gateway *gWay){
//	return gWay->slvTimeout;
//
//}


/**
  * @brief send to the serial line
  * @param rtuBuffer: pointer to the rtu buffer, max size up to 256 bytes
  * @param tcpmBuffer: pointer to the rtu buffer,
  * @retval error status: 0 = Error, 1 = ok
  */
uint8_t MBG_SendToSerial(uint8_t *buffer, uint16_t size){
	MBS_Serial *serial;
	serial = MBS_GetInstance();
	if(!serial->isBusy){
		serial->isBusy = 1;

		memcpy(serial->txBuff, buffer, size);
		serial->txBuffSize = size;

		if( MBS_Send(serial) != MB_OK){
			serial->isBusy = 0;
			return MB_ERROR;
		}
//		MBG_StartTimeout(gw);
		return MB_OK;
	}else{
		return MB_ERROR;
	}

}

/**
  * @brief receives from the serial line
  */
//int16_t MBG_RecvFromSerial(uint8_t *buffer){
//
//	MBS_Serial *serial;
//	serial = MBS_GetInstance();
//
//	if(serial->isDataReceived){
//		memcpy(buffer, serial->txBuff, serial->txBuffSize);
//		serial->isBusy = 0;
//		return serial->txBuffSize;
//	}
//	return 0;
//}

