/*
 * Test.c
 *
 *  Created on: Jan 18, 2024
 *      Author: wsrra
 */
#include <Task_Handler.h>
#include "main.h"
#include "Debug.h"
#include "Callbacks.h"
#include "l6360.h"
#include "GlobalVars.h"
#include "Settings.h"
#include "IOLink_SMI.h"
#include "IOLink_PL.h"
#include "DigitalOut.h"
#include "DigitalIn.h"

/*Old-------------*/
#include "Task_Handler.h"
#include "Scheduler.h"
#include "Debug.h"
#include "MB_TCP.h"
#include "MB_MsgQ.h"
#include "Tools.h"

#include "MB_Custom.h"
#include "MB_RTUMaster.h"
#include "MB_TCPClient.h"
#include "MB_Tools.h"
#include "MB_Register.h"
#include "Settings.h"
#include "Callbacks.h"


extern GlobalVar_t gVar;

/*UART for IOLink ports----------*/
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern I2C_HandleTypeDef hi2c4;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim24;


/*Old-------------*/
//extern GlobalVar gVar;
//extern DebugStruct dbug;
TIM_HandleTypeDef htim2;
extern MB_RTUHandler mbHandle;



/*IOExtender-------------*/
typedef enum IOExt_PinIndex{
	IOEXT_AO_1_MODE_INDEX = 0,
	IOEXT_AO_2_MODE_INDEX,
	IOEXT_AO_3_MODE_INDEX,
	IOEXT_AO_4_MODE_INDEX,
	IOEXT_RS485_TX_LED_INDEX,
	IOEXT_RS485_RX_LED_INDEX,
	IOEXT_LED_1_INDEX,
	IOEXT_LED_2_INDEX,
	IOEXT_MAX_INDEX,
}IOExt_PinIndex;

const uint8_t IOEXT_AOMODEPIN[8]={
		AO_1_VI_MODE_PIN,
		AO_2_VI_MODE_PIN,
		AO_3_VI_MODE_PIN,
		AO_4_VI_MODE_PIN,
		RS485_TX_LED_PIN,
		RS485_RX_LED_PIN,
		LED_1_PIN,
		LED_2_PIN,
};


typedef union IOEXState{
	uint8_t value;
	struct{
	uint8_t ao1Mode : 1;	//lsb
	uint8_t ao2Mode : 1;
	uint8_t ao3Mode : 1;
	uint8_t ao4Mode : 1;
	uint8_t rs485RxLed : 1;
	uint8_t rs485TxLed : 1;
	uint8_t led1 : 1;
	uint8_t led2 : 1;	//msb
	}bit;
}IOEXState;



void Task_L6360Init(void);

//void DBG_Tx(uint8_t* data, uint16_t len){
//	CDC_Transmit_FS(data,len);
//	HAL_Delay(1);
//}


/*Test purpose-------*/
#define PORTCNF_LEN IOL_ARGBLOCK_LEN_PORTCONFIGLIST
uint8_t portCnfList[PORTCNF_LEN] = {
		(uint8_t)(IOL_ARGBLOCK_ID_PORTCONFIGLIST>>8),
		(uint8_t)(IOL_ARGBLOCK_ID_PORTCONFIGLIST),
		IOL_PORT_MODE_IOL_AUTOSTART,
		IOL_VALIDBACKUP_NO_DEVICE_CHECK,
		IOL_IQBEHAVIOR_NOT_SUPPORTED,
		IOL_PORT_CYC_TIME_AFAP,
		0,	//vendor id MSB
		0,	//vendor id LSB
		0xA0,	//device ID MSB
		0,	//device ID MMSB
		0,	//device ID MLSB
		0	//device ID LSB
};




void TaskInit(void){




	/*Init IOLINK & L6360 chip*/
	Task_L6360Init();


	/*IOLink Initialization------------------*/
	uint8_t numOfPort = 1;
	IOL_SMI_Initialize(numOfPort);
	for(uint8_t i = 0; i< numOfPort; i++){
		IOL_PL_Initialize(i+IOL_PORT_1, &gVar.iolm.port[i]);
	}
	uint8_t index = IOL_PI_0;

	IOL_PL_Initialize(index+1, &gVar.iolm.port[index]);

//	L6360_Status stat = L6360_I2CReadStatusParity(&gVar.iolm.port[0], &gVar.iolm.port[0].reg.sr.value, &gVar.iolm.port[0].reg.pr.value);
//	DBG_SPRINT_NL("hi2c4_ptr: %p, %p, Stat: %d, St: %d, pr: %d",&hi2c4, gVar.iolm.port[0].i2c.hi2c, stat, gVar.iolm.port[0].reg.sr.value, gVar.iolm.port[0].reg.pr.value);
//	/*Test*/
	L6360_Status stat = L6360_GenWURQ(&gVar.iolm.port[index]);
	DBG_SPRINT_NL("IOL_%d: Stat: %d",index+1, stat);
//	stat = L6360_GenWURQ(&gVar.iolm.port[IOL_PI_1]);
//	DBG_SPRINT_NL("IOL_2: Stat: %d", stat);
//	stat = L6360_GenWURQ(&gVar.iolm.port[IOL_PI_2]);
//	DBG_SPRINT_NL("IOL_3: Stat: %d", stat);
//	stat = L6360_GenWURQ(&gVar.iolm.port[IOL_PI_3]);
//	DBG_SPRINT_NL("IOL_4: Stat: %d", stat);

	/*IOLink port configuration-------------*/
	char pData[256] = {0};
	DBG_SPRINT_APPEND(pData,"buff: ");
	for(uint8_t i = 0; i < PORTCNF_LEN; i++){
		DBG_SPRINT_APPEND(pData, "%02X ", portCnfList[i]);
	}
	DBG_PRINT(pData);

	IOL_SMI_PortConfig_Req(1, 1, IOL_ARGBLOCK_ID_VOIDBLOCK, PORTCNF_LEN, portCnfList);
	DBG_SPRINT_NL("Started....");
}

void TaskHandler(void){

//	IOL_SMI_Handler(IOL_PORT_1);

}


void Task_L6360Init(void){


	L6360_LplusEnable(&gVar.iolm.port[IOL_PI_0]);
	L6360_LplusEnable(&gVar.iolm.port[IOL_PI_1]);
	L6360_LplusEnable(&gVar.iolm.port[IOL_PI_2]);
	L6360_LplusEnable(&gVar.iolm.port[IOL_PI_3]);
	HAL_Delay(1000);	// this time is needed to stable the signal


//
//	/*Reset L6360 IC*/
//	L6360_Reset(&gVar.iolm.ports[IOL_PI_0].l6360);
//	HAL_Delay(1000);
	/*Enable L+ line*/
//	L6360_LplusEnable(&gVar.iolm.ports[IOL_PI_0].l6360);
	/*Enable C/Q output*/
//	L6360_CqEnable(&gVar.iolm.ports[IOL_PI_0].l6360);
//			uint8_t data[9] = {0};
//			L6360_I2CRead(&gVar.iolm.ports[0].l6360, L6360_I2C_DEV_ADDR_1, 0, data, 9);
//			for(uint8_t i = 0; i < 9; i++){
//				DBG_SPRINT_NL("reg: %d, %02X", i, data[i]);
//				HAL_Delay(1);
//			}


}

/*Print Device Information-------------------------*/
void TH_PrintDevInfo(void){
	char str[64];
	DBG_SPRINT_NL("\r\nCluster Fieldbus IO Board");
	DBG_SPRINT_NL("Hardware Version: %02X.%X.%X\r\nFirmware Version: %02X.%X.%X\r\n",
			HW_VIRSION>>8, (uint8_t)(HW_VIRSION&0xFF)>>4, (uint8_t)(HW_VIRSION&0x0F),			// Hardware version
			FW_VIRSION>>8, (uint8_t)(FW_VIRSION&0xFF)>>4,(uint8_t)(FW_VIRSION&0x0F));			// Firmware version
}




/* Digital Output --------------------------------*/
/**
  * @brief 	Digital output handler
  */
void TH_DigitalOut(void){
	DigitalOut_t *dout;
	for(uint8_t i = DO_1; i < DO_MAX; i ++){
		dout =  &gVar.dio.dout[i];
		switch (dout->source) {
			case DO_SRC_MODBUS:
				dout->mode = DO_MODE_FOLLOW;
				break;
			default:
				// for digital input 1 to 8
				if(dout->source >= DO_SRC_DI_0 && dout->source <= DO_SRC_DI_7 ){
					switch (dout->mode) {
						case DO_MODE_FOLLOW:
							dout->state = gVar.dio.di[dout->source].state;
							break;
						case DO_MODE_TOGGLE:
							//DEBUG_PRINT("Dinp-2\r\n");
							if(gVar.dio.di[dout->source].state && dout->isToggle == 0U){
								dout->isToggle = 1U;
								dout->state = (dout->state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET);
							}else if(!gVar.dio.di[dout->source].state){
								dout->isToggle = 0U;
							}
							break;
						default:
							break;
					}
				}else{ // if invalid source
					dout->state = GPIO_PIN_RESET;
				}
			break;
		}
	}
	/*Update Output status to the output pins*/
	for(uint8_t i = DO_1; i < DO_MAX; i ++){
		if(gVar.dio.dout[i].lastState != gVar.dio.dout[i].state){
			DO_WRITE(&gVar.dio.dout[i], gVar.dio.dout[i].state);			//REPLACED
//			HAL_GPIO_WritePin(gVar.dio.dout[i].port, gVar.dio.dout[i].pin, gVar.dio.dout[i].state);
//			DBG_SPRINT_NL("DO-%d",i);
			gVar.dio.dout[i].lastState = gVar.dio.dout[i].state;
		}
	}
}



/* Digital Input --------------------------------*/
/**
  * @brief  Digital Input handler function
  */
void TH_DigitalIn(void){
	for(uint8_t i = DI_1; i < DI_MAX; i ++){
		if((gVar.dio.enc.enable > 0U) && (i <= DI_3)){

			switch (gVar.dio.enc.configs.mode) {
				case ENC_MODE_QUAD:
					ENC_ReadQuad(&gVar.dio.enc);
					i =  DI_3;		// skips for di channels 1,2&3
					continue;
				case ENC_MODE_INCRE_CHA:
//					ENC_ReadChA(&gVar.dio.enc);
					if(i ==  DI_1) continue;	// skips for  di channels 1, input is captured by external interrupt
					break;
				case ENC_MODE_INCRE_CHB:
//					ENC_ReadChB(&gVar.dio.enc);
					if(i ==  DI_2) continue; // skips for  di channels 2
					break;
				case ENC_MODE_INCRE_CHAB:
//					ENC_ReadChZ(&gVar.dio.enc);
					if(i <=  DI_2) continue; // skips for  di channels 1 &2
					break;
				default:
					break;
			}

		}

		DigitalIn_t *di;
		di = &gVar.dio.di[i];
		switch (di->config.mode) {
			case DI_MODE_NORMAL:
				DI_Read(di);
				break;
			case DI_MODE_PULSE_CAPTURE:
				DI_Counter(di);
				break;
			default:
				break;
		}
	}
}


