/*
 * AnalogIO.h
 *
 *  Created on: Nov 18, 2022
 *      Author: wsrra
 */

#ifndef INC_ANALOGIO_H_
#define INC_ANALOGIO_H_
#include "main.h"
#include "Filters.h"
#include "Define.h"
/*-------- Analog Output Define Start ---------*/
#define AO_PWM_RESOLUTION			4095U


#define AO_PWM_POLARITY_INV					// PWM signal polarity

#define AO_PWM_OFFSET_4T20MA		0U				// current output offset
#define AO_PWM_DUTY_AT_4MA			0.18f			// duty cycle at 4mA output
#define AO_PWM_DUTY_AT_20MA			0.91f			// duty cycle at 2mmA output

#define AO_PWM_OFFSET_0T10V			0U				// current output offset
#define AO_PWM_DUTY_AT_0V			0.0f			// duty cycle at 4mA output
#define AO_PWM_DUTY_AT_10V			0.846f			// duty cycle at 2mmA output
/*-------- Analog Output Define End ---------*/


/*-------- Analog Input Define Start ---------*/
/*
 * If it is uncomment adc DMA will activate otherwise polling will activate
 * */
#ifdef USE_AI_ADC_DMA
	#ifndef AI_USE_ADC_DMA
		#define AI_USE_ADC_DMA
	#endif
#endif

#define AI_CURRENT_LOWER_THRESHOLD				795				// analog input lower threshold at 4mA
#define AI_CURRENT_UPPER_THRESHOLD				3970			// analog input UPPER threshold at 20mA
#define AI_CURRENT_SCALE_FACTOR         		160.0f			// (It is actually the load resistor for input current)160 is the factor to get the actual current (4mA to 20mA) that is input to the measurement channel 1&2

#define AI_VOLT_LOWER_THRESHOLD					5				//lower threshold input voltage
#define AI_VOLT_UPPER_THRESHOLD					3730			// upper threshold input voltage
#define AI_VOLT_R1								56.0f//55.8			// 56 Kohm resister to make voltage divider
#define AI_VOLT_R2								24.0f//23.85f			// 24 Kohm resister to make voltage divider
#define AI_VOLT_SCALE_FACTOR 					(float)((AI_VOLT_R1+AI_VOLT_R2)/AI_VOLT_R2)	//  the factor calculated from the voltage divider circuit in the analog input
/*-------- Analog Input Define End ---------*/





/*
 * Analog IO type could be current or voltage
 * */
typedef enum AIO_Mode{
	AIO_MODE_CURRENT = 0,
	AIO_MODE_VOLT
}AIO_Mode;


/*-------- Analog Output Start ---------*/

// Sources by which driver board is operated
typedef enum AO_InpSrc{
	AO_PWM_SRC_NONE = 0U,		// no source applied
	AO_PWM_SRC_MODBUS,				// source HMI
	AO_PWM_SRC_PID,				// source HMI
//	AO_PWM_SRC_AI_1,			// source input channel (It may be 4 to 20mA or 0 to 10V)
//	AO_PWM_SRC_AI_2,			// source input channel (It may be 4 to 20mA or 0 to 10V)
//	AO_PWM_SRC_AI_3,			// source input channel (It may be 4 to 20mA or 0 to 10V)
//	AO_PWM_SRC_AI_4,			// source input channel (It may be 4 to 20mA or 0 to 10V)
	AO_PWM_SRC_MAX,
}AO_InpSrc;


typedef struct AO_PWM{
	uint8_t enabled;
	float frequency;		//PWM frequency
	uint32_t onDuration;		//On duration
	uint32_t offDuration;		//Off duration
	uint32_t period;			//Cycle Time
	uint8_t dutyCycle;
	uint8_t lastDutyCycle;
	uint32_t channel;					// pwm output channel
//	uint8_t changeFlag;					// changeFlag, 0 for no change, 1 for enabled variable, 2 for duty cycle
}AO_PWM;





typedef struct AnalogOut{
	uint8_t id;
	uint8_t enabled;
	AO_InpSrc source;
	AIO_Mode mode;
	AIO_Mode lastMode;
	uint16_t mbValue;						// use to calculate duty cycle
	uint16_t value;						// use to calculate duty cycle
	uint16_t lastValue;						// use to calculate duty cycle
	AO_PWM pwm;
	GPIO_TypeDef *vEnPort;				// Volt output enable port

//	uint16_t dutyCycle;					// duty cycle range: 20 - 100%
//	float frequency;
//	uint16_t period;
//	uint16_t onDuration;
////	uint16_t offDuration;
//	uint16_t channel;					// pwm output channel
//	uint8_t changeFlag;					// changeFlag, 0 for no change, 1 for enabled variable, 2 for duty cycle
}AnalogOut;



void AO_Init(TIM_HandleTypeDef *htim);
uint8_t AO_Enable(AnalogOut *ao);
uint8_t AO_Disable(AnalogOut *ao);
void AO_CalculatePWM(AnalogOut *ao);
uint8_t AO_UpdatePWM(AnalogOut *ao);
uint8_t AO_IsValidInpSrc(AO_InpSrc src);
/*-------- Analog Output End ---------*/



/*-------- Analog Input Start ---------*/


typedef struct AnalogIn{
	uint8_t id;
	uint32_t channel;					// input channel
	AIO_Mode mode;						// analog input mode
	uint16_t rawValue;					// raw adc value
	float voltage;						// in mV
	float current;						// in mA
	FLTR_Median medFilter;				// median filter
	FLTR_MovingAverage movAvgFilter;	// moving average filter
}AnalogIn;


void AI_Init(ADC_HandleTypeDef *hadc);
void AI_Config(AnalogIn *ai, uint32_t channel, AIO_Mode mode);
void AI_Read(AnalogIn *ai);
#ifdef AI_USE_ADC_DMA
uint32_t AI_StartReadingDMA(uint32_t *pData, uint32_t len);
uint32_t AI_StopReadingDMA(void);
#endif
/*-------- Analog Input End ---------*/

/*-------- Analog IO common function ---------*/

uint8_t AIO_IsValidMode(AIO_Mode mode);

#endif /* INC_ANALOGIO_H_ */
