/*
 * IRQHandler.c
 *
 *  Created on: Jan 14, 2023
 *      Author: wsrra
 */
#include "stm32h7xx_hal.h"
#include "IRQHandler.h"
#include "stm32h7xx_it.h"
#include "Scheduler.h"
#include "Debug.h"
#include "Encoder.h"
#include "Task_Handler.h"
#include "TimeStamp.h"
#include "MB_UART.h"
#include "DigitalIn.h"
#include "GlobalVars.h"
/* Exported functions prototypes from stm32fxx_it.c file
 **/

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_i2c1_tx;
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern I2C_HandleTypeDef hi2c1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim24;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern SPI_HandleTypeDef hspi1;


//extern UART_HandleTypeDef huart2;
extern GlobalVar_t gVar;
//extern DebugStruct dbug;
extern TimeStamp timStamp;

extern uint8_t spiTxCplt;
extern uint8_t spiRxCplt;
extern uint32_t rtuErrCount;
extern uint32_t rtuRxCount;

#ifdef IRQ_ENABLE_USER
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{

  while (1)
  {
  }

}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{

	DBG_SPRINT_NL("----------HardFault----------");

  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{

	DBG_SPRINT_NL("----------MemManageFault----------");

  while (1)
  {

  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

	DBG_SPRINT_NL("----------BusFault----------");

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

	DBG_SPRINT_NL("----------UsageFault----------");

  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{

}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
//TODO: speed calculation will be implemented later
//	if(gVar.dio.enableEnc > 0){
//		if(HAL_GetTick() > 1000U){
//			ENC_CalcSpeedQuad(&gVar.dio.enc);
//		}
//	}


  HAL_IncTick();

}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/
const uint32_t EXTI_REG = 0x40000000UL + 0x00010000UL + 0x3C00UL;




/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
	/*For incremental encoder channel Z----------------*/
	//	ENC_ReadChZ(&gVar.dio.enc); //TODO: speed calculation will be implemented later
	if(gVar.dio.enc.enable > 0 && gVar.dio.enc.configs.mode == ENC_MODE_QUAD){
		gVar.dio.enc.counterZ ++;
		gVar.dio.enc.countQuadAtRef = __HAL_TIM_GET_COUNTER(gVar.dio.enc.configs.quadrature.timer);
	}else{
		gVar.dio.enc.counterZ = 0;
		gVar.dio.enc.countQuadAtRef = 0;
	}

	if (__HAL_GPIO_EXTI_GET_IT(DI_3_ENC_Z_Pin) != 0x00U)
	{
	__HAL_GPIO_EXTI_CLEAR_IT(DI_3_ENC_Z_Pin);
	}

//  HAL_GPIO_EXTI_IRQHandler(DI_3_Pin);
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_4_Pin) != 0x00U)
	{
	__HAL_GPIO_EXTI_CLEAR_IT(DI_4_Pin);
	}

//  HAL_GPIO_EXTI_IRQHandler(DI_4_Pin);
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
	/*For incremental encoder channel A-----------------*/
	//ENC_ReadChA(&gVar.dio.enc);
	if(gVar.dio.enc.enable > 0){
		if(gVar.dio.enc.configs.mode & ENC_MODE_INCRE_CHA){gVar.dio.enc.counterA++;}
	}else{
		gVar.dio.enc.counterA = 0;
	}
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_1_ENC_A_Pin) != 0x00U)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(DI_1_ENC_A_Pin);
	}

	/*For incremental encoder channel B----------------*/
	  //ENC_ReadChB(&gVar.dio.enc);
	if(gVar.dio.enc.enable > 0){
		if(gVar.dio.enc.configs.mode & ENC_MODE_INCRE_CHB){	gVar.dio.enc.counterB ++;}
	}else{
		gVar.dio.enc.counterB = 0;
	}
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_2_ENC_B_Pin) != 0x00U)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(DI_2_ENC_B_Pin);
	}

	/*DI 5-----------------------------------------------*/
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_5_Pin) != 0x00U)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(DI_5_Pin);
	}

	/*DI 6-----------------------------------------------*/
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_6_Pin) != 0x00U)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(DI_6_Pin);
	}

	/*DI 6-----------------------------------------------*/
	// clear interrupt and make ready for next interrupt
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(DI_7_Pin) != 0x00U)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(DI_7_Pin);
	}

//  HAL_GPIO_EXTI_IRQHandler(DI_1_Pin);
//  HAL_GPIO_EXTI_IRQHandler(DI_2_Pin);
//  HAL_GPIO_EXTI_IRQHandler(DI_5_Pin);
//  HAL_GPIO_EXTI_IRQHandler(DI_6_Pin);
//  HAL_GPIO_EXTI_IRQHandler(DI_7_Pin);
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */

  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(DI_8_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}







