/*
 * DIO_Handler.c
 *
 *  Created on: Mar 14, 2022
 *      Author: RASEL_EEE
 */

#include <DigitalIn.h>
#include "Tools.h"
#include "Debug.h"


TIM_HandleTypeDef *htim;
TIM_HandleTypeDef *htimDI;

void (*DI_StateChangecallback)(DigitalIn_t *di);
void (*DI_CounterExtiCallback)(uint16_t pin);



typedef struct DI_CounterByIntrpt{
	uint16_t pin;
	uint8_t lastState;
	DI_Trigger triger;
	uint32_t counter;
}DI_CounterByIntrpt;




/**
  * @brief  Initialize the digital input as normal input
  * @param  dinp: digital input struct
  * @retval none
  */
DI_Error DI_Init(DigitalIn_t *di){
	if(di == NULL) return DI_ERR_NULL_POINTER;

	di->state = GPIO_PIN_RESET;
	// configure gpio in digital input
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = di->config.pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(di->config.port, &GPIO_InitStruct);

	FLTR_ClearDebounce(&di->dbunce);

	return DI_ERR_NONE;
}

/**
  * @brief  configs the digital input mode to capture the pulse
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_ConfigPulsCapMode(DigitalIn_t *di){

	switch (di->config.counterType ) {
		case DI_COUNT_TYPE_TIMER:
//			DEBUG_SPRINT_NL("counterType: 2");
			DI_ConfigPulsCapChannel(di);
			break;
//		case DI_COUNT_TYPE_EXTI:{
//			GPIO_InitTypeDef GPIO_InitStruct = {0};
//	//		HAL_GPIO_DeInit(di->config.port, di->config.pin);
//			switch (di->config.trigMode) {
//				case DI_TRIG_MODE_RISING:
//					GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//					break;
//				case DI_TRIG_MODE_FALLING:
//					GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
//					break;
//				case DI_TRIG_MODE_RISING_FALLING:
//					GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
//					break;
//				default:				// for others
//
//					break;
//			}
//
//			HAL_NVIC_SetPriority(di->config.inturrptId, 0, 0);
//			HAL_NVIC_EnableIRQ(di->config.inturrptId);
//			GPIO_InitStruct.Pin = di->config.pin;
//			GPIO_InitStruct.Pull = GPIO_NOPULL;
//			HAL_GPIO_Init(di->config.port, &GPIO_InitStruct);
//
//		}break;
		default:
			break;
	}


}

/**
  * @brief  configs the digital input in normal mode
  * @param  dinp: digital input struct
  * @retval none
  */

void DI_ConfigNormalMode(DigitalIn_t *di){
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pin = di->config.pin;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(di->config.port, &GPIO_InitStruct);
}

/**
  * @brief  configs the digital input
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_Config(DigitalIn_t *di){
	/* GPIO Ports Clock Enable */
	switch (di->config.mode) {
		case DI_MODE_NORMAL:							// config the pin in normal io mode
			DI_ConfigNormalMode(di);
			break;
		case DI_MODE_PULSE_CAPTURE:
			DI_ConfigPulsCapMode(di);
			break;
		default:
			break;
	}
}


/**
  * @brief  Initiate the timer for pulse counter mode
  * @param  dinp: digital input struct
  * @retval none
  */
DI_Error DI_AttachTimer(TIM_HandleTypeDef *tim){
	if(tim == NULL) return DI_ERR_NULL_POINTER;
	htimDI = tim;
	return DI_ERR_NONE;
}



