/*
 * l6360.c
 *
 *  Created on: Jan 16, 2022
 *      Author: wsrra
 */

#include "l6360.h"
#include "Debug.h"
#include "string.h"
#include "IOLink_PL.h"
#include "IOLink_Stm32Hw.h"


#define BITWISE_XOR(x) 			((x&0x01)^((x>>1)&0x01)^((x>>2)&0x01)^((x>>3)&0x01)^((x>>4)&0x01)^((x>>5)&0x01)^((x>>6)&0x01)^((x>>7)&0x01))
#define BITWISE_XOR_ODD(x) 		(((x>>1)&0x01)^((x>>3)&0x01)^((x>>5)&0x01)^((x>>7)&0x01))
#define BITWISE_XOR_EVEN(x) 	((x&0x01)^((x>>2)&0x01)^((x>>4)&0x01)^((x>>6)&0x01))

#define CHECK_NULL_POINTER(ptr) (ptr == NULL)

/*Macro functions.....................*/



/*Calculate parity-------------------*/
inline static uint8_t L6360_I2CCalcSingleWriteParity(uint8_t data);
inline static uint8_t L6360_I2CCalcSeqlWriteParity(uint8_t* data);

/*Timer related services-------------------------------*/
uint64_t (*getTime_us)(void);
inline static uint64_t GetTimeUS(void);
inline static uint64_t GetTimeMS(void);
inline static void Delay_US(uint64_t imeout);
inline static void Delay_MS(uint64_t imeout);

static inline L6360_Status ConvertError(uint16_t error);


/*Services for l6360 register-------------*/
/* get status resister*/
int16_t L6360_GetSR(L6360_IOLink_hw_t* l6360){
	L6360_I2CReadStatusParity(l6360, &l6360->reg.sr.value, &l6360->reg.pr.value);
	//TODO: to handle the error
	return l6360->reg.sr.value;
}

/* get Configuration resister*/
int16_t L6360_GetCNFR(L6360_IOLink_hw_t* l6360){
	L6360_I2CRead(l6360, l6360->i2c.devAddress, L6360_I2C_REG_ADDR_CONFIG, &l6360->reg.cnfr.value, 1);
	//TODO: to handle the error
	return l6360->reg.cnfr.value;
}
/* get Control resister 1*/
int16_t L6360_GetCR1(L6360_IOLink_hw_t* l6360){
	return L6360_OK;
}
/* get Control resister 2*/
int16_t L6360_GetCR2(L6360_IOLink_hw_t* l6360){
	return L6360_OK;
}
/* get LED 1 resister (2-byte)*/
int32_t L6360_GetLED1(L6360_IOLink_hw_t* l6360){
	return L6360_OK;
}
/* get LED 2 resister (2-byte)*/
int32_t L6360_GetLED2(L6360_IOLink_hw_t* l6360){
	return L6360_OK;
}
/* get parity resister*/
int16_t L6360_GetPR(L6360_IOLink_hw_t* l6360){
	return L6360_OK;
}


/**
  * @brief  set configuration register value
  * @param  l6360: pointer to the structure
  * 		value: register value
  * @retval error status
  */
L6360_Status L6360_SetCNFR(L6360_IOLink_hw_t* l6360, L6360_CNFRcq value){
	if(value >= L6360_CNFR_CQ_MAX){
		return L6360_VALUE_OUT_OF_RANGE;
	}
	L6360_ConfigReg_t cnfr = {0};
	cnfr.bf.cq = value;
	DBG_SPRINT_NL("i2c_ptr: %p, devAddr: %02X",l6360->i2c.hi2c, l6360->i2c.devAddress, cnfr.value);
	L6360_Status status = L6360_I2CWriteSingle(l6360, l6360->i2c.devAddress,
							L6360_I2C_REG_ADDR_CONFIG, cnfr.value);
	if(status == L6360_OK){
		l6360->reg.cnfr = cnfr;
	}
	return status;

}

/* set Control resister 1*/
L6360_Status L6360_SetCR1(L6360_IOLink_hw_t* l6360, uint8_t value){
	return L6360_OK;
}
/* set Control resister 2*/
L6360_Status L6360_SetCR2(L6360_IOLink_hw_t* l6360, uint8_t value){
	return L6360_OK;
}
/* set LED 1 resister (2-byte)*/
L6360_Status L6360_SetLED1(L6360_IOLink_hw_t* l6360, uint16_t value){
	return L6360_OK;
}
/* set LED 2 resister (2-byte)*/
L6360_Status L6360_SetLED2(L6360_IOLink_hw_t* l6360, uint16_t value){
	return L6360_OK;
}

/*I2C functions--------------------------*/
/**
  * @brief  I2c init
  * @param  i2c pointer
  * @retval nonr
  */
