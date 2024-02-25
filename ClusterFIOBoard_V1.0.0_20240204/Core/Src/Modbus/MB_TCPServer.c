/*
 * MB_TCPServer.c
 *
 *  Created on: Feb 5, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "MB_TCPServer.h"
#include "MB_Tools.h"
#include "Debug.h"
#include "SocketHandler.h"
#include "MB_Handler.h"
#include "MB_Serial.h"
#include "MB_Gateway.h"
#include "Task_Handler.h"

//extern DebugStruct dbug;// todo: need to uncomment

uint8_t (*MBTCPS_SendCallback)(MBTCPS_ClientParams *client);					// callback for client connect event
uint8_t (*MBTCPS_SendExcepCallback)(MBTCPS_ClientParams *client);

/*
* @brief initialize the modbus tcp server parameters
* @param mbtcps: modbus tcp server handler struct
* @retval none
*/
void MBTCPS_Init(MB_TCPServer *mbtcps){
//	memset(mbtcps->netInfo.mac,0, sizeof(mbtcps->netInfo.mac));
//	memset(mbtcps->netInfo.ip,0, sizeof(mbtcps->netInfo.ip));
//	memset(mbtcps->netInfo.sn,0, sizeof(mbtcps->netInfo.sn));
//	memset(mbtcps->netInfo.gw,0, sizeof(mbtcps->netInfo.gw));
//	memset(mbtcps->netInfo.dns,0, sizeof(mbtcps->netInfo.dns));
//	mbtcps->netInfo.dhcpMode = 0;
	mbtcps->acl.enabled = 0U;
	mbtcps->nbOfConns = 0U;
	for(uint8_t i = 0U; i < MBTCPS_MAX_CONNS; i++){
		MBTCPS_InitClientParams(&mbtcps->clients[i]);
	}
//	MBMQ_init(&mbtcps->msgQ);
	MBMQ_clearMsg(&mbtcps->msg);

}

/**
* @brief initialize  client which will connect to the modbus tcp server
* @param cParams: pointer to the struct of client params
* @retval none
*/
void MBTCPS_InitClientParams(MBTCPS_ClientParams *cParams){
	MBTCPS_ClearClientParams(cParams);
	//	cParams->id 		 = 0U;
//	cParams->port  	 = 0U;
//	cParams->transId  	 = 0U;
//	cParams->protocolId  = 0U;
//	cParams->length		 = 0U;
//	cParams->uintId 	 = 0U;
//	cParams->excepCode 	 = 0U;
////	cParams->curntTrnsNo = 0U;
//	memset(cParams->ip,0, (sizeof(cParams->ip)/sizeof(cParams->ip[0])));
//	memset(cParams->mac,0, (sizeof(cParams->ip)/sizeof(cParams->mac[0])));
//	memset(cParams->rxBuff,0, (sizeof(cParams->rxBuff)/sizeof(cParams->rxBuff[0])));
//	memset(cParams->txBuff,0, (sizeof(cParams->txBuff)/sizeof(cParams->txBuff[0])));
//	cParams->rxBuffSize = 0U;
//	cParams->txBuffSize = 0U;
}


/**
* @brief initialize  client which will connect to the modbus tcp server
* @param cParams: pointer to the struct of client params
* @retval none
*/
void MBTCPS_ClearClientParams(MBTCPS_ClientParams *cParams){
	cParams->id 		 = 0U;
	cParams->sockId 		 = 0U;
	cParams->port  	 = 0U;
	cParams->transId  	 = 0U;
	cParams->protocolId  = 0U;
	cParams->length		 = 0U;
	cParams->unitId 	 = 0U;
	cParams->excepCode 	 = 0U;
//	cParams->curntTrnsNo = 0U;
	memset(cParams->ip,0, (sizeof(cParams->ip)/sizeof(cParams->ip[0])));
	memset(cParams->mac,0, (sizeof(cParams->ip)/sizeof(cParams->mac[0])));
	memset(cParams->rxBuff,0, (sizeof(cParams->rxBuff)/sizeof(cParams->rxBuff[0])));
	memset(cParams->txBuff,0, (sizeof(cParams->txBuff)/sizeof(cParams->txBuff[0])));
	cParams->rxBuffSize = 0U;
	cParams->txBuffSize = 0U;
}



