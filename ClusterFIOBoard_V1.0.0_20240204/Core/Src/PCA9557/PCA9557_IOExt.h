/*
 * PCA9557.h
 *
 *  Created on: Feb 14, 2024
 *      Author: wsrra
 *
 *  PCA9557 IO extender
 */

#ifndef SRC_PCA9557_PCA9557_IOEXT_H_
#define SRC_PCA9557_PCA9557_IOEXT_H_
#include "stdint.h"
#include "stm32h7xx_hal.h"

/* to lock unlock the process
 * */
typedef enum PCA_ProcessLock{
  PCA_UNLOCKED = 0x00U,
  PCA_LOCKED   = 0x01U,
} PCA_ProcessLock_te;

#define PCA_BUSY		PCA_ERR_BUSY
#define PCA_LOCK(__handle__)    do{ if((__handle__)->lock == PCA_LOCKED) return PCA_BUSY;	\
                                    else (__handle__)->lock = PCA_LOCKED;    \
                                  }while (0U)
#define PCA_UNLOCK(__handle__)    do{(__handle__)->lock = PCA_UNLOCKED;}while (0U)



#define PCA_DEV_ADDR_MAX		8U //

/*Sets the io mode*/
#define PCA_SET_OUTPUTMODE(pca, pin)  (pca)->CR.value &= (uint8_t)~(pin);
#define PCA_SET_INPUTMODE(pca, pin)  (pca)->CR.value |= (pin);

/*Set the polarity*/
#define PCA_SET_POLARITYNONE(pca, pin)  (pca)->PR.value &= (uint8_t)~(pin);
#define PCA_SET_POLARITYINV(pca, pin)  (pca)->PR.value |= (pin);

/*Set State*/
#define PCA_SET_LOW(pca, pin)  (pca)->SR.value &= (uint8_t)~(pin);
#define PCA_SET_HIGH(pca, pin)  (pca)->SR.value |= (pin);

/*Errors*/
typedef enum PCA_Error{
	PCA_ERR_NONE 	= 0,
	PCA_ERR_I2C_ERROR	= (-1),
	PCA_ERR_I2C_BUSY	= (-2),
	PCA_ERR_I2C_TIMEOUT	= (-3),
	PCA_ERR_NULL_PTR	= (-4),
	PCA_ERR_BUSY	= (-5),
	PCA_ERR_INVALID_DEV_ADDRES	= (-6),
	PCA_ERR_INVALID_REGISTER	= (-7),
}PCA_Error_te;

/*Pin number*/
typedef enum PCA_Pin{
	PCA_PIN_0 = 0x01,
	PCA_PIN_1 = 0x02,
	PCA_PIN_2 = 0x04,
	PCA_PIN_3 = 0x08,
	PCA_PIN_4 = 0x10,
	PCA_PIN_5 = 0x20,
	PCA_PIN_6 = 0x40,
	PCA_PIN_7 = 0x80,
	PCA_PIN_ALL = 0xFF,
}PCA_Pin_te;

/*Pin state*/
typedef enum PCA_PinState{
	PCA_PIN_RESET = 0,
	PCA_PIN_SET
}PCA_PinState_te;

/*Pin Mode*/
typedef enum PCA_PinMode{
	PCA_MODE_OUTPUT = 0,
	PCA_MODE_INPUT
}PCA_PinMode_te;

/*Pin Polarity Inverse*/
typedef enum PCA_PinPolarity{
	PCA_POLARITY_NONE = 0,
	PCA_POLARITY_INV
}PCA_PinPolarity_te;

/* Control Register
 * Basically, it is used to select the register to read or write
 * 0 - Register_0: read-only, Input port register
 * 1 - Register_1: read/write, Output port register
 * 2 - Register_2: read/write, Polarity inversion register
 * 3 - Register_3: read/write, Configuration register
 * */
typedef enum PCA_Registers{
	PCA_INPUT_REG = 0,	// Register_0: read-only, Input port register
	PCA_OUTPUT_REG,		// Register_1: read/write, Output port register
	PCA_POLARINV_REG,	// Register_2: read/write, Polarity inversion register
	PCA_CONFIG_REG,		// Register_3: read/write, Configuration register
	PCA_MAX_REG
}PCA_Registers_te;

typedef enum PCA_RequestType{
	PCA_REQ_TYPE_NONE = 0,
	PCA_REQ_TYPE_READ,
	PCA_REQ_TYPE_WRITE,
	PCA_REQ_TYPE_CONFIG,
	PCA_REQ_TYPE_MAX,
}PCA_RequestType_te;

