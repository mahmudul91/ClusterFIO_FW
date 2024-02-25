/*
 * AnalogIO.c
 *
 *  Created on: Nov 18, 2022
 *      Author: wsrra
 */

#include "AnalogIO.h"
#include "math.h"
#include "ADC.h"
#include "Debug.h"

TIM_HandleTypeDef *timer;			// timer for generating the PWM to get the analog output
ADC_HandleTypeDef *hadc;
TIM_OC_InitTypeDef chConfigOC = {0};
static uint32_t sysClock = 0;
static uint32_t aoPWMFreq = 0;

/*-------- Analog Output Start ---------*/


/**
  * @brief initialize timer for the PWM of analog output
  * @param _htim: pointer to the struct of timer which generating the PWM
  * @retval none
  */
void AO_Init(TIM_HandleTypeDef *_htim){
	timer = _htim;
	sysClock = HAL_RCC_GetHCLKFreq();
	aoPWMFreq = (float)(sysClock/(timer->Init.Period*timer->Init.Prescaler));

	chConfigOC.OCMode = TIM_OCMODE_PWM1;
	chConfigOC.Pulse = 0;
	#ifdef AO_PWM_POLARITY_INV
		chConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	#else
		chConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
	#endif
	chConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
}

/**
  * @brief Enable the analog output. Basically it will enable the the pwm channel
  * @param ao: pointer to the struct of AnalogOut
  * @retval error status
  */
uint8_t AO_Enable(AnalogOut *ao){
	return HAL_TIM_PWM_Start(timer, ao->pwm.channel);
}

/**
  * @brief Disable the analog output. Basically it will disable the pwm channel
  * @param ao: pointer to the struct of AnalogOut
  * @retval error status
  */
uint8_t AO_Disable(AnalogOut *ao){
	return HAL_TIM_PWM_Stop(timer, ao->pwm.channel);
}


/**
  * @brief calculates pwm parameters for the analog output
  * @param ao: pointer to the struct for the analog output
  * @retval none
  */
void AO_CalculatePWM(AnalogOut *ao){
	/*If value is not changed, it returns from the function immediately*/

	if(ao->value == ao->lastValue && ao->value != 0) return;
	ao->lastValue = ao->value;
//	DEBUG_SPRINT_NL("AO_1");
	/*check the Modbus value is not in the rang, it returns from the function immediately*/
	if(!((ao->value >= 0U && ao->value <= AO_PWM_RESOLUTION)) && (ao->pwm.period == 0)) return;
//	DEBUG_SPRINT_NL("AO_2");
	ao->pwm.period = timer->Init.Period;
	ao->pwm.frequency = aoPWMFreq;
	switch (ao->mode){
		case AIO_MODE_CURRENT:
				ao->pwm.onDuration = (uint16_t)roundf(((AO_PWM_DUTY_AT_20MA - AO_PWM_DUTY_AT_4MA) *
						(float)((float)(ao->value * ao->pwm.period) / (float)AO_PWM_RESOLUTION)) +
						(ao->pwm.period * AO_PWM_DUTY_AT_4MA));
			break;
		case AIO_MODE_VOLT:
				ao->pwm.onDuration = (uint16_t)roundf(((AO_PWM_DUTY_AT_10V - AO_PWM_DUTY_AT_0V) *
						(float)((float)(ao->value * ao->pwm.period) / (float)AO_PWM_RESOLUTION)) +
						(ao->pwm.period * AO_PWM_DUTY_AT_0V));
			break;
		default:
			return;
			break;
	}
	ao->pwm.dutyCycle = (uint16_t)roundf((float)((ao->pwm.onDuration*100.0f)/(float)ao->pwm.period));
	ao->pwm.offDuration = ao->pwm.period - ao->pwm.onDuration;
}


/**
  * @brief Generates PWM for analog output
  * @param ao: pointer to the struct for the analog output
  * @retval error status
  */
uint8_t AO_UpdatePWM(AnalogOut *ao){
	HAL_StatusTypeDef ret = HAL_OK;
	if(ao->enabled){
		/*if duty cycle not changed return from the loop*/
		if(ao->pwm.dutyCycle == ao->pwm.lastDutyCycle && ao->pwm.dutyCycle != 0) {return HAL_OK;}
		ao->pwm.lastDutyCycle = ao->pwm.dutyCycle ;
		if(!(ao->pwm.onDuration >= 0 && ao->pwm.onDuration <= timer->Init.Period)){return HAL_OK;}
		chConfigOC.Pulse = ao->pwm.onDuration;

	}else{
		chConfigOC.Pulse = 0;
	}
		/*Stop timer---------*/
		HAL_TIM_PWM_Stop(timer, (uint32_t)ao->pwm.channel);
		/*COnfig---------------------------------------*/

		ret = HAL_TIM_PWM_ConfigChannel(timer, &chConfigOC, (uint32_t)ao->pwm.channel);
		/*Start the timer again-----------*/
		HAL_TIM_PWM_Start(timer, (uint32_t)ao->pwm.channel);
	return ret;
}

/**
  * @brief Check analog output source is valid or not
  * @retval 1 = valid, 0 = invalid
  */