void L6360_I2CInit(I2C_HandleTypeDef* i2c){

	if(HAL_I2C_IsDeviceReady(i2c, L6360_I2C_DEV_ADDR_1, 3, 100) == HAL_OK){
		DBG_SPRINT_NL("i2c ok");
	}else{
		DBG_SPRINT_NL("i2c not ok");
	}

//	l6360_i2cReadByte(L6360_I2C_DEV_ADDR_1, L6360_I2C_REG_ADDR_STATUS+1, data);
//	DBG_SPRINT_NL("i2c_data: %d, %d",data[0],data[1]);
}


/**
  * @brief  reads status register
  * @param  l6360: pointer to the struct
  * @retval error status/ or value
  */
L6360_Status L6360_I2CReadStatusParity(L6360_IOLink_hw_t* l6360, uint8_t *status, uint8_t* parity){
	L6360_Status statusRet = L6360_OK;
	uint8_t data[2] = {0};
	uint8_t devAddress = l6360->i2c.devAddress;
	data[0] = L6360_I2C_REG_ADDR_STATUS;

	if(CHECK_NULL_POINTER(l6360) || CHECK_NULL_POINTER(status)|| CHECK_NULL_POINTER(parity) || CHECK_NULL_POINTER(l6360->i2c.hi2c)){
		return L6360_NULL_PTR_PARAM;
	}else if(!L6360_CHECK_I2C_ADDR(devAddress)){
		return L6360_INVALID_DEV_ADDRESS;
	}

	/*Check and lock the process*/
	L6360_LOCK(l6360->i2c);

	/*send register address 1st*/
#ifdef L6360_MCU_STM32XXX
	HAL_StatusTypeDef errStatus = HAL_I2C_Master_Transmit(l6360->i2c.hi2c, devAddress ,  data, 1, 2);
	if(errStatus == HAL_OK){
		/*receive data*/
		data[0] = 0;
		errStatus = HAL_I2C_Master_Receive(l6360->i2c.hi2c, (devAddress | L6360_RW_MODE_READ), data, 2, 3);
	}
	if(errStatus != HAL_OK){
		statusRet = ConvertError(errStatus);
	}

#else
	statusRet = l6360->i2c.write_cb(devAddress ,  data, 1);
	if(statusRet == L6360_OK){
		/*receive data*/
		data[0] = 0;
		statusRet = l6360->i2c.read_cb((devAddress | L6360_RW_MODE_READ), data, 2);
	}
#endif

	/*Set received data*/
	if(statusRet == L6360_OK){
		*status = data[0];
		*parity = data[1];
	}
	/*Process unlock*/
	L6360_UNLOCK(l6360->i2c);

	return statusRet;
}


/**
  * @brief  Write current(single) register value
  * @param  devAddr: device address
  * 		regAddr: register address
  * 		data: data pointer
  * @retval routine status
  */
L6360_Status L6360_I2CWriteSingle(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t regAddr, uint8_t data){
	if(CHECK_NULL_POINTER(l6360)){
		return L6360_NULL_PTR_PARAM;
	}else if(!L6360_CHECK_I2C_ADDR(devAddr)){
		return L6360_INVALID_DEV_ADDRESS;
	}else if(regAddr >= L6360_I2C_REG_ADDR_MAX){
		return L6360_INVALID_REG_ADDRESS;
	}
	L6360_LOCK(l6360->i2c);
	L6360_Status statusRet = L6360_OK;
	uint8_t buff[2] = {0};						// minus 1: to exclude status register (read only)
	if(l6360->i2c.wRetry < L6360_MAX_WRITE_RETRY) l6360->i2c.wRetry++;
	else{
		//TODO: to implement later
	}
	/*For single register write*/
	buff[0] = data;
	buff[1] = (L6360_I2CCalcSingleWriteParity(buff[0]) | regAddr);
	/*Send write data*/
	devAddr &= (L6360_RW_MODE_WRITE|0xFE);
#ifdef L6360_MCU_STM32XXX
	HAL_StatusTypeDef errStatus = HAL_I2C_Master_Transmit(l6360->i2c.hi2c, devAddr ,  buff, 2, 3);
	if(errStatus != HAL_OK){
		statusRet = ConvertError(errStatus);
	}
#else
	statusRet = l6360->i2c.write_cb(devAddr, buff, 2);
#endif
	L6360_UNLOCK(l6360->i2c);
	return statusRet;
}


/**
  * @brief  Write Sequential(All) register value
  * @param  devAddr: device address
  * 		regAddr: register address
  * 		data: data pointer
  * @retval routine status
  */