/**
* @brief Process the request from the client
* @param mbtcps: pointer to the struct of modbus tcp server
* @param client: pointer to the struct of client
* @param reqType: request type, two types of the request connection or data request
* @retval none
*/
//
//void MBTCPS_ProcessRequest(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client, uint8_t reqType){
//	// check the request type connection request or data request
////	DBG_SPRINT("\r\n reqType 1: %d", reqType);
//	switch (reqType) {
//		case SOCK_REQ_CONN:
//			mbtcps->acl.enabled = 0; // TODO: uncomment
//			if (mbtcps->acl.enabled) {
//				/*
//				 * TODO: write code for ACL checking
//				 * if IP is forbidden just refused the connection
//				 * else if the IP is authorized simply accept the connection
//				 */
//
//			}
//			/*	check the max number of client connections
//			 * Note: By the way w5500 network chip handles this option
//			 * */
//			if(mbtcps->nbOfConns < MBTCPS_MAX_CONNS){
//				mbtcps->nbOfConns++;
//				/*
//				 * Simply establish the connection
//				 * TODO: to write code later for handling the connection establishment
//				 *
//				 * */
//
//			}else{
//				/*
//				 * Close the oldest, unused & no priority connection
//				 * TODO: To implement at the end of this project.
//				 * Write code for closing of oldest, unused, no priority connection
//				 * Maybe no need to implement this section, because it can handle only
//				 * 2 connections,
//				 * */
//			}
//			DBG_SPRINT("\r\n reqType 2: %d", reqType);
//			break;
//		case SOCK_REQ_DATA:
//			DBG_SPRINT("\r\n reqType 3: %d", reqType);
//			//TODO: to write code for PDU checking
//			if(MBTCPS_FindClientById(mbtcps, client->id) == MB_OK){
//				mbtcps->clients[mbtcps->clientIndex].port = client->port;
//				mbtcps->clients[mbtcps->clientIndex].isConnected = client->isConnected;
//				memcpy(mbtcps->clients[mbtcps->clientIndex].rxBuff, client->rxBuff, client->rxBuffSize);
//				mbtcps->clients[mbtcps->clientIndex].rxBuffSize = client->rxBuffSize;
//				DBG_PRINT("\r\n Preq con-3");
//				MBTCPS_ProcessMBAPHeader(mbtcps);
////				MBMQ_Msg msg;
////				msg.key =  client->id;
////				msg.size = client->rxBuffSize;
////				memcpy(msg.buff, client->rxBuff, client->rxBuffSize);
////
////				MBMQ_enque(&mbtcps->msgQ, &msg);
//
////				MBTCPS_ProcessReqADU(mbtcps);
//
//			}
//
//			break;
//		default:
//			break;
//	}
//}




//
///**
//* @brief Process requested adu buffer received by tcp server
//* @param mbtcp: modbus tcp handler struct
//* @retval error status
//*/
//uint8_t MBTCPS_ProcessReqPDU(MB_TCPServer *mbtcps){
//
//	MBMQ_Msg msg;
//	if(MBMQ_deque(&mbtcps->msgQ, &msg)>0){				// if there is a msg at least
//		uint8_t processStatus = 0U;
//
//		if(MBTCPS_FindClientByIdAndPort(mbtcps, msg.id, msg.key) == MB_OK){
//			uint8_t  clientIndex = mbtcps->clientIndex;
//			MB_setReqBuff((msg.buff+MBTCP_INDX_UNIT_ID-1), msg.size);								// set the request buff to process the modbus request
//			MB_setRespBuff((mbtcps->clients[clientIndex].txBuff+MBTCP_INDX_UNIT_ID-1));	// set the response buffer to get the response according to th request
//			uint8_t funCode = MB_getFunctionCode();
//
//			if( funCode != MB_FC_DIAG_08 && funCode != MB_FC_GCEC_11 && MB_isFunctionCodeValid(funCode)>0U){
//				processStatus = MB_executeRequest(funCode);
//			}else{
//				mbtcps->clients[clientIndex].excepCode = MB_EC_ILF_1;
//				processStatus = MB_EC_ILF_1;												// exception code 1 for illegal function code
//			}
//
//
//			if (processStatus != 0u){
//				if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){							// for exception response
//					mbtcps->clients[clientIndex].excepCode = processStatus;
//					MB_excepRespBuilder(processStatus);					// processStatus = 255 means, no exception and the response has been built
//				}
//				mbtcps->clients[clientIndex].txBuffSize = MB_getRespBuffSize()+3;
//				MBTCPS_SetMBAPHeaderToTXBuff(mbtcps, clientIndex);
//
//				MBTCPS_RespHandler(mbtcps, clientIndex);
//				return MB_OK;
//			}
//		}
//	}
//
//	return MB_OK;
//
//
////
////	if(MBTCPS_CheckProtocolID(mbtcps->clients[mbtcps->clientIndex].protocolId) == MB_OK){
////		uint8_t processStatus = 0U;
////		if(mbtcps->curntTrnsNo < MBTCPS_MAX_TRANS){
////			// set the buffer start address from unit id r slave id
////			MB_setReqBuff((mbtcps->clients[mbtcps->clientIndex].rxBuff+MBTCP_H_INDX_UNIT_ID-1), mbtcps->clients[mbtcps->clientIndex].length);
////			MB_setRespBuff((mbtcps->clients[mbtcps->clientIndex].txBuff+MBTCP_H_INDX_UNIT_ID-1));
////			uint8_t funCode = MB_getFunctionCode();
////
////			if( funCode != MB_FC_DIAG_08 && funCode != MB_FC_GCEC_11 && MB_isFunctionCodeValid(funCode)>0U){
////				processStatus = MB_executeRequest(funCode);
////			}else{
////				mbtcps->clients[mbtcps->clientIndex].excepCode = MB_EC_ILF_1;
////				processStatus = MB_EC_ILF_1;												// exception code 1 for illegal function code
////			}
////		}else{																				// if no transaction is available
////			mbtcps->clients[mbtcps->clientIndex].excepCode = MB_EC_SDB_6;					// set the server busy exception code
////			processStatus = MB_EC_SDB_6;
////		}
////
////		if (processStatus != 0u){
////			if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){							// for exception response
////				mbtcps->clients[mbtcps->clientIndex].excepCode = processStatus;
////				MB_excepRespBuilder(processStatus);					// processStatus = 255 means, no exception and the response has been built
////			}
////			mbtcps->clients[mbtcps->clientIndex].txBuffSize = MB_getRespBuffSize()+3;
////			MBTCPS_SetMBAPHeaderToTXBuff(mbtcps);
////			return MB_OK;
////		}
////		return MB_OK;
////	}else{
////		return MB_ERROR;
////	}
//
//}
//


