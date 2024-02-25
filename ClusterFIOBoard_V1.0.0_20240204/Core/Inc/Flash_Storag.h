/*
 * Flash_Storage.c
 *
 *  Created on: Mar 25, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#include "main.h"
#include "Storage_Address.h"

/*
 * Page size of STM32F030XC is 2 kb
 * Page size of STM32F030XC is 1 kb
 * */

#define STM32F030XC
#define	STM32F103XX

//#define FS_NO_OF_PAGE 			128				// 128 number of pages for flash memory
//
#if defined (STM32F030XC)
	#define FS_PAGE1_ADDRESS		0x0803F000		//
	#define FS_PAGE2_ADDRESS		0x0803F800
#elif defined (STM32F103XX)
	#define FS_PAGE1_ADDRESS		0x0801FC00
	#define FS_PAGE2_ADDRESS 		0x0801F800
#endif

#define FS_PAGE_START               0
#define FS_MAX_STORAGE_SIZE			SA_MAX				// storage size 50 means, we can store max 50 number of data, each data size 4bytes
#define FS_RESET_VALUE 				0xFFFFFFFF




void FS_UpdateBuffer(void);
void FS_UpdateFlash(void);
uint32_t FS_Write( uint32_t *data);
void FS_Read(uint32_t startAddress, uint32_t *buffer, uint16_t noOfData);
uint32_t FS_WriteIntNum (uint32_t address, uint32_t number);
uint32_t  FS_ReadIntNum (uint32_t address);
uint32_t FS_WriteFloatNum (uint32_t address, float number);
float  FS_ReadFloatNum (uint32_t address);
void FS_EraseFlashPage(uint32_t startAddress);
uint32_t FS_GetActivePage(void);


#endif /* INC_FLASH_STORAGE_C_ */
