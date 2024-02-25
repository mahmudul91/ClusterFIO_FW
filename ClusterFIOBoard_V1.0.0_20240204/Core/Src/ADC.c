/*
 * ADC.c
 *
 *  Created on: Dec 27, 2022
 *      Author: wsrra
 */
#include "ADC.h"
#include "main.h"
#include "Debug.h"
#include "DigitalIn.h"
#include "Debug/Debug.h"


//ADC channel configuration parameter
ADC_ChannelConfTypeDef sConfig = {0};

extern DMA_HandleTypeDef hdma_adc1;



#define COUNT_MAX 	16			// adc stability delay
__IO uint32_t counter = 0U;
ADC_Common_TypeDef *tmpADC_Common;

/*ADC Poll------------------------------------------*/
/**
  * @brief ADC Initialization Function
  * @param hadc: pointer to the adc handler
  * @param SamplingTime: sample time in cycle
  * @retval None
  */
void ADC_Init(ADC_HandleTypeDef *hadc){
	  ADC_MultiModeTypeDef multimode = {0};
//	hadc->Instance = ADC1;
//	hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10;
//	hadc->Init.Resolution = ADC_RESOLUTION_12B;
//	hadc->Init.ScanConvMode = ENABLE;
//	hadc->Init.ContinuousConvMode = DISABLE;
//	hadc->Init.DiscontinuousConvMode = DISABLE;
//	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
//	hadc->Init.DataAlign = ADC3_DATAALIGN_RIGHT;	// todo: need to uncomment
//	hadc->Init.NbrOfConversion = 1;
//	hadc->Init.DMAContinuousRequests = DISABLE;
//	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  hadc->Instance = ADC1;
	  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV10;
	  hadc->Init.Resolution = ADC_RESOLUTION_12B;
	  hadc->Init.ScanConvMode = ADC_SCAN_ENABLE;
	  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  hadc->Init.LowPowerAutoWait = DISABLE;
	  hadc->Init.ContinuousConvMode = DISABLE;
	  hadc->Init.NbrOfConversion = 1;
	  hadc->Init.DiscontinuousConvMode = DISABLE;
	  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	  hadc->Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
	  hadc->Init.Overrun = ADC_OVR_DATA_PRESERVED;
	  hadc->Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	  hadc->Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(hadc) != HAL_OK){
		Error_Handler();
	}
	  /** Configure the ADC multi-mode
	  */

	  multimode.Mode = ADC_MODE_INDEPENDENT;
	  if (HAL_ADCEx_MultiModeConfigChannel(hadc, &multimode) != HAL_OK)
	  {
	    Error_Handler();
	  }
////	COUNT_MAX = 16;//(HAL_RCC_GetHCLKFreq()/1000000UL);
//	DEBUG_SPRINT_NL("%u",COUNT_MAX, SystemCoreClock);
}


/**
  * @brief Select ADC channel
  * @param hadc: pointer to the adc handler
  * @param Channel: adc channel
  * @retval None
  */
