/*
 * DigitalOut.c
 *
 *  Created on: Feb 12, 2024
 *      Author: wsrra
 */

#include "DigitalOut.h"


/**
  * @brief  Initializes the Digital out peripheral according to the specified parameters in the DO_Init.
  * @param  dout: pointer to a DigitalOut structure
  * @retval error status
  */
DO_Error DO_Init(DigitalOut_t *dout){

	if(dout == NULL) return DO_ERR_NULL_POINTER;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/*Initial state*/
	HAL_GPIO_WritePin(dout->port, dout->pin, GPIO_PIN_RESET);

	/*Digital out Configure */
	GPIO_InitStruct.Pin = dout->pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	return DO_ERR_NONE;
}


/**
  * @brief  Deinitializes the Digital out peripheral
  * @param  dout: pointer to a DigitalOut structure
  * @retval error status
  */
DO_Error DO_Deinit(DigitalOut_t *dout){
	if(dout == NULL) return DO_ERR_NULL_POINTER;
	HAL_GPIO_DeInit(dout->port, dout->pin);
	return DO_ERR_NONE;
}

/**
  * @brief Set digital output mode
  * @param  dout: pointer to a DigitalOut structure
  * @retval error status
  */
DO_Error DO_SetMode(DigitalOut_t *dout, DO_OutMode mode){
	if(dout == NULL) return DO_ERR_NULL_POINTER;
	else if(mode >= DO_MODE_MAX ) return DO_ERR_INVALIED_MODE;
	dout->mode = mode;
	return DO_ERR_NONE;
}

/**
  * @brief Get digital output mode
  * @param  dout: pointer to a DigitalOut structure
  * @retval none
  */
DO_OutMode DO_GetMode(DigitalOut_t *dout){
	return dout->mode;
}

/**
  * @brief Write digital output
  * @param  dout: pointer to a DigitalOut structure
  * @retval none
  */
void DO_Write(DigitalOut_t *dout){
	HAL_GPIO_WritePin(dout->port, dout->pin, dout->state);
}

/**
  * @brief Read digital output
  * @param  dout: pointer to a DigitalOut structure
  * @retval none
  */
uint8_t DO_Read(DigitalOut_t *dout){
	dout->state = HAL_GPIO_ReadPin(dout->port, dout->pin);
	return dout->state;
}