void MBTCPS_ConnHandler(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client){

	/*Checks the ACL---------------*/
	mbtcps->acl.enabled = 0; // TODO: uncomment
	if (mbtcps->acl.enabled) {
		/*
		 * TODO: write code for ACL checking
		 * if IP is forbidden just refused the connection
		 * else if the IP is authorized simply accept the connection
		 */

	}

	if(MBTCPS_FindClientById(mbtcps, client->id)!=MB_OK){
//		MBTCPS_UpdateClient(mbtcps, client);		//TODO: no need to update the connection
//	}else{
		/*	check the max number of client connections
		 * Note: By the way w5500 network chip handles this option
		 * */
		if(mbtcps->nbOfConns < MBTCPS_MAX_CONNS){
			/*
			 * Simply establish the connection
			 * TODO: to write code later for handling the connection establishment
			 *
			 * */
			MBTCPS_AddNewClient(mbtcps, client);
//			DBG_SPRINT_NL("TCPS clint conn");
		}else{
			/*
			 * Close the oldest, unused & no priority connection
			 * TODO: To implement at the end of this project.
			 * Write code for closing of oldest, unused, no priority connection
			 * Maybe no need to implement this section, because it can handle only
			 * 2 connections,
			 * */
		}
	}
	mbtcps->maxTrnsNo = MBTCPS_MAX_TRANS;
	DBG_SPRINT_NL("NumConns: %d", mbtcps->nbOfConns);
	DBG_SPRINT_NL("Socket-%d(TS): %s %s %d.%d.%d.%d:%d",
			mbtcps->clientIndex, STR_CONNECTED, STR_TO,
			mbtcps->clients[mbtcps->clientIndex].ip[0],
			mbtcps->clients[mbtcps->clientIndex].ip[1],
			mbtcps->clients[mbtcps->clientIndex].ip[2],
			mbtcps->clients[mbtcps->clientIndex].ip[3],
			mbtcps->clients[mbtcps->clientIndex].port);
}


