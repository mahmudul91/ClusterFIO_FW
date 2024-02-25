/*
 * PCA9557.c
 *
 *  Created on: Feb 14, 2024
 *      Author: wsrra
 */

#include "PCA9557_IOExt.h"
#include "Debug.h"

/*Fixed address part of the device actual address*/
#define PCA_DEV_FIXED_ADDR		0x30
#define PCA_WR_DIR_W_AND_MASK	0xFE

#define CONV_ERROR(err)			(PCA_Error_te)(0-err)
#define GET_DEV_ADDR(addr, rwDir)   ((PCA_DEV_FIXED_ADDR | (addr<<1)) | rwDir);

/*Read/ write direction*/
typedef enum PCA_WRDirection{
	PCA_WR_DIR_W = 0,
	PCA_WR_DIR_R
}PCA_WRDirection_te;




I2C_HandleTypeDef* i2c;

/*Callback------------*/
/*This callback are used in case of interrupt and DMA implementation*/
void (*PCA_WriteCmpltCallback)(PCA9557_IOExt_ts *pca);
void (*PCA_ReadCmpltCallback)(PCA9557_IOExt_ts *pca);
void (*PCA_ErrorCallback)(PCA9557_IOExt_ts *pca, uint32_t i2cErrCode);

/*Services--------------------------*/
/**
  * @brief  Attach I2C to PCA9557 IO Extender
  * @param  i2c: pointer to the i2c port struct
  * @retval none
  */
PCA_Error_te PCA_AttachI2C(I2C_HandleTypeDef* hi2c){
	if(hi2c == NULL) return PCA_ERR_NULL_PTR;
	i2c = hi2c;
	return PCA_ERR_NONE;
}

/**
  * @brief  Initialize PCA9557 and set address
  * @param  pca: pointer to the PCA9557_IOExt_ts struct
  * @param  devAddr: address of the PCA9557 chip
  * @retval none
  */
PCA_Error_te PCA_Init(PCA9557_IOExt_ts* pca, uint8_t devAddr){
//	DBG_SPRINT_NL("hi2c1: %p", hi2c1);
	if(pca == NULL || i2c == NULL) {
		return PCA_ERR_NULL_PTR;
	}else if(devAddr >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}
	PCA9557_IOExt_ts pcaIOEx = {0};
	*pca = pcaIOEx;
	pca->devAddress = devAddr;
	devAddr = GET_DEV_ADDR(devAddr, PCA_WR_DIR_W);
	HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(i2c, devAddr, 3, 3);
	if(status != HAL_OK){
		return CONV_ERROR(status);
	}
	return PCA_ERR_NONE;
}


/**
  * @brief  Configure PCA9557 Pins
  * @param  i2c: void pointer to the i2c port struct
  * @retval none
  */
PCA_Error_te PCA_ConfigPin(PCA9557_IOExt_ts* pca){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}
	/*Write Pin mode configuration register*/
	PCA_Error_te errStatus = PCA_WriteReg(pca->devAddress,
											PCA_CONFIG_REG,
											pca->CR.value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	/*Write pin polarity inversion register*/
	errStatus = PCA_WriteReg( pca->devAddress,
								PCA_POLARINV_REG,
								pca->PR.value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}

	return errStatus;
}


