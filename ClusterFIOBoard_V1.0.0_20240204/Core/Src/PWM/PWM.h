/*
 * PWM.h
 *
 *  Created on: Dec 30, 2022
 *      Author: wsrra
 */

#ifndef SRC_PWM_PWM_H_
#define SRC_PWM_PWM_H_
#include "stdint.h"
#include "main.h"

/*Un-comment to inverse the signal*/
//#define PWM_POLARITY_INV

#define PWM_DUTYCYCLE_RESOLUTION			4096U



/*PWM with timer-------------*/
typedef struct PWM{
	uint8_t enabled;			// turn on/off PWM
	uint32_t counter;
	uint8_t dutyCycle;
	uint16_t duty12Bits;			// duty cycle value in 12-bits (0 to 4095)
//	uint32_t sysClock;
//	float frequency;			// PWM frequency in Hz
//	float freqMax;				// PWM max supported frequency in Hz
//	float freqMin;				// PWM min supported frequency in Hz
	uint32_t period;			// Cycle Time in us
	uint32_t onDuration;		// On duration in us
	uint32_t offDuration;		// Off duration in us
	uint32_t channel;			// pwm output channel
	TIM_HandleTypeDef *timer;
	TIM_OC_InitTypeDef chConfigOC;
}PWM;

void PWM_Init(PWM *pwm, TIM_TypeDef *instance);
void PWM_ConfigTimer(PWM *pwm);

void PWM_InitChannel(PWM *pwm);
void PWM_ConfigChannel(PWM *pwm);
void PWM_Stop(PWM *pwm);
void PWM_Start(PWM *pwm);
double PWM_CalcPrescaler(double sysfreq, double pwmFreq, double pwmPeriod);
double PWM_CalcPeriod(double sysfreq, double pwmFreq, double pwmPrescaler);
#endif /* SRC_PWM_PWM_H_ */
