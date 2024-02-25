/*
 * Flash_StorageExt.h
 *
 *  Created on: Feb 21, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef INC_FLASH_STORAGEEXT_H_
#define INC_FLASH_STORAGEEXT_H_
/*External flash storage using AT24Cxx series EEPROM
 * */
#include "stdint.h"
#include "Storage_Address.h"
#include "EEPROM_AT24CXX.h"

//#define EEPROM_DIRECT_ACCESS_EEPROM

#define EEPROM_START_ADDRES			0
#define EEPROM_MAX_STORAGE_SIZE		SA_MAX
#define EEPROM_MASS_CHANGE_AFTER	5

void EEPROM_Init(I2C_HandleTypeDef *i2c, EEPROM_AT24_DataSize dataSize);
uint8_t EEPROM_UpdateBuffer(void);
uint8_t EEPROM_UpdateEEPROM(void);

uint8_t EEPROM_WriteIntNum(uint16_t address, uint32_t data);
uint32_t EEPROM_ReadIntNum(uint16_t address);

uint8_t EEPROM_WriteFloatNum(uint16_t address, float data);
float EEPROM_ReadFloatNum(uint16_t address);

uint8_t EEPROM_Write(uint16_t startAddr, uint32_t *data, uint16_t quantity);
uint8_t EEPROM_Read(uint16_t startAddr, uint32_t *data, uint16_t quantity);



uint8_t EEPROM_EraseNum(uint16_t address);
uint8_t EEPROM_Erase(uint16_t startAddr, uint16_t quantity);
uint8_t EEPROM_EraseFull(void);

void EEPROM_CheckAndUpdateChanges(void);

#endif /* INC_FLASH_STORAGEEXT_H_ */
