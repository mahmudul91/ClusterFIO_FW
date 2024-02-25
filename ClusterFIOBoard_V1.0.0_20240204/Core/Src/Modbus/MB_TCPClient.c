/*
 * MB_TCPClient.c
 *
 *  Created on: Feb 5, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "MB_TCPClient.h"
//#include "MB_TCP.h"
#include "MB_Config.h"
#include "MB_Tools.h"
#include "MB_Custom.h"
#include "Debug.h"
#include "math.h"

static MBMQ_Msg msgTemp;

void MBTCPC_Init(MB_TCPClient *client){
	MBMQ_clear(&client->msgQ);
	MBMQ_clearMsg(&msgTemp);
	MBTCPC_ClearPendingList(client->pendingList);
}


/**
* @brief build the Modbus request to be send to the serve
* @param client: modbus client struct pointer
* @param buff: pointer of a buffer consist unitId+PDU. and the buffer must not contain MBAP header except unitId
* @param size: buffer size
* @retval returns MBTCP_Status
*/
uint8_t MBTCPC_BuildRequest(MB_TCPClient *client, uint8_t *buff, uint16_t size){

	/*
	 * 1. check the transaction is available or not
	 * 2. if not, generate a negative confirmation
	 * 3. if yes,
	 * 	  a) Initialize the transaction & add to the pending list
	 * 	  b) Encode the MB request PDU
	 * 	  c) Encode the MBAP header
	 * 	  d) enqueue the request in msg queue
	 * */
//	DBG_SPRINT_NL("curntTrnsNo: %d ",client->curntTrnsNo);
	/*1. checking the transaction available or not*/
	if(client->curntTrnsNo < MBTCPC_MAX_TRANS){  //3. if yes, build the request

		if((size+MBTCP_INDX_HEADER_MAX-1) >= MBTCP_BUFF_SIZE){ return MBTCP_STATUS_INVALID_LENGTH;}

		/* a) Initialize the transaction & add to the pending list*/

		MBTCPC_TransInfo transInfo = {0};
		client->header.transId ++;		// TODO: to uncomment
		transInfo.transId = client->header.transId;
		transInfo.unitId = client->header.unitId;
		transInfo.funCode = buff[MB_FC_INDEX];
		if((transInfo.funCode >= MB_FC_RC_01 && transInfo.funCode <= MB_FC_RIR_04)
				|| (transInfo.funCode >= MB_FC_WMC_15 && transInfo.funCode <= MB_FC_WMR_16)
				|| (transInfo.funCode >= MB_CFC_RC_101 && transInfo.funCode <= MB_CFC_WMR_106)){
			transInfo.startAddr = MBTOOL_CombBytesToU16(buff[MB_FC_INDEX+1], buff[MB_FC_INDEX+2]);
			transInfo.quantity = MBTOOL_CombBytesToU16(buff[MB_FC_INDEX+3], buff[MB_FC_INDEX+4]);

			if((transInfo.funCode >= MB_FC_WMC_15 && transInfo.funCode <= MB_FC_WMR_16)
					|| (transInfo.funCode >= MB_CFC_WMC_105 && transInfo.funCode <= MB_CFC_WMR_106)){
				transInfo.byteCount = buff[MB_FC_INDEX+5];
			}

		}
		if((transInfo.funCode >= MB_FC_WSC_05 && transInfo.funCode <= MB_FC_WSR_06)
				|| (transInfo.funCode >= MB_CFC_WMC_105 && transInfo.funCode <= MB_CFC_WMR_106)){
			transInfo.startAddr = MBTOOL_CombBytesToU16(buff[MB_FC_INDEX+1], buff[MB_FC_INDEX+2]);
		}
		if((transInfo.funCode >= MB_CFC_RIRDI_107 && transInfo.funCode <= MB_CFC_WMHRCO_109)){
			transInfo.slvStartAddr = buff[MB_FC_INDEX+1];
			transInfo.numOfSlv = buff[MB_FC_INDEX+2];
		}


		transInfo.lastRetryTime = 0;
		transInfo.retryCounter = 0;
//
//		DBG_SPRINT_NL("buff: %d: ",size);
//		for(uint8_t i = 0; i < size;  i++){
//			DBG_SPRINT("%02X ",buff[i] );
//		}
		/*b) Encode the MB request PDU
		 * unit id will be added by the Add header function
		 * */
//		memset(transInfo.txBuff, 0, MBTCP_BUFF_SIZE);
		memcpy(&transInfo.txBuff[MBTCP_INDX_HEADER_MAX], &buff[1], size);


		/*c) Encode the MBAP header*/
		client->header.length = size;
		transInfo.txBuffSize = size+MBTCP_INDX_HEADER_MAX-1;  // 1 is subtracted, because unitId is in the buffer as well as in the header so we need to subtract
		MBTCP_AddHeader(transInfo.txBuff, &client->header);

//		DBG_SPRINT_NL("transInfo: %d: ",transInfo.txBuffSize);
//		for(uint8_t i = 0; i < transInfo.txBuffSize;  i++){
//			DBG_SPRINT("%02X ",transInfo.txBuff[i] );
//		}

		/* d) add to the pending list*/
		MBTCPC_AddToTxPendingList(client->pendingList, transInfo);		// add to the pending list
		client->curntTrnsNo++;
		return MBTCP_STATUS_OK;

	}else{ 			//2. if  transaction is not available , generate a negative confirmation
		//TODO: to write code generate a negative confirmation
		return MBTCP_STATUS_EXCED_MAX_TRANS;
	}


}

