/*
 * Flash_StorageExt.c
 *
 *  Created on: Feb 21, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

/*External flash storage using AT24Cxx series EEPROM
 * */

#include "EEPROM_StorageExt.h"
#include "Debug.h"
#include "Storage_Address.h"
#include "string.h"




 I2C_HandleTypeDef hi2c1;

typedef struct EEPROMBuffStatus{
	uint16_t modified[EEPROM_MAX_STORAGE_SIZE];
	uint16_t count;
}EEPROMBuffStatus;


static EEPROMBuffStatus buffStatus;

static EEPROM_AT24_DataSize dataSize;
 uint32_t EEPROM_Buffer[EEPROM_MAX_STORAGE_SIZE];


void EEPROM_SetChngFlag( uint16_t address);
void EEPROM_ClearAllChngFlag(void);


uint8_t EEPROM_UpdateBuffer(void){
	uint8_t status = EEPROM_Read(EEPROM_START_ADDRES, EEPROM_Buffer, EEPROM_MAX_STORAGE_SIZE);
	if(status <= 0) memset(EEPROM_Buffer, 0xFF, EEPROM_MAX_STORAGE_SIZE*sizeof(EEPROM_Buffer[0]));
	return status;
}

uint8_t EEPROM_UpdateEEPROM(void){
	return EEPROM_Write(EEPROM_START_ADDRES, EEPROM_Buffer, EEPROM_MAX_STORAGE_SIZE);
}


void EEPROM_Init(I2C_HandleTypeDef *i2c, EEPROM_AT24_DataSize dataSize_){
	EEPROM_AT24_Initialize(i2c);
	dataSize = dataSize_;
	buffStatus.count = 0;
	if(HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_AT24_I2C_DEV_ADDRESS, 3, 100) != HAL_OK){
		DBG_SPRINT_NL("Error: EEPROM Is Busy & Locked. Trying To Reset...");
		HAL_Delay(1);
		HAL_I2C_DeInit(&hi2c1);
		HAL_Delay(1);
		HAL_I2C_Init(&hi2c1);

		if(HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_AT24_I2C_DEV_ADDRESS, 3, 100) != HAL_OK){
			DBG_SPRINT_NL("Error: EEPROM Failed To Reset !");
		}else{
			DBG_SPRINT_NL("EEPROM: Reset Successful! Ready To Use...");
		}
	}
}

/**
  * @brief write integer number to the flash
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_WriteIntNum(uint16_t address, uint32_t data){

#ifdef EEPROM_DIRECT_ACCESS_EEPROM
	return EEPROM_Write(address, &data, 1);
#else
	 EEPROM_Buffer[address] = data;
	 EEPROM_SetChngFlag(address);
	 return 1;
#endif
}

/**
  * @brief read integer number from the flash
  * @retval 0 = error, 1 = no error
  */
uint32_t EEPROM_ReadIntNum(uint16_t address){
	uint32_t data = 0;
#ifdef EEPROM_DIRECT_ACCESS_EEPROM
	EEPROM_Read(address, &data,1);
#else
	data = EEPROM_Buffer[address];
#endif
	return data;
}


/**
  * @brief write float number to the flash
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_WriteFloatNum(uint16_t address, float data){
	union{
		float floatNumber;
		uint32_t uintNumb;
	}convNum;

	convNum.floatNumber = data;
#ifdef EEPROM_DIRECT_ACCESS_EEPROM
	return EEPROM_Write(address, &convNum.uintNumb,1);
#else
	 EEPROM_Buffer[address] = convNum.uintNumb;
	 EEPROM_SetChngFlag(address);
	 return 1;
#endif
}


/**
  * @brief read float number from the flash
  * @retval 0 = error, 1 = no error
  */
float EEPROM_ReadFloatNum(uint16_t address){
	union{
		float floatNumber;
		uint32_t uintNumb;
	}convNum;
#ifdef EEPROM_DIRECT_ACCESS_EEPROM
	EEPROM_Read(address,  &convNum.uintNumb,1);
#else
	convNum.uintNumb = EEPROM_Buffer[address];
#endif
	return convNum.floatNumber;

}