///**
//  * @brief This function handles EXTI line[15:10] interrupts.
//  */
//void EXTI15_10_IRQHandler(void)
//{
//	// clear interrupt and make ready for next interrupt
//	if ((((EXTI_TypeDef *)EXTI_REG)->PR & W5500_IRQ_Pin) != RESET)
//	  {
//	    ((EXTI_TypeDef *)EXTI_REG)->PR = W5500_IRQ_Pin;
////	    DEBUG_SPRINT_NL("w5500_interrupt, %d", HAL_GPIO_ReadPin(W5500_IRQ_GPIO_Port, W5500_IRQ_Pin));
////	    HAL_GPIO_EXTI_Callback(W5500_IRQ_Pin);
//	  }

//
////	Server_ReadInterpt(&gVar.server);
////	HAL_GPIO_EXTI_IRQHandler(W5500_IRQ_Pin);
//}
//
//
//
//
//
//
//
//
///**
//  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
//  */
//void TIM1_UP_TIM10_IRQHandler(void)
//{
////  HAL_TIM_IRQHandler(&htim1);
////  HAL_TIM_IRQHandler(&htim10);
//}
//


/**
  * @brief This function handles TIM4 global interrupt.
  *
  * MBS_ChackRcvdFrame(), function execution time is 12us
  * to reduce the execution time, that function job is implemented here directly.
  * The achievement is that, we are able to save 4.15 us.
  * So that, MBS_ChackRcvdFrame(), function is deprecated
  *
  */
void TIM4_IRQHandler(void)
{
	/*-------- for 1.5 char -------------*/
//	HAL_GPIO_TogglePin(DO_2_PWM_GPIO_Port, DO_2_PWM_Pin);
//	DBG_PRINT("\r\nTIM4  1.5 char");
//	MBS_ChackRcvdFrame(&gVar.mbSerial, &gVar.mbRTUSlave);		// DEPRECATED: Sept 20, 2023

	//REPLACED: Sept 20, 2023---------------------
	if(htim4.Instance->SR & TIM_SR_UIF){		// TODO:  uncomment
		MBS_STOP_TIMER(&htim4);
//		HAL_TIM_Base_Stop_IT(&htim4);
		gVar.mbSerial.frameOkFlag = 0U;
		if(gVar.mbSerial.rxBuffSize >= (MBS_RTU_PDU_MIN_SIZE-1U)){		// if the buffer size is less than the max buffer size

			gVar.mbHandle->diag.bus_msg_count++;
			if(gVar.mbSerial.rxBuff[MB_SA_INDEX] == gVar.mbRTUSlave.slave_address
				|| gVar.mbSerial.rxBuff[MB_SA_INDEX] == MB_BROADCAST_ADDR){			// if slave address is matched

				uint16_t receivedCRC16 = (gVar.mbSerial.rxBuff[gVar.mbSerial.rxBuffSize-1] << 8) | gVar.mbSerial.rxBuff[gVar.mbSerial.rxBuffSize-2];
				if(receivedCRC16 == MB_CalcCRC16(gVar.mbSerial.rxBuff, gVar.mbSerial.rxBuffSize-2)){//MB_CRCCheck(gVar.mbSerial.rxBuff, gVar.mbSerial.rxBuffSize)){
					gVar.mbRTUSlave.rxBuffer = &gVar.mbSerial.rxBuff[0];
					gVar.mbRTUSlave.txBuffer = &gVar.mbSerial.txBuff[0];
					gVar.mbRTUSlave.rxBufferSize = gVar.mbSerial.rxBuffSize;
					gVar.mbRTUSlave.txBufferSize = gVar.mbSerial.txBuffSize;
					gVar.mbSerial.frameOkFlag=1;
				}else{
					gVar.mbSerial.frameOkFlag = 0;
					gVar.mbHandle->diag.bus_com_err_count++;	// counts the quantity of CRC errors encountered by the slave since its last restart
				}
				gVar.mbHandle->busy_status = MB_STATUS_OK;
			}
		}
	}

//	DIO_WRITE_PIN_RESET(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin);
	//TODO: to implement this LED using IOExtender
	//----------------------------------------------
//	HAL_TIM_IRQHandler(&htim4);
//	MBS_START_TIMER(&htim4);

}



/**
  * @brief This function handles TIM5 global interrupt.
  * MBS_StopReceiving(), function execution time is 12us
  * to reduce the execution time, that function job is implemented here directly.
  * The achievement is that, we are able to save 4.15 us.
  * So that, MBS_StopReceiving(), function is deprecated
  */
void TIM6_DAC_IRQHandler(void)
{
	/*-------- for 3.5 char -------------*/
//	HAL_GPIO_TogglePin(DO_3_PWM_GPIO_Port, DO_3_PWM_Pin);
//	DBG_PRINT("\r\nTIM6 3.5 char");

	gVar.mbSerial.isSending = 0;
	MBS_StopReceiving(&gVar.mbSerial);			// DEPRECATED: Sept 20, 2023
	HAL_TIM_Base_Start_IT(&htim2);
	//REPLACED: Sept 20, 2023---------------------
	if(htim6.Instance->SR & TIM_SR_UIF){
//		HAL_TIM_Base_Stop_IT(&htim6);
		MBS_STOP_TIMER(&htim6);		//stop the 3.5 char timer, once a data packet is received
		gVar.mbSerial.isDataReceived = 1U;


//		DBG_ENABLE();
//			DEBUG_SPRINT("\nR%d",gVar.mbSerial.rxBuffSize); //\r\nR:(%d)
//			dbug.enable = !dbug.enable;
		if(gVar.mbSerial.rxBuffSize <= MB_PDU_MIN_SIZE || gVar.mbSerial.rxBuffSize >= MB_PDU_MAX_SIZE){
			gVar.mbSerial.rxBuffSize = 0 ;
			gVar.mbSerial.isDataReceived = 0U;
		}else{
			TH_MBSerial();
			gVar.mbSerial.rxBuffSize = 0 ;
		}
		gVar.mbSerial.isReceiving = 0;
		rtuRxCount++; //TODO: to remove
	}


//	DIO_WRITE_PIN_RESET(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin);
	//TODO: to implement this LED using IOExtender
	//-----------------------------------
//  HAL_TIM_IRQHandler(&htim6);
//	MBS_START_TIMER(&htim6);

}