L6360_Status L6360_I2CWriteAll(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t* data){
	L6360_LOCK(l6360->i2c);
	L6360_Status statusRet = L6360_OK;
	uint8_t buff[L6360_I2C_REG_ADDR_MAX-1] = {0};						// minus 1: to exclude status register (read only)
	uint8_t len = L6360_I2C_REG_ADDR_MAX-2;
	/*copy data to temp buffer*/
	memcpy(buff, data, len);
	/*calculate and set the parity*/
	buff[len++] = L6360_I2CCalcSeqlWriteParity(buff);
	/*Send write data*/
	devAddr &= (L6360_RW_MODE_WRITE|0xFE);
#ifdef L6360_MCU_STM32XXX
	HAL_StatusTypeDef errStatus = HAL_I2C_Master_Transmit(l6360->i2c.hi2c, devAddr ,  buff, len, (len+1));
	if(errStatus != HAL_OK){
		statusRet = ConvertError(errStatus);
	}
#else
	statusRet = l6360->i2c.write_cb(devAddr, buff, len);
#endif
	L6360_UNLOCK(l6360->i2c);
	return statusRet;
}


/**
  * @brief  read current(single) register value
  * @param  devAddr: device address
  * 		regAddr: register address
  * 		data: data pointer
  * @retval routine status
  */
L6360_Status L6360_I2CRead(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len){
	L6360_LOCK(l6360->i2c);
	L6360_Status status = L6360_OK;
	uint8_t txBuf[1] ={0};
	txBuf[0] = regAddr;
	/*send start register address 1st*/
#ifdef L6360_MCU_STM32XXX
	HAL_StatusTypeDef errStatus = HAL_I2C_Master_Transmit(l6360->i2c.hi2c, devAddr ,  txBuf, 1, 2);
	if(errStatus == HAL_OK){
		devAddr |= L6360_RW_MODE_READ;
		errStatus = HAL_I2C_Master_Receive(l6360->i2c.hi2c, devAddr, data, len, len+1);
	}
	if(errStatus != HAL_OK){
		status = ConvertError(errStatus);
	}
#else
	status = l6360->i2c.write_cb(devAddr, txBuf, 1);
	/*receive data*/
	if(status == L6360_OK){
		devAddr |= L6360_RW_MODE_READ;
		status = l6360->i2c.read_cb(devAddr, data, len);
	}
#endif
	L6360_UNLOCK(l6360->i2c);
	return status;
}


/**
  * @brief  Calculate parity for single register write
  * @param  data: 1 byte data
  * @retval calculated parity
  */
inline static uint8_t L6360_I2CCalcSingleWriteParity(uint8_t data)
{
	uint8_t P0,P1,P2=0;
	uint8_t parity=0;
	/*Calculate parity*/
	P0 = BITWISE_XOR(data);
	P1 = BITWISE_XOR_ODD(data);
	P2 = BITWISE_XOR_EVEN(data);;

	parity = ((P0|(P1<<1)|(P2<<2))<< 5);
	return parity;
}

/**
  * @brief  Calculate parity for all register write
  * @param  data: 1 byte data
  * @retval calculated parity
  */
inline static uint8_t L6360_I2CCalcSeqlWriteParity(uint8_t* data)
{
	uint8_t P0;
	uint8_t parity=0;

	for(uint8_t i = 0; i < (L6360_I2C_REG_ADDR_MAX-2); i++){	// max address - 2 : to exclude status & parity register
		P0 = BITWISE_XOR(data[i]);
		parity |= (P0<<(6-i));
	}
	return parity;
}

/**
  * @brief  Handle the interrupt
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_I2CIRQHandler(L6360_IOLink_hw_t* l6360){
	L6360_I2CReadStatusParity(l6360, &l6360->reg.sr.value, &l6360->reg.pr.value);
	/* TODO: If it is needed to implement retry for write operation according to the
	 * error by checking status and parity register. We have to implement here.
	 * */
}

/**
  * @brief  Attach i2c write callback
  * @param  l6360: l6360 structure pointer
  * 		devAddr: device address
  * 		regAddr: register address
  * 		data: data pointer
  * 		len: data length
  * @retval none
  */
void L6360_I2CAttachWriteCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (uint8_t devAddress, uint8_t* data, uint16_t len)){
//#ifdef L6360_MCU_STM32XXX
	l6360->i2c.write_cb = callback;
//#endif
}

/**
  * @brief  Attach i2c read callback
  * @param  l6360: l6360 structure pointer
  * 		devAddr: device address
  * 		regAddr: register address
  * 		data: data pointer
  * 		len: data length
  * @retval none
  */
void L6360_I2CAttachReadCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (uint8_t devAddress, uint8_t* data, uint16_t len)){
//#ifdef L6360_MCU_STM32XXX
	l6360->i2c.read_cb = callback;
//#endif
}


/*Services for Uart port----------------------------------*/
#ifdef L6360_MCU_STM32XXX
/** @brief	UART init service
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
L6360_Status L6360_UartInit( L6360_IOLink_hw_t* l6360, uint32_t baudrate){
	/*Check baudrate*/
	l6360->uart.baudrate = baudrate;
#ifdef L6360_MCU_STM32XXX
	HAL_StatusTypeDef status = IOL_STM32_UartInit( l6360, l6360->portNum, baudrate );