void ADC_SelectChannel(ADC_HandleTypeDef *hadc, uint32_t Channel){
	  sConfig.Channel = Channel;
//	  sConfig.Rank = 1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;// todo: need to uncomment
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLETIME_64CYCLES_5;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


/**
  * @brief  read the adc value of a selected channel for ADC
  * @param hadc: pointer to the adc handler
  * @param Channel: adc channel
  * @retval none
  */

uint32_t ADC_ReadValue(ADC_HandleTypeDef *hadc, uint32_t channel){
	uint32_t adc_value = 0u;
	ADC_SelectChannel(hadc, channel);
	HAL_ADC_Start(hadc);									// start adc to read the value
	if(HAL_ADC_PollForConversion(hadc,  1u) == HAL_OK){		// start conversion
		adc_value = HAL_ADC_GetValue(hadc);					// get the adc value after completing the conversion
	}
	HAL_ADC_Stop(hadc);										// stop the adc
	return adc_value;

}

//#define  COUNT_MAX  (((const unsigned long)(sysClock)) / 1000000U)


/*
 * Bare metal
 * */
uint32_t ADC_ReadValue_BM(ADC_HandleTypeDef *hadc, uint32_t channel){
	uint32_t adc_value = 0u;
////	ADC_SelectChannel(hadc, channel);
//	  sConfig.Channel = channel;
//	  sConfig.Rank = 1;
//	  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;// todo: need to uncomment
//
//	  /* if ADC_Channel_10 ... ADC_Channel_18 is selected */
//	  if (sConfig.Channel >= ADC_CHANNEL_10)
//	  {
//	    hadc->Instance->SMPR1 &= ~ADC_SMPR1(ADC_SMPR1_SMP10, sConfig.Channel);	/* Clear the old sample time */
//	    hadc->Instance->SMPR1 |= ADC_SMPR1(sConfig.SamplingTime, sConfig.Channel);	/* Set the new sample time */
//	  }
//
//	  /* For Rank 1 to 6 */
//	  if (sConfig.Rank < 7U)
//	  {
//	    hadc->Instance->SQR3 &= ~ADC_SQR3_RK(ADC_SQR3_SQ1, sConfig.Rank);/* Clear the old SQx bits for the selected rank */
//	    hadc->Instance->SQR3 |= ADC_SQR3_RK(sConfig.Channel, sConfig.Rank);/* Set the SQx bits for the selected rank */
//	  }
//
////	HAL_ADC_Start(hadc);
//
//
//
//	  /* Enable the ADC peripheral */
//	  /* Check if ADC peripheral is disabled in order to enable it and wait during
//	  Tstab time the ADC's stabilization */
//	  if((hadc->Instance->CR2 & ADC_CR2_ADON) != ADC_CR2_ADON)
//	  {
//	    /* Enable the Peripheral */
//	    __HAL_ADC_ENABLE(hadc);
//
//	    /* Delay for ADC stabilization time */
//	    /* Compute number of CPU cycles to wait for */
//	    counter = COUNT_MAX; //(1 * (SystemCoreClock / 1000000U));//ADC_STAB_DELAY_US
//
//	    while(counter != 0U)
//	    {
//	      counter--;
//	    }
//
//	  }
//
//	  /* Start conversion if ADC is effectively enabled */
//	  if(HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_ADON))
//	  {
//	    /* Set ADC state                                                          */
//	    /* - Clear state bitfield related to regular group conversion results     */
//	    /* - Set state bitfield related to regular group operation                */
//	    ADC_STATE_CLR_SET(hadc->State,
//	                      HAL_ADC_STATE_READY | HAL_ADC_STATE_REG_EOC | HAL_ADC_STATE_REG_OVR,
//	                      HAL_ADC_STATE_REG_BUSY);
//
//
//	    ADC_CLEAR_ERRORCODE(hadc);	      /* Reset ADC all error code fields */
//
//	    /* Pointer to the common control register to which is belonging hadc    */
//	    /* (Depending on STM32F4 product, there may be up to 3 ADCs and 1 common */
//	    /* control register)                                                    */
//	    tmpADC_Common = ADC_COMMON_REGISTER(hadc);
//
//	    /* Clear regular group conversion flag and overrun flag */
//	    /* (To ensure of no unknown state from potential previous ADC operations) */
//	    __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_EOC | ADC_FLAG_OVR);
//
//	    /* Check if Multimode enabled */
//	    if(HAL_IS_BIT_CLR(tmpADC_Common->CCR, ADC_CCR_MULTI))
//	    {
//	        /* if no external trigger present enable software conversion of regular channels */
//	        if((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET)
//	        {
//	          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;/* Enable the selected ADC software conversion for regular group */
//	        }
//	    }
//	    else
//	    {
//	      /* if instance of handle correspond to ADC1 and  no external trigger present enable software conversion of regular channels */
//	      if((hadc->Instance == ADC1) && ((hadc->Instance->CR2 & ADC_CR2_EXTEN) == RESET))
//	      {
//	          hadc->Instance->CR2 |= (uint32_t)ADC_CR2_SWSTART;     /* Enable the selected ADC software conversion for regular group */
//	      }
//	    }
//	  }
//	  else
//	  {
//	    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_INTERNAL);/* Update ADC state machine to error */
//	    SET_BIT(hadc->ErrorCode, HAL_ADC_ERROR_INTERNAL);/* Set ADC error code to ADC IP internal error */
//	  }
//
//
//
//
//
//
//
//
//
//
////
////	// start adc to read the value
////	if(HAL_ADC_PollForConversion(hadc,  1u) == HAL_OK){		// start conversion
////		adc_value = hadc->Instance->DR;//HAL_ADC_GetValue(hadc);					// get the adc value after completing the conversion
////	}
////
////
//
//	  /* Verification that ADC configuration is compliant with polling for      */
//	  /* each conversion:                                                       */
//	  /* Particular case is ADC configured in DMA mode and ADC sequencer with   */
//	  /* several ranks and polling for end of each conversion.                  */
//	  /* For code simplicity sake, this particular case is generalized to       */
//	  /* ADC configured in DMA mode and polling for end of each conversion.     */
////	  if (HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_EOCS) &&
////	      HAL_IS_BIT_SET(hadc->Instance->CR2, ADC_CR2_DMA)    )
////	  {
////		  DEBUG_SPRINT_NL("ADC_1");
////	    /* Update ADC state machine to error */
////	    SET_BIT(hadc->State, HAL_ADC_STATE_ERROR_CONFIG);
////	    goto END_LABEL; // Label
////	  }
//
//	  /* Get tick */
//	  uint32_t Timeout = 1;
//	  uint32_t tickstart = 0U;
//	  tickstart = HAL_GetTick();
//
//	  /* Check End of conversion flag */
//	  while(!(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)))
//	  {
//
//	    /* Check if timeout is disabled (set to infinite wait) */
//	    if(Timeout != HAL_MAX_DELAY)
//	    {
//	      if((Timeout == 0U) || ((HAL_GetTick() - tickstart ) > Timeout))
//	      {
//	        /* New check to avoid false timeout detection in case of preemption */
//	        if(!(__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOC)))
//	        {
//	          /* Update ADC state machine to timeout */
//	          SET_BIT(hadc->State, HAL_ADC_STATE_TIMEOUT);
//
//	          goto END_LABEL; // Label
//	        }
//	      }
//	    }
//	  }
//
//	  /* Clear regular group conversion flag */
//	  __HAL_ADC_CLEAR_FLAG(hadc, ADC_FLAG_STRT | ADC_FLAG_EOC);
//
//	  /* Update ADC state machine */
//	  SET_BIT(hadc->State, HAL_ADC_STATE_REG_EOC);
//
//	  /* Determine whether any further conversion upcoming on group regular       */
//	  /* by external trigger, continuous mode or scan sequence on going.          */
//	  /* Note: On STM32F4, there is no independent flag of end of sequence.       */
//	  /*       The test of scan sequence on going is done either with scan        */
//	  /*       sequence disabled or with end of conversion flag set to            */
//	  /*       of end of sequence.                                                */
//	  if(ADC_IS_SOFTWARE_START_REGULAR(hadc)                   &&
//	     (hadc->Init.ContinuousConvMode == DISABLE)            &&
//	     (HAL_IS_BIT_CLR(hadc->Instance->SQR1, ADC_SQR1_L) ||
//	      HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_EOCS)  )   )
//	  {
//
//	    /* Set ADC state */
//	    CLEAR_BIT(hadc->State, HAL_ADC_STATE_REG_BUSY);
//
//	    if (HAL_IS_BIT_CLR(hadc->State, HAL_ADC_STATE_INJ_BUSY))
//	    {
////	    	DEBUG_SPRINT_NL("ADC_4");
//	      SET_BIT(hadc->State, HAL_ADC_STATE_READY);
//	    }
//	  }
//
//
//	  adc_value = hadc->Instance->DR;//HAL_ADC_GetValue(hadc);
//
//	END_LABEL:
//
//
//
////	HAL_ADC_Stop(hadc);										// stop the adc
//
//
//	  /* Stop potential conversion on going, on regular and injected groups */
//	  /* Disable ADC peripheral */
//	  __HAL_ADC_DISABLE(hadc);
//
//	  /* Check if ADC is effectively disabled */
//	  if(HAL_IS_BIT_CLR(hadc->Instance->CR2, ADC_CR2_ADON))
//	  {
//	    /* Set ADC state */
//	    ADC_STATE_CLR_SET(hadc->State,
//	                      HAL_ADC_STATE_REG_BUSY | HAL_ADC_STATE_INJ_BUSY,
//	                      HAL_ADC_STATE_READY);
//	  }
//
//
//
//


	return adc_value;

}


/*ADC DMA------------------------------------------*/
/**
  * @brief ADC Initialization Function
  * @param hadc: pointer to the adc handler
  * @param SamplingTime: sample time in cycle
  * @retval None
  */
void ADC_InitDMA(ADC_HandleTypeDef *hadc ){
	  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	  */
	hadc->Instance = ADC1;
	hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc->Init.Resolution = ADC_RESOLUTION_12B;
	hadc->Init.ScanConvMode = ENABLE;
	hadc->Init.ContinuousConvMode = ENABLE;
	hadc->Init.DiscontinuousConvMode = DISABLE;
	hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
//	hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;// todo: need to uncomment
	hadc->Init.NbrOfConversion = 4;
	hadc->Init.DMAContinuousRequests = ENABLE;
	hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(hadc) != HAL_OK)
	{
		Error_Handler();
	}

	/* ADC1 DMA Init */
	/* ADC1 Init */
	hdma_adc1.Instance = DMA2_Stream4;
//	hdma_adc1.Init.Channel = DMA_CHANNEL_0;// todo: need to uncomment
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;
	hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
	hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
	{
	  Error_Handler();
	}

	__HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);

	/* ADC1 interrupt Init */
	HAL_NVIC_SetPriority(ADC_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(ADC_IRQn);
}