/**
  * @brief  Digital Input Configuration
  */
void TH_DigitalInConfig(void){

	for(uint8_t i = DI_1; i < DI_MAX; i ++){
		if((gVar.dio.enc.enable > 0U) && (i <= DI_3)){
			if(gVar.dio.enc.isChanged <= 0) continue;
			gVar.dio.enc.isChanged = 0;
			ENC_Config(&gVar.dio.enc);
			switch (gVar.dio.enc.configs.mode) {
				case ENC_MODE_QUAD:
					i =  DI_3;		// skips for di channels 1,2&3
					continue;
				case ENC_MODE_INCRE_CHA:
					if(i ==  DI_1) continue;	// skips for  di channels 1
					break;
				case ENC_MODE_INCRE_CHB:
					if(i ==  DI_2) continue; // skips for  di channels 2
					break;
				case ENC_MODE_INCRE_CHAB:
					if(i <=  DI_2) continue; // skips for  di channels 1 &2
					break;
				default:
					break;
			}

		}
		if(gVar.dio.di[i].isChanged <= 0) continue;
		gVar.dio.di[i].isChanged = 0;
		DI_Config(&gVar.dio.di[i]);

	}
}




/**
  * @brief  detect the timer channel and measured the pulse information
  */

void TH_DICapturePulse(TIM_HandleTypeDef* tim){
	switch (tim->Channel) {
		case HAL_TIM_ACTIVE_CHANNEL_1:
			gVar.dio.di[DI_1].pulseInf.lastEdgeTime = gVar.dio.di[DI_1].pulseInf.edgeTime;
			gVar.dio.di[DI_1].pulseInf.edgeTime = htim2.Instance->CCR1;
			gVar.dio.di[DI_1].pulseInf.isEdgeDtctd = 1;
			gVar.dio.di[DI_1].pulseInf.isPulseDtctd = 1;
			break;
		case HAL_TIM_ACTIVE_CHANNEL_2:
			gVar.dio.di[DI_2].pulseInf.lastEdgeTime = gVar.dio.di[DI_2].pulseInf.edgeTime;
			gVar.dio.di[DI_2].pulseInf.edgeTime = htim2.Instance->CCR2;
			gVar.dio.di[DI_2].pulseInf.isEdgeDtctd = 1;
			gVar.dio.di[DI_2].pulseInf.isPulseDtctd = 1;
			break;
		default:
			break;
	}
}

void TH_DICaptureOverflow(TIM_HandleTypeDef* tim){
	if(gVar.dio.di[DI_1].config.mode == DI_MODE_PULSE_CAPTURE && gVar.dio.di[DI_1].config.counterType == DI_COUNT_TYPE_TIMER ){
		gVar.dio.di[DI_1].pulseInf.countOVF++;
	}

	if(gVar.dio.di[DI_2].config.mode == DI_MODE_PULSE_CAPTURE && gVar.dio.di[DI_2].config.counterType == DI_COUNT_TYPE_TIMER ){
		gVar.dio.di[DI_2].pulseInf.countOVF++;
	}

}

/*
 * @brief If there is no pulse detected by the digital inputs
 * this function will clear the related parameters
 * it is necessary to avoid unexpected behavior when next pulse is being
 * detected
 * */
void TH_DIClearOnNoPulse(void){

	static uint32_t tick[2] = {0};
	static uint8_t runOnce[2] = {0};
	for(uint8_t i = DI_1; i<=DI_2; i++){
		if(gVar.dio.di[i].config.mode == DI_MODE_PULSE_CAPTURE){
			if(gVar.dio.di[i].pulseInf.isPulseDtctd>0){
				gVar.dio.di[i].pulseInf.isPulseDtctd = 0;
				tick[i] = HAL_GetTick();
				runOnce[i] = 1;
			}else{
				if(runOnce[i] > 0){
					if((uint32_t)fabs(HAL_GetTick() - tick[i]) >= DEF_DI_CNTR_PULSE_DETECT_TIMEOUT){
						gVar.dio.di[i].pulseInf.period = 0;
						gVar.dio.di[i].pulseInf.edgeTime = 0;
						gVar.dio.di[i].pulseInf.lastEdgeTime = 0;
						runOnce[i] = 0;
					}
				}
			}

		}

	}
}


/*Old-------------*/

/*Modbus TCP master  handler ------------ */
void TH_MBRTUMaster(void){
	/*
	 * To read from slave & send to the TCP client
	 * 1. Build a request buffer for reading input register & discrete input
	 * 2. send the request to the rtu master layer
	 * */
//	DEBUG_SPRINT_NL("sockConnct: %d, mstEn: %d",gVar.server.sock[0].isConnected,  gVar.mbRTUMaster.enable);
	gVar.server.sockIndex = 0;
	if(!gVar.server.sock[0].isConnected || !gVar.mbRTUMaster.enable){

		gVar.mbRTUMaster.isBusy = 0;
		return;
	}

//	MB_CUST_SetTimeout(&gVar.mbRTUMaster.custom.reTransTimer, (gVar.mbRTUMaster.replyTimer.timeout*gVar.mbRTUMaster.custom.numOfSlv));
//	DEBUG_SPRINT_NL("Muster reTransTimer: %d, socCOn: %d", gVar.mbRTUMaster.custom.reTransTimer.timeout, gVar.server.sock[0].isConnected );
	if(gVar.mbRTUMaster.custom.reTransTimer.timeout<=0
			||	(gVar.mbRTUMaster.custom.numOfSlv <= 0 || gVar.mbRTUMaster.custom.numOfSlv > MB_CUST_NUM_OF_SLAVE_MAX)
			|| (gVar.mbRTUMaster.custom.slvStartAddr <= 0 || gVar.mbRTUMaster.custom.numOfSlv > MB_SLAVE_ADDR_MAX)) return;

	if(MB_CUST_IsTimeout(&gVar.mbRTUMaster.custom.reTransTimer ) && gVar.mbRTUMaster.isPendingRqst == 0){
//		DEBUG_SPRINT_NL("Muster reTransTimer: %d", gVar.mbRTUMaster.custom.reTransTimer.timeout);
		gVar.mbRTUMaster.custom.funCode = MB_CFC_RIRDI_107;
		gVar.mbRTUMaster.txBuffSize = MB_CUST_BuildRTUReqRIRDI(&gVar.mbRTUMaster.custom, gVar.mbRTUMaster.txBuff);
//		DEBUG_SPRINT_NL("cust: ");
//		for(uint8_t i = 0; i < gVar.mbRTUMaster.txBuffSize;  i++){
//			DEBUG_SPRINT("%02X ",gVar.mbRTUMaster.txBuff[i] );
//		}
		gVar.mbRTUMaster.isPendingRqst = 1;	// TODO: to uncomment
		gVar.mbRTUMaster.state = MBRM_STATE_IDLE;
		gVar.mbRTUMaster.isBusy = 1;
//		DEBUG_SPRINT_NL("MBM Bs: 1");
		//TODO: TEST-20230901:
//		MB_CUST_ResetTimer(&gVar.mbRTUMaster.custom.reTransTimer);
	}
	MBRM_Execute(&gVar.mbRTUMaster);

}


/*Modbus 107 packe handler ------------ */
void TH_MB_107PackHandler(void){
//DEBUG_SPRINT_NL("107...");
	if(gVar.server.sock[0].isReadyToSend != 0) return;
	gVar.server.sockIndex = 0;
	if( gVar.server.sock[0].status == SOCK_ESTABLISHED ){
		Socket *sock;
		sock = &gVar.server.sock[0];
//		uint8_t clientIndex = 0;
		/* 2. find the client who actually own the response*/
//		if(MBTCPS_FindClientByIdAndPort(&gVar.mbTCPServer, ConvArrToVar4Bytes(sock->destIP), sock->destPort) != MB_OK){ return;}
//		clientIndex = gVar.mbTCPServer.clientIndex;
//		client = &gVar.mbTCPServer.clients[gVar.mbTCPServer.clientIndex];



		/* Read master IR & DI data*/
//		const uint8_t dataStart = 4; // for the received data from serial for custom function code
		uint8_t status = 0;

		sock->txBuffSize = 4; // 4 = Transaction id 2 bytes + protocol id 2 bytes
		memcpy(sock->txBuff, sock->rxBuff, sock->txBuffSize);
		sock->txBuffSize =MBTCP_INDX_UNIT_ID;
		sock->txBuff[sock->txBuffSize++] = MB_BROADCAST_ADDR;	// unit id
		sock->txBuff[sock->txBuffSize++] = MB_CFC_RIRDI_107;
		sock->txBuff[sock->txBuffSize++] = gVar.mbRTUMaster.custom.slvStartAddr;
		sock->txBuff[sock->txBuffSize++] = gVar.mbRTUMaster.custom.numOfSlv;

		// get local RI & DI data
		MB_setRespBuff(&sock->txBuff[sock->txBuffSize]);
		status = MB_executeRequest(MB_CFC_RIRDI_107);
		sock->txBuffSize += MB_CUST_DATA_BLOCK_SIZE;
		if(status == MB_RS_255){
			sock->txBuff[sock->txBuffSize -2] = MB_CUST_ONLINE;
		}else{
			 MB_CUST_BuildEmptySubPacket(&sock->txBuff[MB_CUST_MASTER_DATA_START_INDEX], (MB_CUST_ONLINE | MB_CUST_ERROR | status));
		}

		// copy slave data from the master
		if(gVar.mbRTUMaster.custom.numOfSlv > 0){
			memcpy(&sock->txBuff[sock->txBuffSize], &gVar.mbRTUMaster.rxBuff[4+MB_CUST_DATA_BLOCK_SIZE],(MB_CUST_DATA_BLOCK_SIZE*gVar.mbRTUMaster.custom.numOfSlv)); //4 = slave address + FC+ slave start Adress+ number of slave
			sock->txBuffSize += MB_CUST_DATA_BLOCK_SIZE*gVar.mbRTUMaster.custom.numOfSlv;
		}
		/*now add encoder and counter data to the packet*/
		sock->txBuffSize += TH_AddEncoderDataToPacket(&sock->txBuff[sock->txBuffSize]);
		sock->txBuffSize  += TH_AddCounterDataToPacket(&sock->txBuff[sock->txBuffSize]);
		uint16_t length = sock->txBuffSize - MBTCP_INDX_LENGTH_LO-1;
		MBTOOL_SplitU16ToBytes(&sock->txBuff[MBTCP_INDX_LENGTH_HI], &sock->txBuff[MBTCP_INDX_LENGTH_LO], length);
//		if(sock->txBuffSize == length+MBTCP_INDX_HEADER_MAX -1){
		sock->isReadyToSend = 1;		// set the send flag
//		}

//		Socket_TxHandler(sock);
	}

}