//	HAL_UART_Receive_IT (l6360->uart.huart,l6360->uart.rxBuf, 2);
//	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_FLAG_RXNE);

	IOL_STM32_UartInitDMA(l6360);
	HAL_UARTEx_ReceiveToIdle_DMA(l6360->uart.huart, l6360->uart.rxBuf, 256);
	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_IT_IDLE);
	__HAL_DMA_DISABLE_IT(l6360->uart.huart->hdmarx, DMA_IT_HT);

//	__HAL_UART_DISABLE_IT(l6360->uart.huart, UART_IT_ORE);
//	 HAL_UARTEx_ReceiveToIdle_IT(l6360->uart.huart, l6360->uart.rxBuf, 2);
//		 HAL_UART_Receive_IT(l6360->uart.huart, l6360->uart.rxBuf, 2);
//	 __HAL_UART_ENABLE_IT(l6360->uart.huart, UART_IT_RXNE);
//	 __HAL_DMA_DISABLE_IT(l6360->uart.huart, UART_DMA_);
//	MX_DMA_Init();//TODO: to replace
#endif
	if ( status != HAL_OK){
		return ConvertError(status);
	}
//	HAL_UARTEx_ReceiveToIdle_IT(l6360->uart.huart, l6360->uart.rxBuf, 256);
//	__HAL_UART_DISABLE_IT(l6360->uart.huart, UART_H)
	return L6360_OK;
}



/** @brief  set uart baudrate
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
L6360_Status L6360_UartSetBaudrate(L6360_IOLink_hw_t* l6360, uint32_t baudrate){
	if(!L6360_UART_CHECK_BR(baudrate)){
		return L6360_INVALID_UART_PARAM;
	}
	l6360->uart.baudrate = baudrate;
#ifdef L6360_MCU_STM32XXX
	l6360->uart.huart->Init.BaudRate = baudrate;
	HAL_StatusTypeDef status  = HAL_UART_Init(l6360->uart.huart);
//	HAL_UART_Receive_IT (l6360->uart.huart,l6360->uart.rxBuf, 2);
//	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_FLAG_RXNE);

	IOL_STM32_UartInitDMA(l6360);
	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_FLAG_TC);
	__HAL_UART_ENABLE_IT(l6360->uart.huart, UART_IT_IDLE);
	HAL_UARTEx_ReceiveToIdle_DMA(l6360->uart.huart, l6360->uart.rxBuf, 256);
	__HAL_DMA_DISABLE_IT(l6360->uart.huart->hdmarx, DMA_IT_HT);

#endif
	if ( status != HAL_OK){
		return ConvertError(status);
	}

	return L6360_OK;
}

#endif


/** @brief  get uart baudrate
  * @param  l6360: l6360 structure pointer
  * @retval error status or baud rate
  */
int32_t L6360_UartGetBaudrate(L6360_IOLink_hw_t* l6360){
	uint32_t br;
	L6360_Status status = l6360->uart.config_cb(l6360, L6360_RW_MODE_READ, &br);
	if( status == L6360_OK){
		return br;
	}else{
		return (int32_t)status;
	}

}

/** @brief  UART Receive service
  * @param  l6360: l6360 structure pointer
  * @retval error status
  */
L6360_Status L6360_UartSend(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len){
	if(len <= 0){
		return L6360_INVALID_LEN;
	}
	L6360_CqEnable(l6360);
	L6360_Status status = L6360_OK;
	uint32_t timeout = L6360_UART_CALC_TX_TIME(l6360->uart.huart->Init.BaudRate, len) + L6360_UART_TX_TIME_OFFSET;
#ifdef L6360_MCU_STM32XXX
//	HAL_StatusTypeDef errStatus = HAL_UART_Transmit(l6360->uart.huart, data, len, timeout);
//	HAL_StatusTypeDef errStatus = HAL_UART_Transmit_IT(l6360->uart.huart, data, len);//, timeout);
	HAL_StatusTypeDef errStatus = HAL_UART_Transmit_DMA(l6360->uart.huart, data, len);//, timeout);
	if ( errStatus != HAL_OK){
		status = ConvertError(errStatus);
	}
//	DBG_SPRINT("\r\nUart_Data[%d]:%02X,%02X, br: %u",len, data[0], data[1], l6360->uart.huart->Init.BaudRate);
#else
	status = l6360->uart.tx_cb(l6360, data, len, timeout);
#endif
//	L6360_CqDisable(l6360);
	l6360->uart.rxLen = 0;
	return status;

}



/** @brief	UART Receive service
  * This service is used only for the polling implementation
  * @param  l6360: l6360 structure pointer
  * @retval error status
  */
L6360_Status L6360_UartRecv(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len){
	if(len <= 0){
		return L6360_INVALID_LEN;
	}
	L6360_Status status = L6360_OK;
//	l6360->uart.rxBuf[l6360->uart.rxLen++] = *data;
//	DBG_SPRINT_NL("RX:%d", *data);
	return status;
}

