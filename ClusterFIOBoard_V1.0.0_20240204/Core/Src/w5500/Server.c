/*
 * Server.c
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#include <w5500_chip.h>
#include "Server.h"
#include "w5500.h"
#include "wizchip_conf.h"
#include "Debug.h"
#include "socket.h"
#include "stdio.h"
#include "Strings.h"
#include "main.h"
#include "string.h"
#include "Config.h"
#include "MB_TCP.h"


#if _WIZCHIP_ == 5200
   static uint16_t sock_next_rd[_WIZCHIP_SOCK_NUM_] ={0,};
#endif

//A20150601 : For integrating with W5300
#if _WIZCHIP_ == 5300
   uint8_t sock_remained_byte[_WIZCHIP_SOCK_NUM_] = {0,}; // set by wiz_recv_data()
#endif


#define CHECK_SOCKNUM()   \
   do{                    \
      if(sn > _WIZCHIP_SOCK_NUM_) return SOCKERR_SOCKNUM;   \
   }while(0);             \

#define CHECK_SOCKMODE(mode)  \
   do{                     \
      if((getSn_MR(sn) & 0x0F) != mode) return SOCKERR_SOCKMODE;  \
   }while(0);              \

#define CHECK_SOCKINIT()   \
   do{                     \
      if((getSn_SR(sn) != SOCK_INIT)) return SOCKERR_SOCKINIT; \
   }while(0);              \

#define CHECK_SOCKDATA()   \
   do{                     \
      if(len == 0) return SOCKERR_DATALEN;   \
   }while(0);              \



//extern DebugStruct dbug;





///*
// * Initialize the server
// * */
//void ServerInit(Server *server){
//
//	for(uint8_t sockId = 0; sockId < SERVER_SOCK_MAX; sockId++){
//		W5500_SetMaxSegmentSize(sockId, CONF_DEF_MB_TCP_MSS);			// set the maximum segment size for a socket
//
////		MSQ_init(&server->msgQ[sockId]); 								// initialize the msg queue for each socket
//	}
//
//
//}


/*
 * initialize the server socket
 * protocol maybe TCP, UDP etc
 * */
void  Server_SockInit(Server *server, uint8_t sockId, uint8_t protocol, uint16_t port, uint8_t flag, uint16_t kpAlvTime){

	server->sock[sockId].protocol 			= protocol;
	server->sock[sockId].flag 				= flag;
	server->sock[sockId].id 			= sockId;
	server->sock[sockId].srcPort		= port;
	server->sock[sockId].destPort 			= 0U;
	server->sock[sockId].rxBuffSize 		= 0U;
	server->sock[sockId].txBuffSize 		= 0U;
	server->sock[sockId].kpAlvTime 		= kpAlvTime;
	server->sock[sockId].isConnected 	= 0U;
	server->sock[sockId].intr.val 		= 0U;
	server->sock[sockId].rqstType 		= 0U;
	memset(server->sock[sockId].mac, 	0U, sizeof(server->sock[sockId].mac));
	memset(server->sock[sockId].destIP, 	0U, sizeof(server->sock[sockId].destIP));
	memset(server->sock[sockId].rxBuff, 0U, sizeof(server->sock[sockId].rxBuff));		// set the buffer zero
	memset(server->sock[sockId].txBuff, 0U, sizeof(server->sock[sockId].txBuff));		// set the buffer zero

}




void Server_Handler(Server *server, ServerMode mode){
	switch (mode) {
		case SERVER_MODE_TCP:
			Server_TCP(server);
			break;
		case SERVER_MODE_UDP:
			Server_UDP(server);
			break;
		case SERVER_MODE_MACRAW:
			//TODO: to implement later
			break;
		default:
			break;
	}

}





/*
 * TCP server handler
 * SOCK_INIT: Initiate state.
 * 		This indicates Socket n is opened with TCP mode.
 * 		It is changed to SOCK_INIT when Sn_MR(P[3:0]) = 001 and OPEN command is ordered.
 * 		After SOCK_INIT, user can use LISTEN /CONNECT command.
 *
 * SOCK_LISTEN: Listen state.
 * 		This indicates Socket n is operating as TCP servermode and waiting for connection-request (SYN packet) from a peer TCP client.
 * 		It will change to SOCK_ESTALBLISHED when the connection-request is successfully accepted.
 * 		Otherwise it will change to SOCK_CLOSED after TCPTO Sn_IR(TIMEOUT) = '1') is occurred.
 *
 *
 * SOCK_ESTABLISHED: Success to connect.
 * 		This indicates the status of the connection of Socket n.
 * 		It changes to SOCK_ESTABLISHED when the TCP SERVERprocessed the SYN packet from the TCP CLIENTduring SOCK_LISTEN, or when the CONNECT command is successful.
 * 		During SOCK_ESTABLISHED, DATA packet can be transferred using SEND or RECV command.
 *
 *
 * SOCK_CLOSE_WAIT: This indicates  Socket n received the disconnect-request (FIN packet) from the connected peer.
 * 		This is half-closing status, and data can be transferred.
 * 		For full-closing, DISCON command is used. But For just-closing, CLOSE command is used.
 *
 * SOCK_CLOSED: This indicates that Socket n is released.
 * 		When DICON, CLOSE command is ordered, or when a timeout occurs, it is changed to SOCK_CLOSED regardless of previous status.:
 *
 * */


void Server_TCP(Server *server){



	//	if(server->sockIndex >= SERVER_SOCK_MAX) server->sockIndex = 0U;		//
		Socket_Handler(&server->sock[0]);//server->sockIndex++


}




/*
 * UDP server handler
 * */
void Server_UDP(Server *server){

}





/*
 * Read all interrupts
 * */

//void Server_ReadInterpt(Server *server){
//
//	server->intr.val |= (uint16_t)wizchip_getinterrupt();				// read the socket common interrupt
//
////	sprintf(dbug.str,"\r\nCom: 0x%x",server->intr.val);
////	DEBUG_PRINT(dbug.str);
//
//	for (uint8_t sockId = 0; sockId < SOCK_MAX; sockId++) {
////		if((((uint8_t)(server->intr.val>>8))>>sockId)&0x01){
//			server->sock[sockId].intr.val |= getSn_IR(sockId);				// read the socket-n interrupt
////			setSn_IR(server->sockId,SIK_ALL);
////		}
////		sprintf(dbug.str,"\r\nCom: 0x%x",server->intr.val);
////		DEBUG_PRINT(dbug.str);
////		sprintf(dbug.str,"\r\nSock-%d: 0x%x",sockId,server->sock[sockId].intr.val);
////		DEBUG_PRINT(dbug.str);
//	}
//
//	wizchip_clrinterrupt((IK_IP_CONFLICT | IK_DEST_UNREACH | IK_PPPOE_TERMINATED |  IK_SOCK_ALL));
//}




void Server_Disconnect(Server *server){
	server->sock[server->sockIndex].status = getSn_SR(server->sock[server->sockIndex].id);
	if(server->sock[server->sockIndex].status == SOCK_ESTABLISHED){
		Sock_StatusHandler(&server->sock[server->sockIndex], server->sock[server->sockIndex].status, disconnect(server->sock[server->sockIndex].id));
	}
}


int32_t Serve_Send(uint8_t sockId, uint8_t *buff, uint16_t size){
	return send(sockId, buff, size);
}

