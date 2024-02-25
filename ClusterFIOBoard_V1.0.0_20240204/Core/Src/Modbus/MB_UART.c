/*
 * MB_UART.c
 *
 *  Created on: Jul 7, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */
#include "MB_UART.h"
#include "stm32h7xx_hal_def.h"
#include "Debug.h"

//to link between UART & DMA
#define MB_LINKDMATOUART(__HANDLE__, __PPP_DMA_FIELD__, __DMA_HANDLE__)               \
                        do{                                                      \
                              (__HANDLE__)->__PPP_DMA_FIELD__ = (__DMA_HANDLE__); \
                              (__DMA_HANDLE__)->Parent = (__HANDLE__);             \
                          } while(0U)


/* Private types -------------------------------------------------------------*/
typedef struct DMA_BM_Base_Registers
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_BM_Base_Registers;



UART_HandleTypeDef *uart;
DMA_HandleTypeDef *dma_uart_rx;
DMA_HandleTypeDef *dma_uart_tx;

HAL_StatusTypeDef MB_BM_UartSendDMA(uint8_t *buffer, uint16_t size);

static void MB_UART_EndTxTransfer(UART_HandleTypeDef *huart);
static void MB_UART_EndRxTransfer(UART_HandleTypeDef *huart);

static void MB_UART_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void MB_UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void MB_UART_DMAError(DMA_HandleTypeDef *hdma);

/**
  * @brief Initialize uart for modbus serial line in interrupt mode
  * @param none
  * @retval 0 = error, 1 = no error
  */