/** @brief	TX complete interrupt handler. Call this function when TX complete interrupt occur.
  * @param  l6360: l6360 structure pointer
  * @retval error status
  */
void L6360_UartTxcIRQ(L6360_IOLink_hw_t* l6360){

	L6360_CqDisable(l6360);
//	DBG_SPRINT_NL("UART_TXC:");
}

/** @brief	RX complete interrupt handler. Call this function when RX complete interrupt occur.
  * @param  l6360: l6360 structure pointer
  * @retval error status
  */
void L6360_UartRxcIRQ(L6360_IOLink_hw_t* l6360, uint8_t *data, uint16_t len){
	l6360->uart.rxLen = len;
//	HAL_UARTEx_ReceiveToIdle_DMA(l6360->uart.huart, l6360->uart.rxBuf, 256);
//	__HAL_DMA_DISABLE_IT(l6360->uart.huart->hdmarx, DMA_IT_HT);
//	__HAL_UART_DISABLE_IT(l6360->uart.huart, UART_IT_ORE);

//	HAL_UARTEx_ReceiveToIdle_IT(l6360->uart.huart, l6360->uart.rxBuf, 256);
	DBG_SPRINT_NL("UART_RX[%d]:%02X,%02X",len, l6360->uart.rxBuf[0], l6360->uart.rxBuf[1]);
//	while(1){};
}

/** @brief	Attach uart configuration callback function
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_UartAttachConfigCallback( L6360_IOLink_hw_t* l6360,
		L6360_Status (*callback)(L6360_IOLink_hw_t* l6360,
				L6360_RWMode rwMode, uint32_t* baudrate))
{
	l6360->uart.config_cb = callback;
}

/** @brief	Attach uart transmission callback function
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_UartAttachTXCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (L6360_IOLink_hw_t* l6360,uint8_t* data, uint16_t len, uint32_t timeout)){
	l6360->uart.tx_cb = callback;
}

/** @brief	Attach uart receive callback function
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_UartAttachRXCallback( L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (L6360_IOLink_hw_t* l6360,uint8_t* data, uint16_t len, uint32_t timeout)){
	l6360->uart.rx_cb = callback;
}

/*L6360 Control pins--------------------------*/

/**
  * @brief  Enable the ENL+  gpio pin
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_LplusEnable(L6360_IOLink_hw_t* l6360){

#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->ctrlPins.lpEnPort, l6360->ctrlPins.lpEnPin, GPIO_PIN_SET);
#else
	l6360->ctrlPins.enLplus(l6360, L6360_PIN_SET);
#endif
}


/**
  * @brief  Disable the ENL+ gpio pin
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_LplusDisable(L6360_IOLink_hw_t* l6360){
#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->ctrlPins.lpEnPort, l6360->ctrlPins.lpEnPin, GPIO_PIN_RESET);
#else
	l6360->ctrlPins.enLplus(l6360, L6360_PIN_RESET);
#endif
}


/**
  * @brief  Enable the EN_C/Q gpio pin
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_CqEnable(L6360_IOLink_hw_t* l6360){
#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->ctrlPins.cqEnPort, l6360->ctrlPins.cqEnPin, GPIO_PIN_SET);
#else
	l6360->ctrlPins.encq(l6360, L6360_PIN_SET);
#endif
}

/**
  * @brief  Disable the EN_C/Q  gpio pin
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_CqDisable(L6360_IOLink_hw_t* l6360){
#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->ctrlPins.cqEnPort, l6360->ctrlPins.cqEnPin, GPIO_PIN_RESET);
#else
	l6360->ctrlPins.encq(l6360, L6360_PIN_RESET);
#endif
}

/**
  * @brief  Reset L6360 IC
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
void L6360_Reset(L6360_IOLink_hw_t* l6360){
#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->ctrlPins.rstPort, l6360->ctrlPins.rstPin, GPIO_PIN_RESET);
	Delay_MS(1);
	HAL_GPIO_WritePin(l6360->ctrlPins.rstPort, l6360->ctrlPins.rstPin, GPIO_PIN_SET);
#else
	l6360->ctrlPins.rst(l6360, L6360_PIN_RESET);
	Delay_MS(1);
	l6360->ctrlPins.rst(l6360, L6360_PIN_SET);
#endif
}

/**
  * @brief  Attach L plus pin callback
  * @param  l6360: l6360 structure pointer
  * 		callback: callback to lPlus pin
  * @retval none
  */
void L6360_AttachLplusPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state)){
	l6360->ctrlPins.enLplus = callback;
}

/**
  * @brief  Attach CQ pin callback
  * @param  l6360: l6360 structure pointer
  * 		callback: callback to CQ pin
  * @retval none
  */
void L6360_AttachCQPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state)){
	l6360->ctrlPins.encq = callback;
}

