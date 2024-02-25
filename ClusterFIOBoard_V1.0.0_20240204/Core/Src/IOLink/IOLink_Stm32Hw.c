/*
 * IOLink_Stm32Hw.c
 *
 *  Created on: Jan 29, 2024
 *      Author: wsrra
 */
#include "IOLink_Stm32Hw.h"
#include "main.h"
#include "GlobalVars.h"
#include "Debug.h"

#define IOL_LINKDMA(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = (__DMA_HANDLE__); \
                              (__DMA_HANDLE__)->Parent = (__HANDLE__);             \
                          } while(0)




/** @brief	UART init service
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
HAL_StatusTypeDef IOL_STM32_UartInit( L6360_IOLink_hw_t* l6360, uint8_t portNum, uint32_t baudrate ){
	HAL_StatusTypeDef status = HAL_OK;
	UART_HandleTypeDef* huart = l6360->uart.huart;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	huart->Init.BaudRate = baudrate;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	status = HAL_UART_Init(huart);
	if( status != HAL_OK){ return status; }
	status = HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8);
	if( status != HAL_OK){ return status; }
	status = HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8);
	if( status != HAL_OK){ return status; }
	status = HAL_UARTEx_DisableFifoMode(huart);
	if( status != HAL_OK){ return status; }



	if(huart->Instance == USART1){
	  /** Initializes the peripherals clock */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
		PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16910CLKSOURCE_D2PCLK2;
		status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
		if( status != HAL_OK){ return status; }

		/* Peripheral clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/**USART1 GPIO Configuration
		PB14 --> USART1_TX
		PB15 --> USART1_RX
		*/
		GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
	}else if(huart->Instance == UART4){
	  /** Initializes the peripherals clock*/
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
		PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
		status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
		if( status != HAL_OK){ return status; }

	    /* Peripheral clock enable */
	    __HAL_RCC_UART4_CLK_ENABLE();
	    __HAL_RCC_GPIOA_CLK_ENABLE();
	    __HAL_RCC_GPIOC_CLK_ENABLE();

	    /**UART4 GPIO Configuration
	    PA12 --> UART4_TX
	    PC11 --> UART4_RX
	    */
	    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	}else if(huart->Instance == UART5){
	  /** Initializes the peripherals clock */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
		PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
		status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
		if( status != HAL_OK){ return status; }

		/* Peripheral clock enable */
		__HAL_RCC_UART5_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();

		/**UART5 GPIO Configuration
		PB12 --> UART5_RX
		PB13 --> UART5_TX
		*/
		GPIO_InitStruct.Alternate = GPIO_AF14_UART5;
	}else if(huart->Instance == UART7){
	  /** Initializes the peripherals clock*/
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7;
		PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
		status = HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
		if( status != HAL_OK){ return status; }

		/* Peripheral clock enable */
		__HAL_RCC_UART7_CLK_ENABLE();
		__HAL_RCC_GPIOF_CLK_ENABLE();

		/**UART7 GPIO Configuration
		PF6 --> UART7_RX
		PF7 --> UART7_TX
		*/
		GPIO_InitStruct.Alternate = GPIO_AF7_UART7;
	}
	/*TX GPIO Configuration*/
    GPIO_InitStruct.Pin = l6360->uart.txPin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(l6360->uart.txPort, &GPIO_InitStruct);

    /*RX GPIO Configuration*/
    GPIO_InitStruct.Pin = l6360->uart.rxPin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(l6360->uart.rxPort, &GPIO_InitStruct);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(l6360->uart.irqType, l6360->uart.priority, l6360->uart.subPriority);
    HAL_NVIC_EnableIRQ(l6360->uart.irqType);


	return status;
}

/** @brief	UART init in DMA mode
  * @param  l6360: l6360 structure pointer
  * @param	baudrate: transition rate
  * @retval error status
  */