/**
* @brief When a client is disconnected from the server it will remove this disconnected client
* @param mbtcps: pointer to the struct of Modbus TCP server
* @param id: disconnected client id
* @param port: disconnected client port
* @retval none
*/
void MBTCPS_DisconnHandler(MB_TCPServer *mbtcps, uint32_t id, uint16_t port){
	if(MBTCPS_FindClientByIdAndPort(mbtcps, id, port)==MB_OK){
		MBTCPS_ClearClientParams(&mbtcps->clients[mbtcps->clientIndex]);
		mbtcps->nbOfConns--;
		DBG_SPRINT_NL("NumConns: %d", mbtcps->nbOfConns);
		DBG_SPRINT_NL("Sock-%d: %d.%d.%d.%d:%d Disconnected", mbtcps->clientIndex,
				mbtcps->clients[mbtcps->clientIndex].ip[0],
				mbtcps->clients[mbtcps->clientIndex].ip[1],
				mbtcps->clients[mbtcps->clientIndex].ip[2],
				mbtcps->clients[mbtcps->clientIndex].ip[3],
				mbtcps->clients[mbtcps->clientIndex].port);

		mbtcps->clientIndex = 0;
		mbtcps->maxTrnsNo = MBTCPS_MAX_TRANS;
	}
}


/**
* @brief When a client is disconnected from the server it will remove this disconnected client
* @param mbtcps: pointer to the struct of Modbus TCP server
* @param id: disconnected client id
* @param port: disconnected client port
* @retval none
*/
void MBTCPS_RecvHandler(MB_TCPServer *mbtcps, uint32_t id, uint16_t port, uint8_t *buff, uint16_t size){

	if(MBTCPS_FindClientByIdAndPort(mbtcps, id, port)==MB_OK){
		MBTCPS_ClientParams *client;
		uint8_t clientIndex = mbtcps->clientIndex;
		client = &mbtcps->clients[clientIndex];
		memcpy(client->rxBuff, buff, size);
		client->rxBuffSize = size;
		MBTCPS_ParseMBAPHeader(client); 				// parse the MBAP Header
//		DBG_SPRINT_NL("TS RCH: %d ",mbtcps->curntTrnsNo);

//		DBG_SPRINT_NL("CurntTx: %d, txID: %d, proId: %d, Len: %d, uId: %d",
//				mbtcps->curntTrnsNo,
//				client->transId,
//				client->protocolId,
//				client->length,
//				client->unitId);
		/*Check the protocol id*/



		if(MBTCPS_CheckProtocolID(client->protocolId) == MB_OK){
//			DBG_SPRINT_NL("TS curntTrnsNo: %d, maxTrnsNo %d ",mbtcps->curntTrnsNo, mbtcps->maxTrnsNo);
			/*Check the number of transaction*/
			if(mbtcps->curntTrnsNo < MBTCPS_MAX_TRANS){//mbtcps->maxTrnsNo){											// check the number of server transection
				mbtcps->curntTrnsNo = 0;
//				MBMQ_Msg msg;
				// socket can received 2 packet together, but we have to handle 1 packet at a time. SO client->length+MBTCP_INDX_HEADER_MAX-1 is calculating actual packet siez
				uint16_t msgSize = client->length+MBTCP_INDX_HEADER_MAX-1;
				if(msgSize > MBTCP_BUFF_SIZE){
					msgSize = MBTCP_BUFF_SIZE;
				}
//				DBG_SPRINT_NL("TS msgSize: %d ",msgSize );
				MBMQ_setMsgParams(&mbtcps->msg, id, port, buff, msgSize);
//				if(MBMQ_enque(&mbtcps->msgQ, &msg) > 0){
//					mbtcps->curntTrnsNo++;
////					DBG_SPRINT_NL("CurntTx: %d ",mbtcps->curntTrnsNo);
//				}
//				DBG_SPRINT_NL("TS msg.size: %d ",mbtcps->msg.size );
			}else{																				// if no transaction is available
				client->excepCode = MB_EC_SDB_6;					// set the server busy exception code
				MBTCPS_BuldExcepResp(client);
				MBTCPS_SendExcepCallback(client);
			}

		}


//		mbtcps->clients[mbtcps->clientIndex].
	}
}


