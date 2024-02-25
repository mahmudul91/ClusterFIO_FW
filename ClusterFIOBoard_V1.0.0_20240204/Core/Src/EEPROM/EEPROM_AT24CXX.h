/*
 * EEPROM_AT24CXX.h
 *
 *  Created on: Jan 17, 2023
 *      Author: Mahmudul Hasan (Russell)
 */

#ifndef SRC_EEPROM_EEPROM_AT24CXX_H_
#define SRC_EEPROM_EEPROM_AT24CXX_H_

/*This library is built for ATc4Cxx series EEPROM */

#include "main.h"
#include "stdint.h"


// un-comment to select the chip that you used in your project
//#define EEPROM_AT24_CHIP_AT24C16
#define EEPROM_AT24_CHIP_AT24C32
//#define EEPROM_AT24_CHIP_AT24C64
//#define EEPROM_AT24_CHIP_AT24C128
//#define EEPROM_AT24_CHIP_AT24C256


#if defined EEPROM_AT24_CHIP_AT24C16
	#define EEPROM_AT24_PAGE_SIZE			16U		// in bytes
	#define EEPROM_AT24_PAGE_NUM				128U	// max number of pages
#elif  defined EEPROM_AT24_CHIP_AT24C32
	#define EEPROM_AT24_PAGE_SIZE			32U		// in bytes
	#define EEPROM_AT24_PAGE_NUM				128U	// max number of pages
#elif  defined EEPROM_AT24_CHIP_AT24C64
	#define EEPROM_AT24_PAGE_SIZE			32U		// in bytes
	#define EEPROM_AT24_PAGE_NUM				256U	// max number of pages
#elif  defined EEPROM_AT24_CHIP_AT24C128
	#define EEPROM_AT24_PAGE_SIZE			64U		// in bytes
	#define EEPROM_AT24_PAGE_NUM				256U	// max number of pages
#elif  defined EEPROM_AT24_CHIP_AT24C256
	#define EEPROM_AT24_PAGE_SIZE			64U		// in bytes
	#define EEPROM_AT24_PAGE_NUM				512U	// max number of pages
#endif



#define EEPROM_AT24_I2C_DEV_ADDRESS			0xA0	// 8 bits device address for the I2C bus
#define EEPROM_AT24_WORD_ADDR_SIZE 			2U 		// in bytes
#define EEPROM_AT24_I2C_COMM_TIMEOUT			500U	// in ms,





/*
 * DataSize is the data size to be written at a specific address
 * */
typedef enum EEPROM_AT24_DataSize{
	EEPROM_AT24_DATASIZE_BYTE = 1U,		  		/*!< Program a byte (8-bit) at a specified address           */
	EEPROM_AT24_DATASIZE_HALFWORD,		  		/*!< Program a half-word (16-bit) at a specified address   */
	EEPROM_AT24_DATASIZE_WORD,     				/*!< Program a word (32-bit) at a specified address        */
	EEPROM_AT24_DATASIZE_DOUBLEWORD,  			/*!< Program a double word (64-bit) at a specified address */
}EEPROM_AT24_DataSize;

void EEPROM_AT24_Initialize(I2C_HandleTypeDef *i2c);//, EEPROM_AT24_DataSize dataSize);
uint8_t EEPROM_AT24_WritePage(uint16_t startPage, uint16_t startByte, uint8_t *data, uint16_t size);
uint8_t EEPROM_AT24_ReadPage(uint16_t startPage, uint16_t startByte, uint8_t *data, uint16_t size);
uint8_t EEPROM_AT24_ErasePage(uint16_t pageAddress);

void EEPROM_AT24_AttachErrorCallback(void (*callbackFunc)(uint8_t error));

#endif /* SRC_EEPROM_EEPROM_AT24CXX_H_ */
