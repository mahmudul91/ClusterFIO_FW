/*
 * SocketHandler.c
 *
 *  Created on: Feb 1, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "SocketHandler.h"
#include "wizchip_conf.h"
#include "stdint.h"
#include "string.h"
#include "Debug.h"
#include "socket.h"
#include <math.h>
#include "Client.h"
#include "TimeStamp.h"
#include "DigitalIn.h"
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


   extern TimeStamp timStamp;
   extern uint8_t spiTxCplt;
   extern uint8_t spiRxCplt;
//extern DebugStruct dbug;
static uint8_t SocketUsedList[SOCKET_MAX] = {0};


/*---------Callback functions----------*/
uint8_t (*Sock_ConnCallback)(Socket *sock);					// callback for client connect event
uint8_t (*Sock_DisconnCallback)(Socket *sock);				// callback for client disconnect event
uint8_t (*Sock_RecvCallback)(Socket *sock);					// callback for data received event of the client
uint8_t (*Sock_SendCmpltCallback)(Socket *sock);					// callback for data received event of the client
uint8_t (*Sock_ErrorCallback)(Socket *sock, int32_t error);					// callback for data received event of the client



void Socket_Handler(Socket *sock){
	int16_t retStatus;
	uint8_t sockId = sock->id;

//	uint8_t initState = HAL_GPIO_ReadPin(W5500_IRQ_GPIO_Port, W5500_IRQ_Pin);
//////	DBG_SPRINT_NL("w5500 int: %d", initState);
//	if(initState == 0) return;

//	static uint32_t tick = 0;
//	if((HAL_GetTick() - tick)>=1){

		sock->status = getSn_SR(sockId);//W5500_GetSR();getSn_SR(sockId);				// read the socket current status

//		Sock_ReadInterrupt(sock);
//		W5500_ClearAllInterrupt();
//		tick = HAL_GetTick();
//	}

//		initState = HAL_GPIO_ReadPin(W5500_IRQ_GPIO_Port, W5500_IRQ_Pin);
//			DBG_SPRINT_NL("w5500 int 2: %d", initState);
	static uint32_t lastStatus=0;
	if(lastStatus != sock->status){
		DBG_SPRINT_NL("socket-%d: status-(%d)",sockId,sock->status);
	}
	lastStatus = sock->status;


	/*
	 * IF keep alive timeout occurred the socket will be auto close
	 * */
	if(sock->intr.ir.timeOut>0){
		sock->intr.ir.timeOut=0;
//		setSn_IR(sockId, Sn_IR_TIMEOUT);					// gets the connected client ip
//		DBG_SPRINT_NL("%s-%d: %s %s",STR_SOCKET, sockId, STR_CONNECTION, STR_TIMEOUT);
	}


	switch (sock->status) {
		case SOCK_CLOSED:{
			if(sock->isConnected>0){
				Sock_DisconnCallback(sock);
			}
				if((uint32_t)fabsf(HAL_GetTick()-sock->retryTick) >= sock->retryDelay || sock->mode == SOCK_MODE_TCP_SERVER){



					Sock_PrintState(sock, SOCK_STATE_OPENING);
					sock->rqstType = SOCK_REQ_NONE;
					sock->isConnected = 0U;

					if( sock->mode == SOCK_MODE_TCP_CLIENT)	sock->srcPort += CLIENT_SOCK_MAX;

					retStatus = socket(sockId, sock->protocol, sock->srcPort, sock->flag);



					Sock_StatusHandler(sock, sock->status, retStatus);
					if(sock->protocol == Sn_MR_TCP){
						W5500_SetAutoKeepAlive(sockId, sock->kpAlvTime);//sock->kpAlvTimer);
					}
					sock->retryTick = HAL_GetTick();
				}
			}
			break;
		case SOCK_INIT:	{
//				static uint32_t tick = SOCK_OPEN_RETRY_DELAY;

					switch (sock->mode) {
						case SOCK_MODE_TCP_SERVER:
							retStatus = listen(sockId);

							sock->isConnected = 0U;
							Sock_StatusHandler(sock, sock->status, retStatus);
							break;
						case SOCK_MODE_TCP_CLIENT:
								if((uint32_t)fabsf(HAL_GetTick()-sock->retryTick) >= sock->retryDelay){
									sock->isConnected = 0U;
									Sock_PrintState(sock, SOCK_STATE_CONNECTING);
									retStatus = connect(sockId, sock->destIP, sock->destPort);
	//								DBG_SPRINT_APPEND(dbug.str,"%s %s %d.%d.%d.%d:%d", STR_CONNECTED, STR_TO,
	//										sock->destIP[0], sock->destIP[1], sock->destIP[2], sock->destIP[3],	sock->destPort);
									Sock_StatusHandler(sock, sock->status, retStatus);
									sock->retryTick = HAL_GetTick();
//									sock->connect = 2;
								}

							break;
						default:
							break;
					}


			}
			break;
		case SOCK_ESTABLISHED:
//			DBG_SPRINT_NL("sock->status:%d",  sock->status);
//			//checks client is connected to the server
			if(sock->isConnected ==0){		//	(getSn_IR(sockId) & Sn_IR_CON)){//	// This connect interrupt is issued one time when the connection with peer is successful
				sock->intr.ir.conn = 0U;	// reset the interrupt bit

				setSn_IR(sockId,Sn_IR_CON);

				sock->rqstType = SOCK_REQ_CONN;
				getSn_DIPR(sockId, sock->destIP);					// gets the connected client ip

				sock->destPort = getSn_DPORT(sockId);;				// get the connected client port

				/*connection established Callback function*/
				Sock_ConnCallback(sock);

			}
			sock->isConnected = 1U;
//			static uint32_t tm = 0;
			// checks data receive interrupt

			if( ((sock->rxBuffSize = getSn_RX_RSR(sockId)) > 0)){//sock->intr.ir.recv &&

//				sock->rqstType = MBTCP_REQ_DATA;
				if(sock->rxBuffSize > SOCK_BUF_MAX_SIZE){
					sock->rxBuffSize = SOCK_BUF_MAX_SIZE;
				}
//				Sock_PrintState(sock, SOCK_STATE_RECEIVING);
//				DBG_SPRINT_NL("BR-%ld: %ld", HAL_GetTick(), sock->testCOunter);//TIMSTMP:
//				DBG_SPRINT_NL("SKR-%d)[",  sock->rxBuffSize);
				spiRxCplt = 1;


//				tm = TS_GetUS(&timStamp);

				retStatus = recv(sockId, sock->rxBuff, sock->rxBuffSize);
//				tm = TS_GetUS(&timStamp)-tm;

//				DBG_SPRINT_NL("tm-%ld:", tm);//TIMSTMP:
//				DBG_SPRINT_NL("rtS:%d sz-%d",  retStatus, sock->rxBuffSize);
				if(retStatus>0){
					sock->intr.ir.recv = 0U;			// warning: for non block io mode, it should be placed in the all switch-case block below except socket_busy block
//					setSn_IR(sockId,Sn_IR_RECV);		// clear the receives interrupt
//					Sock_PrintState(sock, SOCK_STATE_RECEIVED);
//					DBG_SPRINT_APPEND(dbug.str,"\r\n%s-%d(%s): Rcvd(", STR_SOCKET, sockId, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
//					DBG_PRINT(dbug.str);
//					DBG_SPRINT_NL("SKR-%d)[",  sock->rxBuffSize);
//					for(uint8_t i = 0; i < sock->rxBuffSize; i++){
//						DBG_SPRINT_APPEND(dbug.str, "%02X",sock->rxBuff[i]);
//						if(i<sock->rxBuffSize-1) DBG_SPRINT_APPEND(dbug.str, ",");
//					}
//					DBG_SPRINT_APPEND(dbug.str, "]");
//					DBG_PRINT(dbug.str);
//					memset(sock->rxBuff,0,SOCK_BUF_MAX_SIZE);
					// data received callback function
//					tm = TS_GetUS(&timStamp);
					Sock_RecvCallback(sock); // TODO: Uncomment
//					tm = TS_GetUS(&timStamp) - tm;
//					DBG_SPRINT_NL("tm: %u",tm);
				}else{

					Sock_StatusHandler(sock, sock->status, retStatus);
				}

//				DBG_SPRINT_NL("SZ-(%d)[",  sock->rxBuffSize);
			}

			/*Send data------------*/

			if(sock->isReadyToSend > 0 && sock->txBuffSize > 0){
//				Sock_PrintState(sock, SOCK_STATE_SENDING);
//				retStatus = Sock_send(sock, sockId, sock->txBuff, sock->txBuffSize);

//
//				DBG_SPRINT_NL("TSBS%ld:", HAL_GetTick());//TIMSTMP:
//				DBG_SPRINT_APPEND(dbug.str,"\r\n%s-%d(%s): Send[", STR_SOCKET, sockId, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
//				DBG_PRINT(dbug.str);
//				DBG_SPRINT("\r\nSKT:%d]",  sock->txBuffSize);
//				for(uint8_t i = 0; i < sock->txBuffSize; i++){
//					DBG_SPRINT_APPEND(dbug.str, "%02X",sock->txBuff[i]);
//					if(i<sock->txBuffSize-1) DBG_SPRINT_APPEND(dbug.str, ",");
//				}
//				DBG_SPRINT_APPEND(dbug.str, "]");
//				DBG_PRINT(dbug.str);

//				tm = TS_GetUS(&timStamp);
				retStatus = send( sockId, sock->txBuff, sock->txBuffSize);
//				tm = TS_GetUS(&timStamp) -tm;
//
//
//				DBG_SPRINT_NL("tm: %u",tm);
				if(retStatus <= 0){
//					sock->txBuffSize  = 0;
					Sock_StatusHandler(sock, sock->status, retStatus);
					if(retStatus < 0){
						close(sockId);
					}
				}else{
					sock->isReadyToSend=0;
//					sock->intr.ir.sendOk = 0U;
//					setSn_IR(sockId,Sn_IR_SENDOK);
//					memset(sock->txBuff, 0, sock->txBuffSize);
					sock->txBuffSize  = 0;

//					Sock_PrintState(sock, SOCK_STATE_SENT);
					Sock_SendCmpltCallback(sock);
				}
			}
			break;
		case SOCK_CLOSE_WAIT:
			sock->intr.ir.disconn = 0U;
			sock->isConnected = 0U;
			retStatus = disconnect(sockId);
			Sock_StatusHandler(sock, sock->status, retStatus);
			// Disconnect callback function
			Sock_DisconnCallback(sock);
			Sock_Clear(sock);
			break;
		case SOCK_LISTEN:
//			sock->isConnected = 0U;

			break;
		default:
			break;
	}
}




