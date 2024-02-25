/*
 * TimeStamp.h
 *
 *  Created on: Mar 28, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef INC_TIMESTAMP_H_
#define INC_TIMESTAMP_H_
#include "stdint.h"
#include "main.h"

typedef enum TS_TimerType{
	TS_TIM_TYP_SYSTICK = 0U,
	TS_TIM_TYP_TIMER,
	TS_TIM_TYP_MAX,
}TS_TimerType;

typedef struct TimeStamp{
	TS_TimerType timerType;
	uint64_t currentTime;
	uint32_t ovfCount;

	uint32_t hr;
	uint32_t min;
	uint32_t sec;
	uint32_t ms;
	uint32_t us;
}TimeStamp;

void TS_AttachTimer(TIM_HandleTypeDef *htim, uint32_t busClock);
void TS_Init(TimeStamp *ts, TS_TimerType type);
void TS_SetTimerType(TimeStamp *ts, TS_TimerType type);
void TS_StartTimer(TimeStamp *ts);
void TS_StopTimer(TimeStamp *ts);

uint64_t TS_GetUS(TimeStamp *ts);
uint64_t TS_GetMS(TimeStamp *ts);
void TS_GetTime(TimeStamp *ts);
void TS_CatchOVF(TimeStamp *ts);

#endif /* INC_TIMESTAMP_H_ */