/**
  * @brief  Configure the timer for pulse counter mode
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_ConfigTimer(void){
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

	    /* TIM2 interrupt Init */
	    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 2);
	    HAL_NVIC_EnableIRQ(TIM2_IRQn);

	  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	  TIM_MasterConfigTypeDef sMasterConfig = {0};

	  htimDI->Instance = TIM2;
	  htimDI->Init.Prescaler = (HAL_RCC_GetHCLKFreq()/1000000);
	  htimDI->Init.CounterMode = TIM_COUNTERMODE_UP;
	  htimDI->Init.Period = 1000000-1;
	  htimDI->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htimDI->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(htimDI) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(htimDI, &sClockSourceConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  if (HAL_TIM_IC_Init(htimDI) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	  if (HAL_TIMEx_MasterConfigSynchronization(htimDI, &sMasterConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }

	  HAL_TIM_Base_Start_IT(htimDI);

//	  DEBUG_SPRINT_NL("ConfTim htimDI: %p", htimDI);
}

void DI_ConfigPulsCapChannel(DigitalIn_t *di){

	DI_ConfigTimer();

	TIM_IC_InitTypeDef sConfigIC = {0};

    HAL_TIM_IC_Stop_IT(htimDI,  di->config.channel);
	switch (di->config.trigMode) {
		case DI_TRIG_MODE_RISING:
//			DEBUG_SPRINT_NL("trigMode: 2");
			sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			break;
		case DI_TRIG_MODE_FALLING:
			sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
			break;
		case DI_TRIG_MODE_RISING_FALLING:
			sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
			break;
		default:
			sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
			di->config.trigMode = DI_TRIG_MODE_RISING;
			break;
	}

	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	if (HAL_TIM_IC_ConfigChannel(htimDI, &sConfigIC, di->config.channel) != HAL_OK)
	{
	Error_Handler();
	}
	 HAL_TIM_IC_Start_IT(htimDI,  di->config.channel);
	 di->pulseInf.countOVF = 0;
//	 DEBUG_SPRINT_NL("Conf Count htimDI: %p", htimDI);
}



void DI_CalculatePulse(DigitalIn_t *di){
	if(di->pulseInf.isEdgeDtctd > 0){
		di->pulseInf.isEdgeDtctd = 0;
		uint16_t ovf = 0;
		if(di->pulseInf.edgeTime < di->pulseInf.lastEdgeTime || di->pulseInf.countOVF>0){

			di->pulseInf.period = ((double)(((htimDI->Init.Period+1)*di->pulseInf.countOVF)  + di->pulseInf.edgeTime - di->pulseInf.lastEdgeTime));///(double)(htimDI->Init.Period+1)

//			DEBUG_SPRINT_NL("Overflow: %d",di->pulseInf.countOVF);
			ovf = di->pulseInf.countOVF;
			di->pulseInf.countOVF = 0;
		}else{
			di->pulseInf.period = ((double)(di->pulseInf.edgeTime - di->pulseInf.lastEdgeTime));///(double)(htimDI->Init.Period+1)
		}
//		DEBUG_SPRINT_NL("DI_%d: { ET0: %luus, ET1: %luus, PRD: %luus, OVF: %d }",di->index, di->pulseInf.lastEdgeTime, di->pulseInf.edgeTime,  di->pulseInf.period, ovf);
		di->pulseInf.period = ((double)di->pulseInf.period/(double)(htimDI->Init.Period+1))*1000.0f;
	}
}



/**
  * @brief  Reads the digital input
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_DetectEdge(DigitalIn_t *di){
	if(di->state > di->lastState){				// here state is the last state & final state is the present state
		di->trigger = DI_TRIG_RISING;
		di->isTriggered = 1U;
	}else if(di->state < di->lastState){
		di->trigger = DI_TRIG_FALLING;
		di->isTriggered = 1U;
	}
}


/**
  * @brief  Reads the digital input
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_Read(DigitalIn_t *di){

	/*Read input*/
	di->dbunce.currentState = DI_READ_PIN(di->config.port, di->config.pin);				// ADDED: instead of HAL_GPIO_ReadPin() to reduce the process time
	/*Apply debounce filter*/
	while(di->dbunce.counter <= di->dbunce.countMax && di->dbunce.finalState != di->dbunce.currentState){
		di->dbunce.currentState = DI_READ_PIN(di->config.port, di->config.pin);
		FLTR_ApplyDebounce(&di->dbunce);													// use debounce filter
	}
	/*Detect edge*/
	di->state = di->dbunce.finalState;
	DI_DetectEdge(di); 						// here state is the last state & final state is the present state
//	if(di->lastState != di->state && di->config.mode == DI_MODE_NORMAL){
//		DI_StateChangecallback(di);
//	}

	di->lastState = di->state;
}

/**
  * @brief  counter for the digital inputs, when inputs are set in counter mode
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_Counter(DigitalIn_t *di){
	/* 1. check the counter type
	 * 2. check the counter trigger mode
	 * 3. check the up/down counter
	 * */
	//1> checking of counter type
	if(di->config.counterType == DI_COUNT_TYPE_NORMAL){										//counter in normal mode
		DI_Read(di);
		//2. checking the  counter trigger mode
		if(di->isTriggered){
			// check the trigger mode
			switch (di->config.trigMode) {
				case DI_TRIG_MODE_RISING:														// counter will count on rising edge trigger
//					DEBUG_SPRINT("\r\nRising");
					if(di->trigger == DI_TRIG_RISING)
						(di->config.countMode == DI_COUNT_MODE_UP ? di->count++ : di->count--);
					break;
				case DI_TRIG_MODE_FALLING:														// counter will count on falling edge trigger
					if(di->trigger == DI_TRIG_FALLING)
						(di->config.countMode == DI_COUNT_MODE_UP ? di->count++ : di->count--);
					break;
				case DI_TRIG_MODE_RISING_FALLING:												// counter will count on rising & falling edge trigger
					di->config.countMode == DI_COUNT_MODE_UP ? di->count++ : di->count--;
					break;
				default:
					break;
			}
			di->isTriggered = 0;
		}
	}else if (di->config.counterType == DI_COUNT_TYPE_EXTI){					//counter in interrupt mode
		if (di->dbunce.isInterrupt > 0U) {

			//TODO: have to test deeply that debounce filter is working or properly or not in interrupt mode

			di->dbunce.currentState = HAL_GPIO_ReadPin(di->config.port, di->config.pin);					// read the digital inputs
//
			FLTR_ApplyDebounceExti(&di->dbunce);													// use debounce filter
			di->state = di->dbunce.finalState;
			DI_DetectEdge(di);
			di->lastState = di->state;
			if(di->dbunce.isInterrupt == 0U){										// isInterrupt is reseting in the FLTR_ApplyDebounceExti()  function
				if(di->isTriggered){
					// check the trigger mode
//					switch (di->config.trigMode) {
//						case DI_TRIG_MODE_RISING:														// counter will count on rising edge trigger

							if(di->trigger == DI_TRIG_RISING)
								(di->config.countMode == DI_COUNT_MODE_UP ? di->count++ : di->count--);
//							break;
//						case DI_TRIG_MODE_FALLING:														// counter will count on falling edge trigger
							if(di->trigger == DI_TRIG_FALLING)
								(di->config.countMode == DI_COUNT_MODE_UP ? di->count++ : di->count--);
//							break;
//						case DI_TRIG_MODE_RISING_FALLING:												// counter will count on rising & falling edge trigger
//							di->config.countMode == DI_COUNT_MODE_UP ? di->counter++ : di->counter--;
//							break;
//						default:
//							break;
//					}
					di->isTriggered = 0;
				}
			}
		}
	}else if(di->config.counterType == DI_COUNT_TYPE_TIMER){
		DI_CalculatePulse(di);
	}
}