void MBTCPS_ReqHandler(MB_TCPServer *mbtcps, MB_Gateway *mbGw){

	MBMQ_Msg *msg = {0};
	msg = &mbtcps->msg;
	if(msg->size > 0){
//		if(MBMQ_GetQueMsg(&mbtcps->msgQ, &msg)){
//		DBG_SPRINT_NL("TS Rq msg->size: %d ",msg->size );
		/*
		 * 1. Parse the header
		 * 2. Parse the PDU
		 * */

		MBTCPS_ClientParams *client;
		uint8_t clientIndex =0;;
		MBTCP_Header header;
		uint8_t funCode = msg->buff[MBTCP_INDX_UNIT_ID+MB_FC_INDEX];
		uint8_t processStatus = 0u;

		//find the client according to the request, here id is ip & key is port
		if(MBTCPS_FindClientByIdAndPort(mbtcps, msg->id, msg->key) == MB_OK){
			clientIndex = mbtcps->clientIndex;
			client = &mbtcps->clients[clientIndex];
//			DBG_SPRINT_NL("yes client");
		}else{
//			DBG_SPRINT_NL("No client");
			return;
		}


		MBTCP_ParseHeader(msg->buff, &header);

//		DBG_SPRINT_NL("unitId: %d ",header.unitId );

		/*Check the request for the local or the remote device*/
		if(header.unitId == MBTCP_SERVER_UNIT_ID){	// if yes, the request for local device

//			DBG_SPRINT_NL("MTSrvr: msgSize: %d ",msg->size);

			MB_setReqBuff(&msg->buff[MBTCP_INDX_UNIT_ID], header.length);
			MB_setRespBuff(&client->txBuff[MBTCP_INDX_UNIT_ID]);
			if( MBTCP_isFunCodeValid(funCode)>0U){									// if the function code is valid

				processStatus = MB_executeRequest(funCode);						//  MB_executeRequest() will return 0x04 and 0xFF, 0x04 = exception occured and 0xFF = no exception

			}else{
				processStatus = MB_EC_ILF_1;												// exception code 1 for illegal function code
			}

			if (processStatus != 0u && funCode != MB_CFC_WMHRCO_109){
				if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){							// for exception response
					MB_excepRespBuilder(processStatus);					// processStatus = 255 means, no exception and the response has been built
				}
				// return 1 to send the response, if it is normal or exception response
				header.length =  MB_getRespBuffSize();
				MBTCP_SetHeader(client->txBuff,  &header);
				client->txBuffSize = header.length+MBTCP_INDX_HEADER_MAX-1;

				MBTCPS_SendCallback(client);
				mbGw->isRegularFC = 0;			// unlock the receive event for the server which is locked by the regular function code
			}

//			if(MBMQ_DeleteQueMsg(&mbtcps->msgQ)>0){
//				mbtcps->curntTrnsNo--;
//			}
			msg->size = 0;
//			DBG_SPRINT_NL("MTSrvr: msgSize: %d ",msg->size);
//			DBG_SPRINT_NL(" txID: %d, proId: %d, Len: %d, uId: %d",
//
//					header.transId,
//					header.protocolId,
//					header.length,
//					header.unitId);
		}else if(MB_checkSlaveAddress(header.unitId) > 0){	// if the request for the remote slave device

			/* 0. CHeck the function code
			 * 1. Check the serial port is not busy
			 * 2. convert TCP to RTU format
			 * 3. Send the packet
			 * 4. Start the timeout timer
			 * */
//			DBG_SPRINT_NL("TCPs: 1");


			if(!mbGw->isBusy){
//				mbGw->isBusy = 1;
				mbGw->isReqSent = 0;
				mbGw->clientId = msg->id;		// id is the ip
				mbGw->clientKey = msg->key;		// key is the port
				mbGw->tcpHeader = header;
				mbGw->txBuffSize = header.length;
//				mbGw->txBuffSize = MBG_ConvertTCP2RTU(mbGw->txBuff, msg.buff,msg.size);
//				DBG_SPRINT_NL("mbGw->txBuffSize: %d ",mbGw->txBuffSize);
				memcpy(mbGw->txBuff, &msg->buff[MBTCP_INDX_HEADER_MAX-1],mbGw->txBuffSize);
//				DBG_SPRINT_NL("mbG: 1");
				//delete msg from the que
//				if(MBMQ_DeleteQueMsg(&mbtcps->msgQ)>0){
//					mbtcps->curntTrnsNo--;
//				}
				msg->size = 0;

			}else{		// if slave id or unit id is not valid then remove the msg and do nothing
	//			if(MBMQ_DeleteQueMsg(&mbtcps->msgQ)>0){
	//				mbtcps->curntTrnsNo--;
	//			}
//				msg->size = 0;//TODO: commented at 19/3/2023
			}

		}

	}
}



/*
*	build response for the local when the device is using as modbus master
* */
void MBTCPS_BuildResp(MB_TCPServer *mbtcps){
	/*Response build in local */
//	if(header.unitId == MBTCP_SERVER_UNIT_ID){	// if yes, the request for local device
//
//	}
}