/*	This function check the reply timeout for the slaves
 * */
void TH_MBRTUMasterTimoutHandler(void){
	const uint8_t dataStart = 4; // Data start index of the received packet from RTU serial for custom function code(107)
	if (!gVar.mbRTUMaster.enable || gVar.devMode != DEV_MODE_MASTER) {
		return;
	}
	if(MBRM_IsReplyTimeout(&gVar.mbRTUMaster.replyTimer)){									// check the reply timer is time out or not
		gVar.mbRTUMaster.custom.timeoutCount++;


		gVar.mbRTUMaster.custom.rcvFailedCount[gVar.mbRTUMaster.custom.rcvdCount]++;

		if(gVar.mbRTUMaster.custom.rcvdCount < gVar.mbRTUMaster.custom.numOfSlv){			// check all the slave is responded or not
//			if(gVar.mbRTUMaster.custom.timeoutCount > gVar.mbRTUMaster.custom.numOfSlv *2){	// wait for the loss of several packets
			if(gVar.mbRTUMaster.custom.rcvFailedCount[gVar.mbRTUMaster.custom.rcvdCount]>=3){		// check for no response count of the slaves
				gVar.mbRTUMaster.custom.rcvFailedCount[gVar.mbRTUMaster.custom.rcvdCount] = 0;
				gVar.mbRTUMaster.custom.timeoutCount = 0;
				uint16_t byteIndex =  dataStart + MB_CUST_DATA_BLOCK_SIZE* (gVar.mbRTUMaster.custom.rcvdCount+1); 						// 1 is added because of there is master data also in the packet, but master is not counted to the number of slave
				gVar.mbRTUMaster.rxBuffSize += MB_CUST_BuildEmptySubPacket(&gVar.mbRTUMaster.rxBuff[byteIndex],(MB_CUST_OFFLINE));		// built an empty packet
			}
			MBRM_RestReplyTimer(&gVar.mbRTUMaster.replyTimer);
			gVar.mbRTUMaster.custom.rcvdCount++;


		}else{
			MBRM_StopReplyTimer(&gVar.mbRTUMaster.replyTimer);
		}
	}



	/*Reset the parameters if the data packet is received from slaves or timeout is occurred
	 * */
	if(gVar.mbRTUMaster.custom.rcvdCount >=  gVar.mbRTUMaster.custom.numOfSlv
			&& gVar.mbRTUMaster.custom.numOfSlv != 0){
		MBRM_StopReplyTimer(&gVar.mbRTUMaster.replyTimer);
//		if(gVar.mbRTUMaster.custom.timeoutCount>0){
//			gVar.mbRTUMaster.custom.timeoutCount--;
//		}

		gVar.mbRTUMaster.custom.rcvdCount = 0;
		gVar.mbSerial.rxBuffSize = 0;
		gVar.mbSerial.txBuffSize = 0;

		gVar.mbSerial.isBusy = 0;
		gVar.mbRTUMaster.isBusy = 0;
		MBS_ClearRXBuffer(&gVar.mbSerial);
		MBS_ClearTXBuffer(&gVar.mbSerial);
	}

	if(gVar.mbRTUMaster.replyTimer.enableTimer <=0 ){
		gVar.mbRTUMaster.custom.rcvdCount = 0;
		gVar.mbSerial.rxBuffSize = 0;
		gVar.mbSerial.txBuffSize = 0;
		gVar.mbSerial.isBusy = 0;
		gVar.mbRTUMaster.isBusy = 0;
		MBS_ClearRXBuffer(&gVar.mbSerial);
		MBS_ClearTXBuffer(&gVar.mbSerial);
	}


	static uint32_t tick = 0;
	if((uint32_t)fabsf(HAL_GetTick() - tick) == (uint32_t)250U){
		if(gVar.mbRTUMaster.custom.timeoutCount > 0){
			gVar.mbRTUMaster.custom.timeoutCount--;
		}
	}
}



/*Modbus RTU slave  handler ------------ */
void TH_MBRTUSlaveTimoutHandler(void){
	/* check timeout timer. this timer will keep this slave in wait to send the other slaves which address less than this slave.
	 * if timeout is happened and the last slave is immediate first slave of this salve, then it will send its response
	 * */
			if(MB_CUST_IsTimeout(&gVar.mbRTUSlave.custom.waitingTimer)){
				MB_CUST_ResetTimer(&gVar.mbRTUSlave.custom.waitingTimer);
				gVar.mbRTUSlave.custom.respondingSlave++;
//				DEBUG_SPRINT(" %d", gVar.mbRTUSlave.custom.respondingSlave);
				if(gVar.mbRTUSlave.custom.respondingSlave == gVar.mbRTUSlave.slave_address){

					gVar.mbSerial.isDataReceived = 1;
//					DEBUG_SPRINT_NL("Slv tSet 1");
//					 MBRS_processRequest(&gVar.mbRTUSlave);
//					  MBS_SetFramOk();
//					MB_ClearBuffer();

//					DEBUG_SPRINT("\nrsp:%d,%d", gVar.mbRTUSlave.custom.respondingSlave-1, gVar.mbRTUSlave.custom.respondingSlave);
//					if(gVar.mbRTUSlave.custom.respondingSlave == gVar.mbRTUSlave.slave_address){
//						MB_ErrorStatus errorStatus = MB_OK;
////						if(statusRet>0){
//		//					DEBUG_SPRINT("\r\n sR:%d,txSz:%d ", statusRet, serial->txBuffSize);
//		//					for(uint8_t i = 0; i < serial->txBuffSize;  i++){
//		//						DEBUG_SPRINT_APPEND(dbug.str, "%02X,", serial->txBuff[i] );
//		//					}
//		//					DEBUG_SPRINT_APPEND(dbug.str, "]");
//		//					DEBUG_SPRINT(dbug.str);
//						MB_setReqBuff(gVar.mbSerial.rxBuff, gVar.mbSerial.rxBuffSize);
//						MB_setRespBuff(gVar.mbSerial.txBuff);
//						mbHandle.txBufferSize = 0;
//						MB_appendToRespPDU_SAFC();						// here the slave address basically is the broadcast address
//						MB_appendToRespPDU(gVar.mbRTUSlave.slave_address);		// local slave address
//						CB_MB_CUST_Build107RespAIDI(&gVar.mbSerial.txBuff[mbHandle.txBufferSize], MB_CUST_DATA_BLOCK_SIZE);
//						mbHandle.txBufferSize += MB_CUST_DATA_BLOCK_SIZE;
//						MB_appendToRespPDU_CRC();
//						gVar.mbSerial.txBuffSize  = mbHandle.txBufferSize;
//								DEBUG_SPRINT("\nt:%d ", gVar.mbSerial.txBuffSize);
////								for(uint8_t i = 0; i < gVar.mbSerial.txBuffSize;  i++){
////									DEBUG_SPRINT_APPEND(dbug.str, "%02X,",gVar.mbSerial.txBuff[i] );
////								}
////								DEBUG_SPRINT_APPEND(dbug.str, "]");
////								DEBUG_SPRINT(dbug.str);
//						gVar.mbSerial.mode = MBS_MODE_SLAVE;
//						gVar.mbSerial.isSending = 0;
//
//						errorStatus = MBS_Send(&gVar.mbSerial);
//
//
//						gVar.mbSerial.rxBuffSize = 0;
//						gVar.mbSerial.txBuffSize = 0;
//
//						MB_ClearBuffer();
//
////						}
//						MBRS_PostProcess(&gVar.mbRTUSlave, errorStatus); // to process some modbus related task after sending the response
//					}else{
//						 gVar.mbSerial.frameOkFlag = 1;
//						MBS_ProcessRcvdFrame(&gVar.mbSerial, &gVar.mbRTUSlave);
					gVar.mbRTUSlave.custom.eventType = MB_CUST_EVENT_107_TIMEOUT;
						MBS_ProcessFrame(&gVar.mbSerial, &gVar.mbRTUSlave);




//					}





				}

				if(gVar.mbRTUSlave.custom.respondingSlave>=gVar.mbRTUSlave.slave_address){
					gVar.mbRTUSlave.custom.respondingSlave = 0;
					gVar.mbSerial.rxBuffSize = gVar.mbRTUSlave.rxBufferSize;
					gVar.mbSerial.txBuffSize = gVar.mbRTUSlave.txBufferSize;
					MB_CUST_StopTimer(&gVar.mbRTUSlave.custom.waitingTimer);
					MB_ClearBuffer();
				}

			}

}


///**
//  * @brief Built request for custom function 107-Read Input Register & Discrete input
//  *
//  * @param none
//  * @retval none
//  */
//void TH_MBRM_buildReqRIRDI(void){
//	/*
//	 * Data frame
//	 * ------------------------------------------------------------------------------------------------------
//	 * Broadcast address | Custom Function Code | Slave Starting Address | Number of Slave (N) | CRC-16 (?)	|
//	 *   	1 byte		 |		 1 bytes	    |			1 byte		 |		1 bytes		   |	2 bytes	|
//	 * ------------------------------------------------------------------------------------------------------
//	 * */
//
//	MB_RTUMaster *mbMaster = {0};
//	mbMaster = &gVar.mbRTUMaster;
//	mbMaster->txBufferSize = 0;
//	MBRM_AppendToTxBuff(MB_BROADCAST_ADDR);			//  Broadcast address
//	MBRM_AppendToTxBuff(MB_BROADCAST_ADDR);			//  Custom Function Code 107-Read Input Register & Discrete input
//}


/*Modbus TCP Server handler ------------ */
/**
  * @brief handles all kind of MB TCP server related tasks
  * @param none
  * @retval none
  */

/*
*	received response from the remote device when the device is using as modbus gateway
* */