void DI_AttachStateChangeCallback(void (*callbackFunc)(DigitalIn_t *di)){
	DI_StateChangecallback = callbackFunc;
}


void DI_AttachCounterExtiCallback(void (*callbackFunc)(uint16_t pin)){
	DI_CounterExtiCallback = callbackFunc;
}


/**
  * @brief  find counter by the pin number
  */
//uint8_t DI_FindCounterByPin(uint16_t pin){
//	for(DI_Index i = (DI_1-1); i < (DI_MAX-DI_1); i++){
//		if(gVar.di[i].config.pin == pin){
//			return i;
//		}
//	}
//	return DI_MAX;
//}

/**
  * @brief  counter for the digital input using external interrupt
  * @param  dinp: digital input struct
  * @retval none
  */
void DI_CounterByExtI(DigitalIn_t *di){
	// check the trigger mode
//	di->dbunce.currentState = HAL_GPIO_ReadPin(di->config.port, di->config.pin);					// read the digital inputs
//	FLTR_ApplyDebounce(&di->dbunce);													// use debounce filter
//	di->state = di->dbunce.finalState;
	FLTR_StartDebounceExti(&di->dbunce);
//	(di->config.countMode == DI_COUNT_MODE_UP ? di->counter++ : di->counter--);

}

/**
  * @brief  Counter callback for external interrupt
  * @param  GPIO_Pin: gpio pin for the external interrupt
  * @retval none
  */


