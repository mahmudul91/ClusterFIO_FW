/*
 * PWM.c
 *
 *  Created on: Dec 30, 2022
 *      Author: wsrra
 */


#include "PWM.h"
#include "math.h"

/**
  * @brief Timer Initialization for the PWM output. Use this function,
  * if timer is not configured using CudeMX code generator software.
  * otherwise, it is recommended to not use.
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_Init(PWM *pwm, TIM_TypeDef *instance){


	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};
	  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	  /* USER CODE BEGIN TIM1_Init 1 */

	  /* USER CODE END TIM1_Init 1 */
	  pwm->timer->Instance = TIM1;
	  pwm->timer->Init.Prescaler = 0;
	  pwm->timer->Init.CounterMode = TIM_COUNTERMODE_UP;
	  pwm->timer->Init.Period = 0;
	  pwm->timer->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  pwm->timer->Init.RepetitionCounter = 0;
	  pwm->timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(pwm->timer) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(pwm->timer, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_PWM_Init(pwm->timer) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(pwm->timer, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	  sBreakDeadTimeConfig.DeadTime = 0;
	  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	  if (HAL_TIMEx_ConfigBreakDeadTime(pwm->timer, &sBreakDeadTimeConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  /* USER CODE BEGIN TIM1_Init 2 */

	  /* USER CODE END TIM1_Init 2 */
//	  HAL_TIM_MspPostInit(&pwm->timer);
}


/**
  * @brief PWM output channel configuration function
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_ConfigTimer(PWM *pwm){
	 if (HAL_TIM_Base_Init(pwm->timer) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void PWM_InitChannel(PWM *pwm){
	  pwm->chConfigOC.OCMode = TIM_OCMODE_PWM1;
	  pwm->chConfigOC.Pulse = 0;
#ifndef PWM_POLARITY_INV
	  pwm->chConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
#else
	  pwm->chConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
#endif
	  pwm->chConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	  PWM_ConfigChannel(pwm);


}

/**
  * @brief PWM output channel configuration function
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_ConfigChannel(PWM *pwm){
//	  TIM_OC_InitTypeDef sConfigOC = {0};
//	if(pwm->timer != NULL){
	  if (HAL_TIM_PWM_ConfigChannel(pwm->timer, &pwm->chConfigOC, pwm->channel) != HAL_OK)
	  {
	    Error_Handler();
	  }
//	}
}

/**
  * @brief start the PWm
  * @param PWM: pointer to the PWM struct
  * @retval None
  */

void PWM_Start(PWM *pwm){
	HAL_TIM_PWM_Start(pwm->timer, pwm->channel);
}


/**
  * @brief stop the PWm for the selected channel
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_Stop(PWM *pwm){
	HAL_TIM_PWM_Stop(pwm->timer, pwm->channel);
}



/**
  * @brief Calculate pwm parameters
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_CalcWithConstFreq(PWM *pwm){
	// value of the duty & freq are changeable by the user

}

/**
  * @brief Calculate pwm parameters
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_CalcWithNonConstFreq(PWM *pwm){
	// value of the duty & freq are changeable by the user

}

/**
  * @brief Updates PWM duty cycle function
  * @param PWM: pointer to the PWM struct
  * @retval None
  */
void PWM_Update(PWM *pwm){
	  pwm->chConfigOC.Pulse = pwm->onDuration;
	  if (HAL_TIM_PWM_ConfigChannel(pwm->timer, &pwm->chConfigOC, pwm->channel) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


/**
  * @brief Calculates prescaler for the PWM. after calculating must subtract by 1 before configure.
  *  ex. if calculated prescaler is 10, so 10-1 = 9 should have to use. otherwise frequency will not be exact
  * @param sysfreq: system clock frequency
  * @param pwmFreq: pwm frequency
  * @param pwmPeriod: pwm period (not in time, its counter value)
  * @retval the calculated prescaler value
  */
double PWM_CalcPrescaler(double sysfreq, double pwmFreq, double pwmPeriod){
	if(pwmFreq<=0.0f || pwmPeriod <= 0.0f) return 0;
	else return ceil(sysfreq/(pwmFreq*pwmPeriod));
}

/**
  * @brief Calculates period for the PWM
  * @param sysfreq: system clock frequency
  * @param pwmFreq: pwm frequency
  * @param pwmPeriod: pwm period (not in time, its counter value)
  * @retval None
  */
double PWM_CalcPeriod(double sysfreq, double pwmFreq, double pwmPrescaler){
	if(pwmFreq<=0.0f || pwmPrescaler <= 0.0f) return 0;
	else return ceil(sysfreq/(pwmFreq*pwmPrescaler));
}
