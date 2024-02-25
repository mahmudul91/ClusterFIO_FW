/*
 * Scheduler.h
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

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

//typedef enum DEV_Mode{
//	DEV_MODE_SLAVE = 0,
//	DEV_MODE_MASTER,
//}DEV_Mode;
//
//typedef enum AI_ID{
//	AI_1 = 0,
//	AI_2,
//	AI_3,
//	AI_4,
//	AI_MAX,
//}AI_ID;
//
//typedef enum AO_ID{
//	AO_1 = 0,
//	AO_2,
//	AO_3,
//	AO_4,
//	AO_MAX,
//}AO_ID;
//
//typedef struct GlobalVar{
//	/*Utilities----------------*/
//	uint32_t counter;
//	uint32_t sysFreq;
//	DEV_Mode devMode;
//	uint8_t restartFlage;
//	uint8_t slvAddrTemp;		// tmporay slave address used for changing slave address
//
//	Diagnosis diag;
//	//encoder
//	uint32_t count;
//	int16_t sCount;
//	uint8_t dir;
//	/*W5500--------------------*/
//	W5500_Phy w5500Chip;
//	Server server;
//	Client client;
//	/*Modbus---------------------*/
//	MB_RTUHandler *mbHandle;
//	MBS_Serial mbSerial;
//	MB_RTUSlave mbRTUSlave;
//	MB_RTUMaster mbRTUMaster;
//	MB_TCPServer mbTCPServer;
//	MB_TCPClient mbTCPClient;
//	MB_Gateway mbGateway;
//	/*Analog Outputs------------*/
//	uint8_t aoId;
//	AnalogOut ao[AO_MAX];
////	AnalogOut ao2;
////	AnalogOut ao3;
////	AnalogOut ao4;
//
//	/*Analog Inputs------------*/
//#ifdef USE_AI_ADC_DMA
//	uint32_t aiBuffDMA[AI_MAX];
//#endif
//	uint8_t aiId;
//	AnalogIn ai[AI_MAX];
////	AnalogIn ai2;
////	AnalogIn ai3;
////	AnalogIn ai4;
//
//	/*Digital IOs-----------*/
//	struct DIO{
//		float pwmFreq;					// common PWM output frequency for all the channels
//		DigitalOut do_[DO_MAX];
//
//		//Digital inputs
//
//		DigitalIn di[DI_MAX];
//		Encoder enc;
//	}dio;
//
//	PID pid1;
//	PID pid2;
//	PID pid3;
//	PID pid4;
//
//
//
//}GlobalVar;
//


void SchedulerInit(void);
void Scheduler(void);

#endif /* INC_SCHEDULER_H_ */