void Socket_RxTxHandler(Socket *sock){
	int16_t retStatus;
	uint8_t sockId = sock->id;
	char str[100] = {0};
	if(sock->isConnected == 0) return;

//	sock->status = getSn_SR(sockId);				// read the socket current status
//	Sock_ReadInterrupt(sock);
	if(sock->status ==SOCK_ESTABLISHED){


		//checks client is connected to the server
//		if(sock->isConnected ==0){		//	(getSn_IR(sockId) & Sn_IR_CON)){//	// This connect interrupt is issued one time when the connection with peer is successful
//			sock->intr.ir.conn = 0U;							// reset the interrupt bit
//			setSn_IR(sockId,Sn_IR_CON);
//			sock->rqstType = SOCK_REQ_CONN;
//			getSn_DIPR(sockId, sock->destIP);					// gets the connected client ip
//			sock->destPort = getSn_DPORT(sockId);;				// get the connected client port
//
//			/*connection established Callback function*/
//			Sock_ConnCallback(sock);
//
//		}
//		sock->isConnected = 1U;
		// checks data receive interrupt
		if(sock->intr.ir.recv && ((sock->rxBuffSize = getSn_RX_RSR(sockId)) > 0)){

//				sock->rqstType = MBTCP_REQ_DATA;
			if(sock->rxBuffSize > SOCK_BUF_MAX_SIZE){
				sock->rxBuffSize = SOCK_BUF_MAX_SIZE;
			}
//				Sock_PrintState(sock, SOCK_STATE_RECEIVING);
			retStatus = recv(sockId, sock->rxBuff, sock->rxBuffSize);


			if(retStatus>0){
				sock->intr.ir.recv = 0U;			// warning: for non block io mode, it should be placed in the all switch-case block below except socket_busy block
				setSn_IR(sockId,Sn_IR_RECV);		// clear the receives interrupt
//					int status = Sock_send(sock, sockId, sock->rxBuff, sock->rxBuffSize);

//					Sock_PrintState(sock, SOCK_STATE_RECEIVED);
				DBG_SPRINT_APPEND(str,"\r\n%s-%d(%s): Rcvd(", STR_SOCKET, sockId, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
				DBG_PRINT(str);
				DBG_SPRINT("%d)[",  sock->rxBuffSize);
//					for(uint8_t i = 0; i < sock->rxBuffSize; i++){
//						DBG_SPRINT_APPEND(dbug.str, "%02X",sock->rxBuff[i]);
//						if(i<sock->rxBuffSize-1) DBG_SPRINT_APPEND(dbug.str, ",");
//					}
//					DBG_SPRINT_APPEND(dbug.str, "]");
//					DBG_PRINT(dbug.str);
//					memset(sock->rxBuff,0,SOCK_BUF_MAX_SIZE);
				// data received callback function
				Sock_RecvCallback(sock); // TODO: Uncomment

			}else{
				Sock_StatusHandler(sock, sock->status, retStatus);
			}


		}



		/*Send data------------*/

		if(sock->isReadyToSend > 0 && sock->txBuffSize > 0){
//				Sock_PrintState(sock, SOCK_STATE_SENDING);
//				retStatus = Sock_send(sock, sockId, sock->txBuff, sock->txBuffSize);


			DBG_SPRINT_APPEND(str,"\r\n%s-%d(%s): Send[", STR_SOCKET, sockId, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
			DBG_PRINT(str);
			DBG_SPRINT("%d]",  sock->txBuffSize);
//				for(uint8_t i = 0; i < sock->txBuffSize; i++){
//					DBG_SPRINT_APPEND(dbug.str, "%02X",sock->txBuff[i]);
//					if(i<sock->txBuffSize-1) DBG_SPRINT_APPEND(dbug.str, ",");
//				}
//				DBG_SPRINT_APPEND(dbug.str, "]");
//				DBG_PRINT(dbug.str);

			retStatus = send( sockId, sock->txBuff, sock->txBuffSize);

			if(retStatus <= 0){
//					sock->txBuffSize  = 0;
				Sock_StatusHandler(sock, sock->status, retStatus);
			}else{
				sock->isReadyToSend=0;
//					sock->intr.ir.sendOk = 0U;
//					setSn_IR(sockId,Sn_IR_SENDOK);
				memset(sock->txBuff, 0, sock->txBuffSize);
				sock->txBuffSize  = 0;

//					Sock_PrintState(sock, SOCK_STATE_SENT);
				Sock_SendCmpltCallback(sock);
			}
		}

//			if(sock->connect<=0 && sock->connectMode == SOCK_CONN_MODE_MANUAL){
//				sock->isConnected = 0U;
//				retStatus = disconnect(sockId);
//
//				Sock_StatusHandler(sock, sock->status, retStatus);
//
//				// Disconnect callback function
//				Sock_DisconnCallback(sock);
//				Sock_Clear(sock);
//				sock->connect = 2;
//			}

	}
}




void Socket_TxHandler(Socket *sock){
	int16_t retStatus;
	uint8_t sockId = sock->id;
	if(sock->isConnected == 0) return;

	sock->status = getSn_SR(sockId);				// read the socket current status

//	sock->status = getSn_SR(sockId);				// read the socket current status
//	Sock_ReadInterrupt(sock);
	if(sock->status ==SOCK_ESTABLISHED){
		/*Send data------------*/

		if(sock->isReadyToSend > 0 && sock->txBuffSize > 0){
//				Sock_PrintState(sock, SOCK_STATE_SENDING);
//				retStatus = Sock_send(sock, sockId, sock->txBuff, sock->txBuffSize);


//			DBG_SPRINT_APPEND(dbug.str,"\r\n%s-%d(%s): Send[", STR_SOCKET, sockId, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
//			DBG_PRINT(dbug.str);
//			DBG_SPRINT("%d]",  sock->txBuffSize);
//				for(uint8_t i = 0; i < sock->txBuffSize; i++){
//					DBG_SPRINT_APPEND(dbug.str, "%02X",sock->txBuff[i]);
//					if(i<sock->txBuffSize-1) DBG_SPRINT_APPEND(dbug.str, ",");
//				}
//				DBG_SPRINT_APPEND(dbug.str, "]");
//				DBG_PRINT(dbug.str);

			retStatus = send( sockId, sock->txBuff, sock->txBuffSize);

			if(retStatus <= 0){
//					sock->txBuffSize  = 0;
				Sock_StatusHandler(sock, sock->status, retStatus);
			}else{
				sock->isReadyToSend=0;
//					sock->intr.ir.sendOk = 0U;
//					setSn_IR(sockId,Sn_IR_SENDOK);
				memset(sock->txBuff, 0, sock->txBuffSize);
				sock->txBuffSize  = 0;

//					Sock_PrintState(sock, SOCK_STATE_SENT);
				Sock_SendCmpltCallback(sock);
			}
		}

//			if(sock->connect<=0 && sock->connectMode == SOCK_CONN_MODE_MANUAL){
//				sock->isConnected = 0U;
//				retStatus = disconnect(sockId);
//
//				Sock_StatusHandler(sock, sock->status, retStatus);
//
//				// Disconnect callback function
//				Sock_DisconnCallback(sock);
//				Sock_Clear(sock);
//				sock->connect = 2;
//			}

	}
}



/**
  * @brief Custom send function. The built-in send function read the interrupt and clear interrupt register.
  * It is not supposed to clear the interrupt register. That's why I build a custom the send function
  * @param schedule: schedule task id
  * @retval none
  */
static uint32_t sock_is_sending = 0;
int32_t Sock_send(Socket *sock, uint8_t sn, uint8_t * buf, uint16_t len){

   uint8_t tmp=0;
   uint16_t freesize=0;

   CHECK_SOCKNUM();
   CHECK_SOCKMODE(Sn_MR_TCP);
   CHECK_SOCKDATA();
   tmp = getSn_SR(sn);
   if(tmp != SOCK_ESTABLISHED && tmp != SOCK_CLOSE_WAIT) return SOCKERR_SOCKSTATUS;


   if( sock_is_sending & (1<<sn) )
   {
//      tmp = getSn_IR(sn);
	   tmp = sock->intr.val;

      if(tmp & Sn_IR_SENDOK)
      {
         setSn_IR(sn, Sn_IR_SENDOK);
         sock->intr.ir.sendOk = 0U;
         //M20150401 : Typing Error
         //#if _WZICHIP_ == 5200
         #if _WIZCHIP_ == 5200
            if(getSn_TX_RD(sn) != sock_next_rd[sn])
            {
               setSn_CR(sn,Sn_CR_SEND);
               while(getSn_CR(sn));
               return SOCK_BUSY;
            }
         #endif
         sock_is_sending &= ~(1<<sn);
      }
      else if(tmp & Sn_IR_TIMEOUT)
      {
         close(sn);
         return SOCKERR_TIMEOUT;
      }
      else{
//    	  DBG_SPRINT_NL("SOCK_BUSY 0, 0x%02x", tmp);
    	  return SOCK_BUSY;
      }
   }
   freesize = getSn_TxMAX(sn);
   if (len > freesize) len = freesize; // check size not to exceed MAX size.
   while(1)
   {
      freesize = getSn_TX_FSR(sn);
      tmp = getSn_SR(sn);
      if ((tmp != SOCK_ESTABLISHED) && (tmp != SOCK_CLOSE_WAIT))
      {
         close(sn);
         return SOCKERR_SOCKSTATUS;
      }
      if((len > freesize) ){ return SOCK_BUSY;}
      if(len <= freesize) break;
   }

   wiz_send_data(sn, buf, len);
   #if _WIZCHIP_ == 5200
      sock_next_rd[sn] = getSn_TX_RD(sn) + len;
   #endif

   #if _WIZCHIP_ == 5300
      setSn_TX_WRSR(sn,len);
   #endif

   setSn_CR(sn,Sn_CR_SEND);
   /* wait to process the command... */
   while(getSn_CR(sn));
   sock_is_sending |= (1 << sn);
   //M20150409 : Explicit Type Casting
   //return len;
   return (int32_t)len;
}



void Sock_ReadInterrupt(Socket *sock){

	sock->intr.val |= getSn_IR(sock->id);				// read the socket-n interrupt


}



void Sock_PrintState(Socket *sock, SOCK_State state){
	static SOCK_State lastState = 0;
	if(state == lastState) return;
	lastState = state;
	char str[50] = {0};
	if(state < SOCK_STATE_MAX){
	DBG_SPRINT_APPEND(str,"%s-%d(%s): ",STR_SOCKET, sock->id,  (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC"));
	switch (state) {
		case SOCK_STATE_OPENING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_OPENING);
			break;
		case SOCK_STATE_OPENED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_OPENED);
			break;
		case SOCK_STATE_LISTENING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_LISTENING);
			break;
		case SOCK_STATE_CONNECTING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_CONNECTING);
			break;
		case SOCK_STATE_CONNECTED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_CONNECTED);
			break;
		case SOCK_STATE_SENDING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_SENDING);
			break;
		case SOCK_STATE_SENT:
			DBG_SPRINT_APPEND(str,"%s" ,STR_SENT);
			break;
		case SOCK_STATE_RECEIVING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_RECEIVING);
			break;
		case SOCK_STATE_RECEIVED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_RECEIVED);
			break;
		case SOCK_STATE_DISCONNECTING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_DISCONNECTING);
			break;
		case SOCK_STATE_DISCONNECTED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_DISCONNECTED);
			break;
		case SOCK_STATE_CLOSING:
			DBG_SPRINT_APPEND(str,"%s..." ,STR_CLOSING);
			break;
		case SOCK_STATE_CLOSED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_CLOSED);
			break;
		case SOCK_STATE_FAILED:
			DBG_SPRINT_APPEND(str,"%s" ,STR_FAILED);
			break;
		default:
			break;
	}
	DBG_SPRINT_NL(str);
	}
}