//
///*
//*	received response from the remote device when the device is using as modbus gateway
//* */
//void MBTCPS_RecvRespGW(MB_TCPServer *mbtcps){
//	/*Modbus TCP server is using the gateway. flow the steps to receive the response
//	 * 1. check if data is received
//	 * 2. find the client who actually own the response
//	 * 3. convert the packet RTU to TCP
//	 * 4. send the TCP packet as a response to the client
//	 * */
//	/*1. check if data is received*/
//	MBS_Serial *serial = {0};
//	serial = MBS_GetInstance();
//
//	if(serial->isDataReceived){
//		serial->isDataReceived = 0;
//		DBG_SPRINT_NL("isData: %d, gid: %ld, gkey: %d ",serial->isDataReceived,  MBG_GetTCPClientId(), MBG_GetTCPClientKey());
//
//
//		uint8_t slvAddr = serial->rxBuff[MB_SA_INDEX];
//		uint8_t funCode = serial->rxBuff[MB_FC_INDEX];
//		/* check the function code & slave address*/
//		// for the custom function code
//		if(slvAddr == MB_BROADCAST_ADDR && MB_CUST_CheckCFunCode(funCode)){
//			/* 0. reset the timer
//			 * 1. check the target slave  address
//			 * 2. put buffer to the cust msg que
//			 * 3. go for waiting to the next slave data
//			 * */
//			// 0. reset the timer
//			MBG_ResetTimeout();
//
//			 /* 1. check the slave address*/
//			uint8_t slvStartAdr = serial->rxBuff[MB_FC_INDEX+1];
//			MB_Gateway *gw;
//			gw = MBG_GetInstance();
//
//			if(slvStartAdr <= (gw->custom.slvStartAddr+gw->custom.numOfSlv)){
//				 /* 2. put buffer to the cust msg que*/
//				MBRS_CUST_EnqueMsg(serial->rxBuff, serial->rxBuffSize);
//
//				if(gw->custom.rcvdCount == gw->custom.numOfSlv){
//					MBTCPS_ClientParams *client;
//					uint8_t clientIndex = 0;
//					/* 2. find the client who actually own the response*/
//					if(MBTCPS_FindClientByIdAndPort(mbtcps, MBG_GetTCPClientId(), MBG_GetTCPClientKey()) != MB_OK){ return;}
//					clientIndex = mbtcps->clientIndex;
//					client = &mbtcps->clients[clientIndex];
//
//
//
//
//			//		DBG_SPRINT_NL("client found: %d ",clientIndex);
//			//		DBG_SPRINT_NL(" RTU Rcv %d: ", serial->rxBuffSize);
//			//		for(uint8_t i = 0; i < serial->rxBuffSize; i++){
//			//			DBG_SPRINT("%02X ",serial->rxBuffer[i]);
//			//		}
//					/*convert the packet RTU to TCP*/
//					client->txBuffSize = MBG_ConvertRTU2TCP(client->txBuff, serial->rxBuff,  serial->rxBuffSize);
//
//					MBG_GetTCPHeader()->length = client->txBuffSize;
//					MBTCP_SetHeader(client->txBuff, MBG_GetTCPHeader());		// set the tcp header to te tx buffer
//					client->txBuffSize = client->txBuffSize + MBTCP_INDX_HEADER_MAX-1;
//
//
//					serial->isBusy = 0;
//			//		DBG_SPRINT_NL(" clnt %d: ", client->txBuffSize);
//			//		for(uint8_t i = 0; i < client->txBuffSize; i++){
//			//			DBG_SPRINT("%02X ",client->txBuff[i]);
//			//		}
//					MBTCPS_SendCallback(client);
//					MBS_ClearRXBuffer(serial);
//					MBS_ClearTXBuffer(serial);
//				}
//
//				gw->custom.rcvdCount++;
//			}
//
//			 /* 3. go for waiting to the next slave data*/
//
//		}else{// for the regular function code
//			MBTCPS_ClientParams *client;
//			uint8_t clientIndex = 0;
//			/* 2. find the client who actually own the response*/
//			if(MBTCPS_FindClientByIdAndPort(mbtcps, MBG_GetTCPClientId(), MBG_GetTCPClientKey()) != MB_OK){ return;}
//			clientIndex = mbtcps->clientIndex;
//			client = &mbtcps->clients[clientIndex];
//
//
//
//
//	//		DBG_SPRINT_NL("client found: %d ",clientIndex);
//	//		DBG_SPRINT_NL(" RTU Rcv %d: ", serial->rxBuffSize);
//	//		for(uint8_t i = 0; i < serial->rxBuffSize; i++){
//	//			DBG_SPRINT("%02X ",serial->rxBuffer[i]);
//	//		}
//			/*convert the packet RTU to TCP*/
//			client->txBuffSize = MBG_ConvertRTU2TCP(client->txBuff, serial->rxBuff,  serial->rxBuffSize);
//
//			MBG_GetTCPHeader()->length = client->txBuffSize;
//			MBTCP_SetHeader(client->txBuff, MBG_GetTCPHeader());		// set the tcp header to te tx buffer
//			client->txBuffSize = client->txBuffSize + MBTCP_INDX_HEADER_MAX-1;
//
//
//			serial->isBusy = 0;
//	//		DBG_SPRINT_NL(" clnt %d: ", client->txBuffSize);
//	//		for(uint8_t i = 0; i < client->txBuffSize; i++){
//	//			DBG_SPRINT("%02X ",client->txBuff[i]);
//	//		}
//			MBTCPS_SendCallback(client);
//			MBS_ClearRXBuffer(serial);
//			MBS_ClearTXBuffer(serial);
//	//		if(MBMQ_DeleteQueMsg(&mbtcps->msgQ)>0){
//	//			mbtcps->curntTrnsNo--;
//	//		}
//	//		DBG_SPRINT_NL("noOfMsg: %d ",mbtcps->msgQ.noOfMsg);
//		}
//	}
//}



