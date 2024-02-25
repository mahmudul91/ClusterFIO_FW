/*
 * EEPROM_AT24CXX.c
 *
 *  Created on: Jan 17, 2023
 *      Author: wsrra
 */
#include "EEPROM_AT24CXX.h"
#include "math.h"
#include "string.h"
#include "Debug.h"


void (*EEPROM_AT24_ErrorCallback)(uint8_t error);

//use to convert float to byte and byte to float
typedef union FS_FloatBytes{
	float floatNumber;
	uint8_t bytes[4u];
}FS_FloatBytes;



typedef struct EEPROM_AT24CXX{
	I2C_HandleTypeDef *i2c;				// pointer of I2C serial bus
//	EEPROM_DataSize dataSize;
}EEPROM_AT24CXX;


static EEPROM_AT24CXX erom;



static uint16_t EEPROM_AT24_CalcDataLenToWrite (uint16_t startByte, uint16_t size);


/**
  * @brief initialize EEPROM I2C bus
  * @param i2c_: I2C communication struct pointer
  * @param dataSize: data size to be programmed in the eeprom
  * @retval none
  */
void EEPROM_AT24_Initialize(I2C_HandleTypeDef *i2c){//, EEPROM_DataSize dataSize){
	erom.i2c = i2c;
//	erom.dataSize = dataSize;
}







/**
  * @brief write EEPROM in block mode it will block the thread by (numOfPage x 6) ms
  * @param address: data address in the rom
  * @param dataSize: data size to be programmed in the eeprom
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_AT24_WritePage(uint16_t startPage, uint16_t startByte, uint8_t *data, uint16_t size){
	/*Format the data word address-----*/
	/* word address (memory address)  = (page address<<byte address size)  | byte address
	 * page address = This is the address of a specific page
	 * byte address = This is the address of a specific byte in a page
	 * */

	/*
	 * find the number of bits of the bytes address in a page
	 * formula: 	2^byteAddrSize = pageSize
	 * 			=>	log(EEPROM_PAGE_SIZE)/log(2)
	 * */



	uint8_t byteAddrSize = log(EEPROM_AT24_PAGE_SIZE)/log(EEPROM_AT24_WORD_ADDR_SIZE);
	uint16_t endPage = startPage + ((startByte + size) / EEPROM_AT24_PAGE_SIZE); // calculate the end page address
	uint16_t numOfPages = (endPage - startPage) + 1U;						// total number of pages to be written
	uint16_t dataIndex = 0U;												// index for the data

	// write the data in the pages
	for(uint16_t i = 0; i < numOfPages; i++){

		uint16_t wordAddr = ((startPage+i) << byteAddrSize) | startByte; 	 // word address of the exact memory location
		uint16_t dataLen = EEPROM_AT24_CalcDataLenToWrite(startByte, size); 		 //Calculate the data length to be written in a selected page

		// write the data page by page
		HAL_StatusTypeDef status = HAL_I2C_Mem_Write( 	erom.i2c, EEPROM_AT24_I2C_DEV_ADDRESS,
							wordAddr, EEPROM_AT24_WORD_ADDR_SIZE,
							&data[dataIndex], dataLen,
							EEPROM_AT24_I2C_COMM_TIMEOUT);

		if(status != HAL_OK){
			EEPROM_AT24_ErrorCallback(status);
			return 0;
		}

		HAL_Delay(6);		// 5ms delay is required in every write
		startByte = 0;  		// Now, we have to write in the new page so the start byte address should be 0
		size -= dataLen; 		// now update the size of the next remaining bytes to be written
		dataIndex += dataLen;	// now set the index for the next bytes of the data for the data buffer

	}
	return 1;

}




/**
  * @brief write EEPROM in non-blocking mode. we can write 1 page at a time with this function
  * @param pageAddr: page address of the eeprom
  * @param startByte:	start byte index in a page
  * @param data: data to be written
  * @param size: size of the data array, size must not be greater then 32 bytes
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_AT24_WritePageNB(uint16_t pageAddr, uint16_t startByte, uint8_t *data, uint16_t size){
	/*Format the data word address-----*/
	/* word address (memory address)  = (page address<<byte address size)  | byte address
	 * page address = This is the address of a specific page
	 * byte address = This is the address of a specific byte in a page
	 * */

	/*
	 * find the number of bits of the bytes address in a page
	 * formula: 	2^byteAddrSize = pageSize
	 * 			=>	log(EEPROM_PAGE_SIZE)/log(2)
	 * */

	static uint32_t tick = 0;
	if((uint32_t)fabs((HAL_GetTick() - tick)) >= 5 ){

	if(size > EEPROM_AT24_PAGE_SIZE) return 0; 	// if the size is greater than page size then return from here

	uint16_t byteAddrSize = log(EEPROM_AT24_PAGE_SIZE)/log(EEPROM_AT24_WORD_ADDR_SIZE);
//	uint16_t endPage = pageAddr + ((startByte + size) / EEPROM_PAGE_SIZE); // calculate the end page address
//	uint16_t numOfPages = 1;//(endPage - pageAddr) + 1U;						// total number of pages to be written
	uint16_t dataIndex = 0U;												// index for the data

	// write the data in the pages
//	for(uint16_t i = 0; i < numOfPages; i++){

		uint16_t wordAddr = ((pageAddr) << byteAddrSize) | startByte; 	 // word address of the exact memory location
		uint16_t dataLen = EEPROM_AT24_CalcDataLenToWrite(startByte, size); 		 //Calculate the data length to be written in a selected page

		// write the data page by page
		HAL_StatusTypeDef status = HAL_I2C_Mem_Write( 	erom.i2c, EEPROM_AT24_I2C_DEV_ADDRESS,
							wordAddr, EEPROM_AT24_WORD_ADDR_SIZE,
							&data[dataIndex], dataLen,
							EEPROM_AT24_I2C_COMM_TIMEOUT);
		if(status != HAL_OK){
			EEPROM_AT24_ErrorCallback(status);
			return 0;
		}

//		HAL_Delay(6);		// 5ms delay is required in every write
//		startByte = 0;  		// Now, we have to write in the new page so the start byte address should be 0
//		size -= dataLen; 		// now update the size of the next remaining bytes to be written
//		dataIndex += dataLen;	// now set the index for the next bytes of the data for the data buffer

//	}

		return 1;
	}else{
		return 0;
	}
}