/**
  * @brief  Attach reset pin callback
  * @param  l6360: l6360 structure pointer
  * 		callback: callback to reset pin
  * @retval none
  */
void L6360_AttachResetPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state)){
	l6360->ctrlPins.rst = callback;
}

/*Default configuration-----------*/
L6360_Status L6360_RestoreFactoryDefault(L6360_IOLink_hw_t* l6360,  uint8_t devAddr){
	uint8_t data[L6360_I2C_REG_ADDR_MAX-2] = {
		L6360_DEF_CONF_REG,
		L6360_DEF_CTRL_REG_1,
		L6360_DEF_CTRL_REG_2,
		L6360_DEF_LED1MSB_REG,
		L6360_DEF_LED1LSB_REG,
		L6360_DEF_LED2MSB_REG,
		L6360_DEF_LED2LSB_REG
	};

	return L6360_I2CWriteAll(l6360, devAddr, data);
}
/*LOLink functionality-----------*/

/*Access bit field*/
typedef union L6360_BitAccess_t{
	struct {
		uint8_t b0:1, b1:1,	b2:1, b3:1,	b4:1, b5:1, b6:1, b7:1;
	}bits;
	uint8_t octet;
}L6360_BitAccess_t;
#define L6360_CHECKSUM_SEED_VALUE		0x52

/* @brief A.1.6 Calculation of the checksum
 * */
uint8_t L6360_CalcChecksum(uint8_t *data, uint16_t len){
	L6360_BitAccess_t checksum_8;		// 8 bit checksum
	L6360_BitAccess_t checksum_6;		// 6 bit checksum

	/*Calculate the 8 bit checksum*/
	checksum_8.octet = L6360_CHECKSUM_SEED_VALUE;
	data[1] &= 0xC0;	// set 0 to checksum(6) bit 0 to 5
	for(uint16_t i = 0; i < len; i++){
		checksum_8.octet ^= data[i];		// xro octet by octet
	}

	/*onvert 8 bit to 6 bit checksum*/
	checksum_6.bits.b0 = checksum_8.bits.b0 ^ checksum_8.bits.b1;
	checksum_6.bits.b1 = checksum_8.bits.b2 ^ checksum_8.bits.b3;
	checksum_6.bits.b2 = checksum_8.bits.b4 ^ checksum_8.bits.b5;
	checksum_6.bits.b3 = checksum_8.bits.b6 ^ checksum_8.bits.b7;
	checksum_6.bits.b4 = checksum_8.bits.b0 ^ checksum_8.bits.b2 ^
						 checksum_8.bits.b4 ^ checksum_8.bits.b6;
	checksum_6.bits.b5 = checksum_8.bits.b1 ^ checksum_8.bits.b3 ^
						 checksum_8.bits.b5 ^ checksum_8.bits.b7;
	checksum_6.bits.b6 = 0;	// set 0 to 6th bit
	checksum_6.bits.b7 = 0;	// set 0 to 7th bit
	return checksum_6.octet;
}
#define DELAY_US(timeout)	do{	\
							uint64_t time = getTime_us();	\
							while((getTime_us() - time) < timeout){;}\
						}while(0);

/**
  * @brief  Generate wake-up request
  * @param  l6360: l6360 structure pointer
  * 		callback: callback to reset pin
  * @retval none
  */
L6360_Status L6360_GenWURQ(L6360_IOLink_hw_t* l6360){

	/*Enable C/Q output*/
	L6360_CqEnable(l6360);

	/*1. Write I2C registers*/
	L6360_Status status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_OFF);
	if(status != L6360_OK) return status;
	status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_PUSH_PULL_1);
	if(status != L6360_OK) return status;

	/*3. if Write I2C registers success, read status register*/
	status = L6360_I2CReadStatusParity(l6360, &l6360->reg.sr.value, &l6360->reg.pr.value);
	if(status != L6360_OK) return status;
//	DBG_SPRINT_NL("st:%02X, prity:%02X", l6360->reg.sr, l6360->reg.pr);
	/*4. after completing this sequence l6360 will generate WURQ*/
	L6360_CqEnable(l6360);
	HAL_UART_DeInit(l6360->uart.huart);
	L6360_ConfigSIOMode(l6360, L6360_PORT_MODE_DO);
#ifdef L6360_MCU_STM32XXX
	HAL_GPIO_WritePin(l6360->uart.txPort, l6360->uart.txPin, GPIO_PIN_RESET);
	DELAY_US(L6360_WURQ_TWU_PULSE-L6360_WURQ_TWU_OFFSET);
	HAL_GPIO_WritePin(l6360->uart.txPort, l6360->uart.txPin, GPIO_PIN_SET);
	HAL_GPIO_DeInit(l6360->uart.txPort, l6360->uart.txPin);
#endif
	L6360_CqDisable(l6360);
	/*Init the uart with the height baudrate*/
	L6360_UartInit(l6360, L6360_UART_COM3_BR230400);


	/*Test Msgby using polling-------------------------------*/
