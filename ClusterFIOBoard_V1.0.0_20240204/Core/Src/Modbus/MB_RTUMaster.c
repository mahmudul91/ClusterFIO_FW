/*
 * MB_RTUMaster.c
 *
 *  Created on: Jan 20, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "MB_RTUMaster.h"
#include "MB_Serial.h"
#include "MB_CRC.h"
#include "Debug.h"
#include "MB_Gateway.h"
#include "TimeStamp.h"

extern TimeStamp timStamp;

static MBS_Serial *serial;
static MB_RTUMaster *mbMstr;


/* initiates modbus master struct
 * */
void MBRM_Init(MB_RTUMaster *master){
	mbMstr = master;
	mbMstr->isPendingRqst = 0;
	mbMstr->isBusy = 0;
	mbMstr->retryCount = 0;
	mbMstr->replyTimer.enableTimer = 0;
	mbMstr->replyTimer.isTimeout = 0;
	mbMstr->replyTimer.timer = 0;
	mbMstr->state = 0;
	mbMstr->status = 0;
	memset(mbMstr->rxBuff, 0, MB_PDU_MAX_SIZE);
	memset(mbMstr->txBuff, 0, MB_PDU_MAX_SIZE);
	mbMstr->rxBuffSize = 0;
	mbMstr->txBuffSize = 0;

	serial = MBS_GetInstance();
}


/* gets the master instance
 * */
MB_RTUMaster *MBRM_GetInstance(void){
	return mbMstr;
}


/* sets the master instance
 *
 */
void MBRM_SetInstance(MB_RTUMaster *master){
	mbMstr = master;
}


/*Binds serial to the Modbus master
 * */
void MBRM_BindSerial(MBS_Serial *serial_){
	serial = serial_;
}


/*unbinds serial to the Modbus master
 * */
void MBRM_UnbindSerial(void){
	serial = NULL;
}


void MBRM_BuildReq(MB_RTUMaster *master){

}


/**
  * @brief processes received data packet and response according to the command
  *
  * @param none
  * @retval None
  */
void MBRM_Execute(MB_RTUMaster *master){

	switch (master->state) {
		case MBRM_STATE_IDLE:
			/* 1. check the pending request is available or not
			 * 2. if yes, initiate to send the request
			 * 3. start the response timeout timer
			 * 4. change the state flag to the waiting replay state
			 * */
			if(master->isPendingRqst > 0){

				master->retryCount = 1;
				master->status = MBRM_STATUS_BUSY;

//				DEBUG_SPRINT_NL("srlBusy: %d: ",serial->isBusy);
				//TODO: ADD-20230902, ------------
				if((serial->protocol == MBS_PROTO_RTU_STD || serial->protocol == MBS_PROTO_RTU_CLUSTER) && (serial->isSending || serial->isReceiving)) return;		// if serial line is busy to send or receive with another request


				if(serial->isBusy) return;		// if serial line is busy with another request
				serial->mode = MBS_MODE_MASTER;
				serial->isBusy = 1;

				// copy the bufffer to the serial buffer
				memcpy(serial->txBuff, master->txBuff, master->txBuffSize);
				serial->txBuffSize = master->txBuffSize;
//				DEBUG_SPRINT_NL("MBM Snt: %d: ",serial->txBuffSize);
				if(MBS_Send(serial) == MB_OK){								// send the request to the slave

					MB_CUST_ResetTimer(&master->custom.reTransTimer);
						MBRM_StartReplyTimer(&master->replyTimer);
						master->isPendingRqst--;						// decrease the pending request

				}else{
					serial->isBusy = 0;
				}
			}else{
				master->status = MBRM_STATUS_OK;
			}
			break;
		case MBRM_STATE_WAITING_REPLY:

			/* 1. waiting for the response
			 * 2. if reply from unexpected slave time out timer is kept running
			 * 3. if an error in the receive frame, retry may be performed
			 * 4. if no reply is received, the Response time-out expires, and an error is generated
			 * 5. go to the Processing error state
			 * */

			if(serial->isDataReceived > 0U){					// if data packet is received
				serial->isDataReceived = 0U;
//				serial->isBusy = 0;
				if(!MBRM_IsUnexpectedSlave()){					// if the packet received from the target slave
					if(MB_CRCCheck(serial->rxBuff, serial->rxBuffSize)){ 		// check the error with the CRC
						master->replyTimer.enableTimer = 0u;			// disable timer
						master->state = MBRM_STATE_PROCESSING_REPLY;		// change the state
					}else{
						master->status = MBRM_STATUS_FRAME_ERROR;
						/*Retry if frame error*/
						if(master->retryCount < master->retryMax){
							master->retryCount++;
							master->replyTimer.timer = TS_GetUS(&timStamp);			// update the time
//							if(serial->isBusy) return;		// if serial line is busy with another request
							serial->mode = MBS_MODE_MASTER;
							serial->isBusy = 1;
							MBS_Send(serial);
						}else{
							master->state = MBRM_STATE_PROCESSING_REPLY;		// change the state

						}
					}
				}else{								// if packet is received from unexpected slave, keep waiting for the target slave response
					master->status = MBRM_STATUS_TIMEOUT_ERROR;
					MBS_ClearRXBuffer(serial);		// clear receive buffer
				}
			}else if(master->replyTimer.isTimeout > 0u){	// if time out
				master->status = MBRM_STATUS_TIMEOUT_ERROR;
				if(master->retryCount < master->retryMax){
					master->retryCount++;
					master->replyTimer.timer = TS_GetUS(&timStamp);			// update the time

//					if(serial->isBusy) return;		// if serial line is busy with another request
					serial->mode = MBS_MODE_MASTER;
					serial->isBusy = 1;
					MBS_Send(serial);
				}else{
					master->state = MBRM_STATE_PROCESSING_ERROR;		// change the state
				}
			}

			break;
		case MBRM_STATE_WAITING_TURNAROUND_DELAY:

			if(MBRM_GetSlaveAddress() == MB_BROADCAST_ADDR && MBRM_GetFunCode() != MB_CFC_RIRDI_107){
				if(master->replyTimer.isTimeout > 0) {
					master->state = MBRM_STATE_IDLE;
					master->replyTimer.isTimeout = 0;
				}
			}

			break;
		case MBRM_STATE_PROCESSING_REPLY:
			//TODO: to implement processing the response

			break;
		case MBRM_STATE_PROCESSING_ERROR:
			master->state = MBRM_STATE_IDLE;
			memset(master->txBuff, 0, master->txBuffSize);
			memset(master->rxBuff, 0, master->rxBuffSize);
			master->txBuffSize = 0;
			master->rxBuffSize = 0;
			MBS_ClearTXBuffer(serial);
			MBS_ClearRXBuffer(serial);
			// TODO: to implement processing error later
			break;
		default:
			break;
	}




}