/**
  * @brief  socket status handler
  * @param status: socket n connection status
  * @param opStatus: socket n operation status under connection status
  * @retval none
  */
void Sock_StatusHandler(Socket *sock, int32_t status, int32_t operStatus ){

	char str[50] = {0};

	DBG_SPRINT_APPEND(str,"%s-%d(%s): %s(%d), ",STR_SOCKET, sock->id, (sock->mode == SOCK_MODE_TCP_SERVER ? "TS": "TC") ,STR_STATUS, status);

	if(operStatus >= SOCK_OK || (status == SOCK_CLOSED && operStatus == sock->id)){
		switch (status) {
			case SOCK_CLOSED:
				if(operStatus == sock->id){
					DBG_SPRINT_APPEND(str,"%s" ,STR_OPENED);
				}
				break;
			case SOCK_INIT:
				switch (sock->mode) {
					case SOCK_MODE_TCP_SERVER:{
						uint8_t ip[4];
						getSIPR(ip);
						uint16_t srcPort = getSn_PORT(sock->id);
						DBG_SPRINT_APPEND(str,"%s %s %d.%d.%d.%d:%d", STR_LISTENING, STR_AT,
								ip[0],ip[1],
								ip[2],ip[3],
								srcPort);
						}
						break;
					case SOCK_MODE_TCP_CLIENT:

						DBG_SPRINT_APPEND(str,"%s %s %d.%d.%d.%d:%d, Src %s: %d", STR_CONNECTED, STR_TO,
							sock->destIP[0], sock->destIP[1], sock->destIP[2], sock->destIP[3],	sock->destPort,STR_PORT, sock->srcPort);

						break;
					default:
						break;
				}
				break;
			case SOCK_CLOSE_WAIT:
				DBG_SPRINT_APPEND(str,"%s ", STR_CLOSED);
				break;
			case SOCK_ESTABLISHED:
				if(sock->mode == SOCK_MODE_TCP_SERVER){
					DBG_SPRINT_APPEND(str,"%s %s %d.%d.%d.%d:%d", STR_CONNECTED, STR_TO,
						sock->destIP[0], sock->destIP[1], sock->destIP[2], sock->destIP[3], sock->destPort);
				}
				break;
			default:
				break;
		}
		DBG_SPRINT_NL(str);
	}else{
		static int32_t lastStatus = 0;
		static int32_t lastOperStatus = 0;
		if(status == lastStatus && lastOperStatus == operStatus) return;
		lastStatus = status;
		lastOperStatus = operStatus;
		Sock_ErrorStatus(sock->id, operStatus);
		Sock_ErrorCallback(sock, operStatus);
	}
}