//	uint8_t txData[10] = {0};
//	txData[0] = 0xA2;
//	txData[1] = 0x00;
//	txData[1] |= L6360_CalcChecksum(txData, 2);
//	uint32_t br = L6360_UART_COM3_BR230400;
//	DELAY_US((L6360_UART_BIT_TX_TIME(br)*32)+250);
//	L6360_CqEnable(l6360);
//	HAL_UART_Transmit(l6360->uart.huart, txData,  2,
//			(L6360_UART_CALC_TX_TIME(br,2)+L6360_UART_TX_TIME_OFFSET));
//
//	L6360_CqDisable(l6360);
//	br = 38400;
//	L6360_UartSetBaudrate(l6360, br);
//	DELAY_US((L6360_UART_BIT_TX_TIME(br)*32));
//	L6360_CqEnable(l6360);
//	HAL_UART_Transmit(l6360->uart.huart, txData,  2,
//			(L6360_UART_CALC_TX_TIME(br,2)+L6360_UART_TX_TIME_OFFSET));
//	L6360_CqDisable(l6360);
//	HAL_UART_Receive (l6360->uart.huart, l6360->uart.rxBuf, 2, 10);  // receive 4 bytes of data
//	DBG_SPRINT_NL("\r\nRxd:%02X, %02X", l6360->uart.rxBuf[0], l6360->uart.rxBuf[1]);
////	DBG_SPRINT_NL("st:%02X, prity:%02X", l6360->reg.sr, l6360->reg.pr);
//	br = 4800;
//	L6360_UartSetBaudrate(l6360, br);
//	DELAY_US((L6360_UART_BIT_TX_TIME(br)*32));
//	L6360_CqEnable(l6360);
//	HAL_UART_Transmit(l6360->uart.huart, txData,  2,
//			(L6360_UART_CALC_TX_TIME(br,2)+L6360_UART_TX_TIME_OFFSET));
//	L6360_CqDisable(l6360);
//	DBG_SPRINT_NL("\r\ntxd:%02X, %02X", txData[0], txData[1]);
//	DBG_SPRINT_NL("st:%02X, prity:%02X", l6360->reg.sr, l6360->reg.pr);
	//*/


	/*Test Msgby using DMA-------------------------------*/
	uint8_t txData[10] = {0};
	txData[0] = 0xA2;
	txData[1] = 0x00;
	txData[1] |= L6360_CalcChecksum(txData, 2);
	//IOL COM3
	uint32_t br = L6360_UART_COM3_BR230400;
	DELAY_US(L6360_UART_COM3_TBIT*32+250);
	L6360_CqEnable(l6360);
	HAL_UART_Transmit_DMA(l6360->uart.huart, txData,  2);
	DELAY_US(L6360_UART_COM3_TBIT*11*3);
	L6360_CqDisable(l6360);
	DELAY_US((L6360_UART_COM2_TBIT*32));

	//IOL COM2
	br = 38400;
	L6360_UartSetBaudrate(l6360, br);
	L6360_CqEnable(l6360);
	HAL_UART_Transmit_DMA(l6360->uart.huart, txData,  2);
	DELAY_US(L6360_UART_COM2_TBIT*11*3);
	L6360_CqDisable(l6360);
	DELAY_US(L6360_UART_COM1_TBIT*32);

	//IOL COM1
	br = 4800;
	L6360_UartSetBaudrate(l6360, br);
	L6360_CqEnable(l6360);
	HAL_UART_Transmit_DMA(l6360->uart.huart, txData,  2);
	DELAY_US(L6360_UART_COM1_TBIT*11*3);
	L6360_CqDisable(l6360);//*/
	DBG_SPRINT_NL("\r\ntxd:%02X, %02X", txData[0], txData[1]);
	DBG_SPRINT_NL("st:%02X, prity:%02X", l6360->reg.sr, l6360->reg.pr);
	//*/

	return L6360_OK;
}


/**
  * @brief  Set uart port modefor IOL SDIC
  * @param  l6360: l6360 structure pointer
  * 		mode: port mode
  * @retval none
  */
