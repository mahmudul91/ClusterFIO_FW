/*
 * MB_RTUMaster.h
 *
 *  Created on: Jan 20, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_RTUMASTER_H_
#define SRC_MODBUS_MB_RTUMASTER_H_
#include "main.h"
#include "MB_Config.h"
#include "MB_Custom.h"
#include "MB_Serial.h"
/*
 * States for the Modbus RTU master
 * */
typedef enum MBRM_States{
	MBRM_STATE_IDLE = 0,					//State "Idle" = no pending request. This is the initial state after power-up.
	MBRM_STATE_WAITING_REPLY,				//When a unicast request is sent to a slave, the master goes into "Waiting for reply" state, and a “Response Time-out” is started.
	MBRM_STATE_WAITING_TURNAROUND_DELAY,	//When a broadcast request is sent on the serial bus, no response is returned from the slaves. Nevertheless a delay is respected by the Master in order to allow any slave to process the current request before sending a new one. This delay is called "Turnaround delay"
	MBRM_STATE_PROCESSING_REPLY,			//When received the reply it goes to the processing reply state
	MBRM_STATE_PROCESSING_ERROR,			//If no reply is received, the Response time-out expires, and an error is generated.
}MBRM_States;

typedef enum MBRM_Status{
	MBRM_STATUS_OK = 0,
	MBRM_STATUS_BUSY,
	MBRM_STATUS_TIMEOUT_ERROR,
	MBRM_STATUS_FRAME_ERROR,
}MBRM_Status;

typedef struct MBRM_ReplyTimer{
	uint8_t enableTimer;		// 1 = enable, 0 = disable
	uint64_t timer;				// count time in ms
	uint32_t timeout;			// timeout period in ms
	uint8_t isTimeout;			// time out flag

}MBRM_ReplyTimer;

typedef struct MB_RTUMaster{
	uint8_t enable;
	MBRM_States state;						// states of the modbus RTU master
	MBRM_Status status;
	MBRM_ReplyTimer replyTimer;
	uint8_t retryMax;
	uint8_t retryCount;
	uint8_t isPendingRqst;					// 0 = no pending request, > 0 means there are some pending request
	uint8_t isBusy;

	MB_Custom custom;

	uint8_t rxBuff[MB_PDU_MAX_SIZE];
	uint8_t txBuff[MB_PDU_MAX_SIZE];
	uint16_t rxBuffSize;
	uint16_t txBuffSize;
}MB_RTUMaster;

void MBRM_Init(MB_RTUMaster *master);
void MBRM_Execute(MB_RTUMaster *master);
MB_RTUMaster *MBRM_GetInstance(void);
void MBRM_SetInstance(MB_RTUMaster *master);
void MBRM_BindSerial(MBS_Serial *serial_);
uint8_t MBRM_IsReplyTimeout(MBRM_ReplyTimer *replyTimer);
void MBRM_StartReplyTimer(MBRM_ReplyTimer *replyTimer);
void MBRM_StopReplyTimer(MBRM_ReplyTimer *replyTimer);
void MBRM_RestReplyTimer(MBRM_ReplyTimer *replyTimer);
uint8_t MBRM_IsUnexpectedSlave(void);
uint8_t MBRM_GetSlaveAddress(void);
uint8_t MBRM_GetFunCode(void);
uint16_t MBRM_AppendToTxBuff(uint8_t data);


#endif /* SRC_MODBUS_MB_RTUMASTER_H_ */
