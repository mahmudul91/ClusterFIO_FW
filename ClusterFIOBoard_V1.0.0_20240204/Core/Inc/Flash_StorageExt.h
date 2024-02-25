/*
 * Flash_StorageExt.h
 *
 *  Created on: Feb 21, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef INC_FLASH_STORAGEEXT_H_
#define INC_FLASH_STORAGEEXT_H_
/*External flash storage using w25qxx series flash
 * */
#include "stdint.h"
#include "Storage_Address.h"
#include "w25Qxx_Interface.h"
#include "main.h"

//#define FSEXT_DIRECT_ACCESS_FLASH

/*
 * Sector_16 of block_128 is used for the storage
 * each sector size is 4kb
 * each block size is 64kb
 * address if sector_16 is 15
 * address if block_128 is 127
 * */
/*W25Qxx attributes*/
#define FSEXT_BLOCK_ADDRESS			0x7F0000		//block address (block 127) to read/write
#define FSEXT_SECTOR_ADDRESS		0xF000			//sector address (address of sector 15) in a block to read/write
#define FSEXT_ADDRESS_OFFSET		(FSEXT_BLOCK_ADDRESS | FSEXT_SECTOR_ADDRESS)		//
#define FSEXT_SECTOR_SIZE			4096U			//4k sector size


#define FSEXT_START_ADDRES			0
#define FSEXT_MAX_STORAGE_SIZE		SA_MAX
#define FSEXT_MASS_CHANGE_AFTER	5

/*
 * DataSize is the data size to be written at a specific address
 * */
typedef enum FSExt_dataType{
	FSEXT_DATATYPE_BYTE 		= 1U,		  		/*!< Program a byte (8-bit) at a specified address           */
	FSEXT_DATATYPE_HALFWORD 	= 2U,		  		/*!< Program a half-word (16-bit) at a specified address   */
	FSEXT_DATATYPE_WORD			= 4U,     				/*!< Program a word (32-bit) at a specified address        */
	FSEXT_DATATYPE_DOUBLEWORD	= 8U,  			/*!< Program a double word (64-bit) at a specified address */
}FSExt_dataType;



uint8_t FSExt_Init(SPI_HandleTypeDef *spi, FSExt_dataType _dataType);
uint8_t FSExt_Download(void);
uint8_t FSExt_Upload(void);

uint8_t FSExt_WriteIntNum(uint16_t address, uint32_t data);
uint32_t FSExt_ReadIntNum(uint16_t address);

uint8_t FSExt_WriteFloatNum(uint16_t address, float data);
float FSExt_ReadFloatNum(uint16_t address);

uint8_t FSExt_Write(uint16_t startAddr, uint32_t *data, uint16_t quantity);
uint8_t FSExt_Read(uint16_t startAddr, uint32_t *data, uint16_t quantity);



uint8_t FSExt_EraseNum(uint16_t address);
uint8_t FSExt_Erase(uint16_t startAddr, uint16_t quantity);
uint8_t FSExt_EraseFull(void);

void FSExt_CheckAndUpdateChanges(void);

#endif /* INC_FLASH_STORAGEEXT_H_ */