///**
//  * @brief This function handles SPI1 global interrupt.
//  */
//void SPI1_IRQHandler(void)
//{
//  /* USER CODE BEGIN SPI1_IRQn 0 */
//
//  /* USER CODE END SPI1_IRQn 0 */
//  HAL_SPI_IRQHandler(&hspi1);
//  /* USER CODE BEGIN SPI1_IRQn 1 */
//
//  /* USER CODE END SPI1_IRQn 1 */
//}
//
//
///**
//  * @brief This function handles DMA2 stream0 global interrupt.
//  */
//void DMA2_Stream0_IRQHandler(void)
//{
//  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
////	 DEBUG_SPRINT("\r\nhdma_spi1_rx");
//  /* USER CODE END DMA2_Stream0_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_spi1_rx);
//  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */
//
//  /* USER CODE END DMA2_Stream0_IRQn 1 */
//}
//
///**
//  * @brief This function handles DMA2 stream3 global interrupt.
//  */
//void DMA2_Stream3_IRQHandler(void)
//{
//  /* USER CODE BEGIN DMA2_Stream3_IRQn 0 */
////	 DEBUG_SPRINT("\r\nhdma_spi1_tx");
//
//  /* USER CODE END DMA2_Stream3_IRQn 0 */
//  HAL_DMA_IRQHandler(&hdma_spi1_tx);
//  /* USER CODE BEGIN DMA2_Stream3_IRQn 1 */
//
//  /* USER CODE END DMA2_Stream3_IRQn 1 */
//}
//
//void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){
////	__HAL_DMA_DISABLE(&hdma_spi1_rx);
////	__HAL_DMA_DISABLE(&hdma_spi1_tx);
////	HAL_SPI_MspInit(hspi);
////	__HAL_DMA_DISABLE(&hdma_spi1_tx);
////	__HAL_DMA_CLEAR_FLAG(&hdma_spi1_tx, DMA_FLAG_TCIF3_7);
////	DEBUG_SPRINT("\r\nhdma_spi1_tx_cplt");
////	hdma_spi1_tx.State = HAL_DMA_STATE_READY;
//	spiTxCplt = 0;
//}
//void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
////	for(uint16_t i = 0; i < hspi->TxXferSize; i++){
////		DEBUG_SPRINT(" %02X", hspi->pTxBuffPtr[i]);
////	}
////	DEBUG_SPRINT(" : ");
////	if( hspi->RxXferSize > 4){
////	for(uint16_t i = 0; i < hspi->RxXferSize; i++){
////		DEBUG_SPRINT(" %02X", hspi->pRxBuffPtr[i]);
////	}
////	DEBUG_SPRINT("\r\n");
////	}
////	__HAL_DMA_DISABLE(&hdma_spi1_rx);
////	__HAL_DMA_DISABLE(&hdma_spi1_tx);
////	HAL_SPI_MspInit(hspi);
////	__HAL_DMA_DISABLE(&hdma_spi1_rx);
////	__HAL_DMA_CLEAR_FLAG(&hdma_spi1_rx, DMA_FLAG_TCIF0_4);
////	DEBUG_SPRINT("\r\nhdma_spi1_rx_cplt");
////	hdma_spi1_rx.State = HAL_DMA_STATE_READY;
//	spiRxCplt = 0;
//
//}
//
//void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi){
//	spiRxCplt = 0;
//	spiTxCplt = 0;
////	DEBUG_SPRINT_NL("hdma_spi1_error: %d", hspi->ErrorCode);
////	DEBUG_SPRINT_NL("hdmatx_error: %d", hspi->hdmatx->ErrorCode);
////	DEBUG_SPRINT_NL("hdmarx_error: %d", hspi->hdmarx->ErrorCode);
//	if(hspi->Instance == SPI1){
//		gVar.w5500Chip.spi.errorCode = hspi->ErrorCode;
//		gVar.w5500Chip.spi.dmaRxErrCode = hspi->hdmarx->ErrorCode;
//		gVar.w5500Chip.spi.dmaTxErrCode =  hspi->hdmatx->ErrorCode;
//		gVar.diag.w5500Spi.ErrCode = hspi->ErrorCode;
//	}
//
////	HAL_SPI_MspInit(hspi);
//}
//

