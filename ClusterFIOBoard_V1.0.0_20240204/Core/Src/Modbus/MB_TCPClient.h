/*
 * MB_TCPClient.h
 *
 *  Created on: Feb 5, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_TCPCLIENT_H_
#define SRC_MODBUS_MB_TCPCLIENT_H_
#include "MB_MsgQ.h"
#include "MB_TCP.h"
#include "MB_Custom.h"



typedef struct MBTCPC_TransInfo{
	uint16_t index;
	uint8_t isAvailable;
	uint16_t transId;
	uint8_t	unitId;
	uint8_t funCode;
	uint16_t startAddr;		// start address of of  registers, coils & discrete inputs
	uint16_t quantity;		// quantity of  registers, coils & discrete inputs
	uint16_t byteCount;		// quantity of  registers, coils & discrete inputs
//#ifdef MB_TCP_CUSTOM
	uint8_t slvStartAddr;
	uint8_t numOfSlv;
//#endif
	uint8_t retryStatus;
	uint16_t lastRetryTime;
	uint16_t retryTimeout;
	uint16_t retryCounter;
	uint16_t retryCount;
	uint8_t txBuff[MBTCP_BUFF_SIZE];
	uint8_t rxBuff[MBTCP_BUFF_SIZE];
	uint16_t txBuffSize;
	uint16_t rxBuffSize;
}MBTCPC_TransInfo;


typedef struct MB_TCPClient{
	uint8_t connect;
	uint8_t id;
	MBTCP_Header header;
	uint8_t nbOfConns;
	uint8_t curntTrnsNo;
	int8_t lastTrnsIndex;
	uint16_t srcPort;
	uint16_t destPort;
	uint8_t srcIP[4];
	uint8_t destIP[4];
	MBTCPC_TransInfo pendingList[MBTCPC_MAX_TRANS];
	MBMQ_Queue msgQ;

	uint32_t txTimIntrvl;
	uint32_t dlyAftrConct;

}MB_TCPClient;


void MBTCPC_Init(MB_TCPClient *client);
uint8_t MBTCPC_BuildRequest(MB_TCPClient *client, uint8_t *buff, uint16_t size);
uint8_t MBTCPC_ProcessResponse(MB_TCPClient *client, uint8_t *buff, uint16_t size);
uint8_t MBTCPC_CheckResp(MBTCPC_TransInfo *respTransInf, MBTCPC_TransInfo *reqTransInf);
int8_t MBTCPC_CheckPendingTrans(MB_TCPClient *client);

void MBTCPC_ClearPendingList(MBTCPC_TransInfo *list);
void MBTCPC_ClearTransInfo(MBTCPC_TransInfo *info);
void MBTCPC_AddToTxPendingList(MBTCPC_TransInfo *list, MBTCPC_TransInfo item);
int8_t MBTCPC_GetFromTxPendingList(MBTCPC_TransInfo *list, MBTCPC_TransInfo *item, uint16_t transId);
int8_t MBTCPC_CheckInTxPendingList(MBTCPC_TransInfo *list, uint16_t transId);
void MBTCPC_RemoveFromTxPendingList(MBTCPC_TransInfo *list, uint8_t index);

#endif /* SRC_MODBUS_MB_TCPCLIENT_H_ */
