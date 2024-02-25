/*
 * IOL_Time.c
 *
 *  Created on: Nov 9, 2023
 *      Author: wsrra
 */




//#include <IOLink_Time.h>
//#include "IOLink_DL.h"
////#include "IOLink.h"
//
///*
// * All kind of delay handler for the IOlink
// * */
//void IOL_TimeHandler(IOL_Port_t *port){
//	IOL_Timer_t *timer = (IOL_Timer_t*)IOL_GetTimerInstance(port);
//
//	/*Check m-sequence duration is elapsed
//	 * */
//	if(TIMER_IsTimeout(&timer->tmseq) == TIMER_TIMEOUT){
////		IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
////		dl->isTmseqElpsd = 1;
//		TIMER_Stop(&timer->tmseq);
//		//TODO: to invoke message handler state machine here.
//	}
//
//	/*Check master message delay time
//	 * */
//	if(TIMER_IsTimeout(&timer->tdmt) == TIMER_TIMEOUT){
//		IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
//		dl->isTdmtElpsd = 1;
//		TIMER_Stop(&timer->tdmt);
//		//TODO: to invoke mode handler state machine here.
//	}
//
//}


#include "IOLink_Time.h"
#include "math.h"

uint64_t (*timeSourceCB)(void);

/*  @brief initialize the timer
 * */
void IOL_Timer_Init(IOL_Timer_t *timer){
	timer->start = IOL_TIMER_OFF;
	timer->time = 0U;
	timer->timeout = 0;
	timer->isReset = 0U;
}

/* @brief Start the timer
 * */
void IOL_Timer_Start(IOL_Timer_t *timer){
	timer->start = IOL_TIMER_ON;
	timer->isTimeout = IOL_TIMER_TIMEOUT_NONE;
	timer->time = timeSourceCB();

}

/* @brief Stop the timer
 * */
void IOL_Timer_Stop(IOL_Timer_t *timer){
	timer->start = IOL_TIMER_OFF;
	timer->time = 0;

}

/* @brief Get time*/
uint64_t IOL_Timer_GetTime(void){
	return (timeSourceCB != NULL ? timeSourceCB() : 0 );
}

/* @brief Set the timer timeout value
 * */
void IOL_Timer_SetTimeout(IOL_Timer_t *timer, uint64_t timeout){
	if(timeout > __IOL_TIMER_MAX_TIMEOUT) timer->timeout = __IOL_TIMER_MAX_TIMEOUT;
	else timer->timeout = timeout;
}

/* @brief Reset the timer
 * */
void IOL_Timer_Reset(IOL_Timer_t *timer){
	timer->time = timeSourceCB();
	timer->isReset = IOL_TIMER_RESET;
}

/* @brief CHeck the timer is started or not
 * */
uint8_t IOL_Timer_IsRunning(IOL_Timer_t *timer){
	return timer->start == IOL_TIMER_ON ? IOL_TIMER_RUNNING : IOL_TIMER_NOT_RUNNING;
}

/* @brief CHeck the timeout of the timer
 * */
uint8_t IOL_Timer_IsTimeout(IOL_Timer_t *timer){
	if(timer->start == IOL_TIMER_OFF) return IOL_TIMER_NOT_RUNNING;
	uint64_t currentTime = timeSourceCB();
	if((uint64_t)fabsf(currentTime - timer->time) >= timer->timeout){
		//timer->time = timer->timeSrcCB();
		timer->isTimeout = IOL_TIMER_TIMEOUT;
		return IOL_TIMER_TIMEOUT;
	}
	timer->isTimeout = IOL_TIMER_TIMEOUT_NONE;
	return IOL_TIMER_TIMEOUT_NONE;
}

/* @brief Check the reset status of the timer. also it will clear the reset flag
 * */
uint8_t IOL_Timer_IsReset(IOL_Timer_t *timer){
	uint8_t reset = timer->isReset;
	timer->isReset = IOL_TIMER_RESET_NONE;
	return reset;
}


/* @brief Attach the time source callback function for the timer
 * */
void IOL_Timer_AttachTimeSourceCallback(uint64_t (*callback)(void)){
	timeSourceCB = callback;
}