/**
  * @brief read page of the EEPROM
  * @param address: data address in the rom
  * @param dataSize: data size to be programmed in the eeprom
  * @retval 0 = error, 1 = no error
  */
uint8_t EEPROM_AT24_ReadPage(uint16_t startPage, uint16_t startByte, uint8_t *data, uint16_t size){
	/*Format the data word address-----*/
	/* word address (memory address)  = (page address<<byte address size)  | byte address
	 * page address = This is the address of a specific page
	 * byte address = This is the address of a specific byte in a page
	 * */

	/*
	 * find the number of bits of the bytes address in a page
	 * formula: 	2^byteAddrSize = pageSize
	 * 			=>	log(EEPROM_PAGE_SIZE)/log(2)
	 * */
	uint8_t byteAddrSize = log(EEPROM_AT24_PAGE_SIZE)/log(EEPROM_AT24_WORD_ADDR_SIZE);

	uint16_t endPage = startPage + ((startByte + size) / EEPROM_AT24_PAGE_SIZE); // calculate the end page address
	uint16_t numOfPages = (endPage - startPage) + 1U;						// total number of pages to be written
	uint16_t dataIndex = 0U;												// index for the data

	// write the data in the pages
	for(uint16_t i = 0; i < numOfPages; i++){

		uint16_t wordAddr = ((startPage+i) << byteAddrSize) | startByte; 	 // word address of the exact memory location
		uint16_t dataLen = EEPROM_AT24_CalcDataLenToWrite(startByte, size); 		 //Calculate the data length to be written in a selected page

		// write the data page by page
		HAL_StatusTypeDef status = HAL_I2C_Mem_Read( 	erom.i2c, EEPROM_AT24_I2C_DEV_ADDRESS,
							wordAddr, EEPROM_AT24_WORD_ADDR_SIZE,
							&data[dataIndex], dataLen,
							EEPROM_AT24_I2C_COMM_TIMEOUT);

		if(status != HAL_OK){
			EEPROM_AT24_ErrorCallback(status);
			return 0;
		}

		startByte = 0;  		// Now, we have to write in the new page so the start byte address should be 0
		size -= dataLen; 		// now update the size of the next remaining bytes to be written
		dataIndex += dataLen;	// now set the index for the next bytes of the data for the data buffer

	}

	return 1;
}





/* @brief Erase page of the EEPROM_AT24
 * */
uint8_t EEPROM_AT24_ErasePage(uint16_t pageAddress){
	uint8_t byteAddrSize = log(EEPROM_AT24_PAGE_SIZE)/log(EEPROM_AT24_WORD_ADDR_SIZE);
	uint16_t wordAddr = (pageAddress << byteAddrSize); 	 // word address of the exact memory location

	// create a buffer to store the reset values
	uint8_t data[EEPROM_AT24_PAGE_SIZE];
	memset(data, 0xFF, EEPROM_AT24_PAGE_SIZE);

	// write the data to the EEPROM
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write( 	erom.i2c, EEPROM_AT24_I2C_DEV_ADDRESS,
						wordAddr, EEPROM_AT24_WORD_ADDR_SIZE,
						data, EEPROM_AT24_PAGE_SIZE,
						EEPROM_AT24_I2C_COMM_TIMEOUT);
	if(status != HAL_OK){
		EEPROM_AT24_ErrorCallback(status);
		return 0;
	}
		HAL_Delay (5);  // write cycle delay
	return 1;
}

/* @brief Calculate the data length to be written in a selected page
 * */
static uint16_t EEPROM_AT24_CalcDataLenToWrite (uint16_t startByte, uint16_t size){
	if ((size+startByte) < EEPROM_AT24_PAGE_SIZE) return size;
	else return (EEPROM_AT24_PAGE_SIZE - startByte);
}

/*
 * This function attach epprom error call back function
 * */
void EEPROM_AT24_AttachErrorCallback(void (*callbackFunc)(uint8_t error)){
	EEPROM_AT24_ErrorCallback = callbackFunc;
}
