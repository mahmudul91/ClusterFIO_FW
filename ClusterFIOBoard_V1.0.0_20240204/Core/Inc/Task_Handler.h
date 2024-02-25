/*
 * Test.h
 *
 *  Created on: Jan 18, 2024
 *      Author: wsrra
 */

#ifndef INC_TASK_HANDLER_H_
#define INC_TASK_HANDLER_H_
#include <DigitalIn.h>
#include "IOLink_Define.h"

/*Old----------------*/
#include "MB_TCP.h"
#include "SocketHandler.h"
#include "MB_TCPServer.h"
#include "PID_Controller.h"
#include "AnalogIO.h"

void TaskInit(void);
void TaskHandler(void);

void TH_PrintDevInfo(void);
void TH_DigitalOut(void);

/*Old----------------*/

void TH_MBTCPSRecvRespGW(MB_TCPServer *mbtcps);
void TH_MBTCPServer(void);
void TH_MBTCPClient(void);
//void TH_CpyClientInfoFromSock(MBTCPS_ClientParams *client, Socket *sock, uint8_t reqType);
void TH_MBGateway(void);
void TH_MBGWCheckTimeOut(void);

void TH_MBSerial(void);
void TH_MBRTUMaster(void);
void TH_MB_107PackHandler(void);
void TH_MBRTUMasterTimoutHandler(void);
void TH_MBRTUSlaveTimoutHandler(void);
uint16_t TH_AddEncoderDataToPacket(uint8_t *buff);
uint16_t TH_AddCounterDataToPacket(uint8_t *buff);

void TH_MBRTUBusErrDiag(void);
void TH_W5500SPIErrDiag(void);

/*Task handler for digital IO ------------ */
void TH_DigitalOut(void);
void TH_DigitalInConfig(void);
void TH_DigitalIn(void);
void TH_DICapturePulse(TIM_HandleTypeDef* tim);
void TH_DICaptureOverflow(TIM_HandleTypeDef* tim);
void TH_DIClearOnNoPulse(void);

/*Analog Outputs---------------------*/
void TH_AnalogOutput(void);
void TH_AOMap2DIandPID(AnalogOut *ao, DigitalIn_t *di, PID *pid);

/*Analog Inputs---------------------*/
void TH_AnalogInput(void);


/*PIDS-----------*/
float TH_ScalePriedTo12Bits(uint32_t period, uint32_t periodMin);
void TH_PIDControl(PID *pid);

void TH_MBREG_UpdateIR(void);
void TH_MBREG_UpdateHR(void);

void TH_FactoryResetButton(void);

/*IO Extender--------------*/
void TH_IOExtender(void);

#endif /* INC_TASK_HANDLER_H_ */