/**
* @brief check the response returned from the server
* */
uint8_t MBTCPC_ProcessResponse(MB_TCPClient *client, uint8_t *buff, uint16_t size){
	/*
	 * 0. parse the header
	 * 1. Check transId in the transacting pending list if not found just discard the response, if yes, follow the steps
	 * 2. Check the protocol id, if not matched return a negative Confirmation, if yes, go next
	 * 3. check the length of the response buffer
	 * 4. check the unitId, If received from the direct server it should be 0xFF, if from the gateway unitId may be different
	 * 5. Check the function code && response format, if ok, return a positive Confirmation.
	 *    If the function code is a MODBUS exception code (Function code + 80H), returns a Positive Confirmation
	 *    if not ok, return a negative Confirmation
	 * */
DBG_SPRINT_NL("process ok 1");
	if(client->curntTrnsNo > 0 && size > 0){

		MBTCP_Header header;
		MBTCPC_TransInfo *transInfoPending = NULL;
//		transInfoPending = NULL;
//		MBTCPC_ClearTransInfo(transInfoPending);

		/*0. parse the header*/
		MBTCP_ParseHeader(buff, &header);
//		MBTCPC_GetFromTxPendingList(&client->pendingList, transInfo, header.transId);
		/*parse the transaction info from response*/
//		transInfoRcvd.transId = header.transId;
//		transInfoRcvd.unitId = header.unitId;

		uint8_t funCodeRcvd = buff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX];


		/*1. parse & Check transId in the transaction pending list*/
		if( !(MBTCPC_GetFromTxPendingList(client->pendingList, transInfoPending, header.transId) >= 0)){
			return MBTCP_STATUS_ERROR;
		}
		memcpy(transInfoPending->rxBuff, buff, size);


		/*2. Check the protocol id,*/
		if(header.protocolId != MBTCP_PROTOCOL_ID){
			return MBTCP_STATUS_INVALID_PROTOCOL_ID;
		}

		/*3. check the length of the response buffer*/
		if(header.length != (size-MBTCP_INDX_LENGTH_LO+1)){
			return MBTCP_STATUS_INVALID_LENGTH;
		}

		/*4. check the unitId, */
		if((header.unitId != transInfoPending->unitId) && (transInfoPending->unitId > MB_SLAVE_ADDR_MAX && transInfoPending->unitId < MBTCP_SERVER_UNIT_ID)){
			return MBTCP_STATUS_INVALID_UNIT_ID;
		}

		/*5.
		 * Check the function code*/
		if(funCodeRcvd == transInfoPending->funCode){
			// parse && check the information from the response
			if((funCodeRcvd >= MB_FC_RC_01 && funCodeRcvd <= MB_FC_RIR_04)
					|| (funCodeRcvd >= MB_CFC_RC_101 && funCodeRcvd <= MB_CFC_RIR_104)){

				uint8_t byteCount = transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+1];
				// check
				if(byteCount != transInfoPending->byteCount){
					return MBTCP_STATUS_INVALID_RESPONSE;
				}

			}
			if((funCodeRcvd >= MB_FC_WSC_05 && funCodeRcvd <= MB_FC_WSR_06)
					|| (funCodeRcvd >= MB_FC_WMC_15 && funCodeRcvd <= MB_FC_WMR_16)
					|| (funCodeRcvd >= MB_CFC_WMC_105 && funCodeRcvd <= MB_CFC_WMR_106)){

				uint16_t startAddr = MBTOOL_CombBytesToU16(transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+1], transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+2]);
				if(startAddr != transInfoPending->startAddr){
					return MBTCP_STATUS_INVALID_RESPONSE;
				}

				if((funCodeRcvd >= MB_FC_WMC_15 && funCodeRcvd <= MB_FC_WMR_16)
						|| (funCodeRcvd >= MB_CFC_WMC_105 && funCodeRcvd <= MB_CFC_WMR_106)){
					uint16_t quantity = MBTOOL_CombBytesToU16(transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+3], transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+4]);
					if(quantity != transInfoPending->quantity){
							return MBTCP_STATUS_INVALID_RESPONSE;
						}
				}
			}
			if((funCodeRcvd >= MB_CFC_RIRDI_107 && funCodeRcvd <= MB_CFC_WMHRCO_109)){
				uint8_t slvStartAddr = transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+1];
				uint8_t numOfSlv = transInfoPending->rxBuff[MBTCP_INDX_HEADER_MAX+MB_FC_INDEX+2];
				if(slvStartAddr != transInfoPending->slvStartAddr || numOfSlv != transInfoPending->numOfSlv ){
						return MBTCP_STATUS_INVALID_RESPONSE;
				}
			}


			MBTCPC_RemoveFromTxPendingList(client->pendingList, transInfoPending->index);
				client->curntTrnsNo--;
			return MBTCP_STATUS_OK;

		}else if(funCodeRcvd == (transInfoPending->funCode+MB_EFC_OFFSET)){		// for error response
			MBTCPC_RemoveFromTxPendingList(client->pendingList, transInfoPending->index);
			return MBTCP_STATUS_OK;
		}else{
			return MBTCP_STATUS_INVALID_FUNC_CODE;
		}


	}else{
		return MBTCP_STATUS_INVALID_LENGTH;
	}

}