uint8_t AO_IsValidInpSrc(AO_InpSrc src){
	return (src > AO_PWM_SRC_NONE && src < AO_PWM_SRC_MAX);
}


/*-------- Analog Output End ---------*/


/*-------- Analog Input Start ---------*/
void AI_Init(ADC_HandleTypeDef *_hadc){
	hadc = _hadc;
#ifdef AI_USE_ADC_DMA
	ADC_InitDMA(hadc);
#else
	ADC_Init(hadc);
#endif
}

void AI_Config(AnalogIn *ai, uint32_t channel, AIO_Mode mode){
	ai->channel = channel;					// input channel
	ai->mode = mode;						// default input mode
	ai->rawValue = 0U;
	ai->current = 0.0f;
	ai->voltage = 0.0f;
	FLTR_ClearMedian(&ai->medFilter);
	FLTR_ClearMovAvg(&ai->movAvgFilter);

#ifdef AI_USE_ADC_DMA
//	uint8_t rank = 1;
//	switch (channel) {
//		case ADC_CHANNEL_10: rank = 1; break;
//		case ADC_CHANNEL_11: rank = 2; break;
//		case ADC_CHANNEL_12: rank = 3; break;
//		case ADC_CHANNEL_13: rank = 4; break;
//		default: break;
//	}
	ADC_ConfigChannel(hadc, channel, (ai->id+1), ADC_SAMPLETIME_84CYCLES);
#endif
}

void AI_Read(AnalogIn *ai){
#ifdef AI_USE_ADC_DMA
	ai->medFilter.nextValue = ai->rawValue;
#else
	ai->medFilter.nextValue = ADC_ReadValue_BM(hadc, ai->channel);			// read adc value
	ai->medFilter.nextValue = ADC_ReadValue(hadc, ai->channel);			// read adc value
#endif
	FLTR_ApplyMedian(&ai->medFilter);									// apply median filter
	ai->movAvgFilter.nextValue = ai->medFilter.value;					// set median output to moving average filter

	FLTR_ApplyMovAvg(&ai->movAvgFilter);								// apply moving average filter
	ai->rawValue = ai->movAvgFilter.value;								// set moving average output to the adc raw value

	switch (ai->mode) {
		case AIO_MODE_CURRENT:
			if(ai->rawValue <= AI_CURRENT_LOWER_THRESHOLD)			ai->rawValue = AI_CURRENT_LOWER_THRESHOLD;
			else if(ai->rawValue >= AI_CURRENT_UPPER_THRESHOLD) 	ai->rawValue = AI_CURRENT_UPPER_THRESHOLD;

			ai->voltage =(float)(ADC_CalcVolt(ai->rawValue)*1000.00f);		// in millivolt
			ai->current = (float)(ai->voltage/(float)AI_CURRENT_SCALE_FACTOR);									// in milliamp
			ai->rawValue =(uint16_t)(((ai->rawValue-AI_CURRENT_LOWER_THRESHOLD)* (ADC_RESOLUTION-1))/
					(AI_CURRENT_UPPER_THRESHOLD-AI_CURRENT_LOWER_THRESHOLD));																	// use the input channel for pwm duty cycle

			break;
		case AIO_MODE_VOLT:
			if(ai->rawValue <= AI_VOLT_LOWER_THRESHOLD)			ai->rawValue = AI_VOLT_LOWER_THRESHOLD;
			else if(ai->rawValue >= AI_VOLT_UPPER_THRESHOLD) 	ai->rawValue = AI_VOLT_UPPER_THRESHOLD;

			ai->voltage = ai->rawValue <= AI_VOLT_LOWER_THRESHOLD? 0.0F: (float)(ADC_CalcVolt(ai->rawValue)*AI_VOLT_SCALE_FACTOR*1000.00f);							// in millivolt
			ai->current = ai->rawValue <= AI_VOLT_LOWER_THRESHOLD? 0.0F: (float)((ai->voltage/1000.00f)/(AI_VOLT_R1+AI_VOLT_R2));									// in milliamp
			ai->rawValue =(uint16_t)(((ai->rawValue-AI_VOLT_LOWER_THRESHOLD)* (ADC_RESOLUTION-1))/
					(AI_VOLT_UPPER_THRESHOLD - AI_VOLT_LOWER_THRESHOLD));																	// use the input channel for pwm duty cycle
//			ai->voltage = ai->voltage/1000.0F;

			break;
		default:
			break;
	}

}

#ifdef AI_USE_ADC_DMA
uint32_t AI_StartReadingDMA(uint32_t *pData, uint32_t len){
	return ADC_StartReadingDMA(hadc, pData, len);
}

uint32_t AI_StopReadingDMA(void){
	return ADC_StopReadingDMA(hadc);
}
#endif
/*-------- Analog Output End ---------*/

/*-------- Analog IO common function ---------*/

/**
  * @brief Check analog IO mod is valid or not
  * @retval 1 = valid, 0 = invalid
  */
uint8_t AIO_IsValidMode(AIO_Mode mode){
	return (mode == AIO_MODE_CURRENT || mode == AIO_MODE_VOLT);
}

