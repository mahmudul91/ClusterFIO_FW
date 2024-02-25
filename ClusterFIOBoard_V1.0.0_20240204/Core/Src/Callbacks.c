/*
 * OD_Callback.c
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#include <Callbacks.h>
#include <Task_Handler.h>
#include "Debug.h"

#include "IOLink.h"
#include "TimeStamp.h"
#include "GlobalVars.h"
#include "IOLink_Time.h"

/*old------*/
#include "Callbacks.h"
#include "main.h"
#include "Settings.h"
#include "MB_Addresses.h"
#include "MB_Serial.h"
#include "MB_Diagnostics.h"
#include "Debug.h"
#include "Storage_Address.h"
#include "Flash_Storag.h"
#include "Config.h"
#include "stdio.h"
#include "Server.h"
#include "MB_TCP.h"
#include "Scheduler.h"
#include "Tools.h"
#include <DigitalIn.h>
#include <MB_Handler.h>
#include "socket.h"
#include "MB_TCPClient.h"
#include "MB_Register.h"
#include "EEPROM_StorageExt.h"
#include "Socket.h"
#include "Task_Handler.h"
#include "MB_Tools.h"
#include "EEPROM_AT24CXX.h"


extern GlobalVar_t gVar;
extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c4;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern SDRAM_HandleTypeDef hsdram1;

/*Old---------*/
//extern DebugStruct dbug;

extern TimeStamp timStamp;



/*Debug callback----------------------------*/


/*L6360 IOLink i2c Read/write callback----------*/
L6360_Status CB_L6360_I2CWrite(uint8_t devAddress, uint8_t* data, uint16_t len);
L6360_Status CB_L6360_I2CRead(uint8_t devAddress, uint8_t* data, uint16_t len);

/*Services for L6360 Control pins-------------*/
void CB_L6360_LplusPin(L6360_IOLink_hw_t* l6360, L6360_PinState state);
void CB_L6360_CQPin(L6360_IOLink_hw_t* l6360, L6360_PinState state);
void CB_L6360_ResetPin(L6360_IOLink_hw_t* l6360, L6360_PinState state);

/*L6360 IOLink i2c Read/write callback----------*/
L6360_Status CB_L6360_UartConfig(L6360_IOLink_hw_t* l6360, L6360_RWMode rwMode, uint32_t* baudrate);
L6360_Status CB_L6360_UartTX(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout);
L6360_Status CB_L6360_UartRX(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout);
/*L6360 time source callback services--------------*/
uint64_t CB_L6360_TimeSrc(void);

/*IOLink callback-----------------------------*/
uint64_t CB_IOL_TimeSource(void);



/*old---------*/
/*
 * Callback functions for the modbus register
 * */
//uint8_t CB_MB_REG_ChangeDICallback(uint32_t address, uint16_t value);
uint8_t CB_MB_REG_ChangeCOCallback(uint32_t address, uint16_t value);
uint8_t CB_MB_REG_ChangeIRCallback(uint32_t address, uint16_t value);
uint8_t CB_MB_REG_ChangeHRCallback(uint32_t address, uint16_t value);


uint8_t CB_MB_readDICallback(MB_DIAddrEnum address, uint8_t *value);
uint8_t CB_MB_writeCOCallback( MB_CoilAddrEnum address, uint8_t value);
uint8_t CB_MB_readCOCallback( MB_CoilAddrEnum address, uint8_t *value);
uint8_t CB_MB_readIRCallback( MB_IRAddrEnum address, uint16_t *value);
uint8_t CB_MB_writeHRCallback( MB_HRAddrEnum address, uint16_t value);
uint8_t CB_MB_readHRCallback( MB_HRAddrEnum address, uint16_t *value);
uint8_t CB_MB_maskWriteHRCallbackS(MB_HRAddrEnum address, uint16_t andMask, uint16_t orMask);
uint8_t CB_MB_CmpltTxCallback(MBS_Serial *mbSerial, uint16_t address, uint16_t value);
uint8_t CB_MB_CmoltRxCallback(MBS_Serial *mbSerial);
uint8_t CB_MB_diagSubFuncCallback(uint16_t subFuncCode, uint16_t value);
uint8_t CB_MB_ChangSACallback(uint8_t slaveAddress);




/*
 * Callback functions for the Server action
 * */
uint8_t CB_Sock_ConnCallback(Socket *sock);					// callback for client connect event
uint8_t CB_Sock_DisconnCallback(Socket *sock);				// callback for client disconnect event
uint8_t CB_Sock_RecvCallback(Socket *sock);					// callback for data received event of the client
uint8_t CB_Sock_SendCmpltCallback(Socket *sock);			// callback for send complete for the socket
uint8_t CB_Sock_ErrorCallback(Socket *sock, int32_t error);			// callback for send complete for the socket

uint8_t CB_MBTCPS_SendCallback(MBTCPS_ClientParams *client);					// callback for client connect event
uint8_t CB_MBTCPS_SendExcpCallback(MBTCPS_ClientParams *client);					// callback for client connect event


void CB_DO_StateChangeCallback(DigitalOut_t *dout);

/*Callback function for digital input as a counter using external interrupt*/
void CB_DI_CounterExtiCallback(uint16_t pin);
void CB_DI_StateChangeCallback(DigitalIn_t *di); // digital input state change callback






void CB_EEPROM_ErrorCallback(uint8_t error);


/*Initialization of callback-------------------------*/
/* It COntains all attach functions for the callback
 * */
void CB_Init(void){

	CB_DIO_InitCallback();
	CB_MB_InitCallback();



	/*L6360 IOLink i2c Read/write callback----------*/
	L6360_AttachTimerCallback(&CB_L6360_TimeSrc);
	for(uint8_t i = 0; i < IOL_PI_MAX; i++){
		L6360_I2CAttachWriteCallback(&gVar.iolm.port[i], &CB_L6360_I2CWrite);
		L6360_I2CAttachReadCallback(&gVar.iolm.port[i], &CB_L6360_I2CRead);
		L6360_UartAttachConfigCallback(&gVar.iolm.port[i], &CB_L6360_UartConfig);
		L6360_UartAttachTXCallback(&gVar.iolm.port[i], &CB_L6360_UartTX);
		L6360_UartAttachRXCallback(&gVar.iolm.port[i], &CB_L6360_UartRX);
		L6360_AttachLplusPinCallback(&gVar.iolm.port[i], &CB_L6360_LplusPin);
		L6360_AttachCQPinCallback(&gVar.iolm.port[i], &CB_L6360_CQPin);
		L6360_AttachResetPinCallback(&gVar.iolm.port[i], &CB_L6360_ResetPin);
	}

	/*IOLInk callbacks-------------------------------*/
	IOL_Timer_AttachTimeSourceCallback(&CB_IOL_TimeSource);


}
/*Debug callback----------------------------*/
void CB_DBG_Tx(uint8_t* data, uint16_t len){

	uint32_t timeout = (11000*len/huart3.Init.BaudRate)+1;
	HAL_UART_Transmit(&huart3, data, len, timeout);

}

/*L6360 IOLink i2c Read/write callback----------*/
/**  @brief  L6360 IOlink chip i2c write callback function
  * @param  devAddress: device address
  * 		data: data pointer
  * 		len: data length
  * @retval error status
  */
L6360_Status CB_L6360_I2CWrite(uint8_t devAddress, uint8_t* data, uint16_t len){
//	return (L6360_Status)HAL_I2C_Master_Transmit(gVar.iolm.hi2c, devAddress,  data, len, (len+1)*2);
	return L6360_OK;
}

/** @brief  L6360 IOlink chip i2c write callback function
  * @param  devAddress: device address
  * 		data: data pointer
  * 		len: data length
  * @retval error status
  */
L6360_Status CB_L6360_I2CRead(uint8_t devAddress, uint8_t* data, uint16_t len){
//	return (L6360_Status)HAL_I2C_Master_Receive(gVar.iolm.hi2c, devAddress, data, len, (len+1)*2);
	return L6360_OK;
}

/*Services for L6360 Control pins-------------*/
void CB_L6360_LplusPin(L6360_IOLink_hw_t* l6360, L6360_PinState state){
	HAL_GPIO_WritePin(l6360->ctrlPins.lpEnPort, l6360->ctrlPins.lpEnPin, state);
}
void CB_L6360_CQPin(L6360_IOLink_hw_t* l6360, L6360_PinState state){
	HAL_GPIO_WritePin(l6360->ctrlPins.cqEnPort, l6360->ctrlPins.cqEnPin, state);
}
void CB_L6360_ResetPin(L6360_IOLink_hw_t* l6360, L6360_PinState state){
	HAL_GPIO_WritePin(l6360->ctrlPins.rstPort, l6360->ctrlPins.rstPin, state);
}

/*L6360 IOLink UART Read/write callback----------------------------*/

/** @brief  UART 1 configuration callback function 1 for L6360 IOlink chip 1
  * @param  data: data pointer
  * 		len: data length
  * 		timeout: timeout value for transmission
  * @retval error status
  */
L6360_Status CB_L6360_UartConfig(L6360_IOLink_hw_t* l6360, L6360_RWMode rwMode, uint32_t* baudrate){
	if(rwMode == L6360_RW_MODE_READ){
		/*Read mode*/
		*baudrate = l6360->uart.huart->Init.BaudRate;
	}else{
		/*Read mode*/
		l6360->uart.huart->Init.BaudRate = *baudrate;
	}
	HAL_StatusTypeDef status = HAL_UART_Init(l6360->uart.huart);
		switch (status) {
			case HAL_OK:
				return L6360_OK;
				break;
			case HAL_ERROR:
				return L6360_ERROR;
				break;
			case HAL_BUSY:
				return L6360_BUSY;
				break;
			case HAL_TIMEOUT:
				return L6360_TIMEOUT;
				break;
			default:
				return L6360_UNKNOWN;
				break;
		}
}

/** @brief  UART TX callback function for L6360 IOlink chip
  * @param  data: data pointer
  * 		len: data length
  * 		timeout: timeout value for transmission
  * @retval error status
  */
L6360_Status CB_L6360_UartTX(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout){

	HAL_StatusTypeDef status = HAL_UART_Transmit(l6360->uart.huart, data, len, timeout);
//	DBG_SPRINT_NL("txd: %02X, %02X, stat: %d", data[0], data[1], status);
	switch (status) {
		case HAL_OK:
			return L6360_OK;
			break;
		case HAL_ERROR:
			return L6360_ERROR;
			break;
		case HAL_BUSY:
			return L6360_BUSY;
			break;
		case HAL_TIMEOUT:
			return L6360_TIMEOUT;
			break;
		default:
			return L6360_UNKNOWN;
			break;
	}

}

/** @brief  UART RX callback function for L6360 IOlink chip
  * @param  data: data pointer
  * 		len: data length
  * 		timeout: timeout value for receiveing
  * @retval error status
  */
L6360_Status CB_L6360_UartRX(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout){
	HAL_StatusTypeDef status = HAL_UART_Receive(l6360->uart.huart, data, len, timeout);
	switch (status) {
		case HAL_OK:
			return L6360_OK;
			break;
		case HAL_ERROR:
			return L6360_ERROR;
			break;
		case HAL_BUSY:
			return L6360_BUSY;
			break;
		case HAL_TIMEOUT:
			return L6360_TIMEOUT;
			break;
		default:
			return L6360_UNKNOWN;
			break;
	}
return L6360_OK;
}



/*L6360 time source callback services--------------*/
/** @brief  l6360 time source
  * @param  none
  * @retval time in us
  */
uint64_t CB_L6360_TimeSrc(void){
	//TODO: HAL_GetTick is ms timer, so need to change with us timer

	return TS_GetUS(&gVar.ts);	//
}

/*IOLInk callback-------------------------------*/
/** @brief  IOLink time source
  * @param  none
  * @retval time in us
  */
uint64_t CB_IOL_TimeSource(void){
	return TS_GetUS(&gVar.ts);	//
}






/*External Interrupt callback---------------------------*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	switch (GPIO_Pin) {
		/*Interrupt handler for L6360 IOlink Master chip*/
		case IOL1_IRQ_Pin:
			L6360_I2CIRQHandler(&gVar.iolm.port[IOL_PI_0]);
			break;
		case IOL2_IRQ_Pin:
			L6360_I2CIRQHandler(&gVar.iolm.port[IOL_PI_1]);
			break;
		case IOL3_IRQ_Pin:
			L6360_I2CIRQHandler(&gVar.iolm.port[IOL_PI_2]);
			break;
		case IOL4_IRQ_Pin:
			L6360_I2CIRQHandler(&gVar.iolm.port[IOL_PI_3]);
			break;
		default:
			break;
	}

	//	DI_CounterExtiCallback(GPIO_Pin);
		if(gVar.dio.di[DI_7].config.pin == GPIO_Pin){
//			CB_DI_CounterExtiCallback(GPIO_Pin); //TODO: to uncomment
		}else if(gVar.w5500Chip.spi.int_pin == GPIO_Pin){
			gVar.w5500Chip.intrptFlag = 1;
	//		W5500_ReadInterrupt(&gVar.w5500Chip);
			// read interrupt for server sockets
			for(uint8_t i = 0; i < SERVER_SOCK_MAX; i++){
	//			Sock_ReadInterrupt(&gVar.server.sock[i]);
			}
			// read interrupt for client sockets
			for(uint8_t i = 0; i < CLIENT_SOCK_MAX; i++){
	//			Sock_ReadInterrupt(&gVar.client.sock[i]);
	//			DBG_SPRINT_NL("Sock-%d: intrpt 0x%02x",gVar.client.sock[i].id ,gVar.client.sock[i].intr.val);
			}
	//		W5500_ClearAllInterrupt();
	//		uint8_t initState = HAL_GPIO_ReadPin(gVar.w5500Chip.spi.int_port, gVar.w5500Chip.spi.int_pin);
	//		DBG_SPRINT_NL("w5500 interrupt !: %d", initState);
		}

}

