/*
 * ADC.h
 *
 *  Created on: Dec 27, 2022
 *      Author: wsrra
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_
#include "main.h"

#define ADC_REF_VOLT		3.3f
#define ADC_RESOLUTION		4096U

void ADC_Init(ADC_HandleTypeDef *hadc);
void ADC_SelectChannel(ADC_HandleTypeDef *hadc, uint32_t Channel);
uint32_t ADC_ReadValue(ADC_HandleTypeDef *hadc, uint32_t channel);
uint32_t ADC_ReadValue_BM(ADC_HandleTypeDef *hadc, uint32_t channel);
float ADC_CalcVolt(uint16_t adcValue);

void ADC_InitDMA(ADC_HandleTypeDef *hadc);
void ADC_ConfigChannel(ADC_HandleTypeDef *hadc, uint32_t Channel, uint32_t rank,  uint32_t SamplingTime);
uint32_t ADC_StartReadingDMA(ADC_HandleTypeDef *hadc, uint32_t* pData, uint32_t Length);
uint32_t ADC_StopReadingDMA(ADC_HandleTypeDef *hadc);
#endif /* INC_ADC_H_ */