/*Digital Input End -----------------------*/

// arrays for digital inputs, outputs and relays
//uint16_t DIO_dinp_arr[DIO_DINP_MAX];
//uint16_t DIO_dout_arr[DIO_DOUT_MAX];
//uint16_t DIO_relay_arr[DIO_RLY_MAX];
//


/**
  * @brief  initializes for IOs
  * @param  none
  * @retval none
  */
//void DIO_Init(DigitalOut_t *dio){
////	for(uint8_t i = DIO_DOUT_1; i < DIO_DOUT_MAX; i++){
//	//	HAL_GPIO_WritePin(dio->outputs[i].port, dio->outputs[i].pin, GPIO_PIN_RESET);
////	}
//}

//
//
///**
//  * @brief  Handles all the IO related functions
//  * @param  none
//  * @retval none
//  */
//void DIO_Handler(DIO *dio){
////	for(uint8_t inpIndex = DIO_DINP_1; inpIndex < DIO_DINP_MAX; inpIndex++){
////		DIO_Input *dioInput;
////		DIO_Output *dioOutput;
////		dioInput = &dio->dioInputs[inpIndex];
////		if(dioInput->output_index != DIO_DOUT_NONE){
////			dioOutput = &dio->dioOutputs[dioInput->output_index];
//
////			if(!dioInput->state){
////				dioInput->state = HAL_GPIO_ReadPin(dioInput->port, dioInput->pin);
////				if(dioInput->state){
////					dioOutput->state = (dioOutput->state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET);
////				}
////			}else{
////				dioInput->state = HAL_GPIO_ReadPin(dioInput->port, dioInput->pin);
////			}
////		}
////	}
//
//	//controls the dout and relays
//	// dout of board is logically inverse to mcu out pin
//	for(uint8_t outIndex = DIO_DOUT_1; outIndex < DIO_DOUT_MAX; outIndex++){
//		DIO_Input *dioInput;
//		DIO_Output *dioOutput;
//		dioOutput = &dio->dioOutputs[outIndex];
//
//		switch (dioOutput->source) {
//			case DIO_SRC_DINP_1:
//				dioInput = &dio->dioInputs[DIO_DINP_1];
//				DIO_TieInpOutOperation(dioInput, dioOutput);
//				break;
//			case DIO_SRC_DINP_2:
//				dioInput = &dio->dioInputs[DIO_DINP_2];
//				DIO_TieInpOutOperation(dioInput, dioOutput);
//				break;
//			case DIO_SRC_DINP_3:
//				dioInput = &dio->dioInputs[DIO_DINP_3];
//				DIO_TieInpOutOperation(dioInput, dioOutput);
//				break;
//			case DIO_SRC_DINP_4:
//				dioInput = &dio->dioInputs[DIO_DINP_4];
//				DIO_TieInpOutOperation(dioInput, dioOutput);
//				break;
//			case DIO_SRC_TRIAC1_ERR_STATUS:
//
//				break;
//			case DIO_SRC_TRIAC2_ERR_STATUS:
//
//				break;
//			default:
//				dioOutput->state = GPIO_PIN_RESET;
//				break;
//		}
//
//
//			HAL_GPIO_WritePin(dioOutput->port, dioOutput->pin, (outIndex <= DIO_DOUT_4 ? !dioOutput->state : dioOutput->state));
//
//	}
//}