void TH_MBTCPSRecvRespGW(MB_TCPServer *mbtcps){
	/*Modbus TCP server is using the gateway. flow the steps to receive the response
	 * 1. check if data is received
	 * 2. find the client who actually own the response
	 * 3. convert the packet RTU to TCP
	 * 4. send the TCP packet as a response to the client
	 * */
	/*1. check if data is received*/
	MBS_Serial *serial = {0};
	serial = &gVar.mbSerial;

	if(serial->isDataReceived){
		serial->isDataReceived = 0;
//		DEBUG_SPRINT_NL("isData: %d, gid: %ld, gkey: %d ",serial->isDataReceived,  MBG_GetTCPClientId(), MBG_GetTCPClientKey());
		gVar.mbGateway.isRegularFC = 0;			// unlock the receive event for the server which is locked by the regular function code

		uint8_t slvAddr = serial->rxBuff[MB_SA_INDEX];
		uint8_t funCode = serial->rxBuff[MB_FC_INDEX];


		if(MB_checkSlaveAddress(slvAddr) && !MB_CUST_CheckCFunCode(funCode)){//funCode != MB_CFC_WMHRCO_109){// for the regular function code
			MBTCPS_ClientParams *client;
			uint8_t clientIndex = 0;
			/* 2. find the client who actually own the response*/
			if(MBTCPS_FindClientByIdAndPort(mbtcps, MBG_GetTCPClientId(), MBG_GetTCPClientKey()) != MB_OK){ return;}
			clientIndex = mbtcps->clientIndex;
			client = &mbtcps->clients[clientIndex];

//			DEBUG_SPRINT_NL("CFC-109, slvAddr: %d, funCode: %ld,  ",slvAddr, funCode);


	//		DEBUG_SPRINT_NL("client found: %d ",clientIndex);
	//		DEBUG_SPRINT_NL(" RTU Rcv %d: ", serial->rxBuffSize);
	//		for(uint8_t i = 0; i < serial->rxBuffSize; i++){
	//			DEBUG_SPRINT("%02X ",serial->rxBuffer[i]);
	//		}
			/*convert the packet RTU to TCP*/
			client->txBuffSize = MBG_ConvertRTU2TCP(client->txBuff, serial->rxBuff,  serial->rxBuffSize);

			MBG_GetTCPHeader()->length = client->txBuffSize;
			MBTCP_SetHeader(client->txBuff, MBG_GetTCPHeader());		// set the tcp header to te tx buffer
			client->txBuffSize = client->txBuffSize + MBTCP_INDX_HEADER_MAX-1;



	//		DEBUG_SPRINT_NL(" clnt %d: ", client->txBuffSize);
	//		for(uint8_t i = 0; i < client->txBuffSize; i++){
	//			DEBUG_SPRINT("%02X ",client->txBuff[i]);
	//		}
			MBG_StopTimeout(&gVar.mbGateway.rplyTimer);
			MBTCPS_Send(client);
			gVar.mbSerial.isBusy = 0;
			gVar.mbGateway.isBusy = 0;
			MBS_ClearRXBuffer(serial);
			MBS_ClearTXBuffer(serial);


//			gVar.mbSerial.mode = MBS_MODE_NONE;
	//		if(MBMQ_DeleteQueMsg(&mbtcps->msgQ)>0){
	//			mbtcps->curntTrnsNo--;
	//		}
	//		DEBUG_SPRINT_NL("noOfMsg: %d ",mbtcps->msgQ.noOfMsg);
		}else if(MB_CUST_CheckCFunCode(funCode)){//gVar.mbGateway.custom.funCode == MB_CFC_WMHRCO_109){
			gVar.mbGateway.isBusy = 0;
			gVar.mbGateway.isReqSent = 0;
			gVar.mbGateway.txBuffSize = 0;
			gVar.mbGateway.rxBuffSize = 0;
			gVar.mbSerial.txBuffSize = 0;
			gVar.mbSerial.rxBuffSize = 0;
//			gVar.mbSerial.mode = MBS_MODE_NONE;
			gVar.mbGateway.custom.funCode = 0;
			gVar.mbGateway.custom.rcvdCount = 0;
			gVar.mbGateway.custom.slvStartAddr = 0;
			gVar.mbGateway.custom.numOfSlv = 0;
			MBG_StopTimeout(&gVar.mbGateway.rplyTimer);
			MBS_ClearRXBuffer(&gVar.mbSerial);
			MBS_ClearTXBuffer(&gVar.mbSerial);
		}
	}
}







/*Modbus TCP Client handler ------------ */
void TH_MBTCPClient(void){


	static uint32_t tick[2] = {0};
	gVar.client.sockIndex = 0;

	if( gVar.client.sock[gVar.client.sockIndex].isConnected ){

//		DEBUG_SPRINT_NL("TC Ctd: %d: ",gVar.client.sock[gVar.client.sockIndex].isConnected);

		if(((uint32_t)fabs(HAL_GetTick()-tick[0]) >= gVar.mbTCPClient.txTimIntrvl)					// Transmission time interval, at this interval client will send packet frequently
				&& ((uint32_t)fabs(HAL_GetTick()-tick[1]) >= gVar.mbTCPClient.dlyAftrConct)			// after each connection it will wait to stable the connection then will continue the Transmission
				&& gVar.mbRTUMaster.enable>0 && gVar.mbTCPServer.isDataRcvd > 0){				//&& gVar.mbTCPServer.isDataRcvd > 0									//




			/* Read master IR & DI data*/
			const uint8_t dataStart = 4; // for the received data from serial for custom function code
			uint8_t status = 0;
	//				MB_setReqBuff(&gVar.mbRTUMaster[MB_CUST_MASTER_DATA_START_INDEX], size);
			MB_setRespBuff(&gVar.mbRTUMaster.rxBuff[dataStart]);
			status = MB_executeRequest(MB_CFC_RIRDI_107);

			if(status == MB_RS_255){
				gVar.mbRTUMaster.rxBuff[4 + MB_CUST_DATA_BLOCK_SIZE -2] = MB_CUST_ONLINE;
			}else{
				 MB_CUST_BuildEmptySubPacket(&gVar.mbRTUMaster.rxBuff[dataStart], (MB_CUST_ONLINE | MB_CUST_ERROR | status));
			}


			gVar.mbRTUMaster.rxBuff[MB_SA_INDEX] = MB_BROADCAST_ADDR;
			gVar.mbRTUMaster.rxBuff[MB_FC_INDEX] = MB_CFC_RIRDI_107;
			gVar.mbRTUMaster.rxBuff[MB_FC_INDEX+1] = gVar.mbRTUMaster.custom.slvStartAddr;
			gVar.mbRTUMaster.rxBuff[MB_FC_INDEX+2] = gVar.mbRTUMaster.custom.numOfSlv;
			gVar.mbRTUMaster.rxBuffSize = 4 + MB_CUST_DATA_BLOCK_SIZE*(gVar.mbRTUMaster.custom.numOfSlv+1);  // 1 is added because of there is master data also whice is not in the number of slave

			/*now add encoder and counter data to the packet*/
			gVar.mbRTUMaster.rxBuffSize += TH_AddEncoderDataToPacket(&gVar.mbRTUMaster.rxBuff[gVar.mbRTUMaster.rxBuffSize]);
			gVar.mbRTUMaster.rxBuffSize  += TH_AddCounterDataToPacket(&gVar.mbRTUMaster.rxBuff[gVar.mbRTUMaster.rxBuffSize]);
//					DEBUG_SPRINT_NL("TC RXsz: %d: ",gVar.mbRTUMaster.rxBuffSize);
	//				for(uint8_t i = 0; i < gVar.mbRTUMaster.rxBuffSize;  i++){
	//					DEBUG_SPRINT("%02X ",gVar.mbRTUMaster.rxBuff[i] );
	//				}


			if( gVar.mbRTUMaster.rxBuffSize > 0 ){
				MBTCPC_BuildRequest(&gVar.mbTCPClient, gVar.mbRTUMaster.rxBuff, gVar.mbRTUMaster.rxBuffSize);
			}

			gVar.mbTCPClient.lastTrnsIndex = MBTCPC_CheckPendingTrans(&gVar.mbTCPClient);
			if(gVar.mbTCPClient.lastTrnsIndex >= 0   && gVar.mbTCPClient.lastTrnsIndex < MBTCPC_MAX_TRANS){
				MBTCPC_TransInfo *transInfo;
				transInfo = &gVar.mbTCPClient.pendingList[gVar.mbTCPClient.lastTrnsIndex];

	//			DEBUG_SPRINT_NL("mbTCPClient: %d: ",transInfo->txBuffSize);
	//			for(uint8_t i = 0; i < transInfo->txBuffSize;  i++){
	//				DEBUG_SPRINT("%02X ",transInfo->txBuff[i] );
	//			}
				memcpy(gVar.client.sock[gVar.client.sockIndex].txBuff, transInfo->txBuff, transInfo->txBuffSize);
				gVar.client.sock[gVar.client.sockIndex].txBuffSize = transInfo->txBuffSize;
				gVar.client.sock[gVar.client.sockIndex].isReadyToSend = 1;
			}

			tick[0] = HAL_GetTick();
		}

	}else{
		tick[1] = HAL_GetTick();		// delay after connection time will update on disconnect
	}

//	MBTCPC_ProcessResponse();
}





void TH_SetMsgParams(MBMQ_Msg *msg, uint8_t *ip, uint16_t port, uint8_t *buff, uint16_t size){
	msg->id =  ConvArrToVar4Bytes(ip);
	msg->key = port;
	memcpy(msg->buff, buff, size);
	msg->size = size;
}


