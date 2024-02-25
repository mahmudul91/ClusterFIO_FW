/*
 * Timer.c
 *
 *  Created on: Nov 8, 2023
 *      Author: wsrra
 */

#include "Timer.h"
#include "math.h"

uint64_t (*timeSrcCB)(void);

/*  @brief initialize the timer
 * */
void TIMER_Init(Timer_t *timer){
	timer->start = TIMER_OFF;
	timer->time = 0U;
	timer->timeout = __TIMER_MAX_TIMEOUT;
	timer->isReset = 0U;
	timeSrcCB = NULL;
}

/* @brief Start the timer
 * */
void TIMER_Start(Timer_t *timer){
	timer->start = TIMER_ON;
	timer->isTimeout = TIMER_TIMEOUT_NONE;
	timer->time = timeSrcCB();

}

/* @brief Stop the timer
 * */
void TIMER_Stop(Timer_t *timer){
	timer->start = TIMER_OFF;
	timer->time = 0;

}

/* @brief Set the timer timeout value
 * */
void TIMER_SetTimeout(Timer_t *timer, uint32_t timeout){
	if(timeout > __TIMER_MAX_TIMEOUT) timer->timeout = __TIMER_MAX_TIMEOUT;
	else timer->timeout = timeout;
}

/* @brief Reset the timer
 * */
void TIMER_Reset(Timer_t *timer){
	timer->time = timeSrcCB();
	timer->isReset = TIMER_RESET;
}

/* @brief CHeck the timer is started or not
 * */
uint8_t TIMER_IsRunning(Timer_t *timer){
	return timer->start == TIMER_ON ? TIMER_RUNNING : TIMER_NOT_RUNNING;
}

/* @brief CHeck the timeout of the timer
 * */
uint8_t TIMER_IsTimeout(Timer_t *timer){
	if(timer->start == TIMER_OFF) return TIMER_NOT_RUNNING;
	uint64_t currentTime = timeSrcCB();
	if((uint32_t)fabsf(currentTime - timer->time) >= timer->timeout){
		//timer->time = timer->timeSrcCB();
		timer->isTimeout = TIMER_TIMEOUT;
		return TIMER_TIMEOUT;
	}
	timer->isTimeout = TIMER_TIMEOUT_NONE;
	return TIMER_TIMEOUT_NONE;
}

/* @brief Check the reset status of the timer. also it will clear the reset flag
 * */
uint8_t TIMER_IsReset(Timer_t *timer){
	uint8_t reset = timer->isReset;
	timer->isReset = TIMER_RESET_NONE;
	return reset;
}


/* @brief Attach the time source callback function for the timer
 * */
void TIMER_AttachTimeSourceCallback(uint64_t (*callback)(void)){
	timeSrcCB = callback;
}
