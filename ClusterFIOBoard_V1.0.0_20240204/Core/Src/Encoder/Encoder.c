/*
 * Encoder.c
 *
 *  Created on: Jan 9, 2023
 *      Author: wsrra
 */
#include "Encoder.h"
#include "math.h"
#include "Debug.h"

/*Private functions------------------*/
static void ENC_ConfigQuad(Encoder *enc);
void DI_ConfigInceExtI(ENC_ConfigIncre *encIcre);

/*Common functions-------------------*/


/*@brief Configs the encoder
 * */
void ENC_Config(Encoder *enc){

	if(enc->enable == 0) return; // if not enable return from the function

//	if(enc != NULL && enc->isChanged > 0){
//		enc->isChanged = 0;

		switch (enc->configs.mode) {
			case ENC_MODE_QUAD:
				{

					// deinit gpio for encoder channel A, B & Z
					HAL_GPIO_DeInit(enc->configs.channelA.port, enc->configs.channelA.pin);
					HAL_GPIO_DeInit(enc->configs.channelB.port, enc->configs.channelB.pin);
					HAL_GPIO_DeInit(enc->configs.channelZ.port, enc->configs.channelZ.pin);

					// config timers for quadrature encoder with channel A & B
					ENC_ConfigQuad(enc);

					// config channel Z for quadrature home position
					DI_ConfigInceExtI(&enc->configs.channelZ);
					enc->counterZ = 0;
				}
				break;
			case ENC_MODE_INCRE_CHA:
				HAL_TIM_Encoder_Stop(enc->configs.quadrature.timer, TIM_CHANNEL_ALL);
				HAL_TIM_Encoder_DeInit(enc->configs.quadrature.timer);
				DI_ConfigInceExtI(&enc->configs.channelA);
				break;
			case ENC_MODE_INCRE_CHB:
				HAL_TIM_Encoder_Stop(enc->configs.quadrature.timer, TIM_CHANNEL_ALL);
				HAL_TIM_Encoder_DeInit(enc->configs.quadrature.timer);
				DI_ConfigInceExtI(&enc->configs.channelB);
				break;
			case ENC_MODE_INCRE_CHAB:
				HAL_TIM_Encoder_Stop(enc->configs.quadrature.timer, TIM_CHANNEL_ALL);
				HAL_TIM_Encoder_DeInit(enc->configs.quadrature.timer);
				DI_ConfigInceExtI(&enc->configs.channelA);
				DI_ConfigInceExtI(&enc->configs.channelB);
				break;
			default:
				break;
		}
//	}

}


/*@brief to set the encode mode
 * @param enc: pointer to encoder structure
 * @param mode: encoder mode, there are 2 encoder mode, quadrature and incremental
 * @retval none
 * */
void ENC_SetMode(Encoder *enc, uint8_t mode){
	enc->configs.mode = mode;
	enc->isChanged = 1;
}

/*@brief to set the isChanged flag manually if user make the changes by direct setting
 * the value to the parameter instead of using set function
 * @param enc: pointer to encoder structure
 * @retval none
 * */
void ENC_NotifyOnChanges(Encoder *enc){
	enc->isChanged = 1;
}


void ENC_CalcPPR(Encoder *enc);


/*Quadrature encoder functions----------------------------*/