/**
* @brief check the request & response
* */
uint8_t MBTCPC_CheckResp(MBTCPC_TransInfo *respTransInf, MBTCPC_TransInfo *reqTransInf){
	/*check the for the read coils & discrete inputs*/
	if((respTransInf->funCode >= MB_FC_RC_01 && respTransInf->funCode <= MB_FC_RDI_02)
			|| (respTransInf->funCode >= MB_CFC_RC_101 && respTransInf->funCode <= MB_CFC_RDI_102)){
//		uint8_t byteCount = (reqTransInf->quantity/8)+((reqTransInf->quantity%8)>0);
		return (respTransInf->byteCount == ((reqTransInf->quantity/8)+((reqTransInf->quantity%8)>0)));

	}else if((respTransInf->funCode >= MB_FC_RHR_03 && respTransInf->funCode <= MB_FC_RIR_04)
			|| (respTransInf->funCode >= MB_CFC_RHR_103 && respTransInf->funCode <= MB_CFC_RIR_104)){
		return (respTransInf->byteCount == reqTransInf->quantity*2);

	}else if(respTransInf->funCode >= MB_FC_WSC_05 && respTransInf->funCode <= MB_FC_WSR_06){
		return (respTransInf->startAddr == reqTransInf->startAddr);

	}else if((respTransInf->funCode >= MB_FC_WMC_15 && respTransInf->funCode <= MB_FC_WMR_16)
			|| (respTransInf->funCode >= MB_CFC_WMC_105 && respTransInf->funCode <= MB_CFC_WMR_106)){
		return (respTransInf->startAddr == reqTransInf->startAddr) &&  (respTransInf->quantity == reqTransInf->quantity);

	}else if(respTransInf->funCode >= MB_CFC_RIRDI_107 && respTransInf->funCode <= MB_CFC_WMHRCO_109){
		return (respTransInf->slvStartAddr == reqTransInf->slvStartAddr) &&  (respTransInf->numOfSlv == reqTransInf->numOfSlv);
	}
	return 0;
}