HAL_StatusTypeDef Uart_Receive_IT(UART_HandleTypeDef *huart)
{
  uint8_t  *pdata8bits;
  uint16_t *pdata16bits;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    if ((huart->Init.WordLength == UART_WORDLENGTH_9B) && (huart->Init.Parity == UART_PARITY_NONE))
    {
      pdata8bits  = NULL;
      pdata16bits = (uint16_t *) huart->pRxBuffPtr;
      *pdata16bits = (uint16_t)(huart->Instance->RDR & (uint16_t)0x01FF);
      huart->pRxBuffPtr += 2U;
    }
    else
    {
      pdata8bits = (uint8_t *) huart->pRxBuffPtr;
      pdata16bits  = NULL;

      if ((huart->Init.WordLength == UART_WORDLENGTH_9B) || ((huart->Init.WordLength == UART_WORDLENGTH_8B) && (huart->Init.Parity == UART_PARITY_NONE)))
      {
        *pdata8bits = (uint8_t)(huart->Instance->RDR & (uint8_t)0x00FF);
      }
      else
      {
        *pdata8bits = (uint8_t)(huart->Instance->RDR & (uint8_t)0x007F);
      }
      huart->pRxBuffPtr += 1U;
    }

    if (--huart->RxXferCount == 0U)
    {
      /* Disable the UART Data Register not empty Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_READY;

      /* Check current reception Mode :
         If Reception till IDLE event has been selected : */
      if (huart->ReceptionType == HAL_UART_RECEPTION_TOIDLE)
      {
        /* Set reception type to Standard */
        huart->ReceptionType = HAL_UART_RECEPTION_STANDARD;

        /* Disable IDLE interrupt */
        ATOMIC_CLEAR_BIT(huart->Instance->CR1, USART_CR1_IDLEIE);

        /* Check if IDLE flag is set */
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE))
        {
          /* Clear IDLE flag in ISR */
          __HAL_UART_CLEAR_IDLEFLAG(huart);
        }


        HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);

      }
      else
      {
        /*Call legacy weak Rx complete callback*/
        HAL_UART_RxCpltCallback(huart);
      }

      return HAL_OK;
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



uint32_t errFlag = (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE);
uint32_t dmarequest = 0x00U;
volatile uint32_t tmpreg = 0x00U;
uint32_t errorflag = 0x00U;


//TODO: to remove
extern uint32_t rtuRxCount;



/**
  * @brief This function handles USART6 global interrupt.
  */
