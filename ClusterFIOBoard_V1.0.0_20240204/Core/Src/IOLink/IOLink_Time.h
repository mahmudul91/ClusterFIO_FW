/*
 * IOL_Time.h
 *
 *  Created on: Nov 9, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_TIME_H_
#define SRC_IOLINK_IOLINK_TIME_H_
#include "stdint.h"
//#include "IOLink_Utility.h"
//#include "Timer.h"
//#include "IOLink.h"
//
//typedef enum IOL_TimerType{
//	IOL_TIMER_NONE = 0,
//	IOL_TIMER_TDMT,
//	IOL_TIMER_TDISO,
//	IOL_TIMER_TDWU,
//	IOL_TIMER_TSD,
//}IOL_TimerType;
//
//
//typedef struct IOL_ATTRIB_PACKED IOL_Timer_t{
//	Timer_t tmseq;
//	Timer_t tdmt;
//	Timer_t tdiso;
//	Timer_t tdwu;
//	Timer_t tsd;
//}IOL_Timer_t;
//
//void IOL_TimeHandler(IOL_Port_t *port);


#define __IOL_TIMER_MAX_TIMEOUT		0xFFFFFFFF


#define IOL_TIMER_OFF 				0
#define IOL_TIMER_ON				1

#define IOL_TIMER_TIMEOUT_NONE		0
#define IOL_TIMER_TIMEOUT			1

#define IOL_TIMER_RESET_NONE		0
#define IOL_TIMER_RESET				1

#define IOL_TIMER_RUNNING			2
#define IOL_TIMER_NOT_RUNNING		3

/* Timer structure
 * */
typedef struct __attribute__((__packed__)) IOL_Timer{
	uint8_t start;
	uint8_t isReset;
	uint64_t time;
	uint64_t timeout;
	uint8_t isTimeout;
//	uint32_t (*timeSrcCB)(void);
}IOL_Timer_t;

void IOL_Timer_Init(IOL_Timer_t *timer);
void IOL_Timer_Start(IOL_Timer_t *timer);
void IOL_Timer_Stop(IOL_Timer_t *timer);
uint64_t IOL_Timer_GetTime(void);
void IOL_Timer_SetTimeout(IOL_Timer_t *timer, uint64_t timeout);
void IOL_Timer_Reset(IOL_Timer_t *timer);
uint8_t IOL_Timer_IsRunning(IOL_Timer_t *timer);
uint8_t IOL_Timer_IsTimeout(IOL_Timer_t *timer);
uint8_t IOL_Timer_IsReset(IOL_Timer_t *timer);
void IOL_Timer_AttachTimeSourceCallback(uint64_t (*callback)(void));

#endif /* SRC_IOLINK_IOLINK_TIME_H_ */