/**
  * @brief config ADC channel
  * @param hadc: pointer to the adc handler
  * @param Channel: adc channel
  * @retval None
  */
void ADC_ConfigChannel(ADC_HandleTypeDef *hadc, uint32_t Channel, uint32_t rank,  uint32_t SamplingTime){
	  sConfig.Channel = Channel;
	  sConfig.Rank = rank;
	  sConfig.SamplingTime = SamplingTime;
	  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


/**
  * @brief  Start reading the adc value of all the channel for ADC
  * @param hadc: pointer to the adc handler
  * @param pData: pointer to the buffer
  * @param Length: length of the buffer
  * @retval none
  */


uint32_t ADC_StartReadingDMA(ADC_HandleTypeDef *hadc, uint32_t* pData, uint32_t Length){
	return HAL_ADC_Start_DMA(hadc, pData, Length);

}

/**
  * @brief  stop reading of all the channel for ADC
  * @param hadc: pointer to the adc handler
  * @retval none
  */
uint32_t ADC_StopReadingDMA(ADC_HandleTypeDef *hadc){
	return HAL_ADC_Stop_DMA(hadc);
}

/**
  * @brief  calculate voltage from adc value
  * @param adcValue: adc raw value
  * @retval none
  */
float ADC_CalcVolt(uint16_t adcValue){
	return (float)((adcValue*ADC_REF_VOLT)/(ADC_RESOLUTION-1U));
}