void USART2_IRQHandler(void){


//		HAL_UART_IRQHandler(&huart2);
	/*--------------------------------------------------------------*/
	  uint32_t isrflags   = huart2.Instance->ISR;//READ_REG(huart->Instance->SR);

	  /* If no error occurs */
	  errorflag = (isrflags & errFlag);

	   /* If some errors occur */
	   if ((errorflag != RESET)) {
		 /* UART parity error interrupt occurred ----------------------------------*/
		 if ((isrflags & USART_ISR_PE) != RESET) {
		   huart2.ErrorCode |= HAL_UART_ERROR_PE;
		   gVar.diag.rtuBus.peCount++;
		 }
		 /* UART noise error interrupt occurred -----------------------------------*/
		 if ((isrflags & USART_ISR_NE) != RESET) {
		   huart2.ErrorCode |= HAL_UART_ERROR_NE;
		   gVar.diag.rtuBus.neCount++;
		 }

		 /* UART frame error interrupt occurred -----------------------------------*/
		 if ((isrflags & USART_ISR_FE) != RESET)  {
			 gVar.diag.rtuBus.feCount++;
		   huart2.ErrorCode |= HAL_UART_ERROR_FE;
		 }

		 /* UART Over-Run interrupt occurred --------------------------------------*/
		 if ((isrflags & USART_ISR_ORE) != RESET) {
			 gVar.diag.rtuBus.oreCount++;
		   huart2.ErrorCode |= HAL_UART_ERROR_ORE;
		 }
	   }

	   //Check the protocol
	 if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA){
	   if ((errorflag != RESET)) {
		 /* Call UART Error Call back function if need be --------------------------*/
		 if (huart2.ErrorCode != HAL_UART_ERROR_NONE)
		 {
		   /* UART in mode Receiver -----------------------------------------------*/
		   if (((isrflags & USART_ISR_RXNE_RXFNE) != RESET) && ((huart2.Instance->CR1 & USART_CR1_RXNEIE) != RESET))
		   {
			 Uart_Receive_IT(&huart2);
		   }
		   /* If Overrun error occurs, or if any error occurs in DMA mode reception, */
		   dmarequest = HAL_IS_BIT_SET(huart2.Instance->CR3, USART_CR3_DMAR);
		   if (((huart2.ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
		   {
			 /* Disable the UART DMA Rx request if enabled */
			 if (HAL_IS_BIT_SET(huart2.Instance->CR3, USART_CR3_DMAR))
			 {
			   ATOMIC_CLEAR_BIT(huart2.Instance->CR3, USART_CR3_DMAR);

			   /* Abort the UART DMA Rx stream */
			   if (huart2.hdmarx != NULL)
			   {
				if (huart2.hdmarx->State != HAL_DMA_STATE_BUSY)
				 {
				   huart2.RxXferCount = 0x00U;
				   huart2.TxXferCount = 0x00U;
				   huart2.hdmarx->ErrorCode = HAL_DMA_ERROR_NO_XFER;
				   gVar.diag.rtuBus.dteCount++;
				   /*Call legacy weak error callback*/
				   HAL_UART_ErrorCallback(&huart2);
				 } else{
					 huart2.hdmarx->State = HAL_DMA_STATE_ABORT; /* Set Abort State  */
				   __HAL_DMA_DISABLE(huart2.hdmarx);			 /* Disable the stream */
				 }
			   }else{
				 /*Call legacy weak error callback*/
				 HAL_UART_ErrorCallback(&huart2);
			   }
			 }else{
			   /*Call legacy weak error callback*/
			   HAL_UART_ErrorCallback(&huart2);
			 }
		   }else{
			 /* Non Blocking error : transfer could go on.*/
			 HAL_UART_ErrorCallback(&huart2);
			 huart2.ErrorCode = HAL_UART_ERROR_NONE;
		   }
		 }
		 return;
	   } /* End if some error occurs */

	   /* Check current reception Mode :
		  If Reception till IDLE event has been selected : */
	   if ((huart2.ReceptionType == HAL_UART_RECEPTION_TOIDLE)  && ((isrflags & USART_ISR_IDLE) != 0U)) {
		   /*Clear the IDLE line detection interrupt */
		   tmpreg = huart2.Instance->ISR;
		   tmpreg = huart2.Instance->RDR;

		   /* DMA mode enabled */
		   /* Check received length : If all expected data are received, do nothing, (DMA cplt callback will be called).
			  Otherwise, if at least one data has already been received, IDLE event is to be notified to user */
		  if (((uint16_t)((DMA_Stream_TypeDef*)huart2.hdmarx->Instance)->NDTR > 0U) && ((uint16_t)((DMA_Stream_TypeDef*)huart2.hdmarx->Instance)->NDTR < huart2.RxXferSize)) {
			 /* Reception is not complete */
			huart2.RxXferCount = (uint16_t)((DMA_Stream_TypeDef*)huart2.hdmarx->Instance)->NDTR;
			/* In Normal mode, end DMA xfer and HAL UART Rx process*/
			/* Disable the DMA transfer for the receiver request by resetting the DMAR bit in the UART CR3 register */
			ATOMIC_CLEAR_BIT(huart2.Instance->CR3, USART_CR3_DMAR);

			/* At end of Rx process, restore huart2.RxState to Ready */
			huart2.RxState = HAL_UART_STATE_READY;
			huart2.ReceptionType = HAL_UART_RECEPTION_STANDARD;
			ATOMIC_CLEAR_BIT(huart2.Instance->CR1, USART_CR1_IDLEIE);

			(void)HAL_DMA_Abort(huart2.hdmarx);		/* Last bytes received, so no need as the abort is immediate */
			HAL_UARTEx_RxEventCallback(&huart2, (huart2.RxXferSize - huart2.RxXferCount));	/*Call legacy weak Rx Event callback*/
			return;
		 }
	   }

	   /* UART in mode Transmitter ------------------------------------------------*/
	   if (((isrflags & USART_ISR_TXE_TXFNF) != RESET) && ((huart2.Instance->CR1 & USART_CR1_TXEIE_TXFNFIE) != RESET))
	   {
		  /* Check that a Tx process is ongoing */
		  if (huart2.gState == HAL_UART_STATE_BUSY_TX)
		  {
			if ((huart2.Init.WordLength == UART_WORDLENGTH_9B) && (huart2.Init.Parity == UART_PARITY_NONE)){
			  huart2.Instance->RDR = (uint16_t)(*huart2.pTxBuffPtr & (uint16_t)0x01FF);
			  huart2.pTxBuffPtr += 2U;
			}else{
			  huart2.Instance->RDR = (uint8_t)(*huart2.pTxBuffPtr++ & (uint8_t)0x00FF);
			}

			if (--huart2.TxXferCount == 0U) {
			  __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);		/* Disable the UART Transmit Complete Interrupt */
			  __HAL_UART_ENABLE_IT(&huart2, UART_IT_TC);		/* Enable the UART Transmit Complete Interrupt */
			}
		  }
		 return;
	   }

	   /* UART in mode Transmitter end --------------------------------------------*/
	   if (((isrflags & USART_ISR_TC) != RESET) && ((huart2.Instance->CR1 & USART_CR1_TCIE) != RESET))
	   {
		   huart2.gState = HAL_UART_STATE_READY;		/* Tx process is ended, restore huart->gState to Ready */
		   __HAL_UART_DISABLE_IT(&huart2, UART_IT_TC);	/* Disable the UART Transmit Complete Interrupt */
		   HAL_UART_TxCpltCallback(&huart2);			/*Call legacy weak Tx complete callback*/
		 return;
	   }
	   (void)tmpreg;

	}else{
		MBS_Receive(&gVar.mbSerial);
	}

//	DBG_SPRINT_NL("RTU_RX:");

}



/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA2 stream1 global interrupt.
  */
void DMA2_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream1_IRQn 0 */

  /* USER CODE END DMA2_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA2_Stream1_IRQn 1 */

  /* USER CODE END DMA2_Stream1_IRQn 1 */
}

