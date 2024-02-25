/*
 * MB_Serial.h
 *
 *  Created on: Feb 24, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_SERIAL_H_
#define INC_MB_SERIAL_H_

#include <MB_Handler.h>
#include "main.h"
#include "math.h"
#include "MB_Config.h"
//#include "stm32f1xx_hal_gpio.h"
//#include "stm32f1xx_hal_tim.h"



#define MBS_RTU_PDU_MAX_SIZE 					MB_PDU_MAX_SIZE
#define MBS_RTU_PDU_MIN_SIZE 					MB_PDU_MIN_SIZE		// minimum application data unit size for the request, SA-1byte, FC-1byte, CRC-2bytes

/*Uncomment to disable Modbus standard time calculation and enable our time calculation method*/
//#define MBS_DISABLE_STANDARD_CHAR_TIM_CALC

#define MBS_1CHAR_TX_TIMEOUT_MAX				1.042f					//At 9600 baud rate the time to send 1 byte is 1.042 ms,
#define MBS_1P5_CHAR							1.5f//7.5f//3.5f//					// char to char interval at most 1.5 x Time_for_tx_1_char
#define MBS_3P5_CHAR							3.5f//14.5f//7.5f//					// frame to frame interval at most 3.5 x Time_for_tx_1_char
#define MBS_1P5_CHAR_INTERVAL_OVER_BR19200		750U					// 750 microsecond interval over 19200 baud rate for 1.5 char
#define MBS_3P5_CHAR_INTERVAL_OVER_BR19200		1750U					// 1750 microsecond interval over 19200 baud rate for 3.5 char

#define MBS_BR_MAX_STD_PROTO					2000000UL
#define MBS_BR_MAX_CLUSTER_PROTO				2000000UL
#define MBS_BR_MAX_CLUSTER_DMA_PROTO			2000000UL

/**
  * @brief Start timer.
  * There are 2 steps to start a timer
  * 	1. clear timer3 counter register to make sure the counter starts from 0
  * 	2. enable the timer counter
  * @param timer, instance of the 1.5 or 3.5 char timer
  * @retval None
  */
#define MBS_START_TIMER(timer)					do{ \
													(timer)->Instance->CNT = 0U; \
													(timer)->Instance->CR1 |= TIM_CR1_CEN; \
												}while(0);
/**
  * @brief stop timer.
  * There are 3 steps to start a timer
  * 	1. clear timer counter register value
  * 	2. stop the timer counter
  * 	3. clear the timer interrupt flag
  * @param timer, instance of the 1.5 or 3.5 char timer
  * @retval None
  */
#define MBS_STOP_TIMER(timer)					do{ \
													(timer)->Instance->CNT = 0U; \
													(timer)->Instance->CR1 &= ~TIM_CR1_CEN; \
													(timer)->Instance->SR &= ~TIM_SR_UIF; \
												}while(0);


/*
 * Check the protocol
 * */
#define MBS_CheckProtocol(protocol)				((protocol >= 0U) && (protocol < MBS_PROTOCOL_MAX))




typedef enum MBS_Protocol{
	MBS_PROTO_RTU_STD = 0U,		// modbus serial line standard protocol
	MBS_PROTO_RTU_CLUSTER,			// our custom serial line protocol. we named it Modbus RTU Cluster
	MBS_PROTO_RTU_CLUSTER_DMA,		// our custom serial line protocol. we named it Modbus RTU Cluster DMA
	MBS_PROTOCOL_MAX,
}MBS_Protocol;




typedef enum MBS_BaudRateIndex{
	MBS_BR_MIN_INDEX = 0U,
	MBS_BR1200_INDEX,
	MBS_BR1800_INDEX,
	MBS_BR2400_INDEX,
	MBS_BR4800_INDEX,
	MBS_BR9600_INDEX,
	MBS_BR14400_INDEX,
	MBS_BR19200_INDEX,
	MBS_BR28800_INDEX,
	MBS_BR38400_INDEX,
	MBS_BR57600_INDEX,
	MBS_BR115200_INDEX,
	MBS_BR230400_INDEX,
	MBS_BR460800_INDEX,
	MBS_BR576000_INDEX,
	MBS_BR921600_INDEX,
	MBS_BR_MAX_INDEX,
}MBS_BaudRateIndex;
typedef enum MBS_DataBits{
	MBS_DB_7 = 7U,
	MBS_DB_8,
}MBS_DataBits;

