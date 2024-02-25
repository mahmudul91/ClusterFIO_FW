/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define RS485_DIR_Pin GPIO_PIN_3
#define RS485_DIR_GPIO_Port GPIOE
#define DI_8_Pin GPIO_PIN_13
#define DI_8_GPIO_Port GPIOC
#define DI_8_EXTI_IRQn EXTI15_10_IRQn
#define DO_5_Pin GPIO_PIN_14
#define DO_5_GPIO_Port GPIOC
#define DO_6_Pin GPIO_PIN_15
#define DO_6_GPIO_Port GPIOC
#define DI_3_Pin GPIO_PIN_2
#define DI_3_GPIO_Port GPIOB
#define DI_3_EXTI_IRQn EXTI2_IRQn
#define DO_1_Pin GPIO_PIN_9
#define DO_1_GPIO_Port GPIOE
#define DO_2_Pin GPIO_PIN_11
#define DO_2_GPIO_Port GPIOE
#define DI_1_Pin GPIO_PIN_6
#define DI_1_GPIO_Port GPIOC
#define DI_1_EXTI_IRQn EXTI9_5_IRQn
#define DI_2_Pin GPIO_PIN_7
#define DI_2_GPIO_Port GPIOC
#define DI_2_EXTI_IRQn EXTI9_5_IRQn
#define DO_3_Pin GPIO_PIN_10
#define DO_3_GPIO_Port GPIOA
#define DO_4_Pin GPIO_PIN_11
#define DO_4_GPIO_Port GPIOA
#define DI_4_Pin GPIO_PIN_4
#define DI_4_GPIO_Port GPIOB
#define DI_4_EXTI_IRQn EXTI4_IRQn
#define DI_5_Pin GPIO_PIN_5
#define DI_5_GPIO_Port GPIOB
#define DI_5_EXTI_IRQn EXTI9_5_IRQn
#define DI_6_Pin GPIO_PIN_8
#define DI_6_GPIO_Port GPIOB
#define DI_6_EXTI_IRQn EXTI9_5_IRQn
#define DI_7_Pin GPIO_PIN_9
#define DI_7_GPIO_Port GPIOB
#define DI_7_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
