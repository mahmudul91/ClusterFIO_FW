/*
 * Timer.h
 *
 *  Created on: Nov 8, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_TIMER_H_
#define SRC_IOLINK_TIMER_H_
#include <stdio.h>
#include <stdint.h>

#define __TIMER_MAX_TIMEOUT		0xFFFFFFFF


#define TIMER_OFF 				0
#define TIMER_ON				1

#define TIMER_TIMEOUT_NONE		0
#define TIMER_TIMEOUT			1

#define TIMER_RESET_NONE		0
#define TIMER_RESET				1

#define TIMER_RUNNING			2
#define TIMER_NOT_RUNNING		3

/* Timer structure
 * */
typedef struct __attribute__((__packed__)) Timer_t{
	uint8_t start;
	uint8_t isReset;
	uint64_t time;
	uint32_t timeout;
	uint8_t isTimeout;
//	uint32_t (*timeSrcCB)(void);
}Timer_t;

void TIMER_Init(Timer_t *timer);
void TIMER_Start(Timer_t *timer);
void TIMER_Stop(Timer_t *timer);
void TIMER_SetTimeout(Timer_t *timer, uint32_t timeout);
void TIMER_Reset(Timer_t *timer);
uint8_t TIMER_IsRunning(Timer_t *timer);
uint8_t TIMER_IsTimeout(Timer_t *timer);
uint8_t TIMER_IsReset(Timer_t *timer);
void TIMER_AttachTimeSourceCallback(uint64_t (*callback)(void));

#endif /* SRC_IOLINK_TIMER_H_ */
