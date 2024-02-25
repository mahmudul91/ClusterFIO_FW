/*
 * TimeStamp.c
 *
 *  Created on: Mar 28, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include "TimeStamp.h"
#include "Debug.h"

static TIM_HandleTypeDef *htim;
static double mcuSysClkFacktor;
static volatile uint64_t lastCount;		// total count from the last power up


void TS_Init(TimeStamp *ts, TS_TimerType type){
	ts->currentTime = 0;
	ts->ovfCount = 0;
	ts->timerType = type;
	ts->us = 0;
	ts->ms = 0;
	ts->sec = 0;
	ts->min = 0;
	ts->hr = 0;
	lastCount = 0;
}

void TS_SetTimerType(TimeStamp *ts, TS_TimerType type){
	ts->timerType = type;
}


/* Attach the time source,
 * busClock is in Hz
 * */
void TS_AttachTimer(TIM_HandleTypeDef *_htim, uint32_t busClock){
	htim = _htim;

	mcuSysClkFacktor = (((htim->Init.Prescaler+1.0f)*1000000.0f)/busClock);
	HAL_TIM_Base_Start_IT(htim);
}
/* This function start the timer
 * */
void TS_StartTimer(TimeStamp *ts){
	if(ts->timerType == TS_TIM_TYP_TIMER){
		HAL_TIM_Base_Start_IT(htim);
	}
}

/* This function start the timer
 * */
void TS_StopTimer(TimeStamp *ts){
	if(ts->timerType == TS_TIM_TYP_TIMER){
		HAL_TIM_Base_Stop_IT(htim);
	}
}

void TS_UpdateTime(TimeStamp *ts){

	ts->currentTime = ((uint64_t)htim->Instance->CNT + lastCount)*mcuSysClkFacktor;
//	ts->currentTime = ts->currentTime*mcuSysClkFacktor;
}

/* This function returns total microsecond
 * */
uint64_t TS_GetUS(TimeStamp *ts){

	ts->currentTime = (uint64_t)(( ts->timerType == TS_TIM_TYP_SYSTICK) ? ((HAL_GetTick()*1000)) : (htim->Instance->CNT + lastCount)*mcuSysClkFacktor);

	return ts->currentTime;
}

/* This function returns total millisecond
 * */
uint64_t TS_GetMS(TimeStamp *ts){
	return (TS_GetUS(ts)/1000);
}

void TS_GetTime(TimeStamp *ts){

	ts->currentTime = (uint64_t)(( ts->timerType == TS_TIM_TYP_SYSTICK) ? ((HAL_GetTick()*1000)) : (htim->Instance->CNT + lastCount)*mcuSysClkFacktor);
	DBG_SPRINT_NL("TS: Time: %0.0f, CNT %ld, sysclk: %0.4f", (double)ts->currentTime, htim->Instance->CNT, mcuSysClkFacktor);
	ts->sec = ts->currentTime/1000000;
	ts->ms = (ts->currentTime - ((uint64_t)ts->sec*1000000))/1000;
	ts->us = (ts->currentTime)%1000;

	ts->min = ts->sec/60;
	ts->hr = ts->min/60;
	ts->sec = ts->sec-(ts->min*60);
	ts->min = ts->min - (ts->hr*60);

}


/**
  * @brief  catch and count timer overflow. call this function inside the timer
  * overflow interrupt sub routine
  * @param  nts: pointer to the timeStamp struct
  * @retval none
  */
void TS_CatchOVF(TimeStamp *ts){
	ts->ovfCount++;
	lastCount =  (uint64_t)((uint64_t)htim->Init.Period * (uint64_t)ts->ovfCount);
}