void MBRM_StartReplyTimer(MBRM_ReplyTimer *replyTimer){
	replyTimer->enableTimer = 1;
	replyTimer->timer = TS_GetUS(&timStamp);			// update the time
}


void MBRM_StopReplyTimer(MBRM_ReplyTimer *replyTimer){
	replyTimer->enableTimer = 0;
}


void MBRM_RestReplyTimer(MBRM_ReplyTimer *replyTimer){
	replyTimer->timer = TS_GetUS(&timStamp);			// update the time

}


/**
  * @brief counts time in every ms, have to execute this function in ms or in faster
  *
  * @param none
  * @retval None
  */
uint8_t MBRM_IsReplyTimeout(MBRM_ReplyTimer *replyTimer){

	if(!replyTimer->enableTimer){ return 0;}
	if((uint64_t)fabsl((long double)(TS_GetUS(&timStamp) - replyTimer->timer)) >= (uint64_t)(replyTimer->timeout)){
//			replyTimer->enableTimer = 0U;
		replyTimer->isTimeout = 1U;					// set the timeout flag
//			replyTimer->timer = HAL_GetTick();			// update the time
		return 1;
	}else{
		replyTimer->isTimeout = 0U;
	}

 return 0;
}


/**
  * @brief if the receviced packet from unexpected slave, returns 1 else 0
  *
  * @param none
  * @retval 1 = unexpected slave, 0 = target slave
  */
uint8_t MBRM_IsUnexpectedSlave(void){
	return (serial->txBuff[MB_SA_INDEX] != serial->rxBuff[MB_SA_INDEX]);
}


uint8_t MBRM_GetSlaveAddress(void){
	return serial->txBuff[MB_SA_INDEX];
}


uint8_t MBRM_GetFunCode(void){
	return serial->txBuff[MB_FC_INDEX];
}


/*
 * @brief Appends data to tx buffer and auto increase data length in the buffer
  * @param data: dat to be append
  * @retval it returns length of the buffer
 * */
uint16_t MBRM_AppendToTxBuff(uint8_t data){
	mbMstr->txBuff[mbMstr->txBuffSize++] = data;
	return mbMstr->txBuffSize;
}