/*Task handler for Modbus gateway ------------ */
void TH_MBGateway(void){

	if(gVar.mbGateway.txBuffSize > 0 && !gVar.mbGateway.isReqSent){// && !gVar.mbSerial.isBusy
		gVar.mbGateway.custom.funCode = gVar.mbGateway.txBuff[MB_FC_INDEX];

		/*0. CHeck the function code*/
		if(MB_CUST_CheckCFunCode(gVar.mbGateway.custom.funCode)>0 ){		// for custom function code

			switch (gVar.mbGateway.custom.funCode) {
				case MB_CFC_RIRDI_107:		// read input register & coil output
					// TODO: RIRDI, to implement later
					break;
				case MB_CFC_WMHRCO_109:{			// write holding register & discrete input

						/* 1. pares the master data
						 * 2. process master data
						 * 3. process slaves data
						 * 4. send the slaves data to the gateway
						 * */
						uint8_t processStatus;
						MB_CUST_MQ_Msg msg;
						uint8_t byteAddrMaster = 4;		// data start address for the master
						// 1. parse data for the master
						MB_setReqBuff(&gVar.mbGateway.txBuff[byteAddrMaster], MB_CUST_DATA_BLOCK_SIZE);
						MB_setRespBuff(msg.buff);


						 /* 2. process master data*/
						processStatus = MB_executeRequest(gVar.mbGateway.custom.funCode);						//  MB_executeRequest() will return 0x04 and 0xFF, 0x04 = exception occured and 0xFF = no exception
						if (processStatus != 0u){
							if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){							// for exception response
//								MBR_CUST_BuildExcepResp(processStatus);					// processStatus = 255 means, no exception and the response has been built
								gVar.mbGateway.rxBuff[byteAddrMaster] = MB_CUST_ONLINE | MB_CUST_ERROR|processStatus;	// set exception status for the master
							}else{
								gVar.mbGateway.rxBuff[byteAddrMaster] = MB_CUST_ONLINE;			// set 0 for no exception
							}
						}

						uint8_t numOfSlave = gVar.mbGateway.txBuff[byteAddrMaster-1];
//						DEBUG_SPRINT_NL("MBG numOfSlave: %d", numOfSlave);
						if(numOfSlave<=0){
							gVar.mbGateway.isBusy = 0;
							gVar.mbGateway.isReqSent = 0;
							gVar.mbGateway.txBuffSize = 0;
							gVar.mbGateway.rxBuffSize = 0;
							break;
						}

						/*3. process slaves data*/

						MBS_Serial *serial;
						serial = &gVar.mbSerial;
						//TODO: ADD-20230902, ------------
						if((serial->protocol == MBS_PROTO_RTU_STD || serial->protocol == MBS_PROTO_RTU_CLUSTER) && (serial->isSending || serial->isReceiving)) break;		// if serial line is busy to send or receive with another request
						/*1. Check the serial port is not busy-----* */
						if(!serial->isBusy){
							/*2. convert TCP to RTU format-----* */
							uint16_t startIndexForSlv = byteAddrMaster+MB_CUST_DATA_BLOCK_SIZE;		// start b byte index for
							gVar.mbGateway.txBuffSize = gVar.mbGateway.txBuffSize- MB_CUST_DATA_BLOCK_SIZE;					// no calculate the size of the buffer after removing data of the master
							memcpy(&gVar.mbGateway.txBuff[byteAddrMaster], &gVar.mbGateway.txBuff[startIndexForSlv], gVar.mbGateway.txBuffSize);	// remove the data of the master device and shift forward other packet of the slaves
							memcpy(serial->txBuff,gVar.mbGateway.txBuff, gVar.mbGateway.txBuffSize );
							serial->txBuffSize = gVar.mbGateway.txBuffSize;

							MBTOOL_SplitU16ToBytes(
									&serial->txBuff[serial->txBuffSize +1],
									&serial->txBuff[serial->txBuffSize],
									MB_CalcCRC16(gVar.mbGateway.txBuff,
									gVar.mbGateway.txBuffSize));

							serial->txBuffSize += 2;
							if(serial->txBuffSize>0){
								serial->mode = MBS_MODE_GATEWAY;
								if(MBG_SendToSerial(serial->txBuff, serial->txBuffSize)== MB_OK){
									gVar.mbGateway.custom.slvStartAddr = serial->txBuff[MB_FC_INDEX+1];
									gVar.mbGateway.custom.numOfSlv = serial->txBuff[MB_FC_INDEX+2];

									if(serial->protocol == MBS_PROTO_RTU_CLUSTER_DMA){
										gVar.mbGateway.isReqSent = 0;
										gVar.mbGateway.isBusy = 1;
										serial->isBusy = 1;
										MBG_StartTimeout(&gVar.mbGateway.rplyTimer);


									}else{

										gVar.mbGateway.txBuffSize = 0;
										gVar.mbGateway.isReqSent = 1;
										gVar.mbGateway.isBusy = 0;
										serial->txBuffSize = 0;
										serial->isBusy = 0;
									}
								}
								else{
									gVar.mbGateway.txBuffSize = 0;
									gVar.mbGateway.isReqSent = 1;
									gVar.mbGateway.isBusy = 0;
									serial->txBuffSize = 0;
									serial->isBusy = 0;
								}
//								DEBUG_SPRINT_NL("MBG snd: 1");
//								gVar.mbGateway.txBuffSize = 0;
//								gVar.mbGateway.isReqSent = 1;
//								gVar.mbGateway.isBusy = 0;
//								serial->txBuffSize = 0;
//								serial->isBusy = 0;
////								MBG_StartTimeout(&gVar.mbGateway);
							}

						}

					}
					break;
				default:
					break;
			}
		}else if(MBTCP_isFunCodeValid(gVar.mbGateway.custom.funCode)>0U){		// for standard function code

			MBS_Serial *serial;
			serial = &gVar.mbSerial;
			/*1. Check the serial port is not busy-----* */

				/*2. convert TCP to RTU format-----* */
//				serial->txBuffSize = MBG_ConvertTCP2RTU(serial->txBuff,gVar.mbGateway.txBuff, gVar.mbGateway.txBuffSize);

				memcpy(serial->txBuff,gVar.mbGateway.txBuff, gVar.mbGateway.txBuffSize );
				serial->txBuffSize = gVar.mbGateway.txBuffSize;
				MBTOOL_SplitU16ToBytes(&serial->txBuff[serial->txBuffSize +1],
						&serial->txBuff[serial->txBuffSize],
						MB_CalcCRC16(gVar.mbGateway.txBuff,
								gVar.mbGateway.txBuffSize));
				serial->txBuffSize += 2;

//				DEBUG_SPRINT_NL(" RTU %d: ", serial->txBuffSize);
//				for(uint8_t i = 0; i < serial->txBuffSize; i++){
//					DEBUG_SPRINT("%02X ",serial->txBuff[i]);
//				}
				if(serial->txBuffSize>0){

					serial->mode = MBS_MODE_GATEWAY;
//					MBG_SetTCPClientId(msg.id);
//					MBG_SetTCPClientKey(msg.key);
//					MBG_SetTCPHeader(&header);	// set the transaction id to track the msg later
	//					DEBUG_SPRINT_NL("dend:refId: %ld  gid: %ld, gkey: %d ",msg.id,  MBG_GetTCPClientId(), MBG_GetTCPClientKey());
//					DEBUG_SPRINT_NL("MBG Send ---- ");
					if(MBG_SendToSerial(serial->txBuff, serial->txBuffSize)== MB_OK){
						gVar.mbGateway.isReqSent = 1;
						gVar.mbGateway.isBusy = 1;

					}

						MBG_StartTimeout(&gVar.mbGateway.rplyTimer);

				}

//			}
		}else{
			//TODO: to write code later for wrong function code
		}
	}




}

void TH_MBGWCheckTimeOut(void){

	// timeout function-------------------------------------------------------
	if(MBG_CheckTimeout(&gVar.mbGateway.rplyTimer) == MB_OK){
		/*
		 * 1. reset timer
		 * 2. set false sub packet for the slave which is not responded to gateway msg que
		 * 3. go for waiting for next slave
		 * */
		//	1. reset timer
		MBG_ResetTimeout(&gVar.mbGateway.rplyTimer);
		gVar.mbGateway.isRegularFC = 0;			// unlock the receive event for the server which is locked by the regular function code

		//2. set false sub packet for the slave which is not responded to gateway msg que
		/* for regular function code------*/
		if(MBTCP_isFunCodeValid(gVar.mbGateway.custom.funCode)>0U) {
//			MBG_StopTimeout(&gVar.mbGateway.rplyTimer);

			gVar.mbGateway.rxBuffSize = 0;
			gVar.mbGateway.rxBuff[gVar.mbGateway.rxBuffSize++] = gVar.mbGateway.txBuff[MB_SA_INDEX];
			gVar.mbGateway.rxBuff[gVar.mbGateway.rxBuffSize++] = gVar.mbGateway.txBuff[MB_FC_INDEX]+MB_EFC_OFFSET;
			gVar.mbGateway.rxBuff[gVar.mbGateway.rxBuffSize++] = MB_EC_GTDFR_11;

			MBTCPS_ClientParams *client;
			uint8_t clientIndex = 0;
			/* 2. find the client who actually own the response*/
			if(MBTCPS_FindClientByIdAndPort(&gVar.mbTCPServer,
					gVar.mbGateway.clientId, gVar.mbGateway.clientKey) != MB_OK){
				gVar.mbSerial.isBusy = 0;
				gVar.mbGateway.isBusy = 0;
				gVar.mbGateway.isReqSent = 0;
				gVar.mbGateway.txBuffSize = 0;
				gVar.mbGateway.rxBuffSize = 0;
				gVar.mbSerial.txBuffSize = 0;
				gVar.mbSerial.rxBuffSize = 0;
				gVar.mbGateway.custom.funCode = 0;
				gVar.mbGateway.custom.rcvdCount = 0;
				gVar.mbGateway.custom.slvStartAddr = 0;
				gVar.mbGateway.custom.numOfSlv = 0;
				MBS_ClearRXBuffer(&gVar.mbSerial);
				MBS_ClearTXBuffer(&gVar.mbSerial);
				MBG_StopTimeout(&gVar.mbGateway.rplyTimer);
				return;
			}
			clientIndex = gVar.mbTCPServer.clientIndex;
			client = &gVar.mbTCPServer.clients[clientIndex];


			/*convert the packet RTU to TCP*/
			// 2 is added here because inside this function 2 is subtracted for 2 byte crc but we have no crc
			MBG_ConvertRTU2TCP(client->txBuff, gVar.mbGateway.rxBuff, gVar.mbGateway.rxBuffSize+2);

			client->txBuffSize = gVar.mbGateway.rxBuffSize;

			gVar.mbGateway.tcpHeader.length = client->txBuffSize;
			MBTCP_SetHeader(client->txBuff, &gVar.mbGateway.tcpHeader);		// set the tcp header to te tx buffer
			client->txBuffSize = client->txBuffSize + MBTCP_INDX_HEADER_MAX-1;
			gVar.mbSerial.isBusy = 0;
			MBTCPS_Send(client);
			gVar.mbGateway.isBusy = 0;
			gVar.mbGateway.isReqSent = 0;
			gVar.mbGateway.txBuffSize = 0;
			gVar.mbGateway.rxBuffSize = 0;
			gVar.mbSerial.txBuffSize = 0;
			gVar.mbSerial.rxBuffSize = 0;
			gVar.mbGateway.custom.funCode = 0;
			gVar.mbGateway.custom.rcvdCount = 0;
			gVar.mbGateway.custom.slvStartAddr = 0;
			gVar.mbGateway.custom.numOfSlv = 0;
			MBS_ClearRXBuffer(&gVar.mbSerial);
			MBS_ClearTXBuffer(&gVar.mbSerial);
			MBG_StopTimeout(&gVar.mbGateway.rplyTimer);
		}else if(gVar.mbGateway.custom.funCode == MB_CFC_WMHRCO_109){
			gVar.mbGateway.isBusy = 0;
			gVar.mbGateway.isReqSent = 0;
			gVar.mbGateway.txBuffSize = 0;
			gVar.mbGateway.rxBuffSize = 0;
			gVar.mbSerial.txBuffSize = 0;
			gVar.mbSerial.rxBuffSize = 0;
//			gVar.mbSerial.mode = MBS_MODE_NONE;
			gVar.mbGateway.custom.funCode = 0;
			gVar.mbGateway.custom.rcvdCount = 0;
			gVar.mbGateway.custom.slvStartAddr = 0;
			gVar.mbGateway.custom.numOfSlv = 0;
			MBG_StopTimeout(&gVar.mbGateway.rplyTimer);
			MBS_ClearRXBuffer(&gVar.mbSerial);
			MBS_ClearTXBuffer(&gVar.mbSerial);
		}
	}

	// time out function for regular function code
	if(MBG_CheckTimeout(&gVar.mbGateway.rplyTimRglrFC)==MB_OK){
		MBG_StopTimeout(&gVar.mbGateway.rplyTimRglrFC);
		gVar.mbGateway.isRegularFC = 0;
	}
}