/**
  * @brief write in the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_Write(uint16_t startAddr, uint32_t *data, uint16_t quantity){

	if(quantity > EEPROM_MAX_STORAGE_SIZE || (startAddr+quantity) > EEPROM_MAX_STORAGE_SIZE) return 0;
	uint8_t bytes[(quantity*dataSize)];
	uint16_t size =0 ;
	uint16_t index = 0;
	while(index < quantity){

		do{
			bytes[size] = (uint8_t)((data[index]>>(dataSize-1-size%dataSize)*8)&0x000000FF);
			size++;
		}while(size%dataSize != 0 );
		index++;
	}
	uint16_t startPage =  (uint16_t)(startAddr/EEPROM_AT24_PAGE_SIZE);
	uint16_t startByte =  (uint16_t)(((startAddr%EEPROM_AT24_PAGE_SIZE)*dataSize));
	if(startPage >= EEPROM_AT24_PAGE_NUM || startByte >= EEPROM_AT24_PAGE_SIZE) return 0;
	uint8_t status = EEPROM_AT24_WritePage(startPage, startByte, bytes, size);
	return status;
}


/**
  * @brief read integer number in the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_Read(uint16_t startAddr, uint32_t *data, uint16_t quantity){
//	DBG_SPRINT_NL("fs read-1");
	if(quantity > EEPROM_MAX_STORAGE_SIZE || (startAddr+quantity) > EEPROM_MAX_STORAGE_SIZE) return 0;
//	DBG_SPRINT_NL("fs read-2");
	uint16_t size = (quantity*dataSize) ;
	uint8_t bytes[size];

	uint16_t startPage = (uint16_t)(startAddr/EEPROM_AT24_PAGE_SIZE);
	uint16_t startByte =  (uint16_t)((startAddr%EEPROM_AT24_PAGE_SIZE)*dataSize);
	if(startPage >= EEPROM_AT24_PAGE_NUM || startByte >= EEPROM_AT24_PAGE_SIZE) return 0;
	uint8_t status = EEPROM_AT24_ReadPage(startPage, startByte, bytes, size);


	if(status>0){
		uint16_t byteIndex = 0;
		for(uint16_t i = 0; i < quantity; i ++){

		data[i] = 0;
		data[i] |= (uint32_t)(bytes[byteIndex++]<<24);
		data[i] |= (uint32_t)(bytes[byteIndex++]<<16);
		data[i] |= (uint32_t)(bytes[byteIndex++]<<8);
		data[i] |= (uint32_t)(bytes[byteIndex++]);
		}
	}
	return status;
}




/**
  * @brief erase single value for a address
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_EraseNum(uint16_t address){
	return EEPROM_Erase(address, 1);
}



/**
  * @brief erase the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_Erase(uint16_t startAddr, uint16_t quantity){
	if(quantity >= EEPROM_MAX_STORAGE_SIZE || (startAddr+quantity) >= EEPROM_MAX_STORAGE_SIZE) return 0;
	uint32_t data[quantity];

	for(uint16_t i = 0; i < quantity; i++){
		data[i] = 0xFFFFFFFF;
	}

	uint8_t status = EEPROM_Write(startAddr, data, quantity);
	if(status > 0){
		memset(&EEPROM_Buffer[startAddr], 0xFF, quantity*sizeof(EEPROM_Buffer[0]));
		EEPROM_ClearAllChngFlag();
	}
	return status;

}



/**
  * @brief erase the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_EraseFull(void){
	;
	for(uint16_t pageAddr = 0; pageAddr < EEPROM_AT24_PAGE_NUM; pageAddr++){
		if(!EEPROM_AT24_ErasePage(pageAddr)) return 0;
		memset(EEPROM_Buffer, 0xFF, EEPROM_MAX_STORAGE_SIZE*sizeof(EEPROM_Buffer[0]));
		EEPROM_ClearAllChngFlag();
	}

	return 1;
}




/*
 * These functions will track the changes of local flash buffer and update the eeprom
 * automatically if EEPROM_CheckChangesAndUpdate() this function is checked periodically
 * */

void EEPROM_SetChngFlag( uint16_t address){
	if(buffStatus.count < EEPROM_MAX_STORAGE_SIZE){
		buffStatus.modified[buffStatus.count++] = address;
	}
}


void EEPROM_ClearAllChngFlag(void){
	buffStatus.count = 0;
    memset(buffStatus.modified, 0, sizeof(buffStatus.modified));
}



void EEPROM_CheckAndUpdateChanges(void){
	if(buffStatus.count<= 0) return;
		if(EEPROM_Write(EEPROM_START_ADDRES, EEPROM_Buffer,EEPROM_MAX_STORAGE_SIZE) > 0){
			DBG_SPRINT_NL("Storage: Updated!");
			EEPROM_ClearAllChngFlag();
			buffStatus.count = 0;
		}else{
			DBG_SPRINT_NL("%s: %s, %s, %s", STR_ERROR, STR_EEPROM, STR_WRITE,STR_FAILED);
		}


//	}
}
