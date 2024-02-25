/*
 * Flash_StorageExt.c
 *
 *  Created on: Feb 21, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

/*External flash storage using AT24Cxx series EEPROM
 * */

#include "Flash_StorageExt.h"
#include "Debug.h"
#include "string.h"


/*u32 int to 4xu8 array converter*/


typedef struct FSExtBuffStatus{
	uint16_t modified[FSEXT_MAX_STORAGE_SIZE];
	uint16_t count;
}FSExtBuffStatus;


w25qxx_handle_t* w25q;
static FSExtBuffStatus buffStatus;
static FSExt_dataType dataType;
/*Local buffer*/
uint32_t FSExt_Buffer[FSEXT_MAX_STORAGE_SIZE];


void FSExt_SetChngFlag( uint16_t address);
void FSExt_ClearAllChngFlag(void);


/**
  * @brief Initialize flash storage
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_Init(SPI_HandleTypeDef *spi, FSExt_dataType _dataType){
	w25q = W25Q_GetInstance();
	dataType = _dataType;
	buffStatus.count = 0;
	 if(W25Q_Init(W25Q64, W25QXX_INTERFACE_SPI, W25QXX_BOOL_FALSE)==0){
		 return 1;
	 }else{
		 return 0;
	 }
}

/**
  * @brief Download buffer from flash storage
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_Download(void){
	uint8_t status = FSExt_Read(FSEXT_START_ADDRES, FSExt_Buffer, FSEXT_MAX_STORAGE_SIZE);

	if(status <= 0) memset(FSExt_Buffer, 0xFF, FSEXT_MAX_STORAGE_SIZE*sizeof(FSExt_Buffer[0]));
	return status;
}

/**
  * @brief Upload buffer to flash storage
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_Upload(void){
	return FSExt_Write(FSEXT_START_ADDRES, FSExt_Buffer, FSEXT_MAX_STORAGE_SIZE);
}


/**
  * @brief write integer number to the flash
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_WriteIntNum(uint16_t address, uint32_t data){

#ifdef FSEXT_DIRECT_ACCESS_FLASH
	return FSExt_Write(address, &data, 1);
#else
	 FSExt_Buffer[address] = data;
	 FSExt_SetChngFlag(address);
	 return 1;
#endif
}



/**
  * @brief read integer number from the flash
  * @retval 0 = error, 1 = no error
  */
uint32_t FSExt_ReadIntNum(uint16_t address){
	uint32_t data = 0;
#ifdef FSEXT_DIRECT_ACCESS_FLASH
	FSExt_Read(address, &data,1);
#else
	data = FSExt_Buffer[address];
#endif
	return data;
}


/**
  * @brief write float number to the flash
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_WriteFloatNum(uint16_t address, float data){
	union{
		float floatNumber;
		uint32_t uintNumb;
	}convNum;

	convNum.floatNumber = data;
#ifdef FSEXT_DIRECT_ACCESS_FLASH
	return FSExt_Write(address, &convNum.uintNumb,1);
#else
	 FSExt_Buffer[address] = convNum.uintNumb;
	 FSExt_SetChngFlag(address);
	 return 1;
#endif
}


/**
  * @brief read float number from the flash
  * @retval 0 = error, 1 = no error
  */