HAL_StatusTypeDef IOL_STM32_UartInitDMA( L6360_IOLink_hw_t* l6360){
	HAL_StatusTypeDef status = HAL_OK;
	UART_HandleTypeDef* huart = l6360->uart.huart;
	DMA_HandleTypeDef* hdmaTx = l6360->uart.dma.hdmaTx;
	DMA_HandleTypeDef* hdmaRx = l6360->uart.dma.hdmaRx;
	IRQn_Type irqTypDmaTx = 0;
	IRQn_Type irqTypDmaRx = 0;



	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();


	if(huart->Instance == USART1){
		hdmaTx->Instance = DMA1_Stream0;
		hdmaRx->Instance = DMA1_Stream1;
		irqTypDmaTx = DMA1_Stream0_IRQn;
		irqTypDmaRx = DMA1_Stream1_IRQn;
		hdmaRx->Init.Request = DMA_REQUEST_UART5_RX;
		hdmaTx->Init.Request = DMA_REQUEST_UART5_TX;
	}else if(huart->Instance == UART4){
		irqTypDmaTx = DMA1_Stream2_IRQn;
		irqTypDmaRx = DMA1_Stream3_IRQn;
		hdmaRx->Init.Request = DMA_REQUEST_UART7_RX;
		hdmaTx->Init.Request = DMA_REQUEST_UART7_TX;
	}else if(huart->Instance == UART5){
		irqTypDmaTx = DMA1_Stream4_IRQn;
		irqTypDmaRx = DMA1_Stream5_IRQn;
		hdmaRx->Init.Request = DMA_REQUEST_USART1_RX;
		hdmaTx->Init.Request = DMA_REQUEST_USART1_TX;
	}else if(huart->Instance == UART7){
		irqTypDmaTx = DMA1_Stream6_IRQn;
		irqTypDmaRx = DMA1_Stream7_IRQn;
		hdmaRx->Init.Request = DMA_REQUEST_UART4_RX;
		hdmaTx->Init.Request = DMA_REQUEST_UART4_TX;
	}


	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(irqTypDmaTx, l6360->uart.dma.priorityTx, l6360->uart.dma.subPriorityTx);
	HAL_NVIC_EnableIRQ(irqTypDmaTx);
	/* DMA1_Stream1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(irqTypDmaRx, l6360->uart.dma.priorityRx, l6360->uart.dma.subPriorityRx);
	HAL_NVIC_EnableIRQ(irqTypDmaRx);

//	    /* UART5 DMA Init */
	    /* UART5_TX Init */
//	    l6360->uart.dma.hdmaTx->Instance = DMA1_Stream0;
//	hdmaTx->Init.Request = DMA_REQUEST_UART5_TX;
	hdmaTx->Init.Direction = DMA_MEMORY_TO_PERIPH;
	hdmaTx->Init.PeriphInc = DMA_PINC_DISABLE;
	hdmaTx->Init.MemInc = DMA_MINC_ENABLE;
	hdmaTx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdmaTx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdmaTx->Init.Mode = DMA_NORMAL;
	hdmaTx->Init.Priority = DMA_PRIORITY_VERY_HIGH;
	hdmaTx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	status = HAL_DMA_Init(hdmaTx);
	if (status != HAL_OK){
	  return status;
	}
	IOL_LINKDMA(huart, hdmatx, hdmaTx);

	    /* UART5_RX Init */
//	    hdmaRx->Instance = DMA1_Stream1;
//	    hdmaRx->Init.Request = DMA_REQUEST_UART5_RX;
	hdmaRx->Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdmaRx->Init.PeriphInc = DMA_PINC_DISABLE;
	hdmaRx->Init.MemInc = DMA_MINC_ENABLE;
	hdmaRx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdmaRx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdmaRx->Init.Mode = DMA_NORMAL;
	hdmaRx->Init.Priority = DMA_PRIORITY_VERY_HIGH;
	hdmaRx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	status = HAL_DMA_Init(hdmaRx);
	if (status != HAL_OK){
	  return status;
	}
	IOL_LINKDMA(huart, hdmarx, hdmaRx);

	return status;
}


/**
  * @brief  configure IOL GPIO
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void  IOL_STM32_GpioInit(L6360_IOLink_hw_t* l6360){
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_ENABLE();
	  __HAL_RCC_GPIOF_CLK_ENABLE();
	  __HAL_RCC_GPIOD_CLK_ENABLE();
	  __HAL_RCC_GPIOG_CLK_ENABLE();
	  __HAL_RCC_GPIOA_CLK_ENABLE();

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin( l6360->ctrlPins.lpEnPort,  l6360->ctrlPins.lpEnPin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin( l6360->ctrlPins.cqEnPort,  l6360->ctrlPins.cqEnPin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin( l6360->ctrlPins.rstPort,  l6360->ctrlPins.rstPin, GPIO_PIN_SET);

	  /*Configure GPIO pins*/
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	  /*Init GPIO pins*/
	  GPIO_InitStruct.Pin =  l6360->ctrlPins.lpEnPin;
	  HAL_GPIO_Init(l6360->ctrlPins.lpEnPort, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin =  l6360->ctrlPins.cqEnPin;
	  HAL_GPIO_Init(l6360->ctrlPins.cqEnPort, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin =  l6360->ctrlPins.rstPin;
	  HAL_GPIO_Init(l6360->ctrlPins.rstPort, &GPIO_InitStruct);

}

