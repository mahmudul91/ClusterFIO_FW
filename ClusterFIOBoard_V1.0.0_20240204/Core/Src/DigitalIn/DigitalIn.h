/*
 * DIO_Handler.h
 *
 *  Created on: Mar 14, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_DIGITALIO_H_
#define INC_DIGITALIO_H_

#include "main.h"
#include "PWM.h"
#include "Define.h"
#include "Filters.h"

#define DI_FCTRY_RST_DELAY 				10000U		// 10000 ms = 10s factory reset delay,

#define DI_READ_PIN(PORTx,PIN)				((PORTx->IDR & PIN) != GPIO_PIN_RESET)
#define DI_WRITE_PIN(PORTx,PIN,STATE)		(PORTx->BSRR = (STATE!=GPIO_PIN_RESET ? PIN : (uint32_t)PIN << 16U))
#define DI_WRITE_PIN_SET(PORTx,PIN)			(PORTx->BSRR = PIN)
#define DIO_WRITE_PIN_RESET(PORTx,PIN)		(PORTx->BSRR = (uint32_t)PIN << 16U)

/*Digital Input Start -----------------------*/


/*
 * TODO:
 * 1. To implement Normal mode
 * 	  i) only trigger in low & high level
 * 	  ii) debounce filter
 * 	  iii) implement callback function
 * 	  iV) necessary config parameters
 *
 * 2. To implement counter mode
 * 	  i) trigger in all 4 triggering mode
 * 	  ii) debounce filter (need to ask)
 * 	  iii) implement callback function
 * 	  iV) necessary config parameters
 *
 * 3. To implement encoder mode
 * 	  i) 1x, 2x, & 4x counter
 * 	  ii) necessary config parameters
 * 	  iii) implement callback function
 * 	  iV) Home/Ref point callback
 * 	  V) reset counter
 * */

typedef enum DI_Error{
	DI_ERR_NONE = 0,
	DI_ERR_NULL_POINTER = (-1),
	DI_ERR_INVALIED_MODE = (-2),

}DI_Error;


//input mode for digital inputs
typedef enum DI_Mode{
	DI_MODE_NORMAL = 0u,					// normal IO operation mode
	DI_MODE_PULSE_CAPTURE,				// counter mode
//	DI_MODE_COUNTER,				// counter mode
//	DI_MODE_ENCODER_QUAD,				// quadrature encoder mode
//	DI_MODE_ENCODER_INCRE,				// quadrature encoder mode
	DI_MODE_MAX,
}DI_Mode;


/* input triggering mode
 *
 * */
typedef enum DI_TriggerMode{
	DI_TRIG_MODE_NONE = 0U,
	DI_TRIG_MODE_RISING,
	DI_TRIG_MODE_FALLING,
	DI_TRIG_MODE_RISING_FALLING,
	DI_TRIG_MAX,
}DI_TriggerMode;

/* input trigger*/

typedef enum DI_Trigger{
	DI_TRIG_NONE = 0U,
	DI_TRIG_RISING,
	DI_TRIG_FALLING,
}DI_Trigger;

/* Counyter type*/
typedef enum DI_CounterType{
	DI_COUNT_TYPE_NORMAL = 0U,			// external pulse counts by software count
	DI_COUNT_TYPE_TIMER,
	DI_COUNT_TYPE_EXTI,			// external pulse counts by using interrupt
}DI_CounterType;

/*Counter direction, up/down
 * */
typedef enum DI_CountMode{
	DI_COUNT_MODE_UP = 0U,
	DI_COUNT_MODE_DOWN,
}DI_CountMode;

typedef struct DI_PulseInf{
	uint32_t edgeTime;
	uint32_t lastEdgeTime;
	uint32_t period;
	uint32_t periodMin;
	uint16_t countOVF;		// over flow counter
	uint8_t isEdgeDtctd;		// used to calculate signal periode

	uint8_t isPulseDtctd;		// used to check, there is a pulse detection happened
}DI_PulseInf;

typedef struct DI_Configs{
	DI_Mode mode;
	DI_CounterType counterType;
	DI_CountMode countMode;				// configs for up and down count mode
	DI_TriggerMode trigMode;			// trigger mode used for the counter mode only only
	GPIO_TypeDef *port;
	uint16_t pin;
	uint32_t channel;
	IRQn_Type inturrptId;
}DI_Configs;

//struct for  digital inputs
typedef struct DigitalIn{
	//DIO_DinpSource input_source;
	uint8_t index;
	uint8_t isChanged;				// if any configuration is changed it will be 1, after applied of changed it will be set to 0
	DI_Configs config;
	FLTR_Debounce dbunce;			// Denounce filter to filter the switch bounce
	DI_Trigger trigger;				// It shows what is the last trigger happened
	uint8_t isTriggered;			// is trigger happened
	GPIO_PinState state;			// GPIO present state
	GPIO_PinState lastState;		// GPIO last state it is using to detect the edge
	uint32_t count;					// it is used only in counter mode
	DI_PulseInf pulseInf;
}DigitalIn_t;

DI_Error DI_Init(DigitalIn_t *di);
DI_Error DI_AttachTimer(TIM_HandleTypeDef *tim);
void DI_ConfigTimer(void);
void DI_ConfigNormalMode(DigitalIn_t *di);
void DI_ConfigPulsCapMode(DigitalIn_t *di);
void DI_ConfigPulsCapChannel(DigitalIn_t *di);
void DI_Config(DigitalIn_t *di);
void DI_DetectEdge(DigitalIn_t *di);
void DI_Read(DigitalIn_t *di);
void DI_Counter(DigitalIn_t *di);
void DI_CounterByExtI(DigitalIn_t *di);
void DI_CalculatePulse(DigitalIn_t *di);

void DI_AttachStateChangeCallback(void (*callbackFunc)(DigitalIn_t *di));
void DI_AttachCounterExtiCallback(void (*callbackFunc)(uint16_t pin));

/*Digital Input End -----------------------*/


//typedef struct DIO{
//	DIO_Input inputs[DIO_DINP_MAX];
//	DIO_Output outputs[DIO_DOUT_MAX];
//	//DIO_PairIO dinpMap[DIO_DINP_MAX];
//	//DIO_PairIO doutMap[DIO_DOUT_MAX];
//	//DIO_PairIO relayMap[DIO_RLY_MAX];
//}DIO;


//void DIO_Init(DigitalOut_t *dio);
//void DIO_Handler(DIO *dio);
//void DIO_TieInpOutOperation(DigitalIn_t *dinp, DigitalOut_t *dout);

void DIO_ReadDinp(DigitalIn_t *dinp);

//void DIO_WriteRelay(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
//GPIO_PinState DIO_ReadRelay(GPIO_TypeDef *port, uint16_t pin);
//void DIO_WriteDout(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
//GPIO_PinState DIO_ReadDout(GPIO_TypeDef *port, uint16_t pin);





#endif /* INC_DIGITALIO_H_ */
