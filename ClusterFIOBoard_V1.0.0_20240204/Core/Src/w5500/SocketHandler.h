/*
 * SocketHandler.h
 *
 *  Created on: Oct 18, 2022
 *      Author: wsrra
 */

#ifndef INC_SOCKETHANDLER_H_
#define INC_SOCKETHANDLER_H_
#include "stdlib.h"
#include "stdio.h"
//#include "stdint.h"
#include "w5500_chip.h"
#include "Define.h"


//Socket configuration
#define SOCK_MAX					4
#define SOCK_BUF_MAX_SIZE			DEF_MBTCP_MAX_BUFF_SIZE	// max buffer size for the sockets, 160 is the max packet size for Modbus data packet
#define SOCK_RETRY_DELAY			3000	// in ms, opening retry in delay
#define SOCK_RETRY_COUNT_MAX		5

//#define SOCK_READY_TO_SEND			1U
#define SOCK_READY_TO_SEND			1U


typedef enum SocketID{
	SOCKET_0 = 0,
	SOCKET_1,
	SOCKET_2,
	SOCKET_3,
	SOCKET_4,
	SOCKET_5,
	SOCKET_6,
	SOCKET_7,
	SOCKET_MAX,
}SocketID;


typedef enum SOCK_State{
	SOCK_STATE_IDLE = 0,
	SOCK_STATE_OPENING,
	SOCK_STATE_OPENED,
	SOCK_STATE_LISTENING,
	SOCK_STATE_CONNECTING,
	SOCK_STATE_CONNECTED,
	SOCK_STATE_SENDING,
	SOCK_STATE_SENT,
	SOCK_STATE_RECEIVING,
	SOCK_STATE_RECEIVED,
	SOCK_STATE_DISCONNECTING,
	SOCK_STATE_DISCONNECTED,
	SOCK_STATE_CLOSING,
	SOCK_STATE_CLOSED,
	SOCK_STATE_FAILED,
	SOCK_STATE_MAX,
}SOCK_State;

//typedef enum SOCK_ReqType{
//	SOCK_REQ_NONE = 0,
//	SOCK_REQ_CONN,
//	SOCK_REQ_DATA,
//}SOCK_ReqType;
//
//typedef enum SOCK_Flag{
//	SOCK_FLAG_NONE = 0,
//	SOCK_FLAG_READY_TO_SEND,
//	SOCK_FLAG_MAX,
//	SOCK_FLAG_MAX,
//}SOCK_Flag;


typedef enum MBTCP_ReqType{
	SOCK_REQ_NONE = 0,
	SOCK_REQ_CONN,
	SOCK_REQ_DISCONN,
	SOCK_REQ_DATA,
}MBTCP_ReqType;



typedef enum SOCK_Mode{
	SOCK_MODE_TCP_SERVER = 0,
	SOCK_MODE_TCP_CLIENT,
	SOCK_MODE_UDP_SERVER,
	SOCK_MODE_UDP_CLIENT,
	SOCK_MODE_MAX,
}SOCK_Mode;

typedef enum SOCK_ConnMode{
	SOCK_CONN_MODE_AUTO = 0,
	SOCK_CONN_MODE_MANUAL,
}SOCK_ConnMode;




typedef struct __attribute__ ((__packed__)) Socket{
	int8_t status;
	uint8_t connectMode;
	uint8_t connect;
	uint8_t id;
	uint8_t isConnected;
	uint8_t isReadyToSend;
	uint8_t destIP[4];
	uint8_t mac[6];
	uint8_t  protocol;
	uint8_t  flag;
	uint8_t rqstType;
	uint8_t rxBuff[SOCK_BUF_MAX_SIZE];
	uint8_t txBuff[SOCK_BUF_MAX_SIZE];
//	uint8_t state;
	uint16_t rxBuffSize;
	uint16_t txBuffSize;
	uint16_t kpAlvTime;		// keep alive time
	uint16_t srcPort;		// src port
	uint16_t destPort;		// destination port


	uint32_t retryTick;			// in ms
	uint32_t retryDelay;		// in ms
	uint32_t retryCounter;
	uint32_t retryCountMax;

	SOCK_Mode mode;
	W5500_SockIntr intr;		// // contain SEND_OK, TIMEOUT, RECV, DISCON & CON interrupt


}Socket;

void Socket_Handler(Socket *sock);
void Socket_RxTxHandler(Socket *sock);
void Socket_TxHandler(Socket *sock);
void Sock_StatusHandler(Socket *sock, int32_t status, int32_t opStatus );
void Sock_ErrorStatus(uint8_t sockId, int32_t status);
void Sock_PrintState(Socket *sock, SOCK_State state);
void Sock_ReadInterrupt(Socket *sock);
int32_t Sock_send(Socket *sock, uint8_t sn, uint8_t * buf, uint16_t len);   // custom send function due to interrupt issue

int8_t Sock_GetFreeId(void);
uint8_t Sock_LockId(uint8_t id);
void Sock_UnlockId(uint8_t id);

void Sock_Clear(Socket *sock);
void Sock_ClearAll(Socket *sock);


void Sock_AttachConnCallback(uint8_t (*callbackFunc)(Socket *sock));				// callback for client connect event
void Sock_AttachDisconnCallback( uint8_t (*callbackFunc)(Socket *sock));				// callback for client disconnect event
void Sock_AttachRecvCallback(uint8_t (*callbackFunc)(Socket *sock));					// callback for data received event of the client
void Sock_AttachSendCmpltCallback(uint8_t (*callbackFunc)(Socket *sock));					// callback for data received event of the client
void Sock_AttachErrorCallback(uint8_t (*callbackFunc)(Socket *sock, int32_t error));					// callback for error event of the client

#endif /* INC_SOCKETHANDLER_H_ */
