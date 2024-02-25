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
#define DO_5_Pin GPIO_PIN_14
#define DO_5_GPIO_Port GPIOC
#define DO_6_Pin GPIO_PIN_15
#define DO_6_GPIO_Port GPIOC
#define IOL2_RX_U7_Pin GPIO_PIN_6
#define IOL2_RX_U7_GPIO_Port GPIOF
#define IOL2_TX_U7_Pin GPIO_PIN_7
#define IOL2_TX_U7_GPIO_Port GPIOF
#define IOL2_RST_Pin GPIO_PIN_8
#define IOL2_RST_GPIO_Port GPIOF
#define IOL4_ENL_Pin GPIO_PIN_9
#define IOL4_ENL_GPIO_Port GPIOF
#define BUTTON_Pin GPIO_PIN_10
#define BUTTON_GPIO_Port GPIOF
#define AO_4_PWM_Pin GPIO_PIN_3
#define AO_4_PWM_GPIO_Port GPIOA
#define AI_1_Pin GPIO_PIN_6
#define AI_1_GPIO_Port GPIOA
#define W25QXX_SPI1_CS_Pin GPIO_PIN_0
#define W25QXX_SPI1_CS_GPIO_Port GPIOB
#define AI_2_Pin GPIO_PIN_1
#define AI_2_GPIO_Port GPIOB
#define DI_3_ENC_Z_Pin GPIO_PIN_2
#define DI_3_ENC_Z_GPIO_Port GPIOB
#define DO_1_PWM_Pin GPIO_PIN_9
#define DO_1_PWM_GPIO_Port GPIOE
#define DO_2_PWM_Pin GPIO_PIN_11
#define DO_2_PWM_GPIO_Port GPIOE
#define AO_3_PWM_Pin GPIO_PIN_10
#define AO_3_PWM_GPIO_Port GPIOB
#define DBUG_RX_U3_Pin GPIO_PIN_11
#define DBUG_RX_U3_GPIO_Port GPIOB
#define IOL1_RX_U5_Pin GPIO_PIN_12
#define IOL1_RX_U5_GPIO_Port GPIOB
#define IOL1_TX_U5_Pin GPIO_PIN_13
#define IOL1_TX_U5_GPIO_Port GPIOB
#define IOL3_TX_U1_Pin GPIO_PIN_14
#define IOL3_TX_U1_GPIO_Port GPIOB
#define IOL3_RX_U1_Pin GPIO_PIN_15
#define IOL3_RX_U1_GPIO_Port GPIOB
#define IOL1_RST_Pin GPIO_PIN_11
#define IOL1_RST_GPIO_Port GPIOD
#define IOL_I2C4_SCL_Pin GPIO_PIN_12
#define IOL_I2C4_SCL_GPIO_Port GPIOD
#define IOL_I2C4_SDA_Pin GPIO_PIN_13
#define IOL_I2C4_SDA_GPIO_Port GPIOD
#define IOL1_ENCQ_Pin GPIO_PIN_2
#define IOL1_ENCQ_GPIO_Port GPIOG
#define IOL1_ENL_Pin GPIO_PIN_3
#define IOL1_ENL_GPIO_Port GPIOG
#define IOL1_IRQ_Pin GPIO_PIN_6
#define IOL1_IRQ_GPIO_Port GPIOG
#define IOL3_RST_Pin GPIO_PIN_7
#define IOL3_RST_GPIO_Port GPIOG
#define DI_1_ENC_A_Pin GPIO_PIN_6
#define DI_1_ENC_A_GPIO_Port GPIOC
#define DI_2_ENC_B_Pin GPIO_PIN_7
#define DI_2_ENC_B_GPIO_Port GPIOC
#define IOL3_ENCQ_Pin GPIO_PIN_8
#define IOL3_ENCQ_GPIO_Port GPIOC
#define IOL3_ENL_Pin GPIO_PIN_9
#define IOL3_ENL_GPIO_Port GPIOC
#define IOL3_IRQ_Pin GPIO_PIN_9
#define IOL3_IRQ_GPIO_Port GPIOA
#define DO_3_PWM_Pin GPIO_PIN_10
#define DO_3_PWM_GPIO_Port GPIOA
#define DO_4_PWM_Pin GPIO_PIN_11
#define DO_4_PWM_GPIO_Port GPIOA
#define IOL4_TX_U4_Pin GPIO_PIN_12
#define IOL4_TX_U4_GPIO_Port GPIOA
#define AO_1_PWM_Pin GPIO_PIN_15
#define AO_1_PWM_GPIO_Port GPIOA
#define DBUG_TX_U3_Pin GPIO_PIN_10
#define DBUG_TX_U3_GPIO_Port GPIOC
#define IOL4_RX_U4_Pin GPIO_PIN_11
#define IOL4_RX_U4_GPIO_Port GPIOC
#define IOL4_IRQ_Pin GPIO_PIN_2
#define IOL4_IRQ_GPIO_Port GPIOD
#define IOL2_ENL_Pin GPIO_PIN_3
#define IOL2_ENL_GPIO_Port GPIOD
#define IOL2_ENCQ_Pin GPIO_PIN_4
#define IOL2_ENCQ_GPIO_Port GPIOD
#define RS485_TX_U2_Pin GPIO_PIN_5
#define RS485_TX_U2_GPIO_Port GPIOD
#define RS485_RX_U2_Pin GPIO_PIN_6
#define RS485_RX_U2_GPIO_Port GPIOD
#define IOL2_IRQ_Pin GPIO_PIN_10
#define IOL2_IRQ_GPIO_Port GPIOG
#define AO_2_PWM_Pin GPIO_PIN_3
#define AO_2_PWM_GPIO_Port GPIOB
#define DI_4_Pin GPIO_PIN_4
#define DI_4_GPIO_Port GPIOB
#define DI_5_Pin GPIO_PIN_5
#define DI_5_GPIO_Port GPIOB
#define MAC_IOX_I2C_SCL_Pin GPIO_PIN_6
#define MAC_IOX_I2C_SCL_GPIO_Port GPIOB
#define MAC_IOX_I2C1_SDA_Pin GPIO_PIN_7
#define MAC_IOX_I2C1_SDA_GPIO_Port GPIOB
#define DI_6_Pin GPIO_PIN_8
#define DI_6_GPIO_Port GPIOB
#define DI_7_Pin GPIO_PIN_9
#define DI_7_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
//#define IOL4_ENCQ_GPIO_Port GPIOA
//#define IOL4_ENCQ_Pin GPIO_PIN_14
//#define IOL4_RST_GPIO_Port GPIOA
//#define IOL4_RST_Pin GPIO_PIN_13
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