static void ENC_ConfigQuad(Encoder *enc){
	// stop the encoder before changing the configuration
	HAL_TIM_Encoder_Stop(enc->configs.quadrature.timer, TIM_CHANNEL_ALL);

	HAL_TIM_Base_DeInit(enc->configs.quadrature.timer);
	HAL_TIM_IC_DeInit(enc->configs.quadrature.timer);

    /* Peripheral clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA0-WKUP     ------> TIM2_CH1
    PA1     ------> TIM2_CH2
    */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = DI_1_ENC_A_Pin|DI_2_ENC_B_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


	TIM_Encoder_InitTypeDef sConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	enc->configs.quadrature.timer->Instance = TIM2;
	enc->configs.quadrature.timer->Init.Prescaler = 0;
	enc->configs.quadrature.timer->Init.CounterMode = TIM_COUNTERMODE_UP;
	enc->configs.quadrature.timer->Init.Period = ENC_QUAD_MAX_TIM_PERIOD;
	enc->configs.quadrature.timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	enc->configs.quadrature.timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = enc->configs.quadrature.trigMode == ENC_TRIG_MODE_RISING ? TIM_ICPOLARITY_RISING : TIM_ICPOLARITY_FALLING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = enc->configs.quadrature.trigMode == ENC_TRIG_MODE_RISING ? TIM_ICPOLARITY_RISING : TIM_ICPOLARITY_FALLING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	enc->configs.quadrature.timerConfig = sConfig;
	if (HAL_TIM_Encoder_Init(enc->configs.quadrature.timer, &enc->configs.quadrature.timerConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(enc->configs.quadrature.timer, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	// start the encoder after changing the configuration
	HAL_TIM_Encoder_Start(enc->configs.quadrature.timer, TIM_CHANNEL_ALL);

}

/**
  * @brief Read quadrature encoder value
  */
void ENC_ReadQuad(Encoder *enc){
	if(enc->enable > 0 && enc->configs.mode == ENC_MODE_QUAD){
		enc->counterQuad = __HAL_TIM_GET_COUNTER(enc->configs.quadrature.timer);

		ENC_GetDirQuad(enc);
		if( (enc->direction == ENC_DIR_CW && enc->lastCountQuad > 0) &&
			(enc->counterQuad >= (int32_t)(ENC_QUAD_MIN_COUNT-1) && enc->counterQuad < (int16_t)(0)) )
		{
			enc->counterQuad = enc->counterQuad&ENC_QUAD_MAX_COUNT;
			__HAL_TIM_SET_COUNTER(enc->configs.quadrature.timer, enc->counterQuad);
		}else if((enc->direction == ENC_DIR_CCW && enc->lastCountQuad < 0) &&
				(enc->counterQuad > (int32_t)(0) && enc->counterQuad <= (int32_t)(ENC_QUAD_MAX_COUNT))){
			enc->counterQuad = (enc->counterQuad - ENC_QUAD_MAX_COUNT );
			__HAL_TIM_SET_COUNTER(enc->configs.quadrature.timer, enc->counterQuad);
		}
		enc->lastCountQuad = enc->counterQuad;
//		DEBUG_SPRINT_NL("CuntA: %u, GetA: %u",enc->counterQuad , __HAL_TIM_GET_COUNTER(enc->configs.quadrature.timer));
	}else{
		enc->counterQuad = 0;
		enc->lastCountQuad = 0;
		enc->direction = 0;
	}
}
/*
 * 0 = no rotation
 * 1 = CW rotation
 * 2 = CCW rotation
 *
 */
void ENC_GetDirQuad(Encoder *enc){
	if(enc->enable > 0 && enc->configs.mode == ENC_MODE_QUAD){
		enc->direction = ((enc->configs.quadrature.timer->Instance->CR1 & TIM_CR1_DIR)>0);		// CW dir = 0, and CCW dir = 1
	}
}

/*
 * Calculates the pulse per revolution (PPR) if ch-z is used
 * */
 void ENC_CalcPPR(Encoder *enc){

	 /*
	  * 0x00FFFFFFU is the 24bits number.
	  * the difference is need to check, because of, if the counter overflow to the max size
	  * then the counter value will set to the '0', in this case the difference will be a
	  * very big & false number, thats why need to check the number
	  *
	  *
	  * */

	 uint32_t difference = (uint32_t)fabs((double)(enc->counterA - enc->lastCountA));

	 if(difference < (uint32_t)0x00FFFFFFU){
		 enc->ppr = difference;
	 }
}


/**
  * @brief calculates speed in a certain time interval. this function should execute by 1ms interval
  *
  */
void ENC_CalcSpeedQuad(Encoder *enc){
	if(enc->configs.mode == ENC_MODE_QUAD){
		uint32_t timeDiff = (uint32_t)fabs((double)(HAL_GetTick()-enc->lastTime));
		if( timeDiff >= (uint32_t)enc->interval){

			enc->count1 = __HAL_TIM_GET_COUNTER(enc->configs.quadrature.timer);

			if( enc->count1 != enc->count0 && timeDiff > 0U && enc->ppr > 0U){
				enc->speed = (float)(fabs((double)(enc->count1-enc->count0))*60.0f*1000.0f/(timeDiff*enc->ppr));
				enc->count0 = enc->count1;
				enc->lastTime = HAL_GetTick();
			}else if(timeDiff >= ENC_STEADY_TIMEOUT){
				enc->speed = 0;
			}

		}


	}
}



void ENC_SetTrigModeQuad(ENC_ConfigsQuad quadrature, ENC_TrigMode mode){
	quadrature.trigMode = mode;


}



/*Incremental encoder functions-----------*/
/**
  * @brief  configs external interrupt for incremental encoder
  * @param  encIcre: encoder config struct
  * @retval none
  */
void DI_ConfigInceExtI(ENC_ConfigIncre *encIcre){

	GPIO_InitTypeDef GPIO_InitStruct = {0};


	HAL_GPIO_DeInit(encIcre->port, encIcre->pin);
	switch (encIcre->trigMode) {
		case ENC_TRIG_MODE_RISING:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
			break;
		case ENC_TRIG_MODE_FALLING:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
			break;
		case ENC_TRIG_MODE_RISING_FALLING:
			GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
			break;
		default:				// for others

			break;
	}

	GPIO_InitStruct.Pin = encIcre->pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(encIcre->port, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(encIcre->intrpt, 0, ENC_INCRE_EXTI_PRIORITY);
	HAL_NVIC_EnableIRQ(encIcre->intrpt);



}



/**
  * @brief Read incremental encoder Channel A
  */
void ENC_ReadChA(Encoder *enc){
	if(enc->enable > 0){
		if(enc->configs.mode & ENC_MODE_INCRE_CHA){	enc->counterA ++;}
	}else{
		enc->counterA = 0;
	}
}



/**
  * @brief Read incremental encoder Channel B
  */
void ENC_ReadChB(Encoder *enc){
	if(enc->enable > 0){
	if(enc->configs.mode & ENC_MODE_INCRE_CHB){	enc->counterB ++;}
	}else{
		enc->counterB = 0;
	}
}

/**
  * @brief Read quadrature encoder Channel Z
  */
void ENC_ReadChZ(Encoder *enc){
	if(enc->enable > 0 && enc->configs.mode == ENC_MODE_QUAD){
		enc->counterZ ++;
		enc->countQuadAtRef = __HAL_TIM_GET_COUNTER(enc->configs.quadrature.timer);
	}else{
		enc->counterZ = 0;
		enc->countQuadAtRef = 0;
	}
}


/**
  * @brief  Set the trigger mode
  * @param  enc: pointer to encoder structure
  * @param  trigMod: the trigger mode, there are 3 trigger mode, Rising, falling, Rising-Falling edge
  * @retval none
  *
  */
void ENC_SetTrigModeIncre(ENC_ConfigIncre *increCh, ENC_TrigMode mode){
	increCh->trigMode = mode;
}

void ENC_SetCountModeIncre(ENC_ConfigIncre *increCh, ENC_CountMode countMod){

}
