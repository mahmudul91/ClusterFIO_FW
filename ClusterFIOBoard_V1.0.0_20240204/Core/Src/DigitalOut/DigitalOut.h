/*
 * DigitalOut.h
 *
 *  Created on: Feb 12, 2024
 *      Author: wsrra
 */

#ifndef SRC_DIGITALOUT_DIGITALOUT_H_
#define SRC_DIGITALOUT_DIGITALOUT_H_
#include "stdint.h"
#include "stm32h7xx_hal.h"
#include "PWM.h"


//#define DO_POLARITY_INVERSE

#define DO_READ(_dout_)				(((_dout_)->port->IDR & (_dout_)->pin) != GPIO_PIN_RESET)
#define DO_WRITE(_dout_,_state_)	((_dout_)->port->BSRR = (_state_!=GPIO_PIN_RESET ? (_dout_)->pin : (uint32_t)(_dout_)->pin << 16U))
#define DO_WRITE_SET(_dout_)		((_dout_)->port->BSRR = (_dout_)->pin)
#define DO_WRITE_RESET(_dout_)		((_dout_)->port->BSRR = (uint32_t)(_dout_)->pin << 16U)


typedef enum DO_Error{
	DO_ERR_NONE = 0,
	DO_ERR_NULL_POINTER = (-1),
	DO_ERR_INVALIED_MODE = (-2),

}DO_Error;

// enum for digital input source
typedef enum DO_InpSource{
	DO_SRC_DI_0 = 0U,
	DO_SRC_DI_1,
	DO_SRC_DI_2,
	DO_SRC_DI_3,
	DO_SRC_DI_4,
	DO_SRC_DI_5,
	DO_SRC_DI_6,
	DO_SRC_DI_7,
	DO_SRC_MODBUS,
	DO_SRC_MAX,
}DO_InpSource;

//Digital output modes
typedef enum DO_OutMode{
	DO_MODE_NONE = 0U,
	DO_MODE_FOLLOW,					// output will follow the input source
	DO_MODE_TOGGLE,					// toggle according to the input source state
	DO_MODE_BLINK,					// High speed pwm output mode
	DO_MODE_PWM,					// High speed pwm output mode
	DO_MODE_MAX,
}DO_OutMode;




//struct for  digital outputs
typedef struct DigitalOut{
	uint8_t index;
	DO_InpSource source;
	DO_OutMode mode;
	uint8_t isToggle;
	uint32_t tick;
	GPIO_PinState state;
	GPIO_PinState lastState;
	GPIO_TypeDef *port;
	uint16_t pin;
	GPIO_InitTypeDef pinInit;
	PWM pwm;
}DigitalOut_t;



DO_Error DO_Init(DigitalOut_t *dout, uint32_t mode, uint32_t pull, uint32_t speed);
DO_Error DO_Deinit(DigitalOut_t *dout);
DO_Error DO_SetMode(DigitalOut_t *dout, DO_OutMode mode);
DO_OutMode DO_GetMode(DigitalOut_t *dout);
void DO_Write(DigitalOut_t *dout);
uint8_t DO_Read(DigitalOut_t *dout);




/*Functions for the high speed PWM output*/
void DO_CalcHSPWM(DigitalOut_t *dout, float pwmFreq, uint32_t sysFreq);
void DO_GenHSPWM(DigitalOut_t *dout);




void DO_AttachStateCHangeCallback(void (*callbackFunc)(DigitalOut_t *dout));

#endif /* SRC_DIGITALOUT_DIGITALOUT_H_ */
