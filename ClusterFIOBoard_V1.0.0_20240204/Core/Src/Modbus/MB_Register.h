/*
 * MB_Register.h
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_REGISTER_H_
#define INC_MB_REGISTER_H_

#include <MB_Handler.h>
#include "main.h"
#include "MB_Addresses.h"


#define MB_REG_DI_DATA_MAX 	MB_DIA_MAX
#define MB_REG_CO_DATA_MAX 	MB_COA_MAX

/*Size of the registers*/
#define MB_REG_DI_SIZE		(((MB_DIA_MAX-MB_DI_ADDR_OFFSET)/8) + ((MB_DIA_MAX-MB_DI_ADDR_OFFSET)%8>0))
#define MB_REG_CO_SIZE		(((MB_COA_MAX-MB_CO_ADDR_OFFSET)/8) + ((MB_COA_MAX-MB_CO_ADDR_OFFSET)%8>0))
#define MB_REG_IR_SIZE		(MB_IRA_MAX-MB_IR_ADDR_OFFSET)
#define MB_REG_HR_SIZE		(MB_HRA_MAX-MB_HR_ADDR_OFFSET)



// Modbaus Register

typedef struct MB_Register{


	uint8_t DI[MB_REG_DI_SIZE];		// register for discrete input
	uint8_t CO[MB_REG_CO_SIZE];			// register for coils
	uint16_t IR_[MB_REG_IR_SIZE];		// register for input register
	uint16_t HR[MB_REG_HR_SIZE];		// register for holding register
}MB_Register;

void MB_REG_Init(void);

uint8_t MB_REG_GetDI(uint32_t address, uint8_t *value);
uint8_t MB_REG_GetCO(uint32_t address, uint8_t *value);
uint8_t MB_REG_GetIR(uint32_t address, uint16_t *value);
uint8_t MB_REG_GetHR(uint32_t address, uint16_t *value);

uint8_t MB_REG_GetDIM(uint8_t*pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_GetCOM(uint8_t*pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_GetIRM(uint16_t*pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_GetHRM(uint16_t*pData, uint32_t startAddress, uint16_t quantity);

uint8_t MB_REG_SetDI(uint32_t address,  uint16_t value);
uint8_t MB_REG_SetDIM(uint8_t *pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_SetCO(uint32_t address, uint16_t value);
uint8_t MB_REG_SetCOM(uint8_t *pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_SetIR(uint32_t address, uint8_t value);
uint8_t MB_REG_SetIRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity);
uint8_t MB_REG_SetHR(uint32_t address, uint16_t value);
uint8_t MB_REG_SetHRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity);

void MB_REG_AttachChangeDIRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value ));
void MB_REG_AttachChangeCORegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value ));
void MB_REG_AttachChangeIRRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value ));
void MB_REG_AttachChangeHRRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value ));


#endif /* INC_MB_REGISTER_H_ */