/*Task handler for Modbus serial ---------------*/
/**
  * @brief  Modbus gatewayhandler function
  * @param  none
  * @retval none
  */

void TH_MBSerial(void){

	switch (gVar.mbSerial.mode) {
		case MBS_MODE_MASTER:{

			const uint8_t dataStart = 4; // for the received data from serial for custom function code

//			gVar.mbSerial.mode = MBS_MODE_NONE;
			if(gVar.mbSerial.isDataReceived && gVar.mbSerial.rxBuffSize > 0){
				gVar.mbSerial.isDataReceived = 0;
//				gVar.mbSerial.isBusy = 0;
				/*It will build & add to the pending list*/
				uint8_t brCastAddr = gVar.mbSerial.rxBuff[MB_SA_INDEX];
				uint8_t funCode = gVar.mbSerial.rxBuff[MB_FC_INDEX];
				uint8_t slvAddr = gVar.mbSerial.rxBuff[MB_FC_INDEX+1];

				// check the received msg
				if(brCastAddr == MB_BROADCAST_ADDR && (funCode == MB_CFC_RIRDI_107 || funCode == (MB_CFC_RIRDI_107+MB_EFC_OFFSET)) &&
						slvAddr >= gVar.mbRTUMaster.custom.slvStartAddr &&
						slvAddr <  (gVar.mbRTUMaster.custom.slvStartAddr+gVar.mbRTUMaster.custom.numOfSlv)){

//					uint8_t slvPackStart = MB_CUST_MASTER_DATA_START_INDEX + (MB_CUST_DATA_BLOCK_SIZE*gVar.mbRTUMaster.custom.rcvdCount);
					uint8_t slvPackStart = dataStart+MB_CUST_DATA_BLOCK_SIZE + (MB_CUST_DATA_BLOCK_SIZE*(slvAddr-gVar.mbRTUMaster.custom.slvStartAddr));//gVar.mbRTUMaster.custom.rcvdCount);
					const uint8_t startByte = 3; // start byte index in slave response
					// for the healthy function code
					if(funCode == MB_CFC_RIRDI_107){
						memcpy(&gVar.mbRTUMaster.rxBuff[slvPackStart], &gVar.mbSerial.rxBuff[startByte],MB_CUST_DATA_BLOCK_SIZE);
						gVar.mbRTUMaster.rxBuff[slvPackStart+MB_CUST_DATA_BLOCK_SIZE-2] = MB_CUST_ONLINE;
					}else if(funCode == (MB_CFC_RIRDI_107+MB_EFC_OFFSET)){ // for the error function code
						memset(&gVar.mbRTUMaster.rxBuff[slvPackStart], 0xFF,MB_CUST_DATA_BLOCK_SIZE);		// set 0xFF as there is no valid value
						/*now set the status byte*/
						gVar.mbRTUMaster.rxBuff[slvPackStart+MB_CUST_DATA_BLOCK_SIZE-2] = MB_CUST_ONLINE|MB_CUST_ERROR|gVar.mbSerial.rxBuff[MB_CUST_ERROR_BYTE_INDEX];
					}

					gVar.mbRTUMaster.rxBuffSize = slvPackStart+MB_CUST_DATA_BLOCK_SIZE;


					//A20231020: ADD consecutive healthy message of a slave
					gVar.mbRTUMaster.custom.rcvFailedCount[gVar.mbRTUMaster.custom.rcvdCount] = 0;

					gVar.mbRTUMaster.custom.rcvdCount++;
					MBRM_RestReplyTimer(&gVar.mbRTUMaster.replyTimer);




				}

				if(gVar.mbRTUMaster.custom.rcvdCount >= gVar.mbRTUMaster.custom.numOfSlv){
					MBRM_StopReplyTimer(&gVar.mbRTUMaster.replyTimer);
					gVar.mbRTUMaster.custom.rcvdCount = 0;

					gVar.mbSerial.isBusy = 0;
					gVar.mbRTUMaster.isBusy = 0;
				}

				MBS_ClearRXBuffer(&gVar.mbSerial);
				MBS_ClearTXBuffer(&gVar.mbSerial);
				gVar.mbSerial.rxBuffSize = 0;
				gVar.mbSerial.txBuffSize = 0;
//				gVar.mbSerial.isBusy = 0;
//				gVar.mbRTUMaster.isBusy = 0;

			}



			}break;
		case MBS_MODE_SLAVE:
//			DEBUG_SPRINT("MS_1");
			MBS_ProcessRcvdFrame(&gVar.mbSerial, &gVar.mbRTUSlave);
			break;
		case MBS_MODE_GATEWAY:
			TH_MBTCPSRecvRespGW(&gVar.mbTCPServer);
			break;
		default:
			break;
	}
}



uint16_t TH_AddEncoderDataToPacket(uint8_t *buff){
	uint16_t size = 0;
	if(gVar.dio.enc.configs.mode == ENC_MODE_QUAD){
		buff[size++] = 0;
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>24)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>16)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>8)&0xFF);
		buff[size++] =(uint8_t)(gVar.dio.enc.counterA&0xFF);
		buff[size++] = gVar.dio.enc.direction;
		uint32_t speed = ((uint32_t)gVar.dio.enc.speed)&0x00FFFFFF;
		buff[size++] = (uint8_t)((speed>>16)&0xFF);
		buff[size++] = (uint8_t)((speed>>8)&0xFF);
		buff[size++] = (uint8_t)(speed&0xFF);
	}else{
		buff[size++] = 1;
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>24)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>16)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterA>>8)&0xFF);
		buff[size++] =(uint8_t)(gVar.dio.enc.counterA&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterB>>24)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterB>>16)&0xFF);
		buff[size++] =(uint8_t)((gVar.dio.enc.counterB>>8)&0xFF);
		buff[size++] =(uint8_t)(gVar.dio.enc.counterB&0xFF);
	}
	return size;
}


uint16_t TH_AddCounterDataToPacket(uint8_t *buff){
	uint16_t size = 0;


		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterA>>24)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterA>>16)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterA>>8)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)(gVar.dio.enc.counterA&0xFF);
		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterB>>24)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterB>>16)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)((gVar.dio.enc.counterB>>8)&0xFF);
		buff[size++] = 0xFF; //(uint8_t)(gVar.dio.enc.counterB&0xFF);

	return size;
}


/*Task handler for Modbus Register ------------*/
/*
 * update the modbus discrete input register
 * */