/**
  * @brief  Write PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePin(PCA9557_IOExt_ts* pca, uint8_t pin, PCA_PinState_te state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}

	uint8_t value = pca->SR.value;
	if(state == PCA_PIN_RESET){
		value &= ~pin;
	}else{
		value |= pin;
	}

	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteReg(pca->devAddress,
											PCA_OUTPUT_REG,
											value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->SR.value = value;
	return PCA_ERR_NONE;
}

/**
  * @brief  Write PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePinAll(PCA9557_IOExt_ts* pca, uint8_t state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}


	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteReg(pca->devAddress,
											PCA_OUTPUT_REG,
											state);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->SR.value = state;
	return PCA_ERR_NONE;
}

/**
  * @brief  Write Pin of PCA9557 registers with interrupt
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePinIT(PCA9557_IOExt_ts* pca, uint8_t pin, PCA_PinState_te state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}

	uint8_t value = pca->SR.value;
	if(state == PCA_PIN_RESET){
		value &= ~pin;
	}else{
		value |= pin;
	}

	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteRegIT(pca, pca->devAddress,
											PCA_OUTPUT_REG,
											value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->flag.writeCmplt = PCA_WRITE_NOT_COMPLETED;
	pca->SR.value = value;
	return PCA_ERR_NONE;
}

/**
  * @brief  Write Pin of PCA9557 registers with interrupt
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePinAllIT(PCA9557_IOExt_ts* pca,  uint8_t state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}


	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteRegIT(pca, pca->devAddress,
											PCA_OUTPUT_REG,
											state);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->flag.writeCmplt = PCA_WRITE_NOT_COMPLETED;
	pca->SR.value = state;
	return PCA_ERR_NONE;
}


/**
  * @brief  Write Pin of PCA9557 registers with interrupt
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePinDMA(PCA9557_IOExt_ts* pca, uint8_t pin, PCA_PinState_te state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}

	uint8_t value = pca->SR.value;
	if(state == PCA_PIN_RESET){
		value &= ~pin;
	}else{
		value |= pin;
	}

	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteRegDMA(pca, pca->devAddress,
											PCA_OUTPUT_REG,
											value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->flag.writeCmplt = PCA_WRITE_NOT_COMPLETED;
	pca->SR.value = value;
	return PCA_ERR_NONE;
}


/**
  * @brief  Write All Pin of PCA9557 registers with interrupt
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WritePinAllDMA(PCA9557_IOExt_ts* pca, uint8_t state){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}

	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteRegDMA(pca, pca->devAddress,
											PCA_OUTPUT_REG,
											state&PCA_PIN_ALL);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->flag.writeCmplt = PCA_WRITE_NOT_COMPLETED;
	pca->SR.value = state;
	return PCA_ERR_NONE;
}

/**
  * @brief  read PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be read
  * @retval none
  */
PCA_Error_te PCA_ReadPin(PCA9557_IOExt_ts* pca, uint8_t pin, uint8_t* state){
	if(pca == NULL || state == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}
	uint8_t value;
	PCA_Error_te errStatus = PCA_ReadReg( pca->devAddress,
											PCA_OUTPUT_REG,
											&value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->SR.value = value;
	*state = (pin&value);
	return PCA_ERR_NONE;
}


/**
  * @brief  Write PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_TogglePin(PCA9557_IOExt_ts* pca, uint8_t pin){
	if(pca == NULL){
		return PCA_ERR_NULL_PTR;
	}else if(pca->devAddress >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}
	uint8_t value = pca->SR.value;
//	uint8_t temp = value&pin;

	/*Toggle the output*/
//	value = (value&(~pin))|((~temp)&pin);
//	value = (value&(~pin)) | ((~value)&pin);
	value ^= pin;

	/*Write Pin Output state register*/
	PCA_Error_te errStatus = PCA_WriteReg(pca->devAddress,
											PCA_OUTPUT_REG,
											value);
	if(errStatus != PCA_ERR_NONE){
		return errStatus;
	}
	pca->SR.value = value;
	return PCA_ERR_NONE;
}

/**
  * @brief  Write PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WriteReg(uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data){
	/*Check errors*/
	if(devAddr >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}else if(selectReg >= PCA_MAX_REG){
		return PCA_ERR_INVALID_REGISTER;
	}

	uint8_t buff[2] = {0};
	buff[0] = (uint8_t)selectReg;
	buff[1] = data;
	/*Write and mask insure the WR direction bit is set to the write mode*/
	devAddr = GET_DEV_ADDR(devAddr, PCA_WR_DIR_W);//(PCA_DEV_FIXED_ADDR | (devAddr<<1)) & PCA_WR_DIR_W_AND_MASK;
	HAL_StatusTypeDef stat = HAL_I2C_Master_Transmit(i2c, devAddr, buff, 2, 2);
	if(stat != HAL_OK){
		return CONV_ERROR(stat);
	}
	i2c->ErrorCode;

	return PCA_ERR_NONE;

}