/*Timer period elapsed callback*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	/*For time stamp*/
	if (htim->Instance == TIM24){

		TS_CatchOVF(&gVar.ts);
	}
}



/*old---------*/


void CB_InitErrorCallback(void){
	EEPROM_AT24_AttachErrorCallback(&CB_EEPROM_ErrorCallback);
}


/**
  * @brief  initialize callback for digital inpur counter
  */
void CB_DIO_InitCallback(void){
	DI_AttachStateChangeCallback(&CB_DI_StateChangeCallback);
	DI_AttachCounterExtiCallback(&CB_DI_CounterExtiCallback);


//	DO_AttachStateCHangeCallback(&CB_DO_StateChangeCallback);
}


/**
  * @brief callback initialize
  */
void CB_MB_InitCallback(void){
//	MB_REG_AttachChangeDIRegCallback(&CB_MB_REG_ChangeDICallback);
//	MB_REG_AttachChangeCORegCallback(&CB_MB_REG_ChangeCOCallback);
//	MB_REG_AttachChangeIRRegCallback(&CB_MB_REG_ChangeIRCallback);
//	MB_REG_AttachChangeHRRegCallback(&CB_MB_REG_ChangeHRCallback);


	MB_attachDICallback(&CB_MB_readDICallback);
	MB_attachCOCallback(&CB_MB_writeCOCallback, &CB_MB_readCOCallback);
	MB_attachIRCallback(&CB_MB_readIRCallback);
	MB_attachHRCallback(&CB_MB_writeHRCallback, &CB_MB_readHRCallback);
	MB_attachMaskWHRCallback(&CB_MB_maskWriteHRCallbackS);

	MB_DIAG_attachCallback(&CB_MB_diagSubFuncCallback);
	MB_attachChangeSACllback(&CB_MB_ChangSACallback);

	MBS_attachCmpltTxCallback(&CB_MB_CmpltTxCallback);
	MBS_attachCmpltRxCallback(&CB_MB_CmoltRxCallback);


	MB_CUST_attachProcess109ReqAODOCallback(&CB_MB_CUST_Process109ReqAODO);
	MB_CUST_attachBuild107RespAIDICallback(&CB_MB_CUST_Build107RespAIDI);

}

/**
  * @brief callback initialize
  */
void CB_Sock_InitCallback(void){
	Sock_AttachConnCallback(&CB_Sock_ConnCallback);
	Sock_AttachDisconnCallback(&CB_Sock_DisconnCallback);
	Sock_AttachRecvCallback(&CB_Sock_RecvCallback);
	Sock_AttachSendCmpltCallback(&CB_Sock_SendCmpltCallback);
	Sock_AttachErrorCallback(&CB_Sock_ErrorCallback);
}


/**
  * @brief callback initialize
  */
void CB_MBTCPS_InitCallback(void){
	MBTCPS_AttachSendCallback(&CB_MBTCPS_SendCallback);
	MBTCPS_AttachSendExcpCallback(&CB_MBTCPS_SendExcpCallback);
}




/**
  * @brief digital input as a counter using external interrupt
  * @param pin: pin number where the interrupt happened
  * @retval return error if an exception happen
  */
void CB_DI_StateChangeCallback(DigitalIn_t *di){
	switch (di->index) {
		case DI_1:
			MB_REG_SetDI(MB_DIA_DI1_STATUS, di->state);
			break;
		case DI_2:
			MB_REG_SetDI(MB_DIA_DI2_STATUS, di->state);
			break;
		case DI_3:
			MB_REG_SetDI(MB_DIA_DI3_STATUS, di->state);
			break;
		case DI_4:
			MB_REG_SetDI(MB_DIA_DI4_STATUS, di->state);
			break;
		case DI_5:
			MB_REG_SetDI(MB_DIA_DI5_STATUS, di->state);
			break;
		case DI_6:
			MB_REG_SetDI(MB_DIA_DI6_STATUS, di->state);
			break;
		case DI_7:
			MB_REG_SetDI(MB_DIA_DI7_STATUS, di->state);
			break;
		case DI_8:
			MB_REG_SetDI(MB_DIA_DI8_STATUS, di->state);
			break;
		default:
			break;
	}
}

/**
  * @brief digital input as a counter using external interrupt
  * @param pin: pin number where the interrupt happened
  * @retval return error if an exception happen
  */
void CB_DI_CounterExtiCallback(uint16_t pin){
	for(DI_Index i = (DI_1-1); i < (DI_MAX-DI_1); i++){
		if(gVar.dio.di[i].config.pin == pin){
			DI_CounterByExtI(&gVar.dio.di[i]);
			break;
		}
	}

}





/*Modbus Register callbacks---------------------------------------*/
/**
  * @brief calback function for discrete input
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readDICallback(MB_DIAddrEnum address, uint8_t *value){
	uint8_t errorStatus = MB_OK;

	switch (address) {
		case MB_DIA_DI1_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_1].config.port, gVar.dio.di[DI_1].config.pin);
			break;
		case MB_DIA_DI2_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_2].config.port, gVar.dio.di[DI_2].config.pin);
			break;
		case MB_DIA_DI3_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_3].config.port, gVar.dio.di[DI_3].config.pin);
			break;
		case MB_DIA_DI4_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_4].config.port, gVar.dio.di[DI_4].config.pin);
			break;
		case MB_DIA_DI5_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_5].config.port, gVar.dio.di[DI_5].config.pin);
			break;
		case MB_DIA_DI6_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_6].config.port, gVar.dio.di[DI_6].config.pin);
			break;
		case MB_DIA_DI7_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_7].config.port, gVar.dio.di[DI_7].config.pin);
			break;
		case MB_DIA_DI8_STATUS:
			*value = HAL_GPIO_ReadPin(gVar.dio.di[DI_8].config.port, gVar.dio.di[DI_8].config.pin);
			break;
		default:

			break;
	}


//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, *value);
	return errorStatus;
}




/**
  * @brief calback function for coil output
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_writeCOCallback(MB_CoilAddrEnum address, uint8_t value){
	uint8_t errStatus = MB_OK;
	/*------Write your code here------*/


	switch (address) {
		case MB_COA_DO1_ENABLE:
			gVar.dio.dout[DO_1].state = value>0;
			DO_Write(&gVar.dio.dout[DO_1]);
			break;
		case MB_COA_DO2_ENABLE:
			gVar.dio.dout[DO_2].state = value>0;
			DO_Write(&gVar.dio.dout[DO_2]);
			break;
		case MB_COA_DO3_ENABLE:
			gVar.dio.dout[DO_3].state = value>0;
			DO_Write(&gVar.dio.dout[DO_3]);
			break;
		case MB_COA_DO4_ENABLE:
			gVar.dio.dout[DO_4].state = value>0;
			DO_Write(&gVar.dio.dout[DO_4]);
			break;
		case MB_COA_DO5_ENABLE:
			gVar.dio.dout[DO_5].state = value>0;
			DO_Write(&gVar.dio.dout[DO_5]);
			break;
		case MB_COA_DO6_ENABLE:
			gVar.dio.dout[DO_6].state = value>0;
			DO_Write(&gVar.dio.dout[DO_6]);
			break;
			/*D20240213: DO7 & 8 are deprecated*/
//		case MB_COA_DO7_ENABLE:
//			gVar.dio.dout[DO_7].state = value>0;
//			DO_Write(&gVar.dio.dout[DO_7]);
//			break;
//		case MB_COA_DO8_ENABLE:
//			gVar.dio.dout[DO_8].state = value>0;
//			DO_Write(&gVar.dio.dout[DO_8]);
//			break;

			/*Analog Outputs 1 to 4-------------*/
		case MB_COA_AO1_ENABLE:
			gVar.ao[AO_1].enabled = value>0;
			if(FSExt_WriteIntNum(SA_AO1_ENABLE, (uint32_t)(gVar.ao[AO_1].enabled)) ==0){
				errStatus = MB_ERROR;
			}

			break;
		case MB_COA_AO2_ENABLE:
			gVar.ao[AO_2].enabled = value>0;
			if(FSExt_WriteIntNum(SA_AO2_ENABLE, (uint32_t)(gVar.ao[AO_2].enabled)) ==0){
				errStatus = MB_ERROR;
			}
			break;
		case MB_COA_AO3_ENABLE:
			gVar.ao[AO_3].enabled = value>0;
			if(FSExt_WriteIntNum(SA_AO3_ENABLE, (uint32_t)(gVar.ao[AO_3].enabled)) ==0){
				errStatus = MB_ERROR;
			}
			break;
		case MB_COA_AO4_ENABLE:
			gVar.ao[AO_4].enabled = value>0;
			if(FSExt_WriteIntNum(SA_AO4_ENABLE, (uint32_t)(gVar.ao[AO_4].enabled)) ==0){
				errStatus = MB_ERROR;
			}
			break;
//		case MB_COA_MB_TCPS_RELOAD:
//			gVar.mbTCPServer.isReloadReq = value>0;
//			break;

			/*Dev net info ------------*/
		case MB_COA_MB_SAVE_NET_INFO:

			if(value > 0){
				for( uint8_t i = 0;i < 6;i++){
					FSExt_WriteIntNum(SA_DEV_MAC_1+i, (uint32_t)(gVar.w5500Chip.netInfo.mac[i]));
					if(i<4){
						FSExt_WriteIntNum(SA_DEV_IP_1+i, (uint32_t)(gVar.w5500Chip.netInfo.ip[i]));
						FSExt_WriteIntNum(SA_DEV_SN_1+i, (uint32_t)(gVar.w5500Chip.netInfo.sn[i]));
						FSExt_WriteIntNum(SA_DEV_GW_1+i, (uint32_t)(gVar.w5500Chip.netInfo.gw[i]));
						FSExt_WriteIntNum(SA_DEV_DNS_1+i, (uint32_t)(gVar.w5500Chip.netInfo.dns[i]));
					}
				}

				address = SA_DEV_GW_4;
			}
			break;
			/*Modbus TCP Client--------------*/

//		case MB_COA_MB_TCPC_SOCK_CONN_DISCONN:
//
//			gVar.client.sock[0].connect = value > 0;
//			if(value <= 0 && gVar.client.sock[0].connectMode == SOCK_CONN_MODE_MANUAL){
//				disconnect(gVar.client.sock[0].id);
//				Sock_Clear(&gVar.client.sock[0]);
//			}
//			break;

			/*NModbus RTU Serial------------*/
		case MB_COA_MB_SAVE_SERIAL_CONFIG:
			if(MB_GetListenOnlyMode() == 0U){
				if(value > 0){
					if(FSExt_WriteIntNum(SA_MB_SERIAL_CONFIG, (uint32_t)(gVar.mbSerial.serialConfigValue)) ==0){
						errStatus = MB_ERROR;
					}
					if(FSExt_WriteIntNum(SA_MB_SERIAl_BAUD_RATE_USER_DEF, (uint32_t)(MBS_GetBaudRateUserDef())) ==0){
						errStatus = MB_ERROR;
					}
					if(MBS_CheckProtocol(value)){
						if(FSExt_WriteIntNum(SA_MB_SERIAl_PROTOCOL, (uint32_t)(gVar.mbSerial.protocolTemp)) ==0){
							errStatus = MB_ERROR;
						}
					}
				}
			}
			break;

			/*NModbus RTU Master------------*/
		case MB_COA_MB_MSTR_ENABLE:
				gVar.mbRTUMaster.enable = value > 0;
				if(FSExt_WriteIntNum(SA_MB_MSTR_ENABLE, (uint32_t)(gVar.mbRTUMaster.enable)) ==0){
					errStatus = MB_ERROR;
				}

			break;
			/*NModbus RTU Slave------------*/
		case MB_COA_MB_SAVE_SLAVE_ADDR:
			if(value > 0){
				if(FSExt_WriteIntNum(SA_SLAVE_ADDRESS, (uint32_t)(gVar.slvAddrTemp )) ==0){
					errStatus = MB_ERROR;
				}
			}
			break;

			/*Encoder--------------------*/
		case MB_COA_ENC_ENABLE:
			gVar.dio.enc.enable = (uint8_t)(value>0);
			if(FSExt_WriteIntNum(SA_ENC_ENABLE, (uint32_t)(gVar.dio.enc.enable)) ==0){
				errStatus = MB_ERROR;
			}
			break;

			/*Dev utilities--------------*/
		case MB_COA_RESTART:
			gVar.restartFlage = (uint8_t)(value > 0);
			break;
		case MB_COA_DBUG_ENABLE:

			(value > 0) ? DBG_ENABLE() : DBG_DISABLE();

			if(FSExt_WriteIntNum(SA_DBUG_ENABLE, (uint32_t)(value > 0)) ==0){
				errStatus = MB_ERROR;
			}
			break;
		default:
			errStatus = MB_ERROR;
			break;
	}