void Sock_ErrorStatus(uint8_t sockId, int32_t status){
	char str[100] = {0};
	if(status <= 0){			// all error status are negative

		switch (status) {
			case SOCK_BUSY:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_SOCKET, STR_IS, STR_BUSY);
				break;
			case SOCKERR_SOCKNUM:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_INVALID, STR_SOCKET, STR_NUMBER);
				break;
			case SOCKERR_SOCKMODE:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s %s",STR_ERROR, status, STR_NOT, STR_SUPPORT, STR_SOCKET, STR_MODE);
				break;
			case SOCKERR_SOCKINIT:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s %s %s %s",STR_ERROR, status, STR_NOT, STR_INITIALIZED, STR_OR, STR_ZERO, STR_IP, STR_ADDRESS);
				break;
			case SOCKERR_SOCKFLAG:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_INVALID, STR_SOCKET, STR_FLAG);
				break;
			case SOCKERR_SOCKCLOSED:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_SOCKET, STR_CLOSED, STR_UNEXPECTEDLY);
				break;
			case SOCKERR_SOCKSTATUS:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s %s %s %s",STR_ERROR, status, STR_INVALID, STR_SOCKET, STR_STATUS, STR_FOR, STR_SOCKET, STR_OPERATION);
				break;
			case SOCKERR_DATALEN:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_ZERO, STR_DATA, STR_LENGTH);
				break;
			case SOCKERR_TIMEOUT:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s",STR_ERROR, status, STR_TIMEOUT, STR_OCCURRED);
				break;
			case SOCKERR_IPINVALID:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s %s",STR_ERROR, status, STR_WRONG, STR_SERVER, STR_IP, STR_ADDRESS);
				break;
			case SOCKERR_PORTZERO:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s %s %s",STR_ERROR, status, STR_SERVER, STR_PORT, STR_ZERO);
				break;
			default:
				DBG_SPRINT_APPEND(str,"%s(%d) - %s",STR_ERROR, status, STR_UNKNOWN);
				break;
		}
		DBG_SPRINT_NL(str);
		if(status < 0){
//			close(sockId);
		}


	}
}



