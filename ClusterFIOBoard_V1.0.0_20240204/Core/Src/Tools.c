/*
 * Tools.c
 *
 *  Created on: Nov 11, 2022
 *      Author: wsrra
 */


#include "Tools.h"
#include "main.h"
#include "stdio.h"
#include "string.h"


/*
 * @brief Restart the MCU
 * */
void RestartMCU(void){
	NVIC_SystemReset();		// to reset the mcu
}


/*
 * @brief converts 4 byte size array into a single 4 byte variable
 * @param arr: pointer to an array
 * @retval returns converted 4 bytes of array data in a single 4 byte number
 */
uint32_t ConvArrToVar4Bytes(uint8_t *arr){
	ArrVarEx_4byts excenger;
	memcpy(excenger.arr, arr, 4);
	return excenger.var;
}

/*
 * @brief converts 4 byte number into 4 byte-size of array
 * @param var: 4 byte number
 * @param arr: pointer to an array, size 4, type uint8_t
 * @retval returns converted 4 bytes of array data in a single 4 byte number
 */
void ConvVarToArr4Bytes(uint8_t *arr, uint32_t var){
	ArrVarEx_4byts excenger;
	excenger.var = var;
	memcpy(arr, excenger.arr, 4);
}


/**
  * @brief converts to percent
  * @param value: value to be converted
  * @param maxValue:  max value
  * @retval none
  */
double ConvToPercent(double value, double maxValue){
 return ((maxValue > 0.0f) ? ((value*100.0f)/maxValue) : 0.0f);
}

double ConvPercentToValue(double percent, double value){
 return ((percent*value)/100.0f);
}