//	if (address < MB_COA_MAX) {
//		MB_REG_SetCO(address, value>0);
//	}
//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, value);

	return errStatus;
}


/**
  * @brief calback function for reading coil output
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readCOCallback( MB_CoilAddrEnum address, uint8_t *value){
	uint8_t errorStatus = MB_OK;

	switch (address) {
	/*Digital outputs------------*/
	//@NB: never change this addressing for the digital inputs, MB read function use this sequence

		case MB_COA_DO1_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_1].port, gVar.dio.dout[DO_1].pin);
			break;
		case MB_COA_DO2_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_2].port, gVar.dio.dout[DO_2].pin);
			break;
		case MB_COA_DO3_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_3].port, gVar.dio.dout[DO_3].pin);
			break;
		case MB_COA_DO4_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_4].port, gVar.dio.dout[DO_4].pin);
			break;
		case MB_COA_DO5_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_5].port, gVar.dio.dout[DO_5].pin);
			break;
		case MB_COA_DO6_ENABLE:
			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_6].port, gVar.dio.dout[DO_6].pin);
			break;
			/*D20240213: DO 7 & 8 are deprecated*/
//		case MB_COA_DO7_ENABLE:
//			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_7].port, gVar.dio.dout[DO_7].pin);
//			break;
//		case MB_COA_DO8_ENABLE:
//			*value = HAL_GPIO_ReadPin(gVar.dio.dout[DO_8].port, gVar.dio.dout[DO_8].pin);
//			break;

			/*Analog outputs------------*/
			//@NB: never change this addressing for the digital inputs, MB read function use this sequence
		case MB_COA_AO1_ENABLE:
			*value = gVar.ao[AO_1].enabled;
			break;
		case MB_COA_AO2_ENABLE:
			*value = gVar.ao[AO_2].enabled;
			break;
		case MB_COA_AO3_ENABLE:
			*value = gVar.ao[AO_3].enabled;
			break;
		case MB_COA_AO4_ENABLE:
			*value = gVar.ao[AO_4].enabled;
			break;
			/*Dev net info ------------*/
		case MB_COA_MB_SAVE_NET_INFO:
			*value = 0;
			break;
			/*NModbus RTU Serial------------*/
		case MB_COA_MB_SAVE_SERIAL_CONFIG:
			*value = 0;
			break;
			/*Modbus TCP Server--------------*/
//		case MB_COA_MB_TCPS_RELOAD:
//			break;

//			/*Modbus TCP Client--------------*/
//		case MB_COA_MB_TCPC_SOCK_CONN_DISCONN:
//
//			*value = 0;//gVar.client.sock[0].connect;
//
//			break;

			/*NModbus RTU Master------------*/
		case MB_COA_MB_MSTR_ENABLE:
			*value = gVar.mbRTUMaster.enable;
			break;
			/*NModbus RTU Slave------------*/
		case MB_COA_MB_SAVE_SLAVE_ADDR:
			*value = 0;
			break;

			/*Encoder--------------------*/
		case MB_COA_ENC_ENABLE:
			*value = gVar.dio.enc.enable;
			break;


			/*Dev utilities--------------*/
		case MB_COA_RESTART:
			*value = gVar.restartFlage;
			break;
		case MB_COA_DBUG_ENABLE:
			*value = DBG_IS_ENABLED();
			break;

		default:
//			if(address < MB_COA_MAX){
//				errorStatus =	MB_REG_GetCO(address, value);
//			}else{
//				errorStatus = MB_ERROR;
//			}
			break;
	}



//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, *value);
	return errorStatus;
}


/**
  * @brief calback function for input register
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  */
uint8_t CB_MB_readIRCallback(MB_IRAddrEnum address, uint16_t *value){
	uint8_t errorStatus = MB_OK;

	switch (address) {
		/*Analog Inputs-----------------------*/
		//Analog Input - 1
		case MB_IRA_AI1_VALUE_12BIT:
			*value = gVar.ai[AI_1].rawValue;
			break;
		case MB_IRA_AI1_CURRENT:
			*value = gVar.ai[AI_1].current*100;
			break;
		case MB_IRA_AI1_VOLTAGE:
			*value = gVar.ai[AI_1].voltage;
			break;
		//Analog Input - 2
		case MB_IRA_AI2_VALUE_12BIT:
			*value = gVar.ai[AI_2].rawValue;
			break;
		case MB_IRA_AI2_CURRENT:
			*value = gVar.ai[AI_2].current*100;
			break;
		case MB_IRA_AI2_VOLTAGE:
			*value = gVar.ai[AI_2].voltage;
			break;
		//Analog Input - 3
//		case MB_IRA_AI3_VALUE_12BIT:
//			*value = gVar.ai[AI_3].rawValue;
//			break;
//		case MB_IRA_AI3_CURRENT:
//			*value = gVar.ai[AI_3].current*100;
//			break;
//		case MB_IRA_AI3_VOLTAGE:
//			*value = gVar.ai[AI_3].voltage;
//			break;
			//Analog Input - 4
//		case MB_IRA_AI4_VALUE_12BIT:
//			*value = gVar.ai[AI_4].rawValue;
//			break;
//		case MB_IRA_AI4_CURRENT:
//			*value = gVar.ai[AI_4].current*100;
//			break;
//		case MB_IRA_AI4_VOLTAGE:
//			*value = gVar.ai[AI_4].voltage;
//			break;

			/*NModbus TCP Server------------*/
			//Socket 0

		case MB_IRA_MB_TCPS_SOCK0_DEST_IP_1:
			*value = gVar.server.sock[0].destIP[0];
			break;
		case MB_IRA_MB_TCPS_SOCK0_DEST_IP_2:
			*value = gVar.server.sock[0].destIP[1];
			break;
		case MB_IRA_MB_TCPS_SOCK0_DEST_IP_3:
			*value = gVar.server.sock[0].destIP[2];
			break;
		case MB_IRA_MB_TCPS_SOCK0_DEST_IP_4:
			*value = gVar.server.sock[0].destIP[3];
			break;
		case MB_IRA_MB_TCPS_SOCK0_DEST_PORT:
			*value = gVar.server.sock[0].destPort;
			break;
		case MB_IRA_MB_TCPS_SOCK0_STATUS:
			*value = gVar.server.sock[0].status;
			break;
			//Socket 1
//
//		case MB_IRA_MB_TCPS_SOCK1_DEST_IP_1:
//			*value = gVar.server.sock[1].destIP[0];
//			break;
//		case MB_IRA_MB_TCPS_SOCK1_DEST_IP_2:
//			*value = gVar.server.sock[1].destIP[1];
//			break;
//		case MB_IRA_MB_TCPS_SOCK1_DEST_IP_3:
//			*value = gVar.server.sock[1].destIP[2];
//			break;
//		case MB_IRA_MB_TCPS_SOCK1_DEST_IP_4:
//			*value = gVar.server.sock[1].destIP[3];
//			break;
//		case MB_IRA_MB_TCPS_SOCK1_DEST_PORT:
//			*value = gVar.server.sock[1].destPort;
//			break;
//		case MB_IRA_MB_TCPS_SOCK1_STATUS:
//			*value = gVar.server.sock[1].status;
//			break;
			/*NModbus TCP client------------*/

		case MB_IRA_MB_TCPC_SOCK_SRC_PORT:
			*value = gVar.client.sock[0].srcPort;
			break;
		case MB_IRA_MB_TCPC_SOCK_STATUS:
			*value = gVar.client.sock[0].status;
			break;

			/*Digital Inputs------------*/
		case MB_IRA_DI1_SIGNAL_PERIOD:
			*value = gVar.dio.di[DI_1].pulseInf.period;
			break;
		case MB_IRA_DI2_SIGNAL_PERIOD:
			*value = gVar.dio.di[DI_2].pulseInf.period;
			break;

			/*Encoder-------------------*/
		case MB_IRA_ENC_QUAD_DIRECTION:
			ENC_GetDirQuad(&gVar.dio.enc);
			*value = (uint16_t)(gVar.dio.enc.direction);
			break;
		case MB_IRA_ENC_QUAD_COUNT_LSB16:
			ENC_ReadQuad(&gVar.dio.enc);
			*value = (uint16_t)(gVar.dio.enc.counterQuad&0x0000FFFF);
			break;
		case MB_IRA_ENC_QUAD_COUNT_MSB16:
			*value = (uint16_t)(gVar.dio.enc.counterQuad>>16)&0x0000FFFF;
			break;
		case MB_IRA_ENC_INCR_CHA_COUNT_LSB16:
			ENC_ReadChA(&gVar.dio.enc);
			*value = (uint16_t)(gVar.dio.enc.counterA&0x0000FFFF);
			break;
		case MB_IRA_ENC_INCR_CHA_COUNT_MSB16:
			*value = (uint16_t)(gVar.dio.enc.counterA>>16)&0x0000FFFF;
			break;
		case MB_IRA_ENC_INCR_CHB_COUNT_LSB16:
			ENC_ReadChB(&gVar.dio.enc);
			*value = (uint16_t)(gVar.dio.enc.counterB&0x0000FFFF);
			break;
		case MB_IRA_ENC_INCR_CHB_COUNT_MSB16:
			*value = (uint16_t)(gVar.dio.enc.counterB>>16)&0x0000FFFF;
			break;


			/*Utilities----------------*/

		case MB_IRA_DEV_HW_VERSION:
			*value = (uint16_t)HW_VIRSION;
			break;
		case MB_IRA_DEV_FW_VERSION:
			*value =  (uint16_t)FW_VIRSION;
			break;

			/*PID-------------------*/
		case MB_IRA_PID1_FEEDBACK:
			*value = gVar.pid1.measurement;
			break;
		case MB_IRA_PID1_OUTPUT:
			*value = gVar.pid1.output_pid;
			break;
		case MB_IRA_PID2_FEEDBACK:
			*value = gVar.pid2.measurement;
			break;
		case MB_IRA_PID2_OUTPUT:
			*value = gVar.pid2.output_pid;
			break;

			/*Diagnosis---------------------*/
			//RTU bus error
		case MB_IRA_DIAG_RTU_OVERRUN_ERR_COUNT:
			*value = gVar.diag.rtuBus.oreCount;
			break;
		case MB_IRA_DIAG_RTU_DMATX_ERR_COUNT:
			*value = gVar.diag.rtuBus.dteCount;
			break;
		case MB_IRA_DIAG_RTU_PARITY_ERR_COUNT:
			*value = gVar.diag.rtuBus.peCount;
			break;
		case MB_IRA_DIAG_RTU_NOISE_ERR_COUNT:
			*value = gVar.diag.rtuBus.neCount;
			break;
		case MB_IRA_DIAG_RTU_FRAME_ERR_COUNT:
			*value = gVar.diag.rtuBus.feCount;
			break;
		case MB_IRA_DIAG_RTU_DMATX_ERR_CODE:
			*value = gVar.diag.rtuBus.dmaTxErrCode;
			break;
		case MB_IRA_DIAG_RTU_DMARX_ERR_CODE:
			*value = gVar.diag.rtuBus.dmaRxErrCode;
			break;
			//SPI bus error
		case MB_IRA_DIAG_SPI_OVERRUN_ERR_COUNT:
			*value = gVar.diag.w5500Spi.oreCount;
			break;
		case MB_IRA_DIAG_SPI_DMATX_ERR_COUNT:
			*value = gVar.diag.w5500Spi.dteCount;
			break;
		case MB_IRA_DIAG_SPI_FLAG_ERR_COUNT:
			*value = gVar.diag.w5500Spi.flgeCount;
			break;
		case MB_IRA_DIAG_SPI_ABORT_ERR_COUNT:
			*value = gVar.diag.w5500Spi.abreCount;
			break;
		case MB_IRA_DIAG_SPI_MODF_ERR_COUNT:
			*value = gVar.diag.w5500Spi.modfeCount;
			break;
		case MB_IRA_DIAG_SPI_CRC_ERR_COUNT:
			*value = gVar.diag.w5500Spi.crceCount;
			break;
		case MB_IRA_DIAG_SPI_FRAME_ERR_COUNT:
			*value = gVar.diag.w5500Spi.feCount;
			break;
		case MB_IRA_DIAG_SPI_DMATX_ERR_CODE:
			*value = gVar.diag.w5500Spi.dmaTxErrCode;
			break;
		case MB_IRA_DIAG_SPI_DMARX_ERR_CODE:
			*value = gVar.diag.w5500Spi.dmaRxErrCode;

			break;
			//I2C comm. err
		case MB_IRA_DIAG_I2C_ERR_CODE:
			*value = gVar.diag.i2c.ErrCode;
			break;
		case MB_IRA_DIAG_I2C_DMATX_CODE:
			*value = gVar.diag.i2c.dmaTxErrCode;
			break;
		case MB_IRA_DIAG_I2C_DMARX_CODE:
			*value = gVar.diag.i2c.dmaRxErrCode;
			break;
		default:
//			if(address < MB_IRA_MAX){
//				errorStatus =	MB_REG_GetIR(address, value);
//			}else{
//				errorStatus = MB_ERROR;
//			}
			errorStatus = MB_ERROR;
			break;
	}
	//TODO: to uncomment later,
//	if(address < MB_IRA_MAX){
//		errorStatus =	MB_REG_GetIR(address, value);
//	}else{
//		errorStatus = MB_ERROR;
//	}

//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, *value);
	return errorStatus;
}