/**
  * @brief This function handles USART6 DMA Rx Event interrupt.
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size){
	if (huart->Instance == USART2) {
		if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA){

//
//			dbug.enable = 1;
//			DEBUG_SPRINT_APPEND(dbug.str,"\r\nR:(%d)",Size);
//			for(uint8_t i = 0; i < Size; i++){
//				DEBUG_SPRINT_APPEND(dbug.str,"%02X,", gVar.mbSerial.rxBuff[i]);
//			}
//			DEBUG_SPRINT_APPEND(dbug.str,"]");
//			DEBUG_SPRINT(dbug.str);
//
//			rtuRxCount++; //TODO: to remove

//			DIO_WRITE_PIN_SET(RS485_RX_LED_GPIO_Port, RS485_RX_LED_Pin);
			//TODO: to implement LED with IOExtender

			gVar.mbSerial.rxBuffSize = Size;
//			MBS_ChackRcvdFrame(&gVar.mbSerial, &gVar.mbRTUSlave);
			gVar.mbSerial.frameOkFlag = 0U;
			//Check the slave address and CRC
			if(gVar.mbSerial.rxBuffSize >= (MBS_RTU_PDU_MIN_SIZE-1U)){		// if the buffer size is less than the max buffer size
				gVar.mbHandle->diag.bus_msg_count++;
				if(gVar.mbSerial.rxBuff[MB_SA_INDEX] == gVar.mbRTUSlave.slave_address
					|| gVar.mbSerial.rxBuff[MB_SA_INDEX] == MB_BROADCAST_ADDR){			// if slave address is matched

					uint16_t receivedCRC16 = (gVar.mbSerial.rxBuff[gVar.mbSerial.rxBuffSize-1] << 8) | gVar.mbSerial.rxBuff[gVar.mbSerial.rxBuffSize-2];
					if(receivedCRC16 == MB_CalcCRC16(gVar.mbSerial.rxBuff, gVar.mbSerial.rxBuffSize-2)){//MB_CRCCheck(gVar.mbSerial.rxBuff, gVar.mbSerial.rxBuffSize)){
						gVar.mbRTUSlave.rxBuffer = &gVar.mbSerial.rxBuff[0];
						gVar.mbRTUSlave.txBuffer = &gVar.mbSerial.txBuff[0];
						gVar.mbRTUSlave.rxBufferSize = gVar.mbSerial.rxBuffSize;
						gVar.mbRTUSlave.txBufferSize = gVar.mbSerial.txBuffSize;
						gVar.mbSerial.frameOkFlag=1;
					}else{
						gVar.mbSerial.frameOkFlag = 0;
						gVar.mbHandle->diag.bus_com_err_count++;	// counts the quantity of CRC errors encountered by the slave since its last restart
					}
					gVar.mbHandle->busy_status = MB_STATUS_OK;
				}
			}

			// Stop receiving and process the data
			MBS_StopReceiving(&gVar.mbSerial);

	//		DEBUG_SPRINT("\r\nR:(%d)",serial->rxBuffSize);
			if(gVar.mbSerial.rxBuffSize <= MB_PDU_MIN_SIZE || gVar.mbSerial.rxBuffSize >= MB_PDU_MAX_SIZE){
				gVar.mbSerial.rxBuffSize = 0 ;
				gVar.mbSerial.isDataReceived = 0U;
			}else{
				gVar.mbSerial.isDataReceived = 1U;
				TH_MBSerial();
			}

			MB_UartReceiveDMA(gVar.mbSerial.rxBuff, MB_PDU_MAX_SIZE);
			gVar.mbSerial.isFreeze = 0;



		}
	}


	if(huart->Instance == UART5){
		L6360_UartRxcIRQ(&gVar.iolm.port[IOL_PI_0],0,Size);
//		DBG_SPRINT_NL("RX[%d]:%02X %02X", Size, gVar.iolm.port[IOL_PI_0].uart.rxBuf[0], gVar.iolm.port[IOL_PI_0].uart.rxBuf[1]);
	}
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	__HAL_UART_ENABLE_IT(huart, UART_FLAG_TC);
//	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
//	HAL_UARTEx_ReceiveToIdle_DMA(huart, gVar.iolm.port[IOL_PI_0].uart.rxBuf, 256);
//	__HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
//	DBG_SPRINT_NL("RXC:%02X %02X", gVar.iolm.port[IOL_PI_0].uart.rxBuf[0], gVar.iolm.port[IOL_PI_0].uart.rxBuf[0]);
//	if(huart->Instance == USART1){
//		__HAL_UAGET
//		L6360_UartRxcIRQ(&gVar.iolm.port[IOL_PI_0],0,0);
//	}

}


/* UART DMA transfer complete callback for TX */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{

	if (huart->Instance == USART2) {
		if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA){
			for(uint32_t i = 0; i <  50; i++){
				__NOP();
			};
			MBS_AllCmpltTxCallback(&gVar.mbSerial, MB_getSCRAddress(), 0);
//			dbug.enable = 1;

			DIO_WRITE_PIN_RESET(RS485_DIR_GPIO_Port, RS485_DIR_Pin);
//			DIO_WRITE_PIN_RESET(RS485_TX_LED_GPIO_Port, RS485_TX_LED_Pin);
			//TODO: to implement LED with IOExtender
			gVar.mbSerial.isSending = 0;

		}
	}
	if(huart->Instance == USART1){
		L6360_UartTxcIRQ(&gVar.iolm.port[IOL_PI_0]);
	}
}


