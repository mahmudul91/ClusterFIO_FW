/*
 * MB_TCPServer.h
 *
 *  Created on: Feb 5, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_TCPSERVER_H_
#define SRC_MODBUS_MB_TCPSERVER_H_

#include <MB_Handler.h>
#include "stdint.h"
#include "MB_TCP.h"
#include "MB_Gateway.h"



typedef struct MBTCPS_ClientParams{
	uint32_t id;			// IP is used as an id
	uint8_t sockIndex;
	uint8_t sockId;
	uint8_t ip[4];
	uint8_t mac[6];
	uint16_t port;
	uint16_t transId;		//	Transaction Identifier: Identification of a MODBUS Request / Response transaction
	uint16_t protocolId;	//	Protocol Identifier: Always 0x0000, (0 = MODBUS protocol)
	uint16_t length;		//	Length: Number of bytes of the following fields, including the Unit Identifier and data fields.
	uint8_t	unitId;			//	Unit Identifier: Identification of a remote slave connected on a serial line or on other buses.
	uint8_t isConnected;

	MB_ExcepCode excepCode;
	uint8_t rxBuff[MBTCP_BUFF_SIZE];
	uint8_t txBuff[MBTCP_BUFF_SIZE];
	uint16_t rxBuffSize;
	uint16_t txBuffSize;

}MBTCPS_ClientParams;


//typedef struct MBTCPS_RequestParams{
//	MBTCP_ReqType type;
//	uint8_t ip[4];
//}MBTCPS_RequestParams;

typedef struct MB_TCPServer{
	uint8_t isDataRcvd;
	uint8_t isReloadReq;
	uint8_t clientIndex;
	uint8_t nbOfConns;
	MBTCP_NetInfo netInfo;
	MBTCPS_ClientParams clients[MBTCPS_MAX_CONNS];
//	MBTCP_MsgQue msgQ[MBTCP_MAX_CONNS];
//	MBMQ_Queue msgQ;
	MBMQ_Msg msg;
	MB_ACL acl;
	uint8_t curntTrnsNo;
	uint8_t maxTrnsNo;
}MB_TCPServer;


void MBTCPS_Init(MB_TCPServer *mbtcps);
void MBTCPS_RemoveClosedClient(MB_TCPServer *mbtcps);
void MBTCPS_InitClientParams(MBTCPS_ClientParams *cParams);
void MBTCPS_ClearClientParams(MBTCPS_ClientParams *cParams);
//uint8_t MBTCPS_ProcessReqPDU(MB_TCPServer *mbtcps);
//void MBTCPS_ProcessRequest(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client, uint8_t reqType);
uint8_t MBTCPS_CheckProtocolID(uint16_t protocolId);
uint8_t MBTCPS_FindClientById(MB_TCPServer *mbtcps,  uint32_t id);
uint8_t MBTCPS_FindClientByPort(MB_TCPServer *mbtcps,  uint32_t port);
uint8_t MBTCPS_FindClientByIdAndPort(MB_TCPServer *mbtcps, uint32_t id,  uint32_t port);
void MBTCPS_SetMBAPHeaderToTXBuff(MB_TCPServer *mbtcps, uint8_t clientIndex);
void MBTCPS_ParseMBAPHeader(MBTCPS_ClientParams *client);

//uint8_t MBTCPS_ProcessMBAPHeader(MB_TCPServer *mbtcps);
void MBTCPS_AddNewClient(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client);
void MBTCPS_UpdateClient(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client);
void MBTCPS_ConnHandler(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client);
void MBTCPS_DisconnHandler(MB_TCPServer *mbtcps, uint32_t id, uint16_t port);
void MBTCPS_RecvHandler(MB_TCPServer *mbtcps, uint32_t id, uint16_t port, uint8_t *buff, uint16_t size);
void MBTCPS_ReqHandler(MB_TCPServer *mbtcps, MB_Gateway *mbGw);
void MBTCPS_RespHandler(MB_TCPServer *mbtcps, uint8_t clientIndex);

void MBTCPS_Send(MBTCPS_ClientParams *client);
void MBTCPS_BuldExcepResp(MBTCPS_ClientParams *client);
void MBTCPS_AttachSendCallback(uint8_t (*callbackFunc)(MBTCPS_ClientParams *client));
void MBTCPS_AttachSendExcpCallback(uint8_t (*callbackFunc)(MBTCPS_ClientParams *client));


#endif /* SRC_MODBUS_MB_TCPSERVER_H_ */