/**
  * @brief calback function for holding register
  * @param reg: pointer to modbus registers
  * @param address: address of a register
  * @retval return error if an exception happen
  *
  */
uint8_t CB_MB_writeHRCallback( MB_HRAddrEnum address, uint16_t value){
	uint8_t errStatus = MB_OK;

	/*------Write your code here------*/

	switch (address) {


		/*Digital Inputs--------------*/
		//Digitla Input 1
		case MB_HRA_DI1_MODE:
			if(value == DI_MODE_NORMAL || value == DI_MODE_PULSE_CAPTURE){
				gVar.dio.di[DI_1].config.mode = value;
				gVar.dio.di[DI_1].isChanged = 1;
				FSExt_WriteIntNum(SA_DI1_MODE, value);
//				DI_Config(&gVar.dio.di[DI_1]);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_DI1_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI1_COUNTER_MODE:
//			break;
		case MB_HRA_DI1_TRIGGER_MODE:
			if(value == DI_TRIG_MODE_RISING || value == DI_TRIG_MODE_FALLING){
				gVar.dio.di[DI_1].config.trigMode = value;
				gVar.dio.di[DI_1].isChanged = 1;
				FSExt_WriteIntNum(SA_DI1_TRIGGER_MODE, value);
//				DI_Config(&gVar.dio.di[DI_1]);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_DI1_DEBOUNCE_MAX:
//
//			break;
		case MB_HRA_DI1_SIGNAL_PERIOD_MIN:
			if(value >= 1 && value <= DEF_U16_MAX_VALUE){
				gVar.dio.di[DI_1].pulseInf.periodMin = value;
				FSExt_WriteIntNum(SA_DI1_SIGNAL_PERIOD_MIN, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

		//Digitla Input 2
		case MB_HRA_DI2_MODE:
			if(value == DI_MODE_NORMAL || value == DI_MODE_PULSE_CAPTURE){
				gVar.dio.di[DI_2].config.mode = value;
				gVar.dio.di[DI_2].isChanged = 1;
				FSExt_WriteIntNum(SA_DI2_MODE, value);
//				DI_Config(&gVar.dio.di[DI_2]);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_DI2_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI2_COUNTER_MODE:
//			break;
		case MB_HRA_DI2_TRIGGER_MODE:
			if(value == DI_TRIG_MODE_RISING || value == DI_TRIG_MODE_FALLING){
				gVar.dio.di[DI_2].config.trigMode = value;
				gVar.dio.di[DI_2].isChanged = 1;
				FSExt_WriteIntNum(SA_DI2_TRIGGER_MODE, value);
//				DI_Config(&gVar.dio.di[DI_2]);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_DI2_DEBOUNCE_MAX:
//
//			break;
		case MB_HRA_DI2_SIGNAL_PERIOD_MIN:
			if(value >= 1 && value <= DEF_U16_MAX_VALUE){
				gVar.dio.di[DI_2].pulseInf.periodMin = value;
				FSExt_WriteIntNum(SA_DI2_SIGNAL_PERIOD_MIN, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

		//Digitla Input 3
//		case MB_HRA_DI3_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_3].config.mode = value;
//				FSExt_WriteIntNum(SA_DI3_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI3_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI3_COUNTER_MODE:
//			break;
//		case MB_HRA_DI3_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI3_DEBOUNCE_MAX:
//
//			break;
//		case MB_HRA_DI4_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_4].config.mode = value;
//				FSExt_WriteIntNum(SA_DI4_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI4_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI4_COUNTER_MODE:
//			break;
//		case MB_HRA_DI4_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI4_DEBOUNCE_MAX:
//
//			break;
//		case MB_HRA_DI5_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_5].config.mode = value;
//				FSExt_WriteIntNum(SA_DI5_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI5_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI5_COUNTER_MODE:
//			break;
//		case MB_HRA_DI5_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI5_DEBOUNCE_MAX:
//
//			break;
//		case MB_HRA_DI6_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_6].config.mode = value;
//				FSExt_WriteIntNum(SA_DI6_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI6_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI6_COUNTER_MODE:
//			break;
//		case MB_HRA_DI6_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI6_DEBOUNCE_MAX:
//
//			break;
//		case MB_HRA_DI7_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_7].config.mode = value;
//				FSExt_WriteIntNum(SA_DI7_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI7_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI7_COUNTER_MODE:
//			break;
//		case MB_HRA_DI7_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI7_DEBOUNCE_MAX:
//
//			break;
//		case MB_HRA_DI8_MODE :
//			if(value >= DI_MODE_NORMAL && value < DI_MODE_MAX){
//				gVar.dio.di[DI_8].config.mode = value;
//				FSExt_WriteIntNum(SA_DI8_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DI8_COUNTER_TYPE:
//			break;
//		case MB_HRA_DI8_COUNTER_MODE:
//			break;
//		case MB_HRA_DI8_TRIGGER_MODDE:
//			break;
//		case MB_HRA_DI8_DEBOUNCE_MAX:



		/*Digital Outputs 1 to 8 -------------*/
//
//		case MB_HRA_DO1_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_1].source = value;
//				FSExt_WriteIntNum(SA_DO1_SRC, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO1_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_1].mode = value;
//				FSExt_WriteIntNum(SA_DO1_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
////		case MB_HRA_DO1_PWM_DUTY:
////			break;
//		case MB_HRA_DO2_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_2].source = value;
//				FSExt_WriteIntNum(SA_DO2_SRC, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO2_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_2].mode = value;
//				FSExt_WriteIntNum(SA_DO2_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
////		case MB_HRA_DO2_PWM_DUTY:
////			break;
//		case MB_HRA_DO3_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_3].source = value;
//				FSExt_WriteIntNum(SA_DO3_SRC, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO3_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_2].mode = value;
//				FSExt_WriteIntNum(SA_DO2_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
////		case MB_HRA_DO3_PWM_DUTY:
////			break;
//		case MB_HRA_DO4_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_4].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO4_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_4].mode = value;
//				FSExt_WriteIntNum(SA_DO4_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
////		case MB_HRA_DO4_PWM_DUTY:
////			break;
//		case MB_HRA_DO5_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_5].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO5_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_5].mode = value;
//				FSExt_WriteIntNum(SA_DO5_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO5_PWM_DUTY:
//			if(value >= 0 && value <= AO_PWM_RESOLUTION){
//				gVar.dio.dout[DO_1].pwm.duty12Bits = value;
//				FSExt_WriteIntNum(SA_DO5_PWM_DUTY, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO6_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_6].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO6_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_6].mode = value;
//				FSExt_WriteIntNum(SA_DO6_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO6_PWM_DUTY:
//			if(value >= 0 && value <= AO_PWM_RESOLUTION){
//				gVar.dio.dout[DO_6].pwm.duty12Bits = value;
//				FSExt_WriteIntNum(SA_DO6_PWM_DUTY, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO7_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_7].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO7_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_7].mode = value;
//				FSExt_WriteIntNum(SA_DO7_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO7_PWM_DUTY:
//			if(value >= 0 && value <= AO_PWM_RESOLUTION){
//				gVar.dio.dout[DO_7].pwm.duty12Bits = value;
//				FSExt_WriteIntNum(SA_DO7_PWM_DUTY, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO8_SRC:
//			if(value > DO_SRC_NONE && value < DO_SRC_MAX){
//				gVar.dio.dout[DO_8].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO8_MODE:
//			if(value > DO_MODE_NONE && value < DO_MODE_MAX){
//				gVar.dio.dout[DO_8].mode = value;
//				FSExt_WriteIntNum(SA_DO8_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_DO8_PWM_DUTY:
//			if(value >= 0 && value <= AO_PWM_RESOLUTION){
//				gVar.dio.dout[DO_8].pwm.duty12Bits = value;
//				FSExt_WriteIntNum(SA_DO8_PWM_DUTY, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;

		/*Analog Inputs-----------*/
		case MB_HRA_AI1_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ai[AI_1].mode = value;
				FSExt_WriteIntNum(SA_AI1_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_AI2_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ai[AI_2].mode = value;
				FSExt_WriteIntNum(SA_AI2_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_AI3_MODE:
//			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
//				gVar.ai[AI_3].mode = value;
//				FSExt_WriteIntNum(SA_AI3_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
//		case MB_HRA_AI4_MODE:
//			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
//				gVar.ai[AI_4].mode = value;
//				FSExt_WriteIntNum(SA_AI4_MODE, value);
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;

		/*Analog Outputs 1 to 4-------------*/
		//Analog Output 1
		case MB_HRA_AO1_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ao[AO_1].mode = value;
				FSExt_WriteIntNum(SA_AO1_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_AO1_SRC:
			if(value == AO_PWM_SRC_MODBUS || value == AO_PWM_SRC_PID){
				gVar.ao[AO_1].source = value;
				FSExt_WriteIntNum(SA_AO1_SRC, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_AO1_DUTY:
			if(value >= 0 && value <= AO_PWM_RESOLUTION){
				gVar.ao[AO_1].mbValue = value;
//				FSExt_WriteIntNum(SA_AO1_DUTY, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			//Analog Output 2
		case MB_HRA_AO2_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ao[AO_2].mode = value;
				FSExt_WriteIntNum(SA_AO2_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_AO2_SRC:
			if(value == AO_PWM_SRC_MODBUS || value == AO_PWM_SRC_PID){
				gVar.ao[AO_2].source = value;
				FSExt_WriteIntNum(SA_AO2_SRC, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_AO2_DUTY:
			if(value >= 0 && value <= AO_PWM_RESOLUTION){
				gVar.ao[AO_2].mbValue = value;
//				FSExt_WriteIntNum(SA_AO2_DUTY, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			//Analog Output 3
		case MB_HRA_AO3_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ao[AO_3].mode = value;
				FSExt_WriteIntNum(SA_AO3_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_AO3_SRC:
//			if(value > AO_PWM_SRC_NONE && value < AO_PWM_SRC_MAX){
//
//				gVar.ao[AO_3].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
		case MB_HRA_AO3_DUTY:
			if(value >= 0 && value <= AO_PWM_RESOLUTION){
				gVar.ao[AO_3].value = value;
//				FSExt_WriteIntNum(SA_AO3_DUTY, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			//Analog Output 4
		case MB_HRA_AO4_MODE:
			if(value == AIO_MODE_CURRENT || value == AIO_MODE_VOLT){
				gVar.ao[AO_4].mode = value;
				FSExt_WriteIntNum(SA_AO4_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_AO4_SRC:
//			if(value > AO_PWM_SRC_NONE && value < AO_PWM_SRC_MAX){
//				gVar.ao[AO_4].source = value;
//			}else{
//				errStatus = MB_ERROR;
//			}
//			break;
		case MB_HRA_AO4_DUTY:
			if(value >= 0 && value <= AO_PWM_RESOLUTION){
				gVar.ao[AO_4].value = value;
//				FSExt_WriteIntNum(SA_AO4_DUTY, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			/*Net Information------------*/
			//Device IP MAC
		case MB_HRA_DEV_MAC_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[0] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_1, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_MAC_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[1] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_MAC_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[2] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_MAC_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[3] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_MAC_5:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[4] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_5, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_MAC_6:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.mac[5] = value;
				FSExt_WriteIntNum(SA_DEV_MAC_6, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
			//Device IP Address
		case MB_HRA_DEV_IP_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){		// first byte of the ip address cannot be zero
				gVar.w5500Chip.netInfo.ip[0] = value;
				FSExt_WriteIntNum(SA_DEV_IP_1, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_IP_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.ip[1] = value;
				FSExt_WriteIntNum(SA_DEV_IP_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_IP_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.ip[2] = value;
				FSExt_WriteIntNum(SA_DEV_IP_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_IP_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.ip[3] = value;
				FSExt_WriteIntNum(SA_DEV_IP_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			//Device Subnet mask
		case MB_HRA_DEV_SN_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.sn[0] = value;
				FSExt_WriteIntNum(SA_DEV_SN_1, value);
			}else{
				errStatus = MB_ERROR;
			}

			break;
		case MB_HRA_DEV_SN_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.sn[1] = value;
				FSExt_WriteIntNum(SA_DEV_SN_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_SN_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.sn[2] = value;
				FSExt_WriteIntNum(SA_DEV_SN_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_SN_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.sn[3] = value;
				FSExt_WriteIntNum(SA_DEV_SN_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
			//Device Gateway
		case MB_HRA_DEV_GW_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.gw[0] = value;
				FSExt_WriteIntNum(SA_DEV_GW_1, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_GW_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.gw[1] = value;
				FSExt_WriteIntNum(SA_DEV_GW_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_GW_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.gw[2] = value;
				FSExt_WriteIntNum(SA_DEV_GW_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_GW_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.gw[3] = value;
				FSExt_WriteIntNum(SA_DEV_GW_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			//Device DNS
		case MB_HRA_DEV_DNS_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.dns[0] = value;
				FSExt_WriteIntNum(SA_DEV_DNS_1, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_DNS_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.dns[1] = value;
				FSExt_WriteIntNum(SA_DEV_DNS_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_DNS_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.dns[2] = value;
				FSExt_WriteIntNum(SA_DEV_DNS_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_DEV_DNS_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.w5500Chip.netInfo.dns[3] = value;
				FSExt_WriteIntNum(SA_DEV_DNS_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			/*NModbus TCP Server------------*/
			//socket - 0 & 1
		case MB_HRA_MB_TCPS_SOCK0_SRC_PORT:
			if(value >= 0 && value <= DEF_U16_MAX_VALUE){
				gVar.server.sock[0].srcPort = value;
				FSExt_WriteIntNum(SA_MB_TCPS_SOCK0_SRC_PORT, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPS_SOCK0_KPALV_TIM:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.server.sock[0].kpAlvTime = value;
				FSExt_WriteIntNum(SA_MB_TCPS_SOCK0_KPALV_TIM, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_MB_TCPS_SOCK1_KPALV_TIM:
////			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
////				gVar.server.sock[1].kpAlvTime = value;
////				FSExt_WriteIntNum(SA_MB_TCPS_SOCK1_KPALV_TIM, value);
////			}else{
////				errStatus = MB_ERROR;
////			}
//			break;
			/*NModbus TCP Client------------*/
		case MB_HRA_MB_TCPC_DEST_IP_1:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.client.sock[0].destIP[0] = value;
				FSExt_WriteIntNum(SA_MB_TCPC_DEST_IP_1, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPC_DEST_IP_2:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.client.sock[0].destIP[1] = value;
				FSExt_WriteIntNum(SA_MB_TCPC_DEST_IP_2, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPC_DEST_IP_3:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.client.sock[0].destIP[2] = value;
				FSExt_WriteIntNum(SA_MB_TCPC_DEST_IP_3, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPC_DEST_IP_4:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.client.sock[0].destIP[3] = value;
				FSExt_WriteIntNum(SA_MB_TCPC_DEST_IP_4, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPC_DEST_PORT:
			if(value >= 0 && value <= DEF_U16_MAX_VALUE){
				gVar.client.sock[0].destPort = value;
				FSExt_WriteIntNum(SA_MB_TCPC_DEST_PORT, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_TCPC_SOCK_KPALV_TIM:
			if(value >= 0 && value <= DEF_U8_MAX_VALUE){
				gVar.client.sock[0].kpAlvTime = value;
				FSExt_WriteIntNum(SA_MB_TCPC_SOCK_KPALV_TIM, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

		case MB_HRA_MB_TCPC_SOCK_CONN_MODE:
			if(value == SOCK_CONN_MODE_AUTO || value == SOCK_CONN_MODE_MANUAL){
				gVar.client.sock[0].connectMode = value;
				FSExt_WriteIntNum(SA_MB_TCPC_SOCK_CONN_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

		case MB_HRA_MB_TCPC_TX_TIM_INTRVAL:
			if(value > 0 && value <= CONF_DEF_MB_TCPC_TX_TIM_INTERVAL_MAX){
				gVar.mbTCPClient.txTimIntrvl = value;
				FSExt_WriteIntNum(SA_MB_TCPC_TX_TIM_INTRVAL, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;

			/*NModbus RTU Serial------------*/
		case MB_HRA_MB_SERIAL_CONFIG:
			if(MB_GetListenOnlyMode() == 0U){

				MBS_Serial serial;
				serial.serialConfigValue = value;
				if(MBS_CheckSerialConfigValidity(&serial) == MB_OK){
					gVar.mbSerial.serialConfigValue = value;
				}else{
					errStatus = MB_ERROR;
				}
			}
			break;

		case MB_HRA_MB_SERIAL_BAUD_RATE_MS16B:
			if(value >= 0 && value <= DEF_U16_MAX_VALUE){
				uint32_t br = MBS_GetBaudRateUserDef();
				br =  ((br&0x0000FFFF) | ((uint32_t)value<<16));
				MBS_SetBaudRateUserDef(br);
			}
			break;
		case MB_HRA_MB_SERIAL_BAUD_RATE_LS16B:
			if(value >= 0 && value <= DEF_U16_MAX_VALUE){
				uint32_t br = MBS_GetBaudRateUserDef();
				br =  ((br&0xFFFF0000) | ((uint32_t)value&0x0000FFFF));
				MBS_SetBaudRateUserDef(br);
			}
			break;

		case MB_HRA_MB_SERIAl_PROTOCOL:
			if(MBS_CheckProtocol(value)){
				gVar.mbSerial.protocolTemp = value;
			}else{
				errStatus = MB_ERROR;
			}
			break;

			/*Modbus RTU Master------------*/
		case MB_HRA_MB_MSTR_SLAVE_START_ADDR:
			if(value > 0 && value <= MB_SLAVE_ADDR_MAX){
				gVar.mbRTUMaster.custom.slvStartAddr =  value;
				FSExt_WriteIntNum(SA_MB_MSTR_SLV_START_ADDR, value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_MSTR_NUM_OF_SLAVES:
			if((value >= 0 && value <= MB_CUST_NUM_OF_SLAVE_MAX )
					&& ((value+gVar.mbRTUMaster.custom.slvStartAddr) < MB_SLAVE_ADDR_MAX)){
				gVar.mbRTUMaster.custom.numOfSlv =  value;
				FSExt_WriteIntNum(SA_MB_MSTR_NUM_OF_SLAVE, value);

			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_MB_MSTR_REPLY_TIMEOUT:
			if(value > 0 && value <= CONF_DEF_MBGW_TIMEOUT_MAX ){
				gVar.mbRTUMaster.replyTimer.timeout =  value*1000; //TODO: value*10; change from ms to us for test purpose
				FSExt_WriteIntNum(SA_MB_MSTR_REPLY_TIMEOUT, gVar.mbRTUMaster.replyTimer.timeout);
			}else{
				errStatus = MB_ERROR;
			}

			break;
		case MB_HRA_MB_MSTR_PERIODIC_REQ_INTERVAL:
			if(value > 0  && value <= CONF_DEF_MBRTUM_PREIODIC_REQ_TIMEOUT_MAX ){
				gVar.mbRTUMaster.custom.reTransTimer.timeout =  value*1000; //TODO: value*10; change from ms to us for test purpose
				FSExt_WriteIntNum(SA_MB_MSTR_PERIODIC_REQ_INTERVAL, gVar.mbRTUMaster.custom.reTransTimer.timeout);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_MB_MSTR_RETRY_MAX:
//			break;

			/*NModbus RTU Slave------------*/
		case MB_HRA_MB_SLAVE_ADDRESS:
			if(value > 0 && value <= MB_SLAVE_ADDR_MAX ){
				gVar.slvAddrTemp = value;
			}else{
				errStatus = MB_ERROR;
			}

			break;
		case MB_HRA_MB_SLAVE_CUST_RESP_TIMEOUT:
			if(value > 0 && value <= CONF_DEF_MBRTUS_CUST_RESP_TIMEOUT_MAX ){
				gVar.mbRTUSlave.custom.waitingTimer.timeout = value*1000; //TODO: value*10; change from ms to us for test purpose
				FSExt_WriteIntNum(SA_MB_SLAVE_CUST_RESP_TIMEOUT, gVar.mbRTUSlave.custom.waitingTimer.timeout );
			}else{
				errStatus = MB_ERROR;
			}
			break;
			/*NModbus Gateway------------*/
		case MB_HRA_MB_GW_TIMEOUT:

			if(value > 0  && value <= CONF_DEF_MBGW_TIMEOUT_MAX ){
				gVar.mbGateway.rplyTimer.timeout =  value*1000; //TODO: value*10; change from ms to us for test purpose
				gVar.mbGateway.rplyTimRglrFC.timeout = value*1000; //TODO: value*10; change from ms to us for test purpose
				FSExt_WriteIntNum(SA_MB_GW_TIMEOUT, gVar.mbGateway.rplyTimer.timeout);
			}else{
				errStatus = MB_ERROR;
			}
			break;
			/*Device Utilities----------------*/
		case MB_HRA_DEV_MODE:
			if(value == DEV_MODE_SLAVE || value == DEV_MODE_MASTER){
				gVar.devMode = value;
//				gVar.mbRTUMaster.isBusy = 0;
				gVar.mbSerial.mode = (value == DEV_MODE_MASTER ? MBS_MODE_MASTER : MBS_MODE_SLAVE);

				MB_SetRTUMode(value == DEV_MODE_SLAVE ? MB_RTU_MODE_SLAVE : MB_RTU_MODE_MASTER);

				FSExt_WriteIntNum(SA_DEV_MODE, value);
			}else{
				errStatus = MB_ERROR;
			}

			break;

			/*Encoder----------------*/
		case MB_HRA_ENC_MODE:
			if(value >= 0 && value < ENC_MODE_MAX ){
				gVar.dio.enc.configs.mode = value;
				gVar.dio.enc.isChanged = 1;
				FSExt_WriteIntNum(SA_ENC_MODE, (uint32_t)value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_ENC_QUAD_TRIG_MODE:
			if(value == ENC_TRIG_MODE_RISING || value == ENC_TRIG_MODE_FALLING ){
				gVar.dio.enc.configs.quadrature.trigMode = value;
				gVar.dio.enc.isChanged = 1;
				FSExt_WriteIntNum(SA_ENC_QUAD_TRIG_MODE, (uint32_t)value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_ENC_CHA_TRIG_MODE:
			if(value >= 0 && value < ENC_TRIG_MAX ){
				gVar.dio.enc.configs.channelA.trigMode = value;
				gVar.dio.enc.isChanged = 1;
				FSExt_WriteIntNum(SA_ENC_CHA_TRIG_MODE, (uint32_t)value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
		case MB_HRA_ENC_CHB_TRIG_MODE:
			if(value >= 0 && value < ENC_TRIG_MAX ){
				gVar.dio.enc.configs.channelB.trigMode = value;
				gVar.dio.enc.isChanged = 1;
				FSExt_WriteIntNum(SA_ENC_CHB_TRIG_MODE, (uint32_t)value);
			}else{
				errStatus = MB_ERROR;
			}
			break;
//		case MB_HRA_ENC_CHZ_TRIG_MODE:
//			break;


			/*PID----------------*/
			//PID 1
		case MB_HRA_PID1_KP:
			gVar.pid1.kp = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID1_KP, value);
			break;
		case MB_HRA_PID1_KI:
			gVar.pid1.ki = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID1_KI, value);
			break;
		case MB_HRA_PID1_KD:
			gVar.pid1.kd = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID1_KD, value);
			break;
		case MB_HRA_PID1_I_OUT_LIMIT:
			gVar.pid1.output_i_max = ((float)value);;
			FSExt_WriteIntNum(SA_PID1_I_OUT_LIMIT, value);
			break;

			//PID 2
		case MB_HRA_PID2_KP:
			gVar.pid2.kp = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID2_KP, value);
			break;
		case MB_HRA_PID2_KI:
			gVar.pid2.ki = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID2_KI, value);
			break;
		case MB_HRA_PID2_KD:
			gVar.pid2.kd = ((float)value/MB_VALUE_MULTIPLIER_100);;
			FSExt_WriteIntNum(SA_PID2_KD, value);
			break;
		case MB_HRA_PID2_I_OUT_LIMIT:
			gVar.pid2.output_i_max = ((float)value);;
			FSExt_WriteIntNum(SA_PID2_I_OUT_LIMIT, value);
			break;
		default:
			errStatus = MB_ERROR;
			break;
	}
//	if (errStatus == MB_OK) {
//		MB_REG_SetHR(address, value);
//	}
//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, value);
	return errStatus;
}


uint8_t CB_MB_readHRCallback( MB_HRAddrEnum address, uint16_t *value){
	uint8_t errorStatus = MB_OK;
	//
	switch(address){
//	/*Digital Inputs--------------*/
		case MB_HRA_DI1_MODE:
			*value = gVar.dio.di[DI_1].config.mode;
			break;
//		case MB_HRA_DI1_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_1].config.counterType;
//			break;
//		case MB_HRA_DI1_COUNTER_MODE:
//			*value = gVar.dio.di[DI_1].config.countMode;
//			break;
		case MB_HRA_DI1_TRIGGER_MODE:
			*value = gVar.dio.di[DI_1].config.trigMode;
			break;
//		case MB_HRA_DI1_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_1].dbunce.countMax;
//			break;
		case MB_HRA_DI1_SIGNAL_PERIOD_MIN:
			*value = gVar.dio.di[DI_1].pulseInf.periodMin;
			break;


		case MB_HRA_DI2_MODE:
			*value = gVar.dio.di[DI_2].config.mode;
			break;
//		case MB_HRA_DI2_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_2].config.counterType;
//			break;
//		case MB_HRA_DI2_COUNTER_MODE:
//			*value = gVar.dio.di[DI_2].config.countMode;
//			break;
		case MB_HRA_DI2_TRIGGER_MODE:
			*value = gVar.dio.di[DI_2].config.trigMode;
			break;
//		case MB_HRA_DI2_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_2].dbunce.countMax;
//			break;
		case MB_HRA_DI2_SIGNAL_PERIOD_MIN:
			*value = gVar.dio.di[DI_2].pulseInf.periodMin;
			break;



//		case MB_HRA_DI3_MODE:
//			*value = gVar.dio.di[DI_3].config.mode;
//			break;
//		case MB_HRA_DI3_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_3].config.counterType;
//			break;
//		case MB_HRA_DI3_COUNTER_MODE:
//			*value = gVar.dio.di[DI_3].config.countMode;
//			break;
//		case MB_HRA_DI3_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_3].config.trigMode;
//			break;
//		case MB_HRA_DI3_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_3].dbunce.countMax;
//			break;
//
//		case MB_HRA_DI4_MODE:
//			*value = gVar.dio.di[DI_4].config.mode;
//			break;
//		case MB_HRA_DI4_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_4].config.counterType;
//			break;
//		case MB_HRA_DI4_COUNTER_MODE:
//			*value = gVar.dio.di[DI_4].config.countMode;
//			break;
//		case MB_HRA_DI4_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_4].config.trigMode;
//			break;
//		case MB_HRA_DI4_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_4].dbunce.countMax;
//			break;
//
//		case MB_HRA_DI5_MODE:
//			*value = gVar.dio.di[DI_5].config.mode;
//			break;
//		case MB_HRA_DI5_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_5].config.counterType;
//			break;
//		case MB_HRA_DI5_COUNTER_MODE:
//			*value = gVar.dio.di[DI_5].config.countMode;
//			break;
//		case MB_HRA_DI5_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_5].config.trigMode;
//			break;
//		case MB_HRA_DI5_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_5].dbunce.countMax;
//			break;
//
//		case MB_HRA_DI6_MODE:
//			*value = gVar.dio.di[DI_6].config.mode;
//			break;
//		case MB_HRA_DI6_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_6].config.counterType;
//			break;
//		case MB_HRA_DI6_COUNTER_MODE:
//			*value = gVar.dio.di[DI_6].config.countMode;
//			break;
//		case MB_HRA_DI6_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_6].config.trigMode;
//			break;
//		case MB_HRA_DI6_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_6].dbunce.countMax;
//			break;
//
//		case MB_HRA_DI7_MODE:
//			*value = gVar.dio.di[DI_7].config.mode;
//			break;
//		case MB_HRA_DI7_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_7].config.counterType;
//			break;
//		case MB_HRA_DI7_COUNTER_MODE:
//			*value = gVar.dio.di[DI_7].config.countMode;
//			break;
//		case MB_HRA_DI7_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_7].config.trigMode;
//			break;
//		case MB_HRA_DI7_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_7].dbunce.countMax;
//			break;
//
//		case MB_HRA_DI8_MODE:
//			*value = gVar.dio.di[DI_8].config.mode;
//			break;
//		case MB_HRA_DI8_COUNTER_TYPE:
//			*value = gVar.dio.di[DI_8].config.counterType;
//			break;
//		case MB_HRA_DI8_COUNTER_MODE:
//			*value = gVar.dio.di[DI_8].config.countMode;
//			break;
//		case MB_HRA_DI8_TRIGGER_MODDE:
//			*value = gVar.dio.di[DI_8].config.trigMode;
//			break;
//		case MB_HRA_DI8_DEBOUNCE_MAX:
//			*value = gVar.dio.di[DI_8].dbunce.countMax;
//			break;

//		/*Digital Outputs--------------*/
//		case MB_HRA_DO_HSPWM_FREQ:
//			*value = gVar.dio.pwmFreq;
//			break;
//		case MB_HRA_DO1_SRC:
//			*value = gVar.dio.dout[DO_1].source;
//			break;
//		case MB_HRA_DO1_MODE:
//			*value = gVar.dio.dout[DO_1].mode;
//				break;
//	//	case MB_HRA_DO1_PWM_DUTY
//	//		break;
//
//		case MB_HRA_DO2_SRC:
//			*value = gVar.dio.dout[DO_2].source;
//			break;
//		case MB_HRA_DO2_MODE:
//			*value = gVar.dio.dout[DO_2].mode;
//			break;
//	//	case MB_HRA_DO2_PWM_DUTY
//	//		break;
//
//		case MB_HRA_DO3_SRC:
//			*value = gVar.dio.dout[DO_3].source;
//			break;
//		case MB_HRA_DO3_MODE:
//			*value = gVar.dio.dout[DO_3].mode;
//			break;
//	//	case MB_HRA_DO3_PWM_DUTY:
//	//		break;
//
//		case MB_HRA_DO4_SRC:
//			*value = gVar.dio.dout[DO_4].source;
//			break;
//		case MB_HRA_DO4_MODE:
//			*value = gVar.dio.dout[DO_4].mode;
//			break;
//	//	case MB_HRA_DO4_PWM_DUTY:
//	//		break;
//
//		case MB_HRA_DO5_SRC:
//			*value = gVar.dio.dout[DO_5].source;
//			break;
//		case MB_HRA_DO5_MODE:
//			*value = gVar.dio.dout[DO_5].mode;
//			break;
//		case MB_HRA_DO5_PWM_DUTY:
//			*value = gVar.dio.dout[DO_5].pwm.duty12Bits;
//			break;
//
//		case MB_HRA_DO6_SRC:
//			*value = gVar.dio.dout[DO_6].source;
//			break;
//		case MB_HRA_DO6_MODE:
//			*value = gVar.dio.dout[DO_6].mode;
//			break;
//		case MB_HRA_DO6_PWM_DUTY:
//			*value = gVar.dio.dout[DO_6].pwm.duty12Bits;
//			break;
//
//		case MB_HRA_DO7_SRC:
//			*value = gVar.dio.dout[DO_7].source;
//			break;
//		case MB_HRA_DO7_MODE:
//			*value = gVar.dio.dout[DO_7].mode;
//			break;
//		case MB_HRA_DO7_PWM_DUTY:
//			*value = gVar.dio.dout[DO_7].pwm.duty12Bits;
//			break;
//
//		case MB_HRA_DO8_SRC:
//			*value = gVar.dio.dout[DO_8].source;
//			break;
//		case MB_HRA_DO8_MODE:
//			*value = gVar.dio.dout[DO_8].mode;
//			break;
//		case MB_HRA_DO8_PWM_DUTY:
//			*value = gVar.dio.dout[DO_8].pwm.duty12Bits;
//			break;

		/*Analog Inputs-----------*/
		case MB_HRA_AI1_MODE:
			*value = gVar.ai[AI_1].mode;
			break;
		case MB_HRA_AI2_MODE:
			*value = gVar.ai[AI_2].mode;
			break;
//		case MB_HRA_AI3_MODE:
//			*value = gVar.ai[AI_3].mode;
//			break;
//		case MB_HRA_AI4_MODE:
//			*value = gVar.ai[AI_4].mode;
//			break;

		/*Analog outputs------------*/
		case MB_HRA_AO1_MODE:
			*value = gVar.ao[AO_1].mode;
			break;
		case MB_HRA_AO1_SRC:
			*value = gVar.ao[AO_1].source;
			break;
		case MB_HRA_AO1_DUTY:
			*value = gVar.ao[AO_1].mbValue;
			break;

		case MB_HRA_AO2_MODE:
			*value = gVar.ao[AO_2].mode;
			break;
		case MB_HRA_AO2_SRC:
			*value = gVar.ao[AO_2].source;
			break;
		case MB_HRA_AO2_DUTY:
			*value = gVar.ao[AO_2].mbValue;
			break;

		case MB_HRA_AO3_MODE:
			*value = gVar.ao[AO_3].mode;
			break;
//		case MB_HRA_AO3_SRC:
//			*value = gVar.ao[AO_3].source;
//			break;
		case MB_HRA_AO3_DUTY:
			*value = gVar.ao[AO_3].value;
			break;

		case MB_HRA_AO4_MODE:
			*value = gVar.ao[AO_4].mode;
			break;
//		case MB_HRA_AO4_SRC:
//			*value = gVar.ao[AO_4].source;
//			break;
		case MB_HRA_AO4_DUTY:
			*value = gVar.ao[AO_4].value;
			break;

		/*Net Information------------*/

		case MB_HRA_DEV_MAC_1:		//ok
			*value = gVar.w5500Chip.netInfo.mac[0];
			break;
		case MB_HRA_DEV_MAC_2:
			*value = gVar.w5500Chip.netInfo.mac[1];
			break;
		case MB_HRA_DEV_MAC_3:
			*value = gVar.w5500Chip.netInfo.mac[2];
			break;
		case MB_HRA_DEV_MAC_4:
			*value = gVar.w5500Chip.netInfo.mac[3];
			break;
		case MB_HRA_DEV_MAC_5:
			*value = gVar.w5500Chip.netInfo.mac[4];
			break;
		case MB_HRA_DEV_MAC_6:
			*value = gVar.w5500Chip.netInfo.mac[5];
			break;

		case MB_HRA_DEV_IP_1:
			*value = gVar.w5500Chip.netInfo.ip[0];
			break;
		case MB_HRA_DEV_IP_2:
			*value = gVar.w5500Chip.netInfo.ip[1];
			break;
		case MB_HRA_DEV_IP_3:
			*value = gVar.w5500Chip.netInfo.ip[2];
			break;
		case MB_HRA_DEV_IP_4:
			*value = gVar.w5500Chip.netInfo.ip[3];
			break;

		case MB_HRA_DEV_SN_1:
			*value = gVar.w5500Chip.netInfo.sn[0];
			break;
		case MB_HRA_DEV_SN_2:
			*value = gVar.w5500Chip.netInfo.sn[1];
			break;
		case MB_HRA_DEV_SN_3:
			*value = gVar.w5500Chip.netInfo.sn[2];
			break;
		case MB_HRA_DEV_SN_4:
			*value = gVar.w5500Chip.netInfo.sn[3];
			break;

		case MB_HRA_DEV_GW_1:
			*value = gVar.w5500Chip.netInfo.gw[0];
			break;
		case MB_HRA_DEV_GW_2:
			*value = gVar.w5500Chip.netInfo.gw[1];
			break;
		case MB_HRA_DEV_GW_3:
			*value = gVar.w5500Chip.netInfo.gw[2];
			break;
		case MB_HRA_DEV_GW_4:
			*value = gVar.w5500Chip.netInfo.gw[3];
			break;

		case MB_HRA_DEV_DNS_1:
			*value = gVar.w5500Chip.netInfo.dns[0];
			break;
		case MB_HRA_DEV_DNS_2:
			*value = gVar.w5500Chip.netInfo.dns[1];
			break;
		case MB_HRA_DEV_DNS_3:
			*value = gVar.w5500Chip.netInfo.dns[2];
			break;
		case MB_HRA_DEV_DNS_4:
			*value = gVar.w5500Chip.netInfo.dns[3];
			break;

		/*NModbus TCP Server------------*/
	//	MB_HRA_MB_TCPS_TRANS_MAX,
		//socket - 0 & 1
		case MB_HRA_MB_TCPS_SOCK0_SRC_PORT:
			*value = gVar.server.sock[0].srcPort;
			break;

		case MB_HRA_MB_TCPS_SOCK0_KPALV_TIM:
			*value = gVar.server.sock[0].kpAlvTime;
			break;

//		case MB_HRA_MB_TCPS_SOCK1_SRC_PORT:
////			*value = gVar.server.sock[1].srcPort;
//			break;
//		case MB_HRA_MB_TCPS_SOCK1_KPALV_TIM:
////			*value = gVar.server.sock[1].kpAlvTime;
//			break;
		/*NModbus TCP Client------------*/

		case MB_HRA_MB_TCPC_DEST_IP_1:

			*value = gVar.client.sock[0].destIP[0];
			break;
		case MB_HRA_MB_TCPC_DEST_IP_2:
			*value = gVar.client.sock[0].destIP[1];
			break;
		case MB_HRA_MB_TCPC_DEST_IP_3:
			*value = gVar.client.sock[0].destIP[2];
			break;
		case MB_HRA_MB_TCPC_DEST_IP_4:
			*value = gVar.client.sock[0].destIP[3];
			break;
		case MB_HRA_MB_TCPC_DEST_PORT:
			*value = gVar.client.sock[0].destPort;
			break;
		case MB_HRA_MB_TCPC_SOCK_KPALV_TIM:
			*value = gVar.client.sock[0].kpAlvTime;
			break;
		case MB_HRA_MB_TCPC_SOCK_CONN_MODE:
			*value = gVar.client.sock[0].connectMode;
			break;
		case MB_HRA_MB_TCPC_TX_TIM_INTRVAL:

			*value = gVar.mbTCPClient.txTimIntrvl;

			break;
			/*NModbus Gateway------------*/
		case MB_HRA_MB_GW_TIMEOUT:
			*value = gVar.mbGateway.rplyTimer.timeout/10;
			break;
			/*NModbus RTU Serial------------*/
		case MB_HRA_MB_SERIAL_CONFIG:
			*value = gVar.mbSerial.serialConfigValue;
			break;
		case MB_HRA_MB_SERIAL_BAUD_RATE_MS16B:
			*value =  (uint16_t)((MBS_GetBaudRateUserDef())>>16);
			break;
		case MB_HRA_MB_SERIAL_BAUD_RATE_LS16B:
			*value =  (uint16_t)(MBS_GetBaudRateUserDef()&0x0000FFFF);
			break;
		case MB_HRA_MB_SERIAl_PROTOCOL:
			*value = gVar.mbSerial.protocolTemp;
			break;

		/*NModbus RTU Master------------*/
			/*Modbus RTU Master------------*/
		case MB_HRA_MB_MSTR_SLAVE_START_ADDR:
			*value = gVar.mbRTUMaster.custom.slvStartAddr;
			break;
		case MB_HRA_MB_MSTR_NUM_OF_SLAVES:
			*value = gVar.mbRTUMaster.custom.numOfSlv;
			break;
		case MB_HRA_MB_MSTR_REPLY_TIMEOUT:
			*value = gVar.mbRTUMaster.replyTimer.timeout/10;
			break;
		case MB_HRA_MB_MSTR_PERIODIC_REQ_INTERVAL:
			*value = gVar.mbRTUMaster.custom.reTransTimer.timeout/10;
			break;

	//	case MB_HRA_MB_MSTR_RETRY_MAX:
	//		break;
		/*NModbus RTU Slave------------*/
		case MB_HRA_MB_SLAVE_ADDRESS:
			*value = gVar.slvAddrTemp;
			break;
		case MB_HRA_MB_SLAVE_CUST_RESP_TIMEOUT:
			*value = gVar.mbRTUSlave.custom.waitingTimer.timeout/10;
			break;
		/*NModbus Gateway------------*/
	//	case MB_HRA_MB_GW_MODE:
	//		break;
	//	case MB_HRA_MB_GW_TIMEOUT:
	//		break;

		case MB_HRA_DEV_MODE:
			*value = gVar.devMode;
			break;


			/*Encoder----------------*/
		case MB_HRA_ENC_MODE:
			*value = gVar.dio.enc.configs.mode;
			break;
		case MB_HRA_ENC_QUAD_TRIG_MODE:
			*value = gVar.dio.enc.configs.quadrature.trigMode;
			break;
		case MB_HRA_ENC_CHA_TRIG_MODE:
			*value = gVar.dio.enc.configs.channelA.trigMode;
			break;
		case MB_HRA_ENC_CHB_TRIG_MODE:
			*value = gVar.dio.enc.configs.channelB.trigMode;
			break;
//		case MB_HRA_ENC_CHZ_TRIG_MODE:
//			break;



			/*PID-------------*/
		case MB_HRA_PID1_KP:
			*value = (uint16_t)(gVar.pid1.kp*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID1_KI:
			*value = (uint16_t)(gVar.pid1.ki*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID1_KD:
			*value = (uint16_t)(gVar.pid1.kd*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID1_I_OUT_LIMIT:
			*value = (uint16_t)gVar.pid1.output_i_max;
			break;

		case MB_HRA_PID2_KP:
			*value = (uint16_t)(gVar.pid2.kp*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID2_KI:
			*value = (uint16_t)(gVar.pid2.ki*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID2_KD:
			*value = (uint16_t)(gVar.pid2.kd*MB_VALUE_MULTIPLIER_100);
			break;
		case MB_HRA_PID2_I_OUT_LIMIT:
			*value = (uint16_t)gVar.pid2.output_i_max;
			break;


		default:
//			if(address < MB_HRA_MAX){
//				errorStatus =	MB_REG_GetHR(address, value);
//			}else{
//				errorStatus = MB_ERROR;
//			}
			errorStatus = MB_ERROR;
			break;
	}

//	DBG_SPRINT_NL("%s %s: { %s: %d, %s: %d }",STR_MB, STR_REQ, STR_ADDRESS, address, STR_VALUE, *value);
	return errorStatus;
}


uint8_t CB_MB_maskWriteHRCallbackS(MB_HRAddrEnum address, uint16_t andMask, uint16_t orMask){
	uint8_t errorStatus = MB_OK;
	/*------Write your code here------*/


	return errorStatus;

}



/*
 * Callback the custom function code 107, to pack Analog Inputs & digital input
 * Regular read functions has lot checking and small function, thats why those make lot of overhead.
 * also those have time complexity. So that, this function is introduced
 * Purpose of this function is to reduce the overhead to build the response packet for 107 fc
 * */

uint8_t CB_MB_CUST_Build107RespAIDI(uint8_t *buff, uint16_t size){

	uint16_t len = 0;
	for(uint16_t i = 0; i < AI_MAX; i++){
		if(len>= size) return len;
		buff[len++] = (gVar.ai[i].rawValue>>8)&0xFF;
		buff[len++] = gVar.ai[i].rawValue&0xFF;
	}
	buff[len++] = 0;	// set the device status byte, it is 9th byte
	if(len>= size) return len;

	//(gVar.dio.di[DI_1].config.port->IDR & gVar.dio.di[DI_1].config.pin) != (uint32_t)GPIO_PIN_RESET;
	buff[len] = 0;
	for(uint8_t i = DI_1; i < DI_MAX; i++){
		buff[len] |= HAL_GPIO_ReadPin(gVar.dio.di[i].config.port, gVar.dio.di[i].config.pin)<<(i-DI_1);
	}
	return ++len;
}


/*
 * Callback the custom function code 109, to pack Analog outputs & digital output
 * Regular write functions has lot checking and small function, thats why those make lot of overhead.
 * also those have time complexity. So that, this function is introduced
 * Purpose of this function is to reduce the overhead to write Analog outputs & digital output from the packet for 109 fc
 * */
uint8_t CB_MB_CUST_Process109ReqAODO(uint8_t *buff, uint16_t size){
	/*
	 * AO-1&2 have PID so we have to set the value to mbValue parameter, but AO-3&4 don't have PID so we directly set the value to value parameter
	*/
	if(size <= 7) return size;
	for(uint8_t i = AO_1; i < AO_MAX; i++){
		uint16_t aoValue = ((uint16_t)buff[(2*i)]<<8 | (uint16_t)buff[(2*i+1)]);
		if(aoValue <= AO_PWM_RESOLUTION) {
			if (i<=AO_2) gVar.ao[i].mbValue  = aoValue;
			else  gVar.ao[i].value = aoValue;
		}
	}
	buff[8] = 0;// set the device status byte, it is 9th byte(len = 9-1 = 8)
	uint8_t doByteIndex = 9;   // index 9 byte is for digital outputs
	if(size <= doByteIndex) return doByteIndex;
	for(uint8_t i = DO_1; i < DO_MAX; i++){
		gVar.dio.dout[i].state = ((buff[doByteIndex]>>(i-DO_1))&0x01);
		DO_Write(&gVar.dio.dout[i]);
	}

	return ++doByteIndex;
}


uint8_t CB_MB_CmpltTxCallback(MBS_Serial *mbSerial, uint16_t address, uint16_t value){
	uint8_t errorStatus = 0U;


		switch (mbSerial->mode) {
			case MBS_MODE_MASTER:

//				MBRM_StartReplyTimer(&gVar.mbRTUMaster.replyTimer);
	//			memcpy(gVar.mbRTUMaster., gVar.mbSerial.rxBuff,gVar.mbSerial.rxBuffSize );
				break;
			case MBS_MODE_SLAVE:
//				gVar.mbSerial.rxBuffSize = 0;
//				gVar.mbSerial.txBuffSize = 0;
//				gVar.mbRTUSlave.rxBufferSize = 0;
//				gVar.mbRTUSlave.txBufferSize = 0;
//				MB_ClearBuffer();

				gVar.mbRTUSlave.custom.numOfSlv = 0;
				break;
			case MBS_MODE_GATEWAY:

				if(MB_CUST_CheckCFunCode(gVar.mbGateway.custom.funCode)>0){
//					DBG_SPRINT("GFC:109");
					gVar.mbGateway.custom.slvStartAddr = gVar.mbSerial.txBuff[MB_FC_INDEX+1];
					gVar.mbGateway.custom.numOfSlv = gVar.mbSerial.txBuff[MB_FC_INDEX+2];
					gVar.mbGateway.txBuffSize = 0;
					gVar.mbGateway.isReqSent = 1;
					gVar.mbGateway.isBusy = 0;
					gVar.mbSerial.txBuffSize = 0;
					gVar.mbSerial.isBusy = 0;
					MBG_StartTimeout(&gVar.mbGateway.rplyTimer);
				}
					else if(MBTCP_isFunCodeValid(gVar.mbGateway.custom.funCode)>0U){
//					DBG_SPRINT("GFC:gen");
//					MBG_StartTimeout(&gVar.mbGateway.rplyTimer);
				}
				else{
					gVar.mbGateway.isReqSent = 1;
					gVar.mbGateway.isBusy = 0;
					gVar.mbSerial.isBusy = 0;
					MBG_StartTimeout(&gVar.mbGateway.rplyTimer);
				}
//				DBG_SPRINT("G_CB: %d",gVar.mbGateway.custom.funCode);

				break;
			default:
				break;
		}
	/*------Write your code here------*/
	if(gVar.restartFlage > 0){
		RestartMCU();
	}
	return errorStatus;
}

uint8_t CB_MB_CmoltRxCallback(MBS_Serial *mbSerial){

	TH_MBSerial();
//	switch (mbSerial->mode) {
//		case MBS_MODE_MASTER:
////			memcpy(gVar.mbRTUMaster., gVar.mbSerial.rxBuff,gVar.mbSerial.rxBuffSize );
//			break;
//		case MBS_MODE_SLAVE:
//
//			break;
//		case MBS_MODE_GATEWAY:
//
//			break;
//		default:
//			break;
//	}
	return 0;
}

uint8_t CB_MB_diagSubFuncCallback(uint16_t subFuncCode, uint16_t value){
	uint8_t errorStatus = 0U;
	/*------Write your code here------*/

	return errorStatus;
}



uint8_t CB_MB_ChangSACallback(uint8_t slaveAddress){
//	uint8_t errorStatus = 0U;
	/*------Write your code here------*/
	gVar.mbRTUSlave.slave_address = slaveAddress;
	return gVar.mbRTUSlave.slave_address == slaveAddress;
}





/*
 * @brief this function will call when a client established a connection to the server
 * */
uint8_t CB_Sock_ConnCallback(Socket *sock){

	uint8_t errorStatus = 0U;
	/*------Write your code here------*/
	switch (sock->mode) {
		case SOCK_MODE_TCP_SERVER:{
				MBTCPS_ClientParams client;
				client.id = ConvArrToVar4Bytes(sock->destIP);
				client.sockId = sock->id;
				client.port = sock->destPort;
				memcpy(client.ip, sock->destIP, sizeof(client.ip));
				memcpy(client.mac, sock->mac, sizeof(client.mac));
				client.isConnected = sock->isConnected;
//				DBG_SPRINT_NL("TCPS ConCB...");
				MBTCPS_ConnHandler(&gVar.mbTCPServer, &client);
			}
			break;
		case SOCK_MODE_TCP_CLIENT:

			break;
		default:
			break;
	}

	return errorStatus;
}

/*
 * @brief this function will call when a client disconnected from a server
 *
 * */

uint8_t CB_Sock_DisconnCallback(Socket *sock){
	uint8_t errorStatus = 0U;
	/*------Write your code here------*/

	switch (sock->mode) {
		case SOCK_MODE_TCP_SERVER:{
			gVar.mbTCPServer.isDataRcvd = 0;
//			DBG_SPRINT_NL("TCPS disconCB...");
			MBTCPS_DisconnHandler(&gVar.mbTCPServer,  ConvArrToVar4Bytes(sock->destIP), sock->destPort);
			}
			break;
		case SOCK_MODE_TCP_CLIENT:
			break;
		default:
			break;
	}
	MBS_ClearRXBuffer(&gVar.mbSerial);
	MBS_ClearTXBuffer(&gVar.mbSerial);
	gVar.mbSerial.isDataReceived = 0;
	return errorStatus;
}

/*
 * @brief this function will call when the data recevied form a client
 */
uint8_t CB_Sock_RecvCallback(Socket *sock){
	uint8_t errorStatus = 0U;
	/*------Write your code here------*/


	switch (sock->mode) {
		case SOCK_MODE_TCP_SERVER:{



			gVar.mbTCPServer.isDataRcvd = 1; 			// this flag is used to stable the tcp client

			if(sock->rxBuffSize > SOCK_BUF_MAX_SIZE){
				sock->rxBuffSize = SOCK_BUF_MAX_SIZE;
			}
			/*checking 109 packet and sending 107 packet--------*/
			uint16_t unitId = sock->rxBuff[MBTCP_INDX_UNIT_ID];
			uint16_t funCode = sock->rxBuff[MBTCP_INDX_UNIT_ID+1];
			// 107 packet
			if(unitId == MB_BROADCAST_ADDR && funCode == MB_CFC_WMHRCO_109){

				TH_MB_107PackHandler();

			}

			// if there is already regular packet is under processing, will return from here
			if(gVar.mbGateway.isRegularFC == 1){
//				DBG_SPRINT_NL("CFC Blocked...");
				break;
			}

			/*Check and parse the last packet if Ethernet chip received multiple packets ---------*/
			uint16_t length = MBTOOL_CombBytesToU16(sock->rxBuff[MBTCP_INDX_LENGTH_HI], sock->rxBuff[MBTCP_INDX_LENGTH_LO]) + MBTCP_INDX_HEADER_MAX -1;  // calculate the rcved packed length
			uint8_t *buff;
			uint16_t buffSize = sock->rxBuffSize ;
			uint16_t size = 0;
			buff = &sock->rxBuff[0];
//			DBG_SPRINT_NL("rxSz: %d, Len: %d",buffSize, length);


			uint16_t protoID = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_PROTOCOL_ID_HI], buff[MBTCP_INDX_PROTOCOL_ID_LO]);  // calculate the rcved packed length

			unitId = buff[MBTCP_INDX_UNIT_ID];
			funCode = buff[MBTCP_INDX_UNIT_ID+1];
			if(		!(protoID == MBTCP_PROTOCOL_ID && length >= 6
					&& ( MB_checkSlaveAddress(unitId) || unitId == MBTCP_SERVER_UNIT_ID)
					&& (MB_isFunctionCodeValid(funCode) || funCode == MB_CFC_WMHRCO_109))){
//				DBG_SPRINT_NL("buff[%d]: ",buffSize);
				for(uint16_t i = 0; i < buffSize; i++){
					size++;
//					DBG_SPRINT("%02X ",buff[i]);
					protoID = MBTOOL_CombBytesToU16(buff[i+MBTCP_INDX_PROTOCOL_ID_HI], buff[i+MBTCP_INDX_PROTOCOL_ID_LO]);  // calculate the rcved packed length
					length = MBTOOL_CombBytesToU16(buff[i+MBTCP_INDX_LENGTH_HI], buff[i+MBTCP_INDX_LENGTH_LO]);  // calculate the rcved packed length
					unitId = buff[i+MBTCP_INDX_UNIT_ID];
					funCode = buff[i+MBTCP_INDX_UNIT_ID+1];
//					DBG_SPRINT("\r\nPID: %04X Len: %04X uID: %02X FC: %02X ",protoID, length, unitId, funCode);
					if(		(protoID == MBTCP_PROTOCOL_ID && length >= 6
							&& ( MB_checkSlaveAddress(unitId) || unitId == MBTCP_SERVER_UNIT_ID)
							&& (MB_isFunctionCodeValid(funCode) || funCode == MB_CFC_WMHRCO_109))){

						length = MBTOOL_CombBytesToU16(buff[i+MBTCP_INDX_LENGTH_HI], buff[i+MBTCP_INDX_LENGTH_LO]) + MBTCP_INDX_HEADER_MAX -1;  // calculate the rcved packed length

//						size = length;
						buffSize -= i;
						buff = &buff[i];
						break;
					}
				}
			}

//			DBG_SPRINT_NL("rxSz: %d, Len: %d: sz: %d ",buffSize, length, size);
//			for(uint16_t i = 0; i<buffSize; i++){
//				DBG_SPRINT("%02X ",buff[i]);
//			}
			while(buffSize > length){
				uint8_t *lastBuff;
				uint16_t lastLen;

				funCode = buff[MBTCP_INDX_UNIT_ID+1];
				if(funCode != MB_CFC_WMHRCO_109){
					buffSize = length;
					gVar.mbGateway.isRegularFC = 1;			// blocks the 109-packet
					MBG_StartTimeout(&gVar.mbGateway.rplyTimRglrFC);
					break;
				}


				buffSize -= length;

				if(buffSize < length){
					lastBuff = buff;
					lastLen = length;
					buff = &buff[length];
					length = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_LENGTH_HI], buff[MBTCP_INDX_LENGTH_LO]) + MBTCP_INDX_HEADER_MAX -1;  // calculate the rcved packed length

					if(length < (6+MBTCP_INDX_HEADER_MAX -1)){
						buff = lastBuff;
						length = lastLen;
					}
					buffSize = length;
//					DBG_SPRINT_NL(" small -1");
					break;
				}else if(buffSize > length){
//					DBG_SPRINT_NL(" greater -1");
					lastBuff = buff;
					lastLen = length;
					buff = &buff[length];
					length = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_LENGTH_HI], buff[MBTCP_INDX_LENGTH_LO]) + MBTCP_INDX_HEADER_MAX -1;  // calculate the rcved packed length
					if(buffSize == length){
//						DBG_SPRINT_NL(" GEqual");
						break;
					}else if(buffSize < length){
						buff = lastBuff;
						length = lastLen;//MBTOOL_CombBytesToU16(buff[MBTCP_INDX_LENGTH_HI], buff[MBTCP_INDX_LENGTH_LO]) + MBTCP_INDX_HEADER_MAX -1;  // calculate the rcved packed length
						buffSize = length;
//						DBG_SPRINT_NL(" Gsmall -2");
						break;
					}
				}else{
//					DBG_SPRINT_NL(" equal");
					break;
				}
			}

			if(buffSize < length){
//								DBG_SPRINT_NL("SockRch...");
				break;
			}



//			DBG_SPRINT_NL("OutrxSz: %d, Len: %d, Sz: %d - ",buffSize, length, size);
//			for(uint16_t i = 0; i<buffSize; i++){
//				DBG_SPRINT("%02X ",buff[i]);
//			}



			MBTCPS_RecvHandler(&gVar.mbTCPServer,
					ConvArrToVar4Bytes(sock->destIP),
					sock->destPort,
					buff,//sock->rxBuff,
					buffSize//sock->rxBuffSize
					);
			}
			break;
		case SOCK_MODE_TCP_CLIENT:
			MBTCPC_ProcessResponse(&gVar.mbTCPClient, sock->rxBuff, sock->rxBuffSize);
			break;
		default:
			break;
	}

	memset(sock->rxBuff, 0, SOCK_BUF_MAX_SIZE);
	return errorStatus;
}

uint8_t CB_Sock_SendCmpltCallback(Socket *sock){
	uint8_t errorStatus = 0U;
	switch (sock->mode) {
		case SOCK_MODE_TCP_SERVER:
			if(MBTCPS_FindClientByIdAndPort(&gVar.mbTCPServer, ConvArrToVar4Bytes(sock->destIP), sock->destPort)==MB_OK){
					uint8_t index = gVar.mbTCPServer.clientIndex;
					memset(gVar.mbTCPServer.clients[index].txBuff,0, gVar.mbTCPServer.clients[index].txBuffSize);
					gVar.mbTCPServer.clients[index].txBuffSize = 0U;
					if(gVar.mbTCPServer.curntTrnsNo>0){
						gVar.mbTCPServer.curntTrnsNo--;
						//MBTCPC_RemoveFromTxPendingList(gVar.mbTCPClient.pendingList, gVar.mbTCPClient.lastTrnsIndex);
					}
				}
			break;
		case SOCK_MODE_TCP_CLIENT:

			if(gVar.mbTCPClient.curntTrnsNo>0){
				gVar.mbTCPClient.curntTrnsNo--;
				MBTCPC_RemoveFromTxPendingList(gVar.mbTCPClient.pendingList, gVar.mbTCPClient.lastTrnsIndex);
			}
			break;
		default:
			break;
	}

	//restart
	if(gVar.restartFlage > 0){
		RestartMCU();
	}


	return errorStatus;
}


uint8_t CB_Sock_ErrorCallback(Socket *sock, int32_t error){
	switch (error) {
				case SOCK_BUSY:

					break;
				case SOCKERR_SOCKNUM:

					break;
				case SOCKERR_SOCKMODE:

					break;
				case SOCKERR_SOCKINIT:

					break;
				case SOCKERR_SOCKFLAG:

					break;
				case SOCKERR_SOCKCLOSED:

					break;
				case SOCKERR_SOCKSTATUS:

					break;
				case SOCKERR_DATALEN:

					break;
				case SOCKERR_TIMEOUT:

					break;
				case SOCKERR_IPINVALID:
//					if(sock->mode == SOCK_MODE_TCP_SERVER){
//						uint32_t value = 0;
//						for( uint8_t i = 0;i < 4;i++){
//							 value = FSExt_ReadIntNum(SA_DEV_IP_1+i);
//							if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
//								uint8_t data[4] = CONF_DEF_NETINFO_IP;
//								sock->[i] = data[i];
//							}else{
//								sock->destIP[i] = (uint8_t)value;
//							}
//						}
//						value = FSExt_ReadIntNum(SA_MB_TCPS_SOCK0_SRC_PORT);
//						if(!(value >= 0 && value <= DEF_U16_MAX_VALUE)){
//							sock->srcPort = CONF_DEF_MB_TCPS_SRC_PORT;
//						}else{
//							sock->destPort = (uint16_t)value;
//						}
//					}
//					else

					if(sock->mode == SOCK_MODE_TCP_CLIENT){
//						uint8_t ip[4] = CONF_DEF_MB_TCPC_DEST_IP;
//						memcpy(sock->destIP, ip, 4);
//						sock->destPort = CONF_DEF_MB_TCPC_DEST_PORT;
						uint32_t value = 0;
						for( uint8_t i = 0;i < 4;i++){
							 value = FSExt_ReadIntNum(SA_MB_TCPC_DEST_IP_1+i);
							if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
								uint8_t data[4] = CONF_DEF_MB_TCPC_DEST_IP;
								sock->destIP[i] = data[i];
							}else{
								sock->destIP[i] = (uint8_t)value;
							}
						}
						value = FSExt_ReadIntNum(SA_MB_TCPC_DEST_PORT);
						if(!(value >= 0 && value <= DEF_U16_MAX_VALUE)){
							sock->destPort = CONF_DEF_MB_TCPC_DEST_PORT;
						}else{
							sock->destPort = (uint16_t)value;
						}
					}


					break;
				case SOCKERR_PORTZERO:
					if(sock->mode == SOCK_MODE_TCP_CLIENT){
						uint32_t value = 0;
						for( uint8_t i = 0;i < 4;i++){
							 value = FSExt_ReadIntNum(SA_MB_TCPC_DEST_IP_1+i);
							if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
								uint8_t data[4] = CONF_DEF_MB_TCPC_DEST_IP;
								sock->destIP[i] = data[i];
							}else{
								sock->destIP[i] = (uint8_t)value;
							}
						}
						value = FSExt_ReadIntNum(SA_MB_TCPC_DEST_PORT);
						if(!(value >= 0 && value <= DEF_U16_MAX_VALUE)){
							sock->destPort = CONF_DEF_MB_TCPC_DEST_PORT;
						}else{
							sock->destPort = (uint16_t)value;
						}
					}
					break;
				default:

					break;
			}
	return 0;
}


uint8_t CB_MBTCPS_SendCallback(MBTCPS_ClientParams *client){
//	DBG_SPRINT_NL("sendCB: sock: %d, id: %ld, port: %d, redy: %d ", client->sockId, client->id, client->port, gVar.server.sock[i].isReadyToSend);



	for(uint8_t i = 0; i < SERVER_SOCK_MAX; i++){
		if(client->id == ConvArrToVar4Bytes(gVar.server.sock[i].destIP)
		&& client->port == gVar.server.sock[i].destPort){

			if(gVar.server.sock[i].isReadyToSend != 0) continue;

			memcpy(gVar.server.sock[i].txBuff, client->txBuff, client->txBuffSize);
			gVar.server.sock[i].txBuffSize =client->txBuffSize;
			gVar.server.sock[i].isReadyToSend=1;
//			Socket_TxHandler(&gVar.server.sock[i]);
			return MB_OK;
		}
	}
	return MB_ERROR;
}

/*
 * send exception responce callback
 * return 0 = failed, 1 = success
 * */
uint8_t CB_MBTCPS_SendExcpCallback(MBTCPS_ClientParams *client){
	if(client->txBuffSize > 0){
		for(uint8_t i = 0; i < SERVER_SOCK_MAX; i++){
			if(client->id == ConvArrToVar4Bytes(gVar.server.sock[i].destIP)
			&& client->port == gVar.server.sock[i].destPort){
				if(gVar.server.sock[i].isReadyToSend != 0) return MB_ERROR;

				memcpy(gVar.server.sock[i].txBuff, client->txBuff, client->txBuffSize);
				gVar.server.sock[i].txBuffSize = client->txBuffSize;
				gVar.server.sock[i].isReadyToSend=1;
				return MB_OK;
			}
		}
	}
	return MB_ERROR;
}


//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
////	DI_CounterExtiCallback(GPIO_Pin);
//	if(gVar.dio.di[DI_7].config.pin == GPIO_Pin){
//		CB_DI_CounterExtiCallback(GPIO_Pin);
//	}else if(gVar.w5500Chip.spi.int_pin == GPIO_Pin){
//		gVar.w5500Chip.intrptFlag = 1;
////		W5500_ReadInterrupt(&gVar.w5500Chip);
//		// read interrupt for server sockets
//		for(uint8_t i = 0; i < SERVER_SOCK_MAX; i++){
////			Sock_ReadInterrupt(&gVar.server.sock[i]);
//		}
//		// read interrupt for client sockets
//		for(uint8_t i = 0; i < CLIENT_SOCK_MAX; i++){
////			Sock_ReadInterrupt(&gVar.client.sock[i]);
////			DBG_SPRINT_NL("Sock-%d: intrpt 0x%02x",gVar.client.sock[i].id ,gVar.client.sock[i].intr.val);
//		}
////		W5500_ClearAllInterrupt();
////		uint8_t initState = HAL_GPIO_ReadPin(gVar.w5500Chip.spi.int_port, gVar.w5500Chip.spi.int_pin);
////		DBG_SPRINT_NL("w5500 interrupt !: %d", initState);
//	}
//}


/*
 * This function is the error handler of eeprom for the I2C communication
 * */
void CB_EEPROM_ErrorCallback(uint8_t error){
	switch (error) {
		case HAL_ERROR:
			DBG_SPRINT_NL("%s: %s %s %s %s (%d) ",STR_ERROR, STR_EEPROM, STR_I2C, STR_COMM, STR_FAILED, error);
			break;
		case HAL_BUSY:
			DBG_SPRINT_NL("%s: %s %s (%d) ",STR_ERROR, STR_EEPROM, STR_BUSY, error);

			break;
		case HAL_TIMEOUT:
			DBG_SPRINT_NL("%s: %s %s %s %s (%d) ",STR_ERROR, STR_EEPROM, STR_TIMEOUT, error);
			break;
		default:
			break;
	}
}

