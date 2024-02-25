/*
 * Tools.h
 *
 *  Created on: Nov 11, 2022
 *      Author: wsrra
 */

#ifndef INC_TOOLS_H_
#define INC_TOOLS_H_
#include "stdint.h"


#define MAX(a,b)   				(a > b ? a : b)			// returns max number between two numbers
#define MIN(a,b)				(a < b ? a : b)			// returns max number between two numbers


/*
 * union to convert array to bytes vice varsa
 * */
typedef union ArrVarEx_4byts{
	uint32_t var;
	uint8_t arr[4];
}ArrVarEx_4byts;


void RestartMCU(void);

uint32_t ConvArrToVar4Bytes(uint8_t *arr);
void ConvVarToArr4Bytes(uint8_t *arr, uint32_t var);
double ConvPercentToValue(double percent, double value);
double ConvToPercent(double value, double maxValue);


#endif /* INC_TOOLS_H_ */