/*Write complete status*/
typedef enum PCA_WriteStatus{
	PCA_WRITE_COMPLETED = 0,
	PCA_WRITE_NOT_COMPLETED,
}PCA_WriteStatus_te;

/*Read complete status*/
typedef enum PCA_ReadStatus{
	PCA_READ_COMPLETED = 0,
	PCA_READ_NOT_COMPLETED,
}PCA_ReadStatus_te;
//typedef struct PCA_RegBits{
//	uint8_t P0 : 1;
//	uint8_t P1 : 1;
//	uint8_t P2 : 1;
//	uint8_t P3 : 1;
//	uint8_t P4 : 1;
//	uint8_t P5 : 1;
//	uint8_t P6 : 1;
//	uint8_t P7 : 1;
//}PCA_RegBits_ts;

/*Pins state*/
typedef union PCA_Reg{
	uint8_t value;
	struct{
		uint8_t P0 : 1;
		uint8_t P1 : 1;
		uint8_t P2 : 1;
		uint8_t P3 : 1;
		uint8_t P4 : 1;
		uint8_t P5 : 1;
		uint8_t P6 : 1;
		uint8_t P7 : 1;
	}bit;
}PCA_Reg_tu;

///*Pins state*/
//typedef union PCA_PinMode{
//	uint8_t allPin;
//	PCA_Pins_ts singlePin;
//}PCA_PinMode_tu;

/*Main Structure to the PCA*/
//typedef struct PCA_IOExtPinConfig{
//	uint8_t pin;
//	uint8_t state;
//	uint8_t mode;
//	uint8_t polarity;
//}PCA_IOExtPinConfig_ts;

typedef struct PCA557_IOExt{
	uint8_t devAddress;
	uint8_t targetReg;
	PCA_RequestType_te reqType;

	/*Registers*/
	PCA_Reg_tu SR;			// IO port register
	PCA_Reg_tu CR;			// Configuration register
	PCA_Reg_tu PR;		// Polarity inversion register

	PCA_ProcessLock_te lock;
	struct{
		uint8_t writeCmplt : 1;
		uint8_t readCmplt : 1;
		uint8_t reserve : 6;
	}flag;
	uint32_t i2cErrCode;		// this variable contains I2C related erros
}PCA9557_IOExt_ts;

/*Services--------------------------*/
PCA_Error_te PCA_AttachI2C(I2C_HandleTypeDef* hi2c);
PCA_Error_te PCA_Init(PCA9557_IOExt_ts* pca, uint8_t devAddr);
PCA_Error_te PCA_ConfigPin(PCA9557_IOExt_ts* pca);
PCA_Error_te PCA_ReadPin(PCA9557_IOExt_ts* pca, uint8_t pin, uint8_t* state);
PCA_Error_te PCA_WritePin(PCA9557_IOExt_ts* pca, uint8_t pin, uint8_t state);
PCA_Error_te PCA_WritePinAll(PCA9557_IOExt_ts* pca, uint8_t state);
PCA_Error_te PCA_WritePinIT(PCA9557_IOExt_ts* pca, uint8_t pin, PCA_PinState_te state);
PCA_Error_te PCA_WritePinAllIT(PCA9557_IOExt_ts* pca,  uint8_t state);
PCA_Error_te PCA_WritePinDMA(PCA9557_IOExt_ts* pca, uint8_t pin, PCA_PinState_te state);
PCA_Error_te PCA_WritePinAllDMA(PCA9557_IOExt_ts* pca, uint8_t state);
PCA_Error_te PCA_TogglePin(PCA9557_IOExt_ts* pca, uint8_t pin);

PCA_Error_te PCA_WriteReg(uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data);
PCA_Error_te PCA_ReadReg(uint8_t devAddr, PCA_Registers_te selectReg, uint8_t *data);

PCA_Error_te PCA_WriteRegIT(PCA9557_IOExt_ts* pca, uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data);
PCA_Error_te PCA_WriteRegDMA(PCA9557_IOExt_ts* pca, uint8_t devAddr, PCA_Registers_te selectReg, uint8_t data);
uint8_t PCA_GetWriteCmpltFlag(PCA9557_IOExt_ts* pca);
uint8_t PCA_GetReadCmpltFlag(PCA9557_IOExt_ts* pca);


/*This callback are used in case of interrupt and DMA implementation*/
void PCA_AttachWriteCmpltCallback( void (*callback)(PCA9557_IOExt_ts *pca));
void PCA_AttachReadCmpltCallback( void (*callback)(PCA9557_IOExt_ts *pca));
void PCA_AttachErrorCallback( void (*callback)(PCA9557_IOExt_ts *pca, uint32_t i2cErrCode));
#endif /* SRC_PCA9557_PCA9557_IOEXT_H_ */