/*
* @brief provides free id automatically to prevent confliction of socket id
* @retval returns free id, if there is no free id it will return -1
*/
int8_t Sock_GetFreeId(void){
	for(uint8_t id = SOCKET_0; id < SOCKET_MAX;id++){
		if(SocketUsedList[id] == 0){
			SocketUsedList[id] = 1;
			return id;
		}
	}
	return -1;
}



/*
* @brief locks used id manually to prevent confliction of socket id
* @retval 1 = locked,  0 = not locked due to not availability
*/
uint8_t Sock_LockId(uint8_t id){
	if(SocketUsedList[id] == 0){
		SocketUsedList[id] = 1;
		return 1;
	}else{
		return 0;
	}
}

/*
* @brief unlock id manually if an id need to free
* @retval 1 = locked,  0 = not locked due to not availability
*/

void Sock_UnlockId(uint8_t id){
	SocketUsedList[id] = 0;
}

/*
 * Clear the socket params
 * */
void Sock_Clear(Socket *sock){
	sock->destPort 			= 0U;
	sock->rxBuffSize 		= 0U;
	sock->txBuffSize		= 0U;
	sock->intr.val 			= 0;
	sock->isConnected 		= 0;
	sock->rqstType 			= 0;
	memset(sock->mac, 		0U, sizeof(sock->mac));
	memset(sock->destIP, 		0U, sizeof(sock->destIP));
	memset(sock->rxBuff, 	0U, sizeof(sock->rxBuff));		// set the buffer zero
	memset(sock->txBuff, 	0U, sizeof(sock->txBuff));		// set the buffer zero

}