//void DIO_TieInpOutOperation(DigitalIn_t *dinp, DigitalOut_t *dout){
////	if(dout->mode == DIO_OM_FOLLOW){			// if output mode is follow to the input
////			dout->state = HAL_GPIO_ReadPin(dinp->port, dinp->pin);
////	}else if(dout->mode == DIO_OM_TOGGLE){										// if output mode is toggle to the input
////		if(!dinp->state){
////			dinp->state = HAL_GPIO_ReadPin(dinp->port, dinp->pin);
////			if(dinp->state){
////				dout->state = (dout->state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET);
////			}
////		}else{
////			dinp->state = HAL_GPIO_ReadPin(dinp->port, dinp->pin);
////		}
////	}
//
////	dinp->dBunce.currentState = HAL_GPIO_ReadPin(dinp->port, dinp->pin);
////	FLTR_ApplyDebounce(&dinp->dBunce);
////	dinp->state = dinp->dBunce.finalState;
//	switch (dout->mode) {
//		case DO_MODE_FOLLOW:
//			dout->state = dinp->state;
//			break;
//		case DO_MODE_TOGGLE:
//			//DEBUG_PRINT("Dinp-2\r\n");
//			if(dinp->state&& dout->isToggle == 0U){
//
//				dout->isToggle = 1U;
//				dout->state = (dout->state == GPIO_PIN_RESET ? GPIO_PIN_SET : GPIO_PIN_RESET);
//			}else if(!dinp->state){
//				dout->isToggle = 0U;
//			}
//			break;
//		default:
//			break;
//	}
//}
//



/**
  * @brief  reads the digital input status
  * @param  port: GPIO port for the digital input pin
  * @param  pin: pin for the digital input
  * @retval relay pin state
  */
void DIO_ReadDinp(DigitalIn_t *dinp){


//	dinp->dBunce.currentState = HAL_GPIO_ReadPin(dinp->port, dinp->pin);
//	FLTR_ApplyDebounce(&dinp->dBunce);
//


//	return HAL_GPIO_ReadPin(port, pin);
}






/**
  * @brief  write the relay status
  * @param  port: GPIO port for the relay pin
  * @param  pin: pin for the relay
  * @param  state: state to be set
  * @retval none
  */
void DIO_WriteRelay(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state){
	HAL_GPIO_WritePin(port, pin, state);
}

/**
  * @brief  reads the relay status
  * @param  port: GPIO port for the relay pin
  * @param  pin: pin for the relay
  * @retval relay pin state
  */
GPIO_PinState DIO_ReadRelay(GPIO_TypeDef *port, uint16_t pin){
	return HAL_GPIO_ReadPin(port, pin);
}


/**
  * @brief  write the digital output status
  * @param  port: GPIO port for the digital output pin
  * @param  pin: pin for the digital output
  * @param  state: state to be set
  * @retval none
  */
void DIO_WriteDout(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state){
	HAL_GPIO_WritePin(port, pin, state);
}

/**
  * @brief  reads the digital output status
  * @param  port: GPIO port for the digital output pin
  * @param  pin: pin for the digital output
  * @retval relay pin state
  */
GPIO_PinState DIO_ReadDout(GPIO_TypeDef *port, uint16_t pin){
	return HAL_GPIO_ReadPin(port, pin);
}