/*
 * Callback for UART communication error.
 *	0x00000000U  = No error
 *	0x00000001U  = Parity error
 *	0x00000002U  = Noise error
 *	0x00000004U  = Frame error
 *	0x00000008U  = Overrun error
 *	0x00000010U  = DMA transfer error
 * */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){
		char str[50] = {0};

		uint16_t len = DBG_SPRINT_APPEND(str,"UartErr: %02X, Rx:%02x, TX:%02X", huart->ErrorCode, huart->hdmarx->ErrorCode, huart->hdmatx->ErrorCode);
		len += DBG_SPRINT_APPEND(&str[len],", ISR: %08X : ",huart->Instance->ISR);
			if(  huart->ErrorCode & HAL_UART_ERROR_NONE){
				len += DBG_SPRINT_APPEND(&str[len],"None");
			}if(  huart->ErrorCode & HAL_UART_ERROR_FE){
				len += DBG_SPRINT_APPEND(&str[len],"FE,");
			}if(  huart->ErrorCode & HAL_UART_ERROR_NE){
				len += DBG_SPRINT_APPEND(&str[len],"NE,");
			}if(  huart->ErrorCode &  HAL_UART_ERROR_ORE){
				__HAL_UART_CLEAR_OREFLAG(huart);
				len += DBG_SPRINT_APPEND(&str[len],"ORE,");
			}if(  huart->ErrorCode &  HAL_UART_ERROR_PE){
				len += DBG_SPRINT_APPEND(&str[len],"PE,");
			}if(  huart->ErrorCode &  HAL_UART_ERROR_RTO){
				len += DBG_SPRINT_APPEND(&str[len],"RTO,");
			}if(  huart->ErrorCode &  HAL_UART_ERROR_DMA){
				len += DBG_SPRINT_APPEND(&str[len],"DMA,");
			}
		DBG_PRINT(str);
//		MX_USART1_UART_Init(); // todo: to replace
		__HAL_UART_ENABLE_IT(huart, UART_FLAG_TC);
		__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
		HAL_UARTEx_ReceiveToIdle_DMA(huart, gVar.iolm.port[0].uart.rxBuf, 256);
		__HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
	}else if(huart->Instance == USART3){
		DBG_SPRINT_NL("DBG: ERR: %d",huart->ErrorCode);
	}
	if (huart->Instance == USART2) {
		if(huart->ErrorCode == 0U) return;

		rtuErrCount++;

		gVar.mbSerial.errorCode = huart->ErrorCode;
		gVar.diag.rtuBus.ErrCode = huart->ErrorCode;
		gVar.diag.rtuBus.dmaRxErrCode = huart->hdmarx->ErrorCode;
		gVar.diag.rtuBus.dmaTxErrCode = huart->hdmatx->ErrorCode;

		if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA){
			MB_UartReceiveDMA(gVar.mbSerial.rxBuff, MB_PDU_MAX_SIZE);
		}
	}

}






/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM3_IRQHandler(void)
{

	// input capture for channel 1
	 if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_CC1) != RESET)
	  {
		__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC1);
		/* Input capture event */
		if ((htim3.Instance->CCMR1 & TIM_CCMR1_CC1S) != 0x00U)
		{
//		  HAL_TIM_IC_CaptureCallback(htim);
			gVar.dio.di[DI_1].pulseInf.lastEdgeTime = gVar.dio.di[DI_1].pulseInf.edgeTime;
			gVar.dio.di[DI_1].pulseInf.edgeTime = htim3.Instance->CCR1;
			gVar.dio.di[DI_1].pulseInf.isEdgeDtctd = 1;
			gVar.dio.di[DI_1].pulseInf.isPulseDtctd = 1;
		}
	  }
	 // input capture for channel 2
	  /* Capture compare 2 event */
	  if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_CC2) != RESET)
	  {
	      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_CC2);
	      /* Input capture event */
	      if ((htim3.Instance->CCMR1 & TIM_CCMR1_CC2S) != 0x00U)
	      {
//	    	HAL_TIM_IC_CaptureCallback(htim);
			gVar.dio.di[DI_2].pulseInf.lastEdgeTime = gVar.dio.di[DI_2].pulseInf.edgeTime;
			gVar.dio.di[DI_2].pulseInf.edgeTime = htim3.Instance->CCR2;
			gVar.dio.di[DI_2].pulseInf.isEdgeDtctd = 1;
			gVar.dio.di[DI_2].pulseInf.isPulseDtctd = 1;
	      }
	  }


	/*To capture and count the overflow*/
	if ((htim3.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE)
	  {
		htim3.Instance->SR = ~TIM_IT_UPDATE;//((0x1UL << (0U))));
//		TH_DICaptureOverflow(&htim3);
		if(gVar.dio.di[DI_1].config.mode == DI_MODE_PULSE_CAPTURE && gVar.dio.di[DI_1].config.counterType == DI_COUNT_TYPE_TIMER ){
			gVar.dio.di[DI_1].pulseInf.countOVF++;
		}

		if(gVar.dio.di[DI_2].config.mode == DI_MODE_PULSE_CAPTURE && gVar.dio.di[DI_2].config.counterType == DI_COUNT_TYPE_TIMER ){
			gVar.dio.di[DI_2].pulseInf.countOVF++;
		}
	  }


//  HAL_TIM_IRQHandler(&htim3);

}