//void TH_MBREG_UpdateDI(void){
//
//}
//
//
///*
// * update the modbus Coil output  register
// * */
//void TH_MBREG_UpdateCO(void){
//
//}
//
///*
// * update the modbus input register
// * */
//void TH_MBREG_UpdateIR(void){
//	const uint16_t startAddr = MB_IR_ADDR_OFFSET;
//	uint16_t value = 0xFFFF;
//	for(uint16_t address = startAddr; address < MB_IRA_MAX; address++){
//		switch (address) {
//			case MB_IRA_AI1_VALUE_12BIT:
//				value = gVar.ai1.rawValue;
//				break;
//			case MB_IRA_AI1_CURRENT:
//				value = gVar.ai1.current;
//				break;
//			case MB_IRA_AI1_VOLTAGE:
//				value = gVar.ai1.voltage;
//				break;
//			case MB_IRA_AI2_VALUE_12BIT:
//				value = gVar.ai2.rawValue;
//				break;
//			case MB_IRA_AI2_CURRENT:
//				value = gVar.ai2.current;
//				break;
//			case MB_IRA_AI2_VOLTAGE:
//				value = gVar.ai2.voltage;
//				break;
//			case MB_IRA_AI3_VALUE_12BIT:
//				value = gVar.ai3.rawValue;
//				break;
//			case MB_IRA_AI3_CURRENT:
//				value = gVar.ai3.current;
//				break;
//			case MB_IRA_AI3_VOLTAGE:
//				value = gVar.ai3.voltage;
//				break;
//			case MB_IRA_AI4_VALUE_12BIT:
//				value = gVar.ai4.rawValue;
//				break;
//			case MB_IRA_AI4_CURRENT:
//				value = gVar.ai4.current;
//				break;
//			case MB_IRA_AI4_VOLTAGE:
//				value = gVar.ai4.voltage;
//				break;
//
//				/*NModbus TCP Server------------*/
//				//Socket 0
////			case MB_IRA_MB_TCPS_SOCK0_SRC_PORT:
////				value = gVar.server.sock[0].srcPort;
////				break;
//			case MB_IRA_MB_TCPS_SOCK0_DEST_IP_1:
//				value = gVar.server.sock[0].destIP[0];
//				break;
//			case MB_IRA_MB_TCPS_SOCK0_DEST_IP_2:
//				value = gVar.server.sock[0].destIP[1];
//				break;
//			case MB_IRA_MB_TCPS_SOCK0_DEST_IP_3:
//				value = gVar.server.sock[0].destIP[2];
//				break;
//			case MB_IRA_MB_TCPS_SOCK0_DEST_IP_4:
//				value = gVar.server.sock[0].destIP[3];
//				break;
//			case MB_IRA_MB_TCPS_SOCK0_DEST_PORT:
//				value = gVar.server.sock[0].destPort;
//				break;
//			case MB_IRA_MB_TCPS_SOCK0_STATUS:
//				value = gVar.server.sock[0].status;
//				break;
//				//Socket 1
////			case MB_IRA_MB_TCPS_SOCK1_SRC_PORT:
////				value = gVar.server.sock[1].srcPort;
////				break;
//			case MB_IRA_MB_TCPS_SOCK1_DEST_IP_1:
//				value = gVar.server.sock[1].destIP[0];
//				break;
//			case MB_IRA_MB_TCPS_SOCK1_DEST_IP_2:
//				value = gVar.server.sock[1].destIP[1];
//				break;
//			case MB_IRA_MB_TCPS_SOCK1_DEST_IP_3:
//				value = gVar.server.sock[1].destIP[2];
//				break;
//			case MB_IRA_MB_TCPS_SOCK1_DEST_IP_4:
//				value = gVar.server.sock[1].destIP[3];
//				break;
//			case MB_IRA_MB_TCPS_SOCK1_DEST_PORT:
//				value = gVar.server.sock[1].destPort;
//				break;
//			case MB_IRA_MB_TCPS_SOCK1_STATUS:
//				value = gVar.server.sock[1].status;
//				break;
//				/*NModbus TCP client------------*/
//
//			case MB_IRA_MB_TCPC_SOCK_SRC_PORT:
//				value = gVar.client.sock[0].srcPort;
//				break;
//			case MB_IRA_MB_TCPC_SOCK_STATUS:
//				value = gVar.client.sock[0].status;
//				break;
//			default:
//				value = 0xFFFF;
//				break;
//		}
//
//			MB_REG_SetIR(address, value);
//	}
//}
//
///*
// * update the modbus holding register
// * */
//void TH_MBREG_UpdateHR(void){
//	const uint16_t startAddr = MB_HR_ADDR_OFFSET;
//	uint16_t value = 0xFFFF;
//	for(uint16_t address = startAddr; address < MB_HRA_MAX; address++){
//		switch (address) {
//
//			/*Digital Outputs 1 to 8 -------------*/
//
//			case MB_HRA_DO8_SRC:
//				value = gVar.dio.do_[DO_8].source;
//				break;
//			case MB_HRA_DO8_MODE:
//				break;
//			case MB_HRA_DO8_PWM_DUTY:
//				break;
//			case MB_HRA_DO7_SRC:
//				value = gVar.dio.do_[DO_7].source;
//				break;
//			case MB_HRA_DO7_MODE:
//				break;
//			case MB_HRA_DO7_PWM_DUTY:
//				break;
//			case MB_HRA_DO6_SRC:
//				value = gVar.dio.do_[DO_6].source;
//
//				break;
//			case MB_HRA_DO6_MODE:
//				break;
//			case MB_HRA_DO6_PWM_DUTY:
//				break;
//			case MB_HRA_DO5_SRC:
//				value = gVar.dio.do_[DO_5].source;
//
//				break;
//			case MB_HRA_DO5_MODE:
//				break;
//			case MB_HRA_DO5_PWM_DUTY:
//				break;
//			case MB_HRA_DO4_SRC:
//				value = gVar.dio.do_[DO_4].source;
//				break;
//			case MB_HRA_DO4_MODE:
//				break;
////			case MB_HRA_DO4_PWM_DUTY:
////				break;
//			case MB_HRA_DO3_SRC:
//				value = gVar.dio.do_[DO_3].source;
//				break;
//			case MB_HRA_DO3_MODE:
//				break;
////			case MB_HRA_DO3_PWM_DUTY:
////				break;
//			case MB_HRA_DO2_SRC:
//				value = gVar.dio.do_[DO_2].source;
//				break;
//			case MB_HRA_DO2_MODE:
//				break;
////			case MB_HRA_DO2_PWM_DUTY:
////				break;
//			case MB_HRA_DO1_SRC:
//				value = gVar.dio.do_[DO_1].source;
//				break;
//			case MB_HRA_DO1_MODE:
//				break;
////			case MB_HRA_DO1_PWM_DUTY:
////				break;
//
//			/*Analog Inputs-----------*/
//			case MB_HRA_AI1_MODE:
//				value = gVar.ai1.mode;
//				break;
//			case MB_HRA_AI2_MODE:
//				value = gVar.ai2.mode;
//				break;
//			case MB_HRA_AI3_MODE:
//				value = gVar.ai3.mode;
//				break;
//			case MB_HRA_AI4_MODE:
//				value = gVar.ai4.mode;
//				break;
//
//			/*Analog Outputs 1 to 4-------------*/
//			//Analog Output 1
//			case MB_HRA_AO1_MODE:
//				value = gVar.ao[AO_1].mode;
//				break;
//			case MB_HRA_AO1_SRC:
//				value = gVar.ao[AO_1].source;
//				break;
//			case MB_HRA_AO1_DUTY:
//				value = gVar.ao[AO_1].value;
//				break;
//
//				//Analog Output 2
//			case MB_HRA_AO2_MODE:
//				value = gVar.ao[AO_2].mode;
//				break;
//			case MB_HRA_AO2_SRC:
//				value = gVar.ao[AO_2].source;
//				break;
//			case MB_HRA_AO2_DUTY:
//				value = gVar.ao[AO_2].value;
//				break;
//
//				//Analog Output 3
//			case MB_HRA_AO3_MODE:
//				value = gVar.ao[AO_3].mode;
//				break;
//			case MB_HRA_AO3_SRC:
//				value = gVar.ao[AO_3].source;
//				break;
//			case MB_HRA_AO3_DUTY:
//				value = gVar.ao[AO_3].value;
//				break;
//
//				//Analog Output 4
//			case MB_HRA_AO4_MODE:
//				value = gVar.ao[AO_4].mode;
//				break;
//			case MB_HRA_AO4_SRC:
//				value = gVar.ao[AO_4].source;
//				break;
//			case MB_HRA_AO4_DUTY:
//				value = gVar.ao[AO_4].value;
//				break;
//
//				/*Net Information------------*/
//
//				//Device IP MAC
//			case MB_HRA_DEV_MAC_1:
//				value = gVar.w5500Chip.netInfo.mac[0];
//				break;
//			case MB_HRA_DEV_MAC_2:
//				value = gVar.w5500Chip.netInfo.mac[1];
//				break;
//			case MB_HRA_DEV_MAC_3:
//				value = gVar.w5500Chip.netInfo.mac[2];
//				break;
//			case MB_HRA_DEV_MAC_4:
//				value = gVar.w5500Chip.netInfo.mac[3];
//				break;
//			case MB_HRA_DEV_MAC_5:
//				value = gVar.w5500Chip.netInfo.mac[4];
//				break;
//			case MB_HRA_DEV_MAC_6:
//				value = gVar.w5500Chip.netInfo.mac[5];
//				break;
//				//Device IP Address
//			case MB_HRA_DEV_IP_1:
//				value = gVar.w5500Chip.netInfo.ip[0];
//				break;
//			case MB_HRA_DEV_IP_2:
//				value = gVar.w5500Chip.netInfo.ip[1];
//				break;
//			case MB_HRA_DEV_IP_3:
//				value = gVar.w5500Chip.netInfo.ip[2];
//				break;
//			case MB_HRA_DEV_IP_4:
//				value = gVar.w5500Chip.netInfo.ip[3];
//				break;
//
//				//Device Subnet mask
//			case MB_HRA_DEV_SN_1:
//				value = gVar.w5500Chip.netInfo.sn[0];
//
//				break;
//			case MB_HRA_DEV_SN_2:
//				value = gVar.w5500Chip.netInfo.sn[1];
//				break;
//			case MB_HRA_DEV_SN_3:
//				value = gVar.w5500Chip.netInfo.sn[2];
//				break;
//			case MB_HRA_DEV_SN_4:
//				value = gVar.w5500Chip.netInfo.sn[3];
//				break;
//				//Device Gateway
//			case MB_HRA_DEV_GW_1:
//				value = gVar.w5500Chip.netInfo.gw[0];
//				break;
//			case MB_HRA_DEV_GW_2:
//				value = gVar.w5500Chip.netInfo.gw[1];
//				break;
//			case MB_HRA_DEV_GW_3:
//				value = gVar.w5500Chip.netInfo.gw[2];
//				break;
//			case MB_HRA_DEV_GW_4:
//				value = gVar.w5500Chip.netInfo.gw[3];
//				break;
//	//		case MB_HRA_DEV_:
//	//			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
//	//				gVar.w5500Chip.netInfo.gw[3] = value;
//	//				FSExt_WriteIntNum(FSA_DEV_GW_4, value);
//	//			}else{
//	//				errStatus = MB_ERROR;
//	//			}
//	//			break;
//			default:
//				value = 0xFFFF;
//				break;
//		}
//
//		MB_REG_SetHR(address, value);
//
//	}
//}
//


/*Task handler for digital IO ------------ */
/**
  * @brief  Digital output handler function
  * @param  none
  * @retval none
  */
//void TH_DigitalOut(void){
//	for(uint8_t i = DO_1; i < DO_MAX; i ++){
//		DigitalOut *do_;
//		do_ =  &gVar.dio.do_[i];
//
//		switch (do_->source) {
//			case DO_SRC_MODBUS:
//				do_->mode = DO_MODE_FOLLOW;
//
//				break;
//			default:
//				// for digital input 1 to 8
//				if(do_->source >= DO_SRC_DI_1 && do_->source <= DO_SRC_DI_8 ){
//					switch (do_->mode) {
//						case DO_MODE_FOLLOW:
//							do_->state = gVar.dio.di[do_->source].state;
//							break;
//						case DO_MODE_TOGGLE:
//							//DEBUG_PRINT("Dinp-2\r\n");
//							if(gVar.dio.di[do_->source].state && do_->isToggle == 0U){
//								do_->isToggle = 1U;
//								do_->state = (do_->state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET);
//							}else if(!gVar.dio.di[do_->source].state){
//								do_->isToggle = 0U;
//							}
//							break;
//						default:
//							break;
//					}
//				}else{ // if invalid source
//					do_->state = GPIO_PIN_RESET;
//				}
//				break;
//		}
////		DO_Write(do_);											//DEPRECATED
//		DIO_WRITE_PIN(do_->port, do_->pin, do_->state);			//REPLACED
//	}
//}
//



/*Analog Putputs---------------------*/
void TH_AnalogOutput(void){

	for(gVar.aoId = AO_1; gVar.aoId < AO_MAX; gVar.aoId++) {
		AO_CalculatePWM(&gVar.ao[gVar.aoId]);
		AO_UpdatePWM(&gVar.ao[gVar.aoId]);
	}
}