/**
* @brief add new transaction into the list
* */
void MBTCPC_ClearPendingList(MBTCPC_TransInfo *list){
    for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
    	MBTCPC_ClearTransInfo(&list[i]);
    }
}

/**
* @brief add new transaction into the list
* */
void MBTCPC_ClearTransInfo(MBTCPC_TransInfo *info){

	info->isAvailable = 0;
	info->transId = 0;
	info->unitId = 0;
	info->funCode = 0;
	info->startAddr = 0;
	info->quantity = 0;
	info->slvStartAddr = 0;
	info->numOfSlv = 0;
	info->byteCount = 0;		// quantity of  registers, coils & discrete inputs
	info->lastRetryTime = 0;
	info->retryTimeout = 0;
	info->retryCounter = 0;
	info->retryCount = 0;
	memset(info->txBuff, 0, MBTCP_BUFF_SIZE);
	memset(info->rxBuff, 0, MBTCP_BUFF_SIZE);
	info->txBuffSize = 0;
	info->rxBuffSize = 0;

}



/**
* @brief check is there any pending transaction
* @retval error status or index of the pending transaction
*  >= 0 , index of the pending transaction
* -1 = MBTCP_ERROR_NO_TRANS
* */
int8_t MBTCPC_CheckPendingTrans(MB_TCPClient *client){
	MBTCPC_TransInfo *transInfo;
	if(client->curntTrnsNo > 0){
		for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
			if(client->pendingList[i].transId != 0){

				transInfo = &client->pendingList[i];
				/* 1. check the timeout timer
				 * 2. check the retry counter
				 * */
				if((uint16_t)fabs(HAL_GetTick()-transInfo->lastRetryTime) > transInfo->retryTimeout){
					transInfo->lastRetryTime = HAL_GetTick();
					if(transInfo->retryCounter <= transInfo->retryCount){
						transInfo->retryCounter++;
						transInfo->retryStatus = MBTCP_STATUS_OK;
						return i;
					}else{
						transInfo->retryStatus = MBTCP_STATUS_EXCED_MAX_RETRY;
					}
					transInfo->retryStatus = MBTCP_STATUS_RESP_TIMEOUT;
				}

			}
		}
	}
//	transInfo->retryStatus = MBTCP_STATUS_NO_TRANS;
	return -1;

}

/**
* @brief add new transaction into the list
* */
void MBTCPC_AddToTxPendingList(MBTCPC_TransInfo *list, MBTCPC_TransInfo item){
    for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
        if(list[i].isAvailable <= 0U){
        	item.isAvailable = 1;
        	item.index = i;
            list[i] = item;
            break;
        }
    }
}





/**
* @brief get  transaction from the list
* */
int8_t MBTCPC_GetFromTxPendingList(MBTCPC_TransInfo *list, MBTCPC_TransInfo *item, uint16_t transId){
    for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
        if(list[i].transId == transId){
            *item = list[i];
            return i;
        }
    }
    return -1;
}

/**
* @brief get  transaction from the list
* */
int8_t MBTCPC_GetIndexOfPendingTx(MBTCPC_TransInfo *list, uint16_t transId){
    for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
        if(list[i].transId == transId){
            return i;
        }
    }
    return -1;
}

/**
* @brief check transaction id is in the list
* @retval if not in the list return -1, if inthe list return index
* */
int8_t MBTCPC_CheckInTxPendingList(MBTCPC_TransInfo *list, uint16_t transId){
    for(uint8_t i = 0; i < MBTCPC_MAX_TRANS; i++){
        if(list[i].transId == transId){
            return i;
        }
    }
    return -1;
}



/**
* @brief remove  transaction from the list
* */

void MBTCPC_RemoveFromTxPendingList(MBTCPC_TransInfo *list, uint8_t index){

	if(index >= 0 && index < MBTCPC_MAX_TRANS){
		MBTCPC_ClearTransInfo(&list[index]);
	}

}