L6360_Status L6360_SetPortMode(L6360_IOLink_hw_t* l6360, uint8_t mode){
	L6360_Status status = L6360_OK;
	l6360->uart.baudrate = 0;
	switch (mode) {
		case IOL_PL_MODE_INACTIVE:
			/*(C/Q line in high impedance)*/
			l6360->portMode = mode;
			/*Turn off the C/Q in/out*/
			L6360_CqDisable(l6360);
			status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_OFF);
			/*TODO: Set LED Behavior*/
			break;
		case IOL_PL_MODE_DI:
			/*DI (C/Q line in digital input mode),*/
			l6360->portMode = mode;
			/*Turn on C/Qin & turn off C/Qout*/
			L6360_CqDisable(l6360);
			status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_OFF);
			if(status == L6360_OK){
				status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_PUSH_PULL_1);
			}
			/* TODO:
			 * 1. Set LED Behavior
			 * 2. Set UART RX pin to input mode,
			 * */
			break;
		case IOL_PL_MODE_DO:
			 /*DO (C/Q line in digital output mode),*/
			l6360->portMode = mode;
			/*Turn off C/Qin & turn on C/Qout*/
			L6360_CqEnable(l6360);
			status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_OFF);
			if(status == L6360_OK){
				status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_PUSH_PULL_1);
			}
			/* TODO:
			 * 1. Set LED Behavior
			 * 2. Set UART TX pin to output mode,
			 * */
			break;
		case IOL_PL_MODE_COM1:
			/*COM1 (C/Q line in COM1 mode)*/
			l6360->uart.baudrate = L6360_UART_COM1_BR4800;
		case IOL_PL_MODE_COM2:
			/*COM2 (C/Q line in COM2 mode)*/
			if(l6360->uart.baudrate == 0){
				l6360->uart.baudrate = L6360_UART_COM2_BR38400;
			}
		case IOL_PL_MODE_COM3:
			/*COM3 (C/Q line in COM3 mode)*/
			l6360->portMode = mode;
//			L6360_CqEnable(l6360);
			status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_OFF);
			if(status == L6360_OK){
				status = L6360_SetCNFR(l6360, L6360_CNFR_CQ_PUSH_PULL_1);
			}
			/* TODO:
			 * 1. Set LED Behavior
			 * 2. Set UART comx mode,
			 * */
			/*Set Uart baud rate*/
			if(l6360->uart.baudrate == 0){
				l6360->uart.baudrate = L6360_UART_COM3_BR230400;
			}
			status = L6360_UartSetBaudrate(l6360, l6360->uart.baudrate);
			break;
		default:

			status = L6360_UNKNOWN;
			break;
	}

	return status;
}

/**
  * @brief  configure uart port for IOL SDIC
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
L6360_Status L6360_ConfigSDCIMode(L6360_IOLink_hw_t* l6360,  uint8_t mode){
	return L6360_OK;
}

/**
  * @brief  configure IOL SIO mode
  * @param  l6360: l6360 structure pointer
  * @retval none
  */
L6360_Status  L6360_ConfigSIOMode(L6360_IOLink_hw_t* l6360, L6360_PortMode mode){
#ifdef L6360_MCU_STM32XXX
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  GPIO_TypeDef* port;
	  uint16_t pin;

	  if(mode == L6360_PORT_MODE_DI){
		  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	  }else if(mode == L6360_PORT_MODE_DO){
		  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  }else{
		  return L6360_VALUE_OUT_OF_RANGE;
	  }

	  port = l6360->uart.txPort;
	  pin = l6360->uart.txPin;

	  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	  /*Configure GPIO pins : PCPin PCPin */
	  GPIO_InitStruct.Pin = pin;

	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	  HAL_GPIO_Init(port, &GPIO_InitStruct);
#endif
	  return L6360_OK;
}


/*Timer related services-------------------------------*/
/**
  * @brief  Get time in us
  * @param  none
  * @retval time in us
  */

inline static uint64_t GetTimeUS(void){
	return CHECK_NULL_POINTER(getTime_us) ? 0 : getTime_us();
}

/**
  * @brief  Get time in ms
  * @param  none
  * @retval time in ms
  */
inline static uint64_t GetTimeMS(void){
	return CHECK_NULL_POINTER(getTime_us) ? 0 : (getTime_us()/1000);
}

/**
  * @brief  time delay in us
  * @param  timeout: timeout value
  * @retval time in us
  */
inline static void Delay_US(uint64_t timeout){
	uint64_t time = GetTimeUS();

	while((GetTimeUS()-time) < timeout){;}
}

/**
  * @brief  time delay in ms
  * @param  timeout value
  * @retval time in ms
  */
inline static void Delay_MS(uint64_t timeout){
	uint64_t time = GetTimeUS();
	uint64_t delay = (timeout*1000);
	while((GetTimeUS()-time) < delay){;}
}

/**
  * @brief  Get time in us
  * @param  none
  * @retval time in us
  */
void L6360_AttachTimerCallback(uint64_t (*callback)(void)){
	getTime_us = callback;
}
/* convert error from other type to l6360 error type
 * */
static inline L6360_Status ConvertError(uint16_t error){
	switch (error) {
#ifdef L6360_MCU_STM32XXX
	case HAL_OK:
		return L6360_OK;
		break;
	case HAL_ERROR:
		return L6360_ERROR;
		break;
	case HAL_BUSY:
		return L6360_BUSY;
		break;
	case HAL_TIMEOUT:
		return L6360_TIMEOUT;
		break;
#endif
	default:
		return L6360_UNKNOWN;
		break;
	}
}