/*Analog Inputs---------------------*/
void TH_AOMap2DIandPID(AnalogOut *ao, DigitalIn_t *di, PID *pid){

	switch (ao->source) {
		case AO_PWM_SRC_MODBUS:
			ao->value = ao->mbValue;
			break;
		case AO_PWM_SRC_PID:
			pid->setpoint =  ao->mbValue;
			if(di->pulseInf.period>0){
				pid->measurement = (float)(di->pulseInf.periodMin*AO_PWM_RESOLUTION/di->pulseInf.period);//(float)ConvPercentToValue(ConvToPercent((double)di->pulseInf.period, (double)di->pulseInf.periodMax), (double)AO_PWM_RESOLUTION);
				pid->measurement = (float)TH_ScalePriedTo12Bits(di->pulseInf.period, di->pulseInf.periodMin);
				if(pid->measurement > AO_PWM_RESOLUTION) pid->measurement = AO_PWM_RESOLUTION;
			}else{
				pid->measurement = 0.0f;		// when the motor is not running period shod by infinity, but here it will be max value instead of infinity
			}


			/*
			 * if measurement value <= 50% of setpoint -> maintain the output at 50% of setpoint
			 * else, measurement value > 50% 0f setpoint -> maintain the output at setpoint
			 * */
			if(pid->measurement <= pid->setpoint && pid->measurement > pid->setpoint*0.5){
//				DEBUG_SPRINT_NL("PID: 1");
				ao->value = pid->setpoint;

			}else if(pid->measurement <= pid->setpoint*0.5){
//				DEBUG_SPRINT_NL("PID: 2");
				pid->error = pid->setpoint*0.5 - pid->measurement;
				PID_Proportional(pid);
				ao->value += pid->output_p;
				if(ao->value > AO_PWM_RESOLUTION){
					ao->value = AO_PWM_RESOLUTION;
				}
			}else{

//				DEBUG_SPRINT_NL("PID: 3");
//				PID_Controller(pid);
//				ao->value = ConvPercentToValue(pid->output_pid, AO_PWM_RESOLUTION);

				pid->error = pid->setpoint - pid->measurement;
				PID_Proportional(pid);
				ao->value += pid->output_p;
				if(ao->value > AO_PWM_RESOLUTION){
					ao->value = AO_PWM_RESOLUTION;
				}else if(ao->value < pid->setpoint){
					ao->value = pid->setpoint;
				}

			}

			break;
		default:
			ao->value  = 0;
			break;
	}
}




/*IOExtender implemented in non-blocking way*/
void TH_IOExtender(void){
	static uint32_t tick = 0;
	static IOEXState stat = {0};
	static uint8_t lastStat = 0;
	if(PCA_GetWriteCmpltFlag(&gVar.ioExt) == PCA_WRITE_COMPLETED){
		for(uint8_t i = 0; i<IOEXT_MAX_INDEX; i++){
			switch (i) {
				case IOEXT_AO_1_MODE_INDEX:
					stat.bit.ao1Mode = (gVar.ao[i].mode == AIO_MODE_VOLT);
					break;
				case IOEXT_AO_2_MODE_INDEX:
					stat.bit.ao2Mode = (gVar.ao[i].mode == AIO_MODE_VOLT);
					break;
				case IOEXT_AO_3_MODE_INDEX:
					stat.bit.ao3Mode = (gVar.ao[i].mode == AIO_MODE_VOLT);
					break;
				case IOEXT_AO_4_MODE_INDEX:
					stat.bit.ao4Mode = (gVar.ao[i].mode == AIO_MODE_VOLT);
					break;
				case IOEXT_RS485_RX_LED_INDEX:{
						static uint32_t lastCount = 0;
						if((gVar.mbSerial.rxCount >= (lastCount + RS485_RX_LED_BLINK_ON_RX_COUNT)) && (gVar.mbSerial.rxCount!=0)){
							stat.bit.rs485RxLed = 1;
							lastCount = gVar.mbSerial.rxCount;
						}else{
							stat.bit.rs485RxLed = 0;
						}

						gVar.led[i-AO_MAX].state = stat.bit.rs485RxLed;
					}break;
				case IOEXT_RS485_TX_LED_INDEX:{
						static uint32_t lastCount = 0;
						if((gVar.mbSerial.txCount >= (lastCount + RS485_TX_LED_BLINK_ON_TX_COUNT))&& (gVar.mbSerial.txCount!=0)){
							stat.bit.rs485TxLed = 1;
							lastCount = gVar.mbSerial.txCount;
						}else{
							stat.bit.rs485TxLed = 0;
						}

						gVar.led[i-AO_MAX].state = stat.bit.rs485TxLed;
					}break;
				case IOEXT_LED_1_INDEX:
					stat.bit.led1 = gVar.led[i-AO_MAX].state > 0;
					break;
				case IOEXT_LED_2_INDEX:
					stat.bit.led2 = gVar.led[i-AO_MAX].state > 0;
					break;
				default:
					break;
			}
		}

		if(stat.value != lastStat){
			if(PCA_WritePinAllIT(&gVar.ioExt, stat.value) != PCA_ERR_NONE){

				//TODO: PCA write err: to implement later
			}
		}

		tick = HAL_GetTick();
	}else{
		/*Check the write complete time out*/
		uint32_t timeout = 10; //ms
		if((HAL_GetTick() - tick) >= timeout){
			gVar.ioExt.flag.writeCmplt = PCA_WRITE_COMPLETED;
			//TODO: PCA write timeout err: to implement later
			tick = HAL_GetTick();
		}
	}

}

/*
 * Scale the motor encoder signal period to 12 bit value
 * if period > 220, formula: (1000000000 X period^-2.554)
 * else if, 140 < period < 220 formula: (2000000 x period^-1.4)
 * else, formula: (period at 4095 x 4095) / period
 *
 * these formulas,coefficient & power are generated in excel sheet

 * */
float TH_ScalePriedTo12Bits(uint32_t period, uint32_t periodMin){
	if(period >= DEF_PRD2FB_FRML_A_LOWER_LIMIT){
		return (DEF_PRD2FB_FRML_A_COEFF*powl(period, DEF_PRD2FB_FRML_A_POW));
	}else if(period >= DEF_PRD2FB_FRML_B_LOWER_LIMIT && period <DEF_PRD2FB_FRML_B_UPPER_LIMIT){
		return (DEF_PRD2FB_FRML_B_COEFF*powl(period, DEF_PRD2FB_FRML_B_POW));
	}else{
		return ((periodMin * AO_PWM_RESOLUTION)/period);
	}
}


/*
 * PID controller
 * */

void TH_PIDControl(PID *pid){

//	if(pid->enable){

		PID_Controller(pid);									// apply pid controller
//	}else{
//		pid->output_pid = 0U;
//	}
}

/**
  * @brief  reset all the parameters to factory default values and restart
  * @retval none
  */
void TH_FactoryResetButton(void){
	static uint32_t tick = 0U;
	if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin)){
		if((HAL_GetTick() - tick) >= DI_FCTRY_RST_DELAY){
			FactorySettings();
			HAL_Delay(1);
			RestartMCU();
		}
	}else{
		tick = HAL_GetTick();
	}
}





/* This function diagnosis Modbus RTU bus
 * */
void TH_MBRTUBusErrDiag(void){

	if(gVar.mbSerial.errorCode & HAL_UART_ERROR_PE){
		if(gVar.diag.rtuBus.peCount < DEF_U16_MAX_VALUE)
			gVar.diag.rtuBus.peCount++;
	}
	if(gVar.mbSerial.errorCode & HAL_UART_ERROR_NE){
		if(gVar.diag.rtuBus.neCount < DEF_U16_MAX_VALUE)
			gVar.diag.rtuBus.neCount++;
	}
	if(gVar.mbSerial.errorCode & HAL_UART_ERROR_FE){
		if(gVar.diag.rtuBus.feCount < DEF_U16_MAX_VALUE)
			gVar.diag.rtuBus.feCount++;
	}
	if(gVar.mbSerial.errorCode & HAL_UART_ERROR_ORE){
		if(gVar.diag.rtuBus.oreCount < DEF_U16_MAX_VALUE)
			gVar.diag.rtuBus.oreCount++;
//		if(gVar.mbSerial.error.oreCount <= 3){
//			if(!TC_IsTimeout(&gVar.mbSerial.errEventTimer)){
//				__HAL_UART_CLEAR_OREFLAG(&huart6);
//				__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);		// enable uart receive interrupt
//			}
//		}
	}
	if(gVar.mbSerial.errorCode & HAL_UART_ERROR_DMA){
		if(gVar.diag.rtuBus.dteCount < DEF_U16_MAX_VALUE)
			gVar.diag.rtuBus.dteCount++;
	}

//	gVar.diag.rtuBus.dmaRxErrCode = gVar.mbSerial.dmaRxErrCode;
//	gVar.diag.rtuBus.dmaTxErrCode = gVar.mbSerial.dmaTxErrCode;
	gVar.mbSerial.errorCode = 0;
}


/* This function diagnosis w5500 SPI
 * */
void TH_W5500SPIErrDiag(void){

	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_MODF){
		if(gVar.diag.w5500Spi.modfeCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.modfeCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_CRC){
		if(gVar.diag.w5500Spi.crceCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.crceCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_OVR){
		if(gVar.diag.w5500Spi.oreCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.oreCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_FRE){
		if(gVar.diag.w5500Spi.feCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.feCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_DMA){
		if(gVar.diag.w5500Spi.dteCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.dteCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_FLAG){
		if(gVar.diag.w5500Spi.flgeCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.flgeCount++;
	}
	if(gVar.w5500Chip.spi.errorCode & HAL_SPI_ERROR_ABORT){
		if(gVar.diag.w5500Spi.abreCount < DEF_U16_MAX_VALUE)
			gVar.diag.w5500Spi.abreCount++;
	}
//	gVar.diag.w5500Spi.dmaRxErrCode = gVar.w5500Chip.spi.dmaRxErrCode;
//	gVar.diag.w5500Spi.dmaTxErrCode = gVar.w5500Chip.spi.dmaTxErrCode;
	gVar.w5500Chip.spi.errorCode = 0;
}


/*Analog Inputs---------------------*/
void TH_AnalogInput(void){

#ifdef USE_AI_ADC_DMA
	gVar.aiId++;
	if(gVar.aiId >= AI_MAX) {gVar.aiId = AI_1;}
	gVar.ai[gVar.aiId].rawValue = gVar.aiBuffDMA[gVar.aiId];		// map the AI data to the buffer
#endif
	for(gVar.aiId = AI_1;  gVar.aiId < AI_MAX; gVar.aiId++){
		AI_Read(&gVar.ai[gVar.aiId]);
	}


}




