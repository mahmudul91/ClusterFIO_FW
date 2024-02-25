/*
 * Flash_Storage.c
 *
 *  Created on: Mar 25, 2022
 *      Author: RASEL_EEE
 */
#include "Flash_Storag.h"
#include "string.h"
#include "stdio.h"
#include "Debug.h"
#include "main.h"
/*
 * there are 2 flash pages are using to store the data. but only 1 page of them is active.
 *
 * */

static uint32_t FS_Buffer[FS_MAX_STORAGE_SIZE];


typedef enum FS_Params{
	FS_DATA_BYTE_SIZE = 4U,				// each data size will be 4 bytes
}FS_Params;

typedef union FS_FloatBytes{
	float floatNumber;
	uint8_t bytes[4u];
}FS_FloatBytes;

//typedef struct FS_Storage{
//	uint32_t page1_start_adress;		// start address of page 1 for selected pages for flash storage
//	uint32_t page2_start_adress;		// start address of page 2 for selected pages for flash storage
//}FS_Storage;
//
//
//
//FS_Storage fStorage;


/*--------- private functions----------*/
//inline static bool FS_IsValidAddress(uint32_t address);




void FS_UpdateBuffer(void){
	FS_Read(FS_PAGE_START, FS_Buffer, (FS_MAX_STORAGE_SIZE));
}

void FS_UpdateFlash(void){
	FS_Write(FS_Buffer);
}

/**
  * @brief stores data array
  * @param startAddress: start address of the flash storage
  * @param data: pointer to a data array
  * @param noOfData: number of data in the data array
  * @retval returns error value while writing data in Flash memory
  */
uint32_t FS_Write(uint32_t *data){
	//uint32_t currentPageStartAdress = FS_GetActivePage();
	uint32_t _startAddress = FS_PAGE1_ADDRESS;

	static FLASH_EraseInitTypeDef flasEraseInit;
	uint32_t pageError;
/*// todo: it is needed but our mcu is not supported
	flasEraseInit.TypeErase = FLASH_TY;
	flasEraseInit.PageAddress = FS_PAGE1_ADDRESS;
	flasEraseInit.NbPages = 1U;
*/

	HAL_FLASH_Unlock();
	/*--------- erase the active page -----------*/
	if(HAL_FLASHEx_Erase(&flasEraseInit, &pageError) != HAL_OK){
		return HAL_FLASH_GetError();
	}

	for(uint16_t address = FS_PAGE_START; address < FS_MAX_STORAGE_SIZE; address++){
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, _startAddress, data[address]) == HAL_OK){			// write the inactive page, now it is active page
			_startAddress += FS_DATA_BYTE_SIZE;  								// use startAddress += 2 for half word and 8 for double word
		 }else{
		   /* Error occurred while writing data in Flash memory*/
			 return HAL_FLASH_GetError ();
		 }
	}


	HAL_FLASH_Lock();
	return 0;
}


/**
  * @brief reads stored data array
  * @param startAddress: start address of the flash storage
  * @param data: pointer to a data array
  * @param noOfData: number of data in the data array
  * @retval none
  */
void FS_Read(uint32_t startAddress, uint32_t *buffer, uint16_t noOfData){
	startAddress = startAddress*FS_DATA_BYTE_SIZE +  FS_PAGE1_ADDRESS ;//FS_GetActivePage();
	while (1)
	{
		*buffer = *(__IO uint32_t *)startAddress;					//reads the stored value from the flash memory
		startAddress += FS_DATA_BYTE_SIZE;
		buffer++;
		if (!(--noOfData)) break;
	}
}



/**
  * @brief erase full flash
  */
void FS_EraseFlashPage(uint32_t startAddress){
	static FLASH_EraseInitTypeDef flasEraseInit;
	uint32_t pageError;
	/*// todo: it is needed but our mcu is not supported
	flasEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	flasEraseInit.PageAddress = startAddress;
	flasEraseInit.NbPages = 1U;
	*/
	HAL_FLASH_Unlock();
	if(HAL_FLASHEx_Erase(&flasEraseInit, &pageError) != HAL_OK){
		//return HAL_FLASH_GetError();
	}
	HAL_FLASH_Lock();
}



/**
  * @brief stores sngle integer number to the flash
  * @param startAddress: start address of the flash storage
  * @param number: floating point number to be stored
  * @retval returns error value while writing data in Flash memory
  */
uint32_t FS_WriteIntNum (uint32_t address, uint32_t number){										// set the floating point number to the FS_FloatBytes union
	if(address >= FS_PAGE_START && address < FS_MAX_STORAGE_SIZE){
		uint32_t lastValue = FS_Buffer[address];
		FS_Buffer[address] = number;
		uint8_t error = FS_Write (FS_Buffer);
		if(error != HAL_FLASH_ERROR_NONE){		// store the bytes for floating point number
			FS_Buffer[address] = lastValue;
			return error;
		}else{
			return error;
		}
	}else{
		/*// todo: it is needed but our mcu is not supported
		return HAL_FLASH_ERROR_PROG;
		*/
		return 0;
	}

}


/**
  * @brief reads single int number from the flash storage
  * @param startAddress: start address of the flash storage
  * @retval returns the floating point number
  */
uint32_t  FS_ReadIntNum (uint32_t address){

		//uint32_t number = FS_RESET_VALUE;												// declare an union of which will help to convert the bytes to the floating point number
		//FS_Read(address, &number, 1u);				// read the bytes for floating point number

		return FS_Buffer[address];											// return the floating point number

}



/**
  * @brief stores floating point number to the flash
  * @param startAddress: start address of the flash storage
  * @param number: floating point number to be stored
  * @retval returns error value while writing data in Flash memory
  */
uint32_t FS_WriteFloatNum (uint32_t address, float number){

	FS_FloatBytes floatbytes;												// declare an union of which will help to convert the floating point number to bytes
	floatbytes.floatNumber = number;										// set the floating point number to the FS_FloatBytes union

	uint32_t data = 0x00000000;
	for(uint8_t i = 0u; i < 4; i++){
		data |= ((uint32_t)floatbytes.bytes[i]<<(8*i));
	}


	if(address >= FS_PAGE_START && address < FS_MAX_STORAGE_SIZE){
		uint32_t lastValue = FS_Buffer[address];
		FS_Buffer[address] = data;
		uint8_t error = FS_Write (FS_Buffer);
		if(error != HAL_FLASH_ERROR_NONE){		// store the bytes for floating point number
			FS_Buffer[address] = lastValue;
			return error;
		}else{
			return error;
		}
	}else{
		/*// todo: it is needed but our mcu is not supported
		return HAL_FLASH_ERROR_PROG;
		*/
		return 0;
	}
}


/**
  * @brief reads floating point number from the flash storage
  * @param startAddress: start address of the flash storage
  * @retval returns the floating point number
  */
float  FS_ReadFloatNum (uint32_t address){

		FS_FloatBytes floatbytes;												// declare an union of which will help to convert the bytes to the floating point number
		uint32_t data = FS_Buffer[address];

		for(uint8_t i = 0u; i < 4; i++){
			floatbytes.bytes[i] = (uint8_t)((data>>(8*i))&0x000000FF);
		}
		return floatbytes.floatNumber;											// return the floating point number

}




/**
  * @brief gets the active page from the pages of the flash storage
  * @param none
  * @retval returns the start address of the active page
  */
uint32_t FS_GetActivePage(void){
	return ((*(__IO uint32_t *)FS_PAGE1_ADDRESS) == FS_PAGE1_ADDRESS ? FS_PAGE1_ADDRESS : FS_PAGE2_ADDRESS);
}