uint8_t MB_UartInit(UART_HandleTypeDef *_uart, USART_TypeDef *Instance, uint32_t baudRate){
	uart = _uart;
	_uart->Instance = Instance;
	_uart->Init.BaudRate = baudRate;
	_uart->Init.WordLength = UART_WORDLENGTH_9B;
	_uart->Init.StopBits = UART_STOPBITS_1;
	_uart->Init.Parity = UART_PARITY_EVEN;
	_uart->Init.Mode = UART_MODE_TX_RX;
	_uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	_uart->Init.OverSampling = UART_OVERSAMPLING_8;
	_uart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	_uart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
	_uart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(_uart) != HAL_OK) {
		return 0;
	}
	if (HAL_UARTEx_SetTxFifoThreshold(_uart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK){
		 return 0;
	}
	if (HAL_UARTEx_SetRxFifoThreshold(_uart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK){
		 return 0;
	}
	if (HAL_UARTEx_DisableFifoMode(_uart) != HAL_OK){
		 return 0;
	}

	return 1;
}


/**
  * @brief Initialize uart for modbus serial line in DMA mode
  * @param none
  * @retval 0 = error, 1 = no error
  */

uint8_t MB_UartInitDMA( DMA_HandleTypeDef *_dma_uart_rx, DMA_HandleTypeDef *_dma_uart_tx){

//	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
//	GPIO_InitTypeDef GPIO_InitStruct = {0};

	  __HAL_RCC_DMA1_CLK_ENABLE();
	dma_uart_rx = _dma_uart_rx;
	dma_uart_tx = _dma_uart_tx;
//
//	/** Initializes the peripherals clock
//	*/
//	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
//	PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
//	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK){
//		 return 0;
//	}
//
//	 /* Peripheral clock enable */
//    __HAL_RCC_USART2_CLK_ENABLE();
//    __HAL_RCC_GPIOD_CLK_ENABLE();
//
////    /**USART2 GPIO Configuration
////    PD5     ------> USART2_TX
////    PD6     ------> USART2_RX
////    */
////    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
////    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
////    GPIO_InitStruct.Pull = GPIO_NOPULL;
////    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
////    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
////    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//
//
//    /* USART2 DMA Init */
//    /* USART2_RX Init */
//    dma_uart_rx->Instance = DMA1_Stream0;
//    dma_uart_rx->Init.Request = DMA_REQUEST_USART2_RX;
//    dma_uart_rx->Init.Direction = DMA_PERIPH_TO_MEMORY;
//    dma_uart_rx->Init.PeriphInc = DMA_PINC_DISABLE;
//    dma_uart_rx->Init.MemInc = DMA_MINC_ENABLE;
//    dma_uart_rx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//    dma_uart_rx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//    dma_uart_rx->Init.Mode = DMA_NORMAL;
//    dma_uart_rx->Init.Priority = DMA_PRIORITY_VERY_HIGH;
//    dma_uart_rx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//	if (HAL_DMA_Init(dma_uart_rx) != HAL_OK){
//		return 0;
//	}
//	MB_LINKDMATOUART(uart,hdmarx,dma_uart_rx);
//
//
//    /* USART2_TX Init */
//	dma_uart_tx->Instance = DMA1_Stream1;
//	dma_uart_tx->Init.Request = DMA_REQUEST_USART2_TX;
//	dma_uart_tx->Init.Direction = DMA_MEMORY_TO_PERIPH;
//	dma_uart_tx->Init.PeriphInc = DMA_PINC_DISABLE;
//	dma_uart_tx->Init.MemInc = DMA_MINC_ENABLE;
//	dma_uart_tx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
//	dma_uart_tx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
//	dma_uart_tx->Init.Mode = DMA_NORMAL;
//	dma_uart_tx->Init.Priority = DMA_PRIORITY_VERY_HIGH;
//	dma_uart_tx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//	if (HAL_DMA_Init(dma_uart_tx) != HAL_OK){
//		return 0;
//	}
//	MB_LINKDMATOUART(uart,hdmatx,dma_uart_tx);
//
//
//    /* USART6 interrupt Init */
//    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(USART2_IRQn);
//    /* DMA interrupt init */
//    /* DMA1_Stream0_IRQn interrupt configuration */
//    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
//    /* DMA1_Stream1_IRQn interrupt configuration */
//    HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	return 1;
}



HAL_StatusTypeDef MB_UartSendDMA(uint8_t *buffer, uint16_t size){
	return HAL_UART_Transmit_DMA(uart, buffer, size);
//	return MB_BM_UartSendDMA(buffer, size);
}



/*
 * MB = Modbus
 * BM = Bare Metal
 * */
HAL_StatusTypeDef MB_BM_UartSendDMA(uint8_t *buffer, uint16_t size){
	 uint32_t *tmp;
	  HAL_StatusTypeDef status = HAL_OK;
	/* Check that a Tx process is not already ongoing */
	  if (uart->gState == HAL_UART_STATE_READY)
	  {
	    if ((buffer == NULL) || (size == 0U))
	    {
	      return HAL_ERROR;
	    }

	    /* Process Locked */
	    __HAL_LOCK(uart);

	    uart->pTxBuffPtr = buffer;
	    uart->TxXferSize = size;
	    uart->TxXferCount = size;

	    uart->ErrorCode = HAL_UART_ERROR_NONE;
	    uart->gState = HAL_UART_STATE_BUSY_TX;

	    /* Set the UART DMA transfer complete callback */
	    uart->hdmatx->XferCpltCallback = MB_UART_DMATransmitCplt;
	    uart->TxXferCount = 0x00U;

	    /* Disable the DMA transfer for transmit request by setting the DMAT bit
	       in the UART CR3 register */
	    ATOMIC_CLEAR_BIT(uart->Instance->CR3, USART_CR3_DMAT);

	    /* Enable the UART Transmit Complete Interrupt */
	    ATOMIC_SET_BIT(uart->Instance->CR1, USART_CR1_TCIE);

	    /* Set the UART DMA Half transfer complete callback */
	    uart->hdmatx->XferHalfCpltCallback = MB_UART_DMATxHalfCplt;


	    /* Set the DMA error callback */
	    uart->hdmatx->XferErrorCallback = MB_UART_DMAError;

	    /* Set the DMA abort callback */
	    uart->hdmatx->XferAbortCallback = NULL;

	    /* Enable the UART transmit DMA stream */
	    tmp = (uint32_t *)&buffer;
	    HAL_DMA_Start_IT(uart->hdmatx, *(uint32_t *)tmp, (uint32_t)&uart->Instance->RDR, size);

	    /* calculate DMA base and stream number */
	    DMA_BM_Base_Registers *regs = (DMA_BM_Base_Registers *)uart->hdmatx->StreamBaseAddress;

	    /* Check the parameters */
	    assert_param(IS_DMA_BUFFER_SIZE(size));

	    /* Process locked */
//	    __HAL_LOCK(hdma);

	    if(HAL_DMA_STATE_READY == uart->hdmatx->State)
	    {
	      /* Change DMA peripheral state */
	    	uart->hdmatx->State = HAL_DMA_STATE_BUSY;

	      /* Initialize the error code */
	    	uart->hdmatx->ErrorCode = HAL_DMA_ERROR_NONE;

	      /* Configure the source, destination address and the data length */
//	      DMA_SetConfig(uart->hdmatx, SrcAddress, DstAddress, DataLength);
		 /* Clear DBM bit */
	    	((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->CR &= (uint32_t)(~DMA_SxCR_DBM);

		  /* Configure DMA Stream data length */
	    	((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->NDTR = size;

		  /* Memory to Peripheral */
			/* Configure DMA Stream destination address */
	    	((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->PAR = (uint32_t)&uart->Instance->RDR;
			/* Configure DMA Stream source address */
	    	((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->M0AR = *((uint32_t *)&buffer);

	      /* Clear all interrupt flags at correct offset within the register */
	      regs->IFCR = 0x3FU << uart->hdmatx->StreamIndex;

	      /* Enable Common interrupts*/
	      ((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;

//	      if(hdma->XferHalfCpltCallback != NULL)
//	      {
//	        hdma->Instance->CR  |= DMA_IT_HT;
//	      }

	      /* Enable the Peripheral */
//	      __HAL_DMA_ENABLE(uart->hdmatx);
	      ((DMA_Stream_TypeDef*)uart->hdmatx->Instance)->CR |=  (0x1UL << (0U));
	    }
	    else
	    {
	      /* Process unlocked */
	      __HAL_UNLOCK(uart->hdmatx);


	      /* Return error status */
	      status = HAL_BUSY;
	    }


	    /* Clear the TC flag in the SR register by writing 0 to it */
//	    __HAL_UART_CLEAR_FLAG(uart, UART_FLAG_TC);
	    uart->Instance->ISR = ~((uint32_t)(0x1UL << (6U)));
	    /* Process Unlocked */
	    __HAL_UNLOCK(uart);

	    /* Enable the DMA transfer for transmit request by setting the DMAT bit
	       in the UART CR3 register */
	    ATOMIC_SET_BIT(uart->Instance->CR3, USART_CR3_DMAT);

	    return status;
	  }
	  else
	  {
	    return HAL_BUSY;
	  }
}


HAL_StatusTypeDef MB_UartReceiveDMA(uint8_t *buffer, uint16_t size){

	HAL_StatusTypeDef status;

	status = HAL_UARTEx_ReceiveToIdle_DMA(uart, buffer, size);
	__HAL_DMA_DISABLE_IT(dma_uart_rx, DMA_IT_HT);
	return status;
}



/**
  * @brief  DMA UART transmit process complete callback.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void MB_UART_DMATransmitCplt(DMA_HandleTypeDef *hdma){
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  /* DMA Normal mode*/

  if ((((DMA_Stream_TypeDef*)hdma->Instance)->CR & DMA_SxCR_CIRC) == 0U)
  {
    huart->TxXferCount = 0x00U;
    /* Disable the DMA transfer for transmit request by setting the DMAT bit
       in the UART CR3 register */
    ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAT);

    /* Enable the UART Transmit Complete Interrupt */
    ATOMIC_SET_BIT(huart->Instance->CR1, USART_CR1_TCIE);

  }
  /* DMA Circular mode */
  else
  {
    /*Call legacy weak Tx complete callback*/
    HAL_UART_TxCpltCallback(huart);

  }
}


/**
  * @brief  End ongoing Tx transfer on UART peripheral (following error detection or Transmit completion).
  * @param  huart UART handle.
  * @retval None
  */
static void MB_UART_EndTxTransfer(UART_HandleTypeDef *huart){
  /* Disable TXEIE and TCIE interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_TXEIE | USART_CR1_TCIE));

  /* At end of Tx process, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;
}

/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
static void MB_UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
  ATOMIC_CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
  ATOMIC_CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);

  /* In case of reception waiting for IDLE event, disable also the IDLE IE interrupt source */
  if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
  {
    ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);
  }

  /* At end of Rx process, restore huart->RxState to Ready */
  huart->RxState = HAL_UART_STATE_READY;
  huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;
}


/**
  * @brief DMA UART transmit process half complete callback
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void MB_UART_DMATxHalfCplt(DMA_HandleTypeDef *hdma){
//  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)(hdma)->Parent;
  HAL_UART_TxHalfCpltCallback((UART_HandleTypeDef *)(hdma)->Parent);
}


/**
  * @brief  DMA UART communication error callback.
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void MB_UART_DMAError(DMA_HandleTypeDef *hdma)
{
  uint32_t dmarequest = 0x00U;
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

  /* Stop UART DMA Tx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAT);
  if ((huart->gState == HAL_UART_STATE_BUSY_TX) && dmarequest)
  {
    huart->TxXferCount = 0x00U;
    MB_UART_EndTxTransfer(huart);
  }

  /* Stop UART DMA Rx request if ongoing */
  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
  if ((huart->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest)
  {
    huart->RxXferCount = 0x00U;
    MB_UART_EndRxTransfer(huart);
  }

  huart->ErrorCode |= HAL_UART_ERROR_DMA;
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);

}