void Sock_ClearAll(Socket *sock){
	sock->id				= 0xFF;
	sock->srcPort			= 0U;
	sock->destPort 			= 0U;
	sock->flag				= 0U;
	sock->protocol			= 0U;
	sock->status			= 0U;
	sock->kpAlvTime			= 0U;
	sock->rxBuffSize 		= 0U;
	sock->txBuffSize		= 0U;
	sock->intr.val 			= 0;
	sock->isConnected 		= 0;
	sock->rqstType 			= 0;
	memset(sock->mac, 		0U, sizeof(sock->mac));
	memset(sock->destIP, 		0U, sizeof(sock->destIP));
	memset(sock->rxBuff, 	0U, sizeof(sock->rxBuff));		// set the buffer zero
	memset(sock->txBuff, 	0U, sizeof(sock->txBuff));		// set the buffer zero
}






void Sock_AttachConnCallback(uint8_t (*callbackFunc)(Socket *sock)){
	Sock_ConnCallback = callbackFunc;
}
void Sock_AttachDisconnCallback( uint8_t (*callbackFunc)(Socket *sock)){
	Sock_DisconnCallback = callbackFunc;
}
void Sock_AttachRecvCallback(uint8_t (*callbackFunc)(Socket *sock)){
	Sock_RecvCallback = callbackFunc;
}

void Sock_AttachSendCmpltCallback(uint8_t (*callbackFunc)(Socket *sock)){
	Sock_SendCmpltCallback = callbackFunc;
}

void Sock_AttachErrorCallback(uint8_t (*callbackFunc)(Socket *sock, int32_t error)){
	Sock_ErrorCallback = callbackFunc;
}