/////**
////  * @brief This function handles htim2 input capture interrupt.
////  */
////void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim)
////{
////	if(htim->Instance == TIM2){
////		TH_DICapturePulse(htim);
////	}
////}
////
/////**
////  * @brief  Period elapsed callback in non-blocking mode
////  * @param  htim TIM handle
////  * @retval None
////  */
////void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
////{
////	if(htim->Instance == TIM2){
////		TH_DICaptureOverflow(htim);
////	}
////}
//
//
///**
//  * @brief This function handles ADC1 global interrupt.
//  */
////void ADC_IRQHandler(void)
////{
////  /* USER CODE BEGIN ADC_IRQn 0 */
////
////  /* USER CODE END ADC_IRQn 0 */
////  HAL_ADC_IRQHandler(&hadc1);
////  /* USER CODE BEGIN ADC_IRQn 1 */
////
////  /* USER CODE END ADC_IRQn 1 */
////}
//
//
/////**
////  * @brief This function handles DMA2 stream4 global interrupt.
////  */
////void DMA2_Stream4_IRQHandler(void)
////{
////  /* USER CODE BEGIN DMA2_Stream4_IRQn 0 */
////
////  /* USER CODE END DMA2_Stream4_IRQn 0 */
////  HAL_DMA_IRQHandler(&hdma_adc1);
////  /* USER CODE BEGIN DMA2_Stream4_IRQn 1 */
////
////  /* USER CODE END DMA2_Stream4_IRQn 1 */
////}
//
///**
//  * @brief This function is the half convention callback for ADC DMA.
//  */
////void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
////{
////	__NOP();
////}
//
///**
//  * @brief This function is the complete convention callback for ADC DMA.
//  */
////void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
////{
////	adcConvF = 1;
////	  __NOP();
////}
///**
//  * @brief This function is the error callback for ADC DMA.
//  */
////void HAL_ADC_ErrorCallback(ADC_HandleTypeDef* hadc){
////	  DEBUG_SPRINT_NL("adc_err: %d",hadc->ErrorCode);
////
////}
//
//
//


/*I2C IRQs-----------------------------------------------------------------*/
/*I2C1 IRQs----------*/
/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{

  HAL_I2C_EV_IRQHandler(&hi2c1);
}


/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&hi2c1);
}


/**
  * @brief This function handles I2C1 error callback.
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c->Instance == I2C1){
		/*Process unlock*/
		PCA_UNLOCK(&gVar.ioExt);

		 DBG_SPRINT_NL("I2C1_Err: (%d)", hi2c->ErrorCode);
		 gVar.diag.i2c.ErrCode = hi2c->ErrorCode;
		 gVar.diag.i2c.dmaRxErrCode = hi2c->hdmarx->ErrorCode;
		 gVar.diag.i2c.dmaTxErrCode = hi2c->hdmatx->ErrorCode;
	}
}




/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_tx);
}

/**
  * @brief This function handles DMA2 stream3 global interrupt.
  */
void DMA2_Stream3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}


/**
  * @brief This function handles Master Tx callback interrupt.
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef* hi2c){
	if(hi2c->Instance == I2C1){
		/*Process unlock*/
		PCA_UNLOCK(&gVar.ioExt);
		gVar.ioExt.flag.writeCmplt = PCA_WRITE_COMPLETED;
//		DBG_SPRINT_NL("IOX_TXCPLT");
	}
}

/**
  * @brief This function handles Master rx callback interrupt.
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c){
	if(hi2c->Instance == I2C1){
		/*Process unlock*/
		PCA_UNLOCK(&gVar.ioExt);
		gVar.ioExt.flag.readCmplt = PCA_READ_COMPLETED;

	}
}


/**
  * @brief This function handles TIM24 global interrupt.
  */
void TIM24_IRQHandler(void)
{

//  HAL_TIM_IRQHandler(&htim24);
	DBG_SPRINT_NL("htim24");
	if ((htim24.Instance->SR & TIM_FLAG_UPDATE) == TIM_FLAG_UPDATE)
	  {
		htim24.Instance->SR = ~TIM_IT_UPDATE;//((0x1UL << (0U))));
		  TS_CatchOVF(&gVar.ts);
//		  gVar.ts.ovfCount++;
	  }
}



#endif