/*
 * Build exception
 * */
void MBTCPS_BuldExcepResp(MBTCPS_ClientParams *client){

	/*Copy the header from rx buffer to tx buffer*/
	client->txBuffSize = MBTCP_PDU_START_INDEX;
	client->txBuffSize = 0;
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_TRANS_ID_HI];
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_TRANS_ID_LO];
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_PROTOCOL_ID_HI];
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_PROTOCOL_ID_LO];
	client->txBuff[client->txBuffSize++] = 0;		// high byte of length
	client->txBuff[client->txBuffSize++] = 3;		// low byte of length
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_UNIT_ID];
	client->txBuff[client->txBuffSize++] = client->rxBuff[MBTCP_INDX_UNIT_ID+MB_FC_INDEX] + MB_EFC_OFFSET ;		// set function code
	client->txBuff[client->txBuffSize++] = client->excepCode;		// set the exception code

}



/*
*
* */
void MBTCPS_ExcepHandler(MB_TCPServer *mbtcps){

}


/**
* @brief adds new client
* @param mbtcps: pointer to the struct of modbus tcp server
* @retval none
*/
void MBTCPS_AddNewClient(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client){
	// fiend the empty slot for the new client
	for(uint8_t i = 0; i < MBTCPS_MAX_CONNS; i++){
		if(mbtcps->clients[i].id == 0){
			mbtcps->clientIndex = i;
		}
	}
	mbtcps->clients[mbtcps->clientIndex] = *client;
	mbtcps->nbOfConns++;

}
/*
* @brief update the old client if the client reconnects with the same IP
* @param mbtcps: pointer to the struct of modbus tcp server
* @retval none
*/

void MBTCPS_UpdateClient(MB_TCPServer *mbtcps, MBTCPS_ClientParams *client){
	mbtcps->clients[mbtcps->clientIndex].port = client->port;
}

/**
* @brief remove not connected client
* @param mbtcps: pointer to the struct of modbus tcp server
* @retval none
*/
void MBTCPS_RemoveClosedClient(MB_TCPServer *mbtcps){
	for(uint8_t i = 0 ; i < MBTCPS_MAX_CONNS; i++){
		if(mbtcps->clients[i].isConnected == 0){
			MBTCPS_ClearClientParams(&mbtcps->clients[i]);
		}
	}
}

/**
* @brief find the client from connected client list by id address
* @param mbtcps: pointer to the struct of modbus tcp server
* @param client: pointer to the struct of client
* @retval returns error status, 0 = not found, 1 = found
*/
uint8_t MBTCPS_FindClientById(MB_TCPServer *mbtcps,  uint32_t id){
	for(uint8_t i = 0; i < MBTCPS_MAX_CONNS; i++){
		if(mbtcps->clients[i].id == id){
			mbtcps->clientIndex = i;
			return MB_OK;
		}
	}
	return MB_ERROR;
}

/**
* @brief find the client from connected client list by port
* @param mbtcps: pointer to the struct of modbus tcp server
* @param client: pointer to the struct of client
* @retval returns error status, 0 = not found, 1 = found
*/
uint8_t MBTCPS_FindClientByPort(MB_TCPServer *mbtcps,  uint32_t port){
	for(uint8_t i = 0; i < MBTCPS_MAX_CONNS; i++){
		if(mbtcps->clients[i].port == port){
			mbtcps->clientIndex = i;
			return MB_OK;
		}
	}
	return MB_ERROR;
}