float FSExt_ReadFloatNum(uint16_t address){
	union{
		float floatNumber;
		uint32_t uintNumb;
	}convNum;
#ifdef FSEXT_DIRECT_ACCESS_FLASH
	FSExt_Read(address,  &convNum.uintNumb,1);
#else
	convNum.uintNumb = FSExt_Buffer[address];
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
uint8_t FSExt_Write(uint16_t startAddr, uint32_t *data, uint16_t quantity){
	if(quantity > FSEXT_MAX_STORAGE_SIZE || (startAddr+quantity) > FSEXT_MAX_STORAGE_SIZE) return 0;
	uint8_t bytes[(quantity*dataType)];
	uint32_t size =0 ;
	uint32_t index = 0;
	/*Convert u32 to u8*/
	while(index < quantity){
		do{
			bytes[size] = (uint8_t)((data[index]>>(dataType-1-size%dataType)*8)&0x000000FF);
			size++;
		}while(size%dataType != 0 );
		index++;
	}

	/*Calculate actual memory address*/
	uint32_t address = (uint32_t)((startAddr*dataType) | FSEXT_ADDRESS_OFFSET);

	/*Write data*/
	if(w25qxx_write(w25q, address, bytes, size) !=0) {
		DBG_SPRINT_NL("Error: Flash write failed");
		return 0;
	}else{
		return 1;
	}

}


/**
  * @brief read integer number in the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_Read(uint16_t startAddr, uint32_t *data, uint16_t quantity){
	/*Check quantity & address*/
	if(quantity > FSEXT_MAX_STORAGE_SIZE || (startAddr+quantity) > FSEXT_MAX_STORAGE_SIZE) return 0;
	uint16_t size = (quantity*dataType) ;
	uint8_t bytes[size];

	/*Calculate actual memory address*/
	uint32_t address = (uint32_t)((startAddr*dataType) | FSEXT_ADDRESS_OFFSET);

	/*Read data*/
	uint8_t status = w25qxx_read(w25q, address, bytes, size);

	/*Convert u8 to u32*/
	if(status==0){
		uint32_t j = 0;
		for(uint32_t i = 0; i < quantity; i ++){
			data[i] = (uint32_t)((bytes[j]<<24) | (bytes[j+1]<<16) | (bytes[j+2]<<8) | (bytes[j+3]));
			j += 4;
		}
		return 1;
	}else{
		DBG_SPRINT_NL("Error: Flash read failed");
		return 0;
	}
}




/**
  * @brief erase single value for a address
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_EraseNum(uint16_t address){
	return FSExt_Erase(address, 1);
}



/**
  * @brief erase the flash
  * @param startAddr: data address
  * @param data: data buffer
  * @param quantity: number off data
  * @retval 0 = error, 1 = no error
  */
uint8_t FSExt_Erase(uint16_t startAddr, uint16_t quantity){
	if(quantity >= FSEXT_MAX_STORAGE_SIZE || (startAddr+quantity) >= FSEXT_MAX_STORAGE_SIZE) return 0;
	uint32_t data[quantity];

	for(uint16_t i = 0; i < quantity; i++){
		data[i] = 0xFFFFFFFF;
	}

	uint8_t status = FSExt_Write(startAddr, data, quantity);
	if(status > 0){
		memset(&FSExt_Buffer[startAddr], 0xFF, quantity*sizeof(FSExt_Buffer[0]));
		FSExt_ClearAllChngFlag();
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
uint8_t FSExt_EraseFull(void){
	uint32_t address = (FSEXT_BLOCK_ADDRESS |FSEXT_SECTOR_ADDRESS);
	return w25qxx_sector_erase_4k(w25q, address);
}




/*
 * @brief These functions will track the changes of local flash buffer and update the eeprom
 * automatically if FSExt_CheckChangesAndUpdate() this function is checked periodically
 * */

void FSExt_SetChngFlag( uint16_t address){
	if(buffStatus.count < FSEXT_MAX_STORAGE_SIZE){
		buffStatus.modified[buffStatus.count++] = address;
	}
}

/* @brief clear change flag
 * */
void FSExt_ClearAllChngFlag(void){
	buffStatus.count = 0;
    memset(buffStatus.modified, 0, sizeof(buffStatus.modified));
}


/* @brief check and update the changes
 * */
void FSExt_CheckAndUpdateChanges(void){
	if(buffStatus.count<= 0) return;
	if(FSExt_Write(FSEXT_START_ADDRES, FSExt_Buffer,FSEXT_MAX_STORAGE_SIZE) > 0){
		DBG_SPRINT_NL("Storage: Updated!");
		FSExt_ClearAllChngFlag();
		buffStatus.count = 0;
	}else{
		DBG_SPRINT_NL("Error: Flash write failed!");
	}
}