/**
  * @brief  Write PCA9557 registers using interrupt
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WriteRegIT(PCA9557_IOExt_ts* pca, uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data){

	/*Check errors*/
	if(devAddr >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}else if(selectReg >= PCA_MAX_REG){
		return PCA_ERR_INVALID_REGISTER;
	}
	/*Process lock*/
	PCA_LOCK(pca);
	uint8_t buff[2] = {0};
	buff[0] = selectReg;
	buff[1] = data;
	/*Write and mask insure the WR direction bit is set to the write mode*/
	devAddr = GET_DEV_ADDR(devAddr, PCA_WR_DIR_W);
	HAL_StatusTypeDef stat = HAL_I2C_Master_Transmit_IT(i2c, devAddr, buff, 2);
	if(stat != HAL_OK){
		/*Process unlock*/
		PCA_UNLOCK(pca);
		return CONV_ERROR(stat);
	}
	return PCA_ERR_NONE;
}

/**
  * @brief  Write PCA9557 registers using DMA
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to be write
  * @retval none
  */
PCA_Error_te PCA_WriteRegDMA(PCA9557_IOExt_ts* pca, uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data){

	/*Check errors*/
	if(devAddr >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}else if(selectReg >= PCA_MAX_REG){
		return PCA_ERR_INVALID_REGISTER;
	}
	/*Process lock*/
	PCA_LOCK(pca);
	uint8_t buff[2] = {0};
	buff[0] = selectReg;
	buff[1] = data;

	/*Write and mask insure the WR direction bit is set to the write mode*/
	devAddr = GET_DEV_ADDR(devAddr, PCA_WR_DIR_W);
	HAL_StatusTypeDef stat = HAL_I2C_Master_Transmit_DMA(i2c, devAddr, buff, 2);//IT(i2c, devAddr, buff, 2);//
	if(stat != HAL_OK){
		/*Process unlock*/
		PCA_UNLOCK(pca);
		return CONV_ERROR(stat);
	}
	return PCA_ERR_NONE;
}

/**
  * @brief  Reads PCA9557 registers
  * @param  devAddr: 3-bits Device address
  * @param  selectReg: register address to select the target register
  * @param  data: data pointer to store the read value
  * @retval none
  */
PCA_Error_te PCA_ReadReg(uint8_t devAddr, PCA_Registers_te selectReg, uint8_t *data){
	/*Check errors*/
	if(devAddr >= PCA_DEV_ADDR_MAX){
		return PCA_ERR_INVALID_DEV_ADDRES;
	}else if(selectReg >= PCA_MAX_REG){
		return PCA_ERR_INVALID_REGISTER;
	}else if(data == NULL){
		return PCA_ERR_NULL_PTR;
	}

	/*Select target register*/
	/*Write and mask insure the WR direction dit is set to the write mode*/
	devAddr = GET_DEV_ADDR(devAddr, PCA_WR_DIR_W);
	HAL_StatusTypeDef stat = HAL_I2C_Master_Transmit(i2c, devAddr, &selectReg, 1, 1);
	if(stat != HAL_OK){
		return CONV_ERROR(stat);
	}

	/*Read the register value*/
//	uint8_t buff[2] = {0};
	devAddr |= PCA_WR_DIR_R;
	stat = HAL_I2C_Master_Receive(i2c, devAddr, data, 1, 1);
	if(stat != HAL_OK){
		return CONV_ERROR(stat);
	}


	return PCA_ERR_NONE;

}

static inline PCA_Error_te SetPolarity(){
	return PCA_ERR_NONE;
}

/**
  * @brief  Gets write complete flag. clears this flag on read
  * @param  pca: pointer to the PCA9557 structure
  * @retval flag value
  */
uint8_t PCA_GetWriteCmpltFlag(PCA9557_IOExt_ts* pca){
	 return pca->flag.writeCmplt;
}

/**
  * @brief  Gets read complete flag. clears this flag on read
  * @param  pca: pointer to the PCA9557 structure
  * @retval flag value
  */
uint8_t PCA_GetReadCmpltFlag(PCA9557_IOExt_ts* pca){
	return pca->flag.readCmplt;

}