typedef enum MBS_ParityBit{
	MBS_PB_NONE = 0U,
	MBS_PB_ODD,
	MBS_PB_EVEN,
}MBS_ParityBit;


typedef enum MBS_StopBit{
	MBS_SB_1 = 1U,
	MBS_SB_2,
}MBS_StopBit;

/*Modbus mode, user can configure the device as modbus master or slave
 * */
typedef enum MBS_Mode{
	MBS_MODE_NONE = 0,
	MBS_MODE_MASTER,
	MBS_MODE_SLAVE ,
	MBS_MODE_GATEWAY,
	MBS_MODE_MAX,
}MBS_Mode;

typedef struct MBS_RespTimer{
	uint8_t enable;		// 1 = enable, 0 = disable
	uint16_t timer;				// count time in ms
	uint16_t timeout;			// timeout period in ms
	uint8_t isTimeout;			// time out flag

}MBS_RespTimer;


typedef struct MBS_Serial{
	MBS_Mode mode;
	MBS_Protocol protocol;
	MBS_Protocol protocolTemp;
	uint8_t isFreeze;  //Freezed
	uint16_t serialConfigMBAddress;
	uint16_t serialConfigValue;
	MBS_BaudRateIndex baudRateIndex;
//	uint32_t baudRateEx;		// extended baud rate
	uint32_t baudRate;
	MBS_DataBits dataBits;
	MBS_ParityBit parityBit;
	MBS_StopBit stopBit;
	uint8_t isDataReceived;	// Received
	uint8_t isBusy;	// is ready to send
	uint8_t isReceiving;	// is ready to send
	uint8_t isSending;
	uint8_t isSendComplete;
	uint8_t frameOkFlag;

	MBS_RespTimer respTimer;

	uint8_t rxBuff[MBS_RTU_PDU_MAX_SIZE];
	uint8_t txBuff[MBS_RTU_PDU_MAX_SIZE];
	uint16_t rxBuffSize;
	uint16_t txBuffSize;
	uint32_t errorCode;
	uint32_t dmaRxErrCode;
	uint32_t dmaTxErrCode;

	uint32_t rxCount;
	uint32_t txCount;
}MBS_Serial;


void MBS_Init(MBS_Serial *serial_, UART_HandleTypeDef *UART, TIM_HandleTypeDef *TIMER_1P5_CHAR, TIM_HandleTypeDef *TIMER_3P5_CHAR, GPIO_TypeDef *GPIO_PORT, uint16_t GPIO_PIN );
MBS_Serial *MBS_GetInstance(void);
MB_ErrorStatus MBS_Config(MBS_Serial *serial);
MB_ErrorStatus MBS_GetSerialConfig(MBS_Serial *serial);
void MBS_EncodeConfigValue(MBS_Serial *serial);
uint8_t MBS_DecodeConfigValue(MBS_Serial *serial);
MB_ErrorStatus MBS_UpdateSerialConfig(MBS_Serial *serial);
uint8_t MBS_CheckSerialConfigValidity(MBS_Serial *serial);
void MBS_SetBaudRateUserDef(uint32_t br);
uint32_t MBS_GetBaudRateUserDef(void);

void MBS_StopTimer(TIM_TypeDef *Timer_Inst);
void MBS_ChackRcvdFrame(MBS_Serial *serial, MB_RTUSlave *mbSlave);
MB_ErrorStatus MBS_Send(MBS_Serial *serial);
void MBS_Receive(MBS_Serial *serial);
void MBS_StopReceiving(MBS_Serial *serial);
void MBS_ProcessFrame(MBS_Serial *serial, MB_RTUSlave *mbSlave);
void MBS_ProcessRcvdFrame(MBS_Serial *serial, MB_RTUSlave *mbSlave);
void MBS_SetFramOk(void);
void MBS_ClearTXBuffer(MBS_Serial *serial);
void MBS_ClearRXBuffer(MBS_Serial *serial);

uint32_t MBS_GetCharTime(uint32_t baudRate, float charType);
uint32_t MBS_GetCharTimeCluster(uint32_t baudRate, float charType);

void MBS_attachCmpltTxCallback(uint8_t (*callbackFunc)(MBS_Serial *mbSerial, uint16_t address, uint16_t value));
void MBS_attachCmpltRxCallback(uint8_t (*callbackFunc)(MBS_Serial *mbSerial));
void MBS_AllCmpltTxCallback(MBS_Serial *serial,  uint16_t address, uint16_t value);


#endif /* INC_MB_SERIAL_H_ */

