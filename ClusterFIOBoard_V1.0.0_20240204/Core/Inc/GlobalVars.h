/*
 * GlobalVars.h
 *
 *  Created on: Jan 30, 2024
 *      Author: wsrra
 */

#ifndef INC_GLOBALVARS_H_
#define INC_GLOBALVARS_H_
#include <stdio.h>
#include "IOLink.h"
#include "../Src/IOLink/l6360.h"
#include "TimeStamp.h"
#include "DigitalOut.h"


#include "Server.h"
#include "w5500_chip.h"
#include "MB_Serial.h"
#include "MB_RTUSlave.h"
#include "MB_TCP.h"
#include "AnalogIO.h"
#include "DigitalIn.h"
#include "Encoder.h"
#include "Client.h"
#include "MB_TCPServer.h"
#include "MB_Gateway.h"
#include "MB_RTUMaster.h"
#include "MB_TCPClient.h"
#include "PID_Controller.h"
#include "TimeStamp.h"
#include "Diagnosis.h"
#include "PCA9557_IOExt.h"
#include "Define.h"

#define IOL_GET_INDEX(portNumber) (portNumber - 1)




/*digital output index---------------*/
typedef enum DO_Index{
	DO_1 = 0,					// Digital output with high speed PWM (up to)
	DO_2,					// Digital output with high speed PWM
	DO_3,					// Digital output with high speed PWM
	DO_4,					// Digital output with high speed PWM
	DO_5,					// Digital output with LOW speed PWM
	DO_6,					// Digital output with LOW speed PWM
	DO_MAX,
}DO_Index;

/*Digital input index--------------*/
typedef enum DI_Index{
	DI_1 = 0U,
	DI_2,
	DI_3,
	DI_4,
	DI_5,
	DI_6,
	DI_7,
	DI_8,
	DI_MAX,
}DI_Index;




/*PORT number*/
typedef enum IOL_PortNumber{
	IOL_PORT_BR	= 0,		// port number for broadcast
	IOL_PORT_1,
	IOL_PORT_2,
	IOL_PORT_3,
	IOL_PORT_4,
	IOL_PORT_MAX,
}IOL_PortNumber;

/*Port array index*/
typedef enum IOL_PortIndex{
	IOL_PI_0 = 0,		//BroadCastID
	IOL_PI_1,
	IOL_PI_2,
	IOL_PI_3,
	IOL_PI_MAX,
}IOL_PortIndex_t;


typedef enum DEV_Mode{
	DEV_MODE_SLAVE = 0,
	DEV_MODE_MASTER,
}DEV_Mode;

typedef enum AI_ID{
	AI_1 = 0,
	AI_2,
//	AI_3,
//	AI_4,
	AI_MAX = 2,
}AI_ID;

typedef enum AO_ID{
	AO_1 = 0,
	AO_2,
	AO_3,
	AO_4,
	AO_MAX,
}AO_ID;


/*General purpose led----------*/
typedef enum LED_ID{
	LED_RS485_RX = 0,
	LED_RS485_TX,
	LED_1,
	LED_2,
	LED_MAX,
}LED_ID;
typedef struct LED_GP{
	LED_ID id;
	PCA_Pin_te pin;	// io extender pin
	uint8_t	state;
	uint8_t	lastState;
}LED_GP_t;


typedef struct IO_Link_Master{
	L6360_IOLink_hw_t port[IOL_PI_MAX];	//IOlink master chip
}IO_Link_Master_t;

typedef struct GlobalVar_t{

	/*Utilities----------------*/
	uint32_t counter;
	uint32_t sysFreq;
	DEV_Mode devMode;
	uint8_t restartFlage;
	uint8_t slvAddrTemp;		// tmporay slave address used for changing slave address

	Diagnosis diag;
	//encoder
	uint32_t count;
	int16_t sCount;
	uint8_t dir;
	/*W5500--------------------*/
	W5500_Phy w5500Chip;
	Server server;
	Client client;
	/*Modbus---------------------*/
	MB_RTUHandler *mbHandle;
	MBS_Serial mbSerial;
	MB_RTUSlave mbRTUSlave;
	MB_RTUMaster mbRTUMaster;
	MB_TCPServer mbTCPServer;
	MB_TCPClient mbTCPClient;
	MB_Gateway mbGateway;
	/*Analog Outputs------------*/
	uint8_t aoId;
	AnalogOut ao[AO_MAX];

	/*Analog Inputs------------*/
#ifdef USE_AI_ADC_DMA
	uint32_t aiBuffDMA[AI_MAX];
#endif
	uint8_t aiId;
	AnalogIn ai[AI_MAX];

	/*Digital IOs-----------*/
	struct DIO{
		float pwmFreq;					// common PWM output frequency for all the channels
		DigitalOut_t dout[DO_MAX];
		DigitalIn_t di[DI_MAX];
		Encoder enc;
	}dio;

	/*Extended IOS---------------*/
	PCA9557_IOExt_ts ioExt;

	LED_GP_t led[LED_MAX];

	/*PID---------*/
	PID pid1;
	PID pid2;
	PID pid3;
	PID pid4;


	/*Digital IOs-----------*/
//	DigitalOut dout[DO_MAX];
	IO_Link_Master_t iolm;
	TimeStamp ts;


}GlobalVar_t;

GlobalVar_t gVar;


#endif /* INC_GLOBALVARS_H_ */

