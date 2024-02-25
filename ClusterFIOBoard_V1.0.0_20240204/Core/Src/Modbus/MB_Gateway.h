/*
 * MB_Gateway.h
 *
 *  Created on: Jan 24, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_GATEWAY_H_
#define SRC_MODBUS_MB_GATEWAY_H_
#include "stdint.h"
#include "MB_TCP.h"
#include "MB_Custom.h"
/*
 * Modbus gatewaye mode
 * */
typedef enum MBG_Mode{
	MBG_MODE_MASTER,
	MBG_MODE_SLAVE,
}MBG_Mode;


typedef enum MBG_ExeStatRgleFC{
	MBG_RGLFC_EXEST_NONE = 0,
	MBG_RGLFC_EXEST_REQ_RECVD,
	MBG_RGLFC_EXEST_PROCESSING,
	MBG_RGLFC_EXEST_RESP_READY,
	MBG_RGLFC_EXEST_TIMEOUT,

}MBG_ExeStatRgleFC;


typedef struct MB_ReplyTimer{
	uint8_t enable;
	uint64_t timer;
	uint32_t timeout;

}MB_ReplyTimer;


typedef struct MB_Gateway{
	uint32_t clientId;				// ip address is used as client id
	uint16_t clientKey;				// port is used as client key
	MBTCP_Header tcpHeader;
	MBG_Mode mode;

	uint32_t slvTimeout;		// time out for the slave
	MB_ReplyTimer rplyTimer;

	uint8_t isBusy;
	uint8_t isReqSent;
	MB_ReplyTimer rplyTimRglrFC;
	uint8_t isRegularFC;
	MB_Custom custom;
	MB_CUST_MQ_Queue rcvMsgQCust;		// msg que for the custom function code
	uint8_t txBuff[MB_PDU_MAX_SIZE];
	uint8_t rxBuff[MB_PDU_MAX_SIZE];
	uint16_t txBuffSize;
	uint16_t rxBuffSize;

}MB_Gateway;



void MBG_Init(MB_Gateway *gway);
void MBG_Clear(void);
MB_Gateway *MBG_GetInstance(void);
MBTCP_Header *MBG_GetTCPHeader(void);
void MBG_SetTCPHeader(MBTCP_Header *header);
uint32_t MBG_GetTCPClientId(void);
void MBG_SetTCPClientId(uint32_t id);
uint32_t MBG_GetTCPClientKey(void);
void MBG_SetTCPClientKey(uint32_t key);

uint16_t MBG_ConvertTCP2RTU(uint8_t *rtuBuffer, uint8_t *tcpmBuffer, uint16_t tcpBufferSize);
uint16_t MBG_ConvertRTU2TCP(uint8_t *tcpmBuffer, uint8_t *rtuBuffer, uint16_t rtuBufferSize);


void MBG_StartTimeout(MB_ReplyTimer *rplyTimer);
void MBG_StopTimeout(MB_ReplyTimer *rplyTimer);
void MBG_ResetTimeout(MB_ReplyTimer *rplyTimer);
void MBG_SetTimeout(MB_ReplyTimer *rplyTimer, uint32_t timeout);
uint8_t MBG_CheckTimeout(MB_ReplyTimer *rplyTimer);

//void MBG_SetSlaveTimeout(MB_Gateway *gWay,uint32_t timeout);
//uint32_t MBG_GetSlaveTimeout(MB_Gateway *gWay);



uint8_t MBG_SendToSerial(uint8_t *buffer, uint16_t size);
//int16_t MBG_RecvFromSerial(uint8_t *buffer);

#endif /* SRC_MODBUS_MB_GATEWAY_H_ */