/**
* @brief find the client from connected client list by id & port
* @param mbtcps: pointer to the struct of modbus tcp server
* @param client: pointer to the struct of client
* @retval returns error status, 0 = not found, 1 = found
*/
uint8_t MBTCPS_FindClientByIdAndPort(MB_TCPServer *mbtcps, uint32_t id,  uint32_t port){
	for(uint8_t i = 0; i < MBTCPS_MAX_CONNS; i++){
		if((mbtcps->clients[i].id == id) && (mbtcps->clients[i].port == port)){
			mbtcps->clientIndex = i;
			return MB_OK;
		}
	}
	return MB_ERROR;
}




/**
* @brief parse the MBAP header
* @param mbtcps: pointer to the struct of modbus tcp server
* @param buff: pointer to ADU buffer
* @retval returns error status, 0 = MB_ERROR, 1 = MB_OK
*/
void MBTCPS_ParseMBAPHeader(MBTCPS_ClientParams *client){
	// parse the MBAP header
	client->transId = MBTOOL_CombBytesToU16(client->rxBuff[MBTCP_INDX_TRANS_ID_HI], client->rxBuff[MBTCP_INDX_TRANS_ID_LO]);
	client->protocolId = MBTOOL_CombBytesToU16(client->rxBuff[MBTCP_INDX_PROTOCOL_ID_HI], client->rxBuff[MBTCP_INDX_PROTOCOL_ID_LO]);
	client->length = MBTOOL_CombBytesToU16(client->rxBuff[MBTCP_INDX_LENGTH_HI], client->rxBuff[MBTCP_INDX_LENGTH_LO]);
	client->unitId = client->rxBuff[MBTCP_INDX_UNIT_ID];
}

/**
* @brief process the modbus application header. It just checks the protocol id first if not matched,
* it will discord the request. otherwise proceed to the next step
* @param mbtcps: pointer to the struct of modbus tcp server
* @param buff: pointer to ADU buffer
* @retval returns error status, 0 = MB_ERROR, 1 = MB_OK
*/
//uint8_t MBTCPS_ProcessMBAPHeader(MB_TCPServer *mbtcps){
//	MBTCPS_ParseMBAPHeader(&mbtcps->clients[mbtcps->clientIndex]);
//	if(MBTCPS_CheckProtocolID(mbtcps->clients[mbtcps->clientIndex].protocolId) == MB_OK){
//		if(mbtcps->curntTrnsNo < mbtcps->maxTrnsNo){
//			MBMQ_Msg msg;
//			msg.id =  mbtcps->clients[mbtcps->clientIndex].id;
//			msg.key =  mbtcps->clients[mbtcps->clientIndex].port;
//			msg.size = mbtcps->clients[mbtcps->clientIndex].rxBuffSize;
//			memcpy(msg.buff,mbtcps->clients[mbtcps->clientIndex].rxBuff, mbtcps->clients[mbtcps->clientIndex].rxBuffSize);
//
//			MBMQ_enque(&mbtcps->msgQ, &msg);
//		}else{																				// if no transaction is available
//			mbtcps->clients[mbtcps->clientIndex].excepCode = MB_EC_SDB_6;					// set the server busy exception code
//		}
//		return MB_OK;
//	}else{
//		return MB_ERROR;
//	}
//}
//

void MBTCP_ExcepRespBuilder(MBTCPS_ClientParams *mbtcpClient){
	MB_excepRespBuilder(mbtcpClient->excepCode);
}



/**
* @brief checks the protocol ID
* @param protocolId: modbus tcp protocol id = 0x0000
* @retval returns error status, 0 = MB_ERROR, 1 = MB_OK
*/
uint8_t MBTCPS_CheckProtocolID(uint16_t protocolId){
	return (protocolId == MBTCP_PROTOCOL_ID ? MB_OK : MB_ERROR);
}


void MBTCPS_SetMBAPHeaderToTXBuff(MB_TCPServer *mbtcps, uint8_t clientIndex){

	memcpy(mbtcps->clients[clientIndex].txBuff, mbtcps->clients[clientIndex].rxBuff, 4);
}



void MBTCPS_Send(MBTCPS_ClientParams *client){
	MBTCPS_SendCallback(client);
}


void MBTCPS_AttachSendCallback(uint8_t (*callbackFunc)(MBTCPS_ClientParams *client)){
	MBTCPS_SendCallback = callbackFunc;
}

void MBTCPS_AttachSendExcpCallback(uint8_t (*callbackFunc)(MBTCPS_ClientParams *client)){
	MBTCPS_SendExcepCallback= callbackFunc;
}

