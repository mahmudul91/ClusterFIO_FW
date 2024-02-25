/*
 * MB_Handler.c
 *
 *  Created on: Jan 18, 2022
 *      Author: RASEL_EEE
 */

#include <MB_Handler.h>
#include "MB_Addresses.h"
#include "Debug.h"

#include "MB_Register.h"
#include "MB_Diagnostics.h"
#include "MB_Config.h"

#include "MB_Custom.h"

//extern UART_HandleTypeDef huart2;
//extern OD_Global odGlobal;

//static MB_Register mbRegister;




uint8_t (*MB_readDICallback)(MB_DIAddrEnum address, uint8_t *value);
uint8_t (*MB_writeCOCallback)(MB_CoilAddrEnum address, uint8_t value);
uint8_t (*MB_readCOCallback)(MB_CoilAddrEnum address, uint8_t *value);
uint8_t (*MB_readIRCallback)(MB_IRAddrEnum address, uint16_t *value);
uint8_t (*MB_writeHRCallback)(MB_HRAddrEnum address, uint16_t value);
uint8_t (*MB_readHRCallback)(MB_HRAddrEnum address, uint16_t *value);
uint8_t (*MB_maskWriteHRCallback)(MB_HRAddrEnum address, uint16_t andMask, uint16_t orMask);

uint8_t (*MB_changSAcallback)(uint8_t slaveAddress);


MB_RTUHandler mbHandle;



/* TODO
 * 1. To make function for default modbus settings
 * 2. To make function to change modbus settings
 * 3. To make function to restore modbus setting
 *
 * */
/*--------- just for testing----------*/



uint16_t getInputRegisterValue(uint16_t address);
uint8_t getInputRegistersValue(uint16_t *value, uint16_t startAddress, uint16_t lastAddresss);
uint8_t setSingleRegisterValue(uint16_t address, uint16_t value);
uint8_t setRegisterValue(uint16_t *data, uint8_t size);



void MB_DEBUG(char *data, uint8_t size);
/*--------- just for testing----------*/

void MB_resetSlave(void);										// to reset the last processed data



								// to get the output address for single coil or rgister write
uint16_t MB_getMWRRefAddress(void);								// to get the reference address for mask write register
//uint16_t MB_getQuantity(void);								// to get the quantity
uint16_t MB_getQuantity(uint8_t firstByteAddress);				// to get the quantity
uint16_t MB_getSCRValue(void);									// to get the output value for single coil or register

uint16_t MB_getMWRAndMask(void);								// to get the andMask value for mask write register
uint16_t MB_getMWROrMask(void);									// to get the orMask value for mask write register

									// to get the data value for the diagnostics sub-function
uint16_t MB_getNewSlaveAddress(void);							// to get the new slave address



uint8_t MB_checkCoilDataQtyAddr(uint16_t maxAddress);							// to check the coil data quantity and address are valid or not
uint8_t MB_checkRegisterDataQtyAddr(uint16_t maxAddress);						// to check the register data quantity and address are valid or not
uint8_t MB_checkMCQtyAddrByteCount(void);			   			// to check the multiple coils data quantity, address and byte count for the write multiple output coils
uint8_t MB_checkMRQtyAddrByteCount(uint16_t maxAddress);						// to check the multiple registers data quantity, address and byte count for the write multiple holding register
uint8_t MB_checkRWMRQtyAddrByteCount(void);						// to check the register data quantity, address and byte count for the read/write multiple holding register
uint8_t MB_checkMWRAddrMask(void);								// to check the reference address, andMask & orMask for mask write holding register
uint8_t MB_checkDataQtyAddr(uint16_t startingAddress,uint16_t maxAddress, uint16_t quantity, uint16_t maxQuantity);	// to check the data quantity and address for the coils and registers
uint8_t MB_checkNewSlaveAddr(void);								// to check the new slave address

uint8_t MB_checkDataQuantity(uint16_t quantity, uint16_t maxQuantity);	// to check requested data quantity
uint8_t MB_checkDataAddress(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity);	// to check requested data address
uint8_t MB_checkSCOutputValue(uint16_t value);						// to check the single coil output value
uint8_t MB_checkSRValue(uint16_t value);								// to check the single register value
uint8_t MB_checkSCRAddr(uint16_t address, uint16_t maxAddress);
uint8_t MB_checkSRValueAddr(void);									// to check the single register address and value
uint8_t MB_checkAndOrMask(uint16_t andMask, uint16_t orMask);			// to check the valid andMask and orMask
//void MB_responseBuilder(uint8_t *resData, uint8_t size);			// to build up the response PDU

/*Diagnostics*/
uint8_t MB_diagnostics(void);										// to Diagnostic
uint8_t MB_getCEC(void);										// to Diagnostic


uint8_t MB_changeSA(void);											// to change slave address



uint8_t MB_getCoil( uint16_t address, uint8_t *value);
uint8_t MB_getCoilsInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress);




uint16_t MB_getHRValue(uint16_t address);
uint8_t MB_getHRsValue(uint16_t *value, uint16_t startAddress, uint16_t quantity);
uint8_t MB_setMaskWHRValue(uint16_t address, uint16_t andMask, uint16_t orMask);
uint8_t MB_setSlaveAddress(uint8_t salveAddress);




/**
  * @brief To initialize the modbus settings,
  * @param none
  * @retval none
  */
void MB_init(MB_RTUHandler *_mbHandle){
	/*
	 * Slave address should be configurable,
	 * Default slave address will be 01 for all the devices
	 * */
//	mbHandle = _mbHandle;

//	MB_ClearCounters();						// to reset the setting on restart
	//MB_DIAG_Init(_mbHandle);
	mbHandle.is_req_cpy_to_resp = 0U;
	mbHandle.diag.listen_only_mode = 0;
}

void MB_SetRTUMode(MB_RTUMode mode){
	mbHandle.mode = mode;
}

MB_RTUMode MB_GetRTUMode(void){
	return mbHandle.mode;
}


void MB_SetHandlerInstance(MB_RTUHandler *_mbHandle){
	mbHandle = *_mbHandle;
}

MB_RTUHandler *MB_GetHandlerInstance(void){
	return &mbHandle;
}


uint8_t MB_IsHandlerBusy(void){
	return mbHandle.isBusy;
}

void MB_SetHandlerBusy(uint8_t flag){
	mbHandle.isBusy = flag;
}

/**
  * @brief sets requested modbus pdu buffer
  * @param buff: pdu buffer pointer
  * @param size: pdu buffer size
  * @retval none
  */
void MB_setReqBuff(uint8_t *buff, uint8_t size){
	mbHandle.rxBuffer = buff;
	mbHandle.rxBufferSize = size;

}

/**
  * @brief gets response of the requested modbus pdu buffer
  * @param buff: pdu buffer pointer
  * @param size: pdu buffer size
  * @retval none
  */

void MB_setRespBuff(uint8_t *buff){
	mbHandle.txBuffer = buff;
}
/*
 * get response buffer size
 * */
uint8_t MB_getRespBuffSize(void){
	return mbHandle.txBufferSize;
}

/**
  * @brief To execute the request which is received from the master
  * @param functionCode = the request function code
  * @retval 2-6,8,10-11, and 255; where 2-6,8 & 10-11 = exception code, and 255 = no exception occurred
  */
uint8_t MB_executeRequest(uint8_t functionCode){
	uint8_t exeStatus = MB_RS_255;
	mbHandle.txBufferSize = 0u;
	switch (functionCode) {
		case MB_FC_RC_01:
			exeStatus = MB_readCoils();
			break;
		case MB_FC_RDI_02:
			exeStatus = MB_readDI();
			break;
		case MB_FC_RHR_03:
			exeStatus = MB_readHR();
			break;
		case MB_FC_RIR_04:
			exeStatus = MB_readIR();
			break;
		case MB_FC_WSC_05:
			exeStatus = MB_writeSC();
			break;
		case MB_FC_WSR_06:
			exeStatus = MB_writeSHR();
			break;
		case MB_FC_RES_07:
			exeStatus = MB_readES();
			break;
		case MB_FC_DIAG_08:

			exeStatus = MB_diagnostics();					// this function is being used only for modbus serial line
			break;
		case MB_FC_GCEC_11:
			exeStatus = MB_getCEC();						// this function is being used only for modbus serial line
			break;
//		case MB_FC_GCEL_12:
//
//			break;
		case MB_FC_WMC_15:
			exeStatus = MB_writeMC();
			break;
		case MB_FC_WMR_16:
			exeStatus = MB_writeMHR();
			break;
//		case MB_FC_RSI_17:
//
//			break;
//		case MB_FC_RFR_20:
//
//			break;
//		case MB_FC_WFR_21:
//
//			break;
		case MB_FC_MWR_22:
			exeStatus = MB_writeHRM();
			break;
		case MB_FC_RWMR_23:
			exeStatus = MB_readWriteMHR();
			break;
//		case MB_FC_RFQ_24:
//
//			break;
//		case MB_FC_EIT_43:
//
//			break;
//		case MB_FC_CSA_66:
//			exeStatus = MB_changeSA();
//			break;
//		case MB_CFC_RC_101:
//
//			break;
//		case MB_CFC_RDI_102:
//
//			break;
//		case MB_CFC_RHR_103:
//
//			break;
//		case MB_CFC_RIR_104:
//
//			break;
//		case MB_CFC_WMC_105:
//
//			break;
//		case MB_CFC_WMR_106:
//
//			break;
		case MB_CFC_RIRDI_107:

			exeStatus = MBR_CUST_readIRDI();
//			DEBUG_SPRINT("\r\n4.rd ");
			break;
//		case MB_CFC_RHRCO_108:
//
//			break;
		case MB_CFC_WMHRCO_109:
			exeStatus = MBR_CUST_writeHRCO();
			break;
//		case MB_CFC_RMHRIRCODI_110:
//
//			break;
		default:
			exeStatus = MB_EC_ILF_1;
			break;
	}
	return exeStatus;
}


/**
  * @brief To clear the modbus PDU buffer,
  * @param none
  * @retval none
  */
void MB_ClearBuffer(void){
//	memset(mbHandle.rxBuffer,0U,mbHandle.rxBufferSize);
//	memset(mbHandle.txBuffer,0U,mbHandle.txBufferSize);
	mbHandle.rxBufferSize = 0U;
	mbHandle.txBufferSize = 0U;
}

void MB_ClearReqBuffer(void){
	memset(mbHandle.rxBuffer,0U,mbHandle.rxBufferSize);
	mbHandle.rxBufferSize = 0U;
}

void MB_ClearResBuffer(void){
	memset(mbHandle.txBuffer,0U,mbHandle.txBufferSize);
	mbHandle.txBufferSize = 0U;
}

/**
  * @brief It append data to response PDU
  * @param data, data byte to append to response PDU
  * @retval none
  */
void MB_appendToRespPDU(uint8_t data){
	mbHandle.txBuffer[mbHandle.txBufferSize++] = data;
	mbHandle.is_req_cpy_to_resp = 0U;
}


/**
  * @brief It append slave address and function code to response PDU
  * @param none
  * @retval none
  */
void MB_appendToRespPDU_SAFC(void){
	MB_appendToRespPDU(MB_getSlaveAddress());
	MB_appendToRespPDU(MB_getFunctionCode());
}






/**
  * @brief It appends CRC16 to response PDU
  * @param none
  * @retval none
  */
void MB_appendToRespPDU_CRC(void){
	if(!mbHandle.is_req_cpy_to_resp){
		uint16_t crc16 = MB_CalcCRC16(mbHandle.txBuffer, mbHandle.txBufferSize);			// calculates CRC
		MB_appendToRespPDU((uint8_t)(crc16 & 0x00FF));
		MB_appendToRespPDU((uint8_t)(crc16>>8));
	}
	mbHandle.is_req_cpy_to_resp = 0U;
}

/**
  * @brief It copies request PDU to response PDU
  * @param none
  * @retval none
  */
void MB_CopyReqToRespPDU(void){
	memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize);
//	mbHandle.txBuffer = mbHandle.rxBuffer;
	mbHandle.txBufferSize = mbHandle.rxBufferSize;
	mbHandle.is_req_cpy_to_resp = 1U;
}

/**
  * @brief It makes response PDU according to the request from the master
  * @param reqData & size, pointer of the requested data array, and size id the length of the requested data array, not the PDU length
  * @retval none
  */
/*void MB_responseBuilder(uint8_t *resData, uint8_t size){
	uint8_t respPDU[(size+4)];
	respPDU[MB_SA_INDEX] = mbHandle.slave_address;		// sets slave address to the response PDU
	respPDU[MB_FC_INDEX] = MB_getFunctionCode();		// sets function code to the response
	for(uint8_t i = 0; i < size; i++){
		respPDU[(MB_FC_INDEX+1+i)] = resData[i];
	}
#ifdef MB_RTU_SLAVE
	uint16_t crc16 = CRC16(respPDU, (sizeof(respPDU)-2));			// calculates CRC
	respPDU[(MB_FC_INDEX+1+size)] = (uint8_t)(crc16 & 0x00FF);		// sets lower byte first, then higher byte
	respPDU[(MB_FC_INDEX+2+size)] = (uint8_t)(crc16>>8);
#endif

	mbHandle.txBufferSize = size+4;		 					// size of the response PDU
	memcpy(mbHandle.txBuffer, respPDU, mbHandle.txBufferSize);
}*/


/**
  * @brief To build exception response
  * @param None
  * @retval 1 or 0, if calculated CRC is matched with received CRC it will return 1, otherwise it will return 0
  */
void MB_excepRespBuilder(uint8_t excepCode){
	MB_appendToRespPDU(MB_getSlaveAddress());							// appends the slave address to the response PDU
	MB_appendToRespPDU(MB_EFC_OFFSET + MB_getFunctionCode());			// appends the exception function code
	MB_appendToRespPDU(excepCode);										// appends exception code

//	MB_appendToRespPDU_CRC();											// appends CRC16 to the response

}


/**
  * @brief To check the function code
  * @param functionCode - received function code
  * @retval 1U or 0U, returns 1U for valid function code, or return 0U for invalid function code
  */
uint8_t MB_isFunctionCodeValid(uint8_t functionCode){
	// 1-227 is the range of supported function code public + user define function code
	return ((functionCode >= MB_FC_RC_01 && functionCode <= MB_FC_DIAG_08)
	|| (functionCode == MB_FC_GCEC_11 )
	|| (functionCode >= MB_FC_WMC_15 && functionCode <= MB_FC_WMR_16)
	|| (functionCode >= MB_FC_MWR_22 && functionCode <= MB_FC_RWMR_23)
	|| (functionCode == 66u)
	);
}


uint8_t MB_checkSlaveAddress(uint16_t address){
	return ((address >= MB_BROADCAST_ADDR) && (address <= MB_SLAVE_ADDR_MAX));
}
/**
  * @brief To check the valid data quantity and address for the coils
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address exception, 3 = illegal data value exception
  */
uint8_t MB_checkCoilDataQtyAddr(uint16_t maxAddress){
	return MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), maxAddress, MB_getQuantity(MB_FC_INDEX+3), MB_MAX_COILS);
}



/**
  * @brief To check the valid data quantity and address for the registers
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkRegisterDataQtyAddr(uint16_t maxAddress){
	return MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), maxAddress, MB_getQuantity(MB_FC_INDEX+3), MB_MAX_REGISTERS);
}


/**
  * @brief To check the valid data quantity and address for the coils
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  *
  */

uint8_t MB_checkMCQtyAddrByteCount(void){
	uint8_t clc_byte_count = (MB_getQuantity(MB_FC_INDEX+3)/8)+(MB_getQuantity(MB_FC_INDEX+3)%8>0);		// to calculate byte count from the output quantity
	if(MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+3), (MB_COA_MAX-MB_CO_ADDR_OFFSET))
			&& (clc_byte_count == mbHandle.rxBuffer[MB_FC_INDEX+5])){				//multiple coils write checks valid quantity of coils
		if(MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+1), (MB_COA_MAX-MB_CO_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+3))){		// MAX coils/registers address 10000, checks valid data address and address+quantity of coils, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}

}

/**
  * @brief To check the valid data quantity, address and byte count for the registers
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkMRQtyAddrByteCount(uint16_t maxAddress){
	uint8_t clc_byte_count = MB_getQuantity(MB_FC_INDEX+3)*2;		// to calculate byte count from the output quantity
	if(MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+3), (maxAddress-MB_HR_ADDR_OFFSET))
			&& (clc_byte_count == mbHandle.rxBuffer[MB_FC_INDEX+5])){				// Max quantity- 1968 for multiple register write checks valid quantity of coils
		if(MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+1), (maxAddress-MB_HR_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+3))){		// MAX coils/registers address 10000, checks valid data address and address+quantity of register, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}

}


/**
  * @brief To check the valid data quantity, address and byte count for the read/write multiple registers
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkRWMRQtyAddrByteCount(void){
	//uint8_t clc_byte_count = MB_getQuantity(MB_FC_INDEX+3)*2;		// to calculate byte count from the output quantity
	if(MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+3U), (MB_HRA_MAX-MB_HR_ADDR_OFFSET))		// to check the quantity to read the register
		&& MB_checkDataQuantity(MB_getQuantity(MB_FC_INDEX+7U), (MB_HRA_MAX-MB_HR_ADDR_OFFSET))	// to check the quantity to write the register
		&& ((MB_getQuantity(MB_FC_INDEX+7U)*2U) == mbHandle.rxBuffer[MB_FC_INDEX+9U])){				// Max quantity- 125 for the register

		if(MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+1U), (MB_HRA_MAX-MB_HR_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+3U))
			&& MB_checkDataAddress(MB_getStartAddress(MB_FC_INDEX+5U), (MB_HRA_MAX-MB_HR_ADDR_OFFSET), MB_getQuantity(MB_FC_INDEX+7U))){		// MAX coils/registers address 10000, checks valid data address and address+quantity of register, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}

}


/**
  * @brief To check the valid reference address and mask for the mask write holding register
  * @param none
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkMWRAddrMask(void){
	//uint8_t clc_byte_count = MB_getQuantity(MB_FC_INDEX+3)*2;		// to calculate byte count from the output quantity
	if(MB_getMWRRefAddress()>=0 && MB_getMWRRefAddress() <= (MB_HRA_MAX-MB_HR_ADDR_OFFSET)){				// Max quantity- 125 for the register
		if(MB_checkAndOrMask(MB_getMWRAndMask(), MB_getMWROrMask()) && (mbHandle.rxBufferSize==10U)){		// MAX coils/registers address 10000, checks valid data address and address+quantity of register, Max value will not exceed 9999
			return MB_EC_NONE_0;											// returns exception code - 0 for no exception
		}else{
			return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
		}
	}else{
		return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
	}

}


/**
  * @brief To check the valid data quantity and address for the coils and registers
  *
  * @param startAddress, quantity, maxQuantity, where startAddress is the starting address
  * of the requested coils or registers, quantity is the number of requested coils or register
  * and maxQuantity is the max number of coils or registers
  *
  * @retval 0, 2 & 3, where 0 = no exception, 2 = illegal data address, 3 = illegal data value
  */
uint8_t MB_checkDataQtyAddr(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity, uint16_t maxQuantity){
	if(MB_checkDataQuantity(quantity, maxQuantity)){						// Max quantity- 2000 for coils and 125 for registers, checks valid quantity of coils
		if(MB_checkDataAddress(startAddress, maxAddress, quantity)){		// MAX coils/registers address 10000, checks valid data address and address+quantity of coils, Max value will not exceed 9999
			return 0;
		}else{
			return MB_EC_ILDA_2;										// returns exception code - 2 for illegal data address and address+quantity
		}
	}else{
		return MB_EC_ILDV_3;											// returns exception code - 3 for illegal data value/quantity
	}
}




/**
  * @brief To check the valid data quantity for the coils and registers
  *
  * @param quantity, maxQuantity, where, quantity is the number of requested coils or register
  * and maxQuantity is the max number of coils or registers
  *
  * @retval 0, 1 0 = invalid, 1 = valid
  */
uint8_t MB_checkDataQuantity(uint16_t quantity, uint16_t maxQuantity){
	return (quantity >= 1 && quantity <= maxQuantity);
}

/**
  * @brief To check the valid data address for the coils and registers
  *
  * @param startAddress and quantity, where, startAddress is the starting address
  * of the requested coils or registers, quantity is the number of requested coils or register
  *
  * @retval 0, 1 0 = invalid, 1 = valid
  */
uint8_t MB_checkDataAddress(uint16_t startAddress, uint16_t maxAddress, uint16_t quantity){
	uint16_t lastAddress = startAddress + quantity;
	return((startAddress >= 0 && startAddress <= maxAddress)
				&& (lastAddress >= 1 && lastAddress <= maxAddress));
}


uint8_t MB_checkAndOrMask(uint16_t andMask, uint16_t orMask){
	return ((andMask >= 0) && (andMask <= MB_MAX_REGISTER_VALUE))
			&& ((orMask >= 0) && (orMask <= MB_MAX_REGISTER_VALUE));
}




/**
  * @brief To check the output value for the coils
  *
  * @param value, it is the output coil value, output coil can be 0x0000 or 0xFFFF
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSCValue(uint16_t value){
	return (value==LOW || value==HIGH);
}


/**
  * @brief To check the output value for the coils
  *
  * @param value, it is the output coil value, output coil can be 0x0000 or 0xFFFF
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSRValue(uint16_t value){
	return (value>=0x0000 && value<=MB_MAX_REGISTER_VALUE);
}


/**
  * @brief To check the single coil or register address
  *
  * @param address, for the target output coil or register
  *
  * @retval 0 or 1, where 1 = no exception,  0 = exception
  */
uint8_t MB_checkSCRAddr(uint16_t address, uint16_t maxAddress){
	return (address>=0U && address<maxAddress);
}


/**
  * @brief To check the single coil output address and value
  *
  * @param address, for the target output coil or register
  *
  * @retval 0,2& 3, where 0 = no exception, 2 = illegal data address,  3 = illegal data value
  */
uint8_t MB_checkSCValueAddr(void){
	if(MB_checkSCValue(MB_getSCRValue())){
		if(MB_checkSCRAddr(MB_getSCRAddress(), (MB_COA_MAX-MB_CO_ADDR_OFFSET))){
			return 0;
		}else{
			return MB_EC_ILDA_2;
		}
	}else{
		return MB_EC_ILDV_3;
	}

}

/**
  * @brief To check the single register address and value
  *
  * @param address, for the target output coil or register
  *
  * @retval 0,2& 3, where 0 = no exception, 2 = illegal data address,  3 = illegal data value
  */
uint8_t MB_checkSRValueAddr(void){
	if(MB_checkSRValue(MB_getSCRValue())){
		if(MB_checkSCRAddr(MB_getSCRAddress(), (MB_HRA_MAX-MB_HR_ADDR_OFFSET))>0u){
			return 0U;
		}else{
			return MB_EC_ILDA_2;
		}
	}else{
		return MB_EC_ILDV_3;
	}

}


/**
  * @brief To check the new slave address
  * @param none
  * @retval 0 or 3, where 0 = no exception,  3 = illegal data value
  */
uint8_t MB_checkNewSlaveAddr(void){
	if(MB_getNewSlaveAddress() == (uint8_t)(~mbHandle.rxBuffer[MB_FC_INDEX+2U])){
		return MB_EC_NONE_0;
	}else{
		return MB_EC_ILDV_3;
	}

}






uint16_t MB_get2byteData(uint8_t firstByteIndex){
	return ((uint16_t)mbHandle.rxBuffer[firstByteIndex]<<8u | (uint16_t)mbHandle.rxBuffer[firstByteIndex+1u]);
}

/**
  * @brief To get slave address
  * @param none
  * @retval 0-127
  */
uint8_t MB_getSlaveAddress(void){
	return mbHandle.rxBuffer[MB_SA_INDEX];
}

/**
  * @brief To get the function code from request PDU buffer
  * @param none
  * @retval 0-127
  */
uint8_t MB_getFunctionCode(void){
	return mbHandle.rxBuffer[MB_FC_INDEX];
}


///**
//  * @brief To get the data start address (without offset) from request PDU buffer
//  * @param none
//  * @retval 0-9999
//  */
//uint16_t MB_getStartAddress(void){
//	return MB_getStartAddress(MB_FC_INDEX+1);//(mbHandle.rxBuffer[MB_FC_INDEX+1]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+2]);
//}

/**
  * @brief To get the data start address (without offset) from request PDU buffer
  * @param none
  * @retval 0-9999
  */
uint16_t MB_getStartAddress(uint8_t firstByteIndex){
	return MB_get2byteData(firstByteIndex);
}

uint16_t MB_getSCRAddress(void){
	return MB_get2byteData(MB_FC_INDEX+1);
}




/**
  * @brief To get reference address (without offset) for mask write register from request PDU buffer
  * @param none
  * @retval 0-9999, holding register reference address without offset
  */
uint16_t MB_getMWRRefAddress(void){
	return MB_get2byteData(MB_FC_INDEX+1);
}


///**
//  * @brief To get the requested data quantity from request PDU buffer
//  * @param none
//  * @retval 1-2000 or 1-125, 1-2000 for coils and discrete input and 1-125 for registers
//  */
//uint16_t MB_getQuantity(void){
//	return MB_getQuantity(MB_FC_INDEX+3);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
//}

/**
  * @brief To get the requested data quantity from request PDU buffer
  * @param none
  * @retval 1-2000 or 1-125, 1-2000 for coils and discrete input and 1-125 for registers
  */
uint16_t MB_getQuantity(uint8_t firstByteIndex){
	return MB_get2byteData(firstByteIndex);
}


/**
  * @brief To get the requested data output value from the request PDU buffer
  * @param none
  * @retval the output value for single coil or register
  */
uint16_t MB_getSCRValue(void){
	return MB_get2byteData(MB_FC_INDEX+3);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
}

/**
  * @brief To get the requested data output value from the request PDU buffer
  * @param none
  * @retval the output value for single coil or register
  */
uint8_t MB_getMCValueInByte(uint8_t index){
	return mbHandle.rxBuffer[index];//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
}

/**
  * @brief To get the requested data output value from the request PDU buffer
  * @param none
  * @retval the output value for single coil or register
  */
uint16_t MB_getMRValueIn2Byte(uint8_t index){
	return MB_get2byteData(index);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4])
}

/**
  * @brief To get the AND mask for the mask write register from the requested PDU buffer
  * @param none
  * @retval 0x0000-0xFFFF, returns AND mask for mask write register
  */
uint16_t MB_getMWRAndMask(void){
	return MB_get2byteData(MB_FC_INDEX+3);//(mbHandle.rxBuffer[MB_FC_INDEX+3]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+4]);
}


/**
  * @brief To get the OR mask for the mask write register from the requested PDU buffer
  * @param none
  * @retval 0x0000-0xFFFF, returns OR mask for mask write register
  */
uint16_t MB_getMWROrMask(void){
	return MB_get2byteData(MB_FC_INDEX+5);//(mbHandle.rxBuffer[MB_FC_INDEX+5]<<8 | mbHandle.rxBuffer[MB_FC_INDEX+6]);
}


/**
  * @brief To get the new slave address from the requested PDU buffer
  * @param none
  * @retval returns the new slave address
  */
uint16_t MB_getNewSlaveAddress(void){
	return mbHandle.rxBuffer[MB_FC_INDEX+1];
}




/**
  * @brief To read the coils status (FC-01) and make the response PDU
  * @param none
  * @retval 2-4 & 255, where 2-4 = exception occurred , 255 = no exception occurred
  */
uint8_t MB_readCoils(void){
	uint8_t excepCode = MB_checkDataQtyAddr( MB_getStartAddress(MB_FC_INDEX+1), (MB_COA_MAX-MB_CO_ADDR_OFFSET),
			MB_getQuantity(MB_FC_INDEX+3), (MB_COA_MAX-MB_CO_ADDR_OFFSET) );//MB_checkCoilDataQtyAddr(MB_CA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_CO_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t data[byteCount];

		if(MB_getCoilsInByte(data, byteCount, startAddress, endAddress)){
			MB_appendToRespPDU_SAFC();							// to append slave address and function code to response PDU
			MB_appendToRespPDU(byteCount);  // total response byte
			for(uint8_t i = 0; i < byteCount; i++){
				MB_appendToRespPDU(data[i]);
			}
//			MB_appendToRespPDU_CRC();
			//HAL_UART_Transmit(&huart2, mbHandle.txBuffer, mbHandle.txBufferSize, MB_TRANS_TIMEOUT);
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;								// returns exception code 04
		}

		/*------- Test end---------*/

	}else{													// if exception occurred
		return excepCode;
	}
}

/**
  * @brief To read the discrete input status (FC-02) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_readDI(void){
	uint8_t excepCode = MB_checkDataQtyAddr( MB_getStartAddress(MB_FC_INDEX+1), (MB_DIA_MAX-MB_DI_ADDR_OFFSET),
						MB_getQuantity(MB_FC_INDEX+3), (MB_DIA_MAX-MB_DI_ADDR_OFFSET) );//MB_checkCoilDataQtyAddr(MB_DIA_MAX);
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/
		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_DI_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t data[byteCount];
		if(MB_getDIInByte(data, byteCount, startAddress, endAddress)){
			MB_appendToRespPDU_SAFC();							// to append slave address and function code to response PDU
			MB_appendToRespPDU(byteCount);  // total response byte

			for(uint8_t i = 0; i < byteCount; i++){
				MB_appendToRespPDU(data[i]);
			}
//			MB_appendToRespPDU_CRC();
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;								// returns exception code 04
		}

	}else{													// if exception occurred
		return excepCode;
	}

}

/**
  * @brief To read the holding Register (FC-03) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_readHR(void){
	uint8_t excepCode =  MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), (MB_HRA_MAX-MB_HR_ADDR_OFFSET),
						 MB_getQuantity(MB_FC_INDEX+3), (MB_HRA_MAX-MB_HR_ADDR_OFFSET));//MB_checkRegisterDataQtyAddr(MB_HRA_MAX);
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/

		uint8_t size = MB_getQuantity(MB_FC_INDEX+3);		// get the requested quantity
		uint16_t data[size];
		uint16_t startAddress =  MB_getStartAddress(MB_FC_INDEX+1)+MB_HR_ADDR_OFFSET;
		uint16_t lastAddress = startAddress+size-1;
		// retrieve registers value and make the response

		if(MB_getHRsValue(data, startAddress, lastAddress)){

			uint16_t andMask = 0x00FF;
			uint8_t byteCount = size*2;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU((uint8_t)(data[i]>>8));
				MB_appendToRespPDU((uint8_t)(data[i] & andMask));

			}

//			MB_appendToRespPDU_CRC();

			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
  * @brief To read the input register(FC-04) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_readIR(void){
	uint8_t excepCode = MB_checkDataQtyAddr(MB_getStartAddress(MB_FC_INDEX+1), (MB_IRA_MAX-MB_IR_ADDR_OFFSET),
						MB_getQuantity(MB_FC_INDEX+3), (MB_IRA_MAX-MB_IR_ADDR_OFFSET));//MB_checkRegisterDataQtyAddr(MB_IRA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint8_t size = MB_getQuantity(MB_FC_INDEX+3);
		uint16_t data[size];
		uint16_t startAddress =  MB_getStartAddress(MB_FC_INDEX+1)+MB_IR_ADDR_OFFSET;
		uint16_t lastAddress = startAddress+size-1;

		if(MB_getIRsValue(data, size,  startAddress, lastAddress)){


			uint8_t byteCount = size*2;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU((uint8_t)(data[i]>>8));
				MB_appendToRespPDU((uint8_t)(data[i] & MB_AND_MASK_16T8));
			}
			return MB_RS_255;
		}else{

			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
  * @brief To write the single coil(FC-05) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_writeSC(void){
	uint8_t excepCode = MB_checkSCValueAddr();
	if(excepCode == 0){										// if there is no exception
		/*------- Test start---------*/

		if(MB_setCoil(MB_getSCRAddress()+MB_CO_ADDR_OFFSET, MB_getSCRValue())){
			memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize); // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			mbHandle.txBufferSize = mbHandle.rxBufferSize; // 2 is subtracted, case of 2 byte crc will add at the end of the processing

			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}


		/*------- Test end---------*/

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
  * @brief To write the single register(FC-06) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_writeSHR(void){
	uint8_t excepCode = MB_checkSRValueAddr();
	if(excepCode == 0U){										// if there is no exception
		if(MB_setHRValue(MB_getSCRAddress()+MB_HR_ADDR_OFFSET, MB_getSCRValue()) == MB_OK){//setSingleRegisterValue(MB_getSCRAddress()+MB_HR_ADDR_OFFSET, MB_getSCRValue())){
			memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize);  // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			if((MB_getSCRAddress()+MB_HR_ADDR_OFFSET) == MB_HRA_MB_SLAVE_ADDRESS){
				mbHandle.txBuffer[MB_SA_INDEX] = MB_getSlaveAddress();
			}
			mbHandle.txBufferSize = mbHandle.rxBufferSize; // 2 is subtracted, case of 2 byte crc will add at the end of the processing
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}
	}else{													// if exception occurred
		return excepCode;
	}
}


/**
  * @brief To Read Exception Status (FC-07) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_readES(void){
	/*------- Test start---------*/
	//TODO: it is not implement properly, later we will implement
	uint8_t excepStatus = MB_GetExcepStatus();
	if( excepStatus >= 0 && excepStatus <= 255u){										// if there is no exception
		MB_appendToRespPDU_SAFC();
		MB_appendToRespPDU(excepStatus);
//		MB_appendToRespPDU_CRC();
		return MB_RS_255;
	}else{													// if exception occurred
		return MB_EC_SDV_4;
	}

	/*------- Test end---------*/
}


/**
  * @brief Diagnostics (FC-08)
  * @param none
  * @retval status code for exception and non exception
  */
uint8_t  MB_diagnostics(void){
	if(MB_DIAG_CheckDSFC(MB_getDiagSFC())){						// to check the sub-function code for diagnostics
		if(MB_getDiagSFC() == MB_DSFC_RQD_0){					// if the sub-function code is 0
			/*
			 * here, 6 	means 6 byte which is equal to the summation of bytes of
			 * 			the 1-byte SA, 1-byte FC, 2-byte SFC and 2-byte CRC (1+1+2+2 = 6)
			 *   	 2 means 2 bytes of data,
			 * where, the requested pdu buffer is subtracting by 6 as a result we have only
			 * the number of data bytes and dividing by 2 to check there is an even number
			 * of data bytes. Because, each data contains 2 bytes. otherwise it will through an exception
			 *
			 * */
			return MB_DSF_ReturnQD(&mbHandle.diag);
		}else{

			if(MB_DIAG_CheckData(MB_getDiagSFC(), MB_getDiagData())){
				uint8_t status = MB_EC_SDV_4;
				switch (MB_getDiagSFC()) {
//					case MB_DSFC_RQD_0:
//						break;
					case MB_DSFC_RCO_1:
						status = MB_DSF_RestartCO(&mbHandle.diag);
						break;
					case MB_DSFC_RDR_2:
						status = MB_DSF_ReturnDR(&mbHandle.diag);
						break;
//					case MB_DSFC_CAID_3:
//
//						break;
					case MB_DSFC_FLOM_4:
						status = MB_DSF_ForceLOM(&mbHandle.diag);
						break;
					case MB_DSFC_CCDR_10:
						status = MB_DSF_ClearCDR(&mbHandle.diag);
						break;
					case MB_DSFC_RBMC_11:
						status = MB_DSF_ReturnBMC(&mbHandle.diag);
						break;
					case MB_DSFC_RBCEC_12:
						status = MB_DSF_ReturnBCEC(&mbHandle.diag);
						break;
					case MB_DSFC_RBEEC_13:
						status = MB_DSF_ReturnBEEC(&mbHandle.diag);
						break;
					case MB_DSFC_RSMC_14:
						status = MB_DSF_ReturnSMC(&mbHandle.diag);
						break;
//					case MB_DSFC_RSNRC_15:
//
//						break;
//					case MB_DSFC_RSNC_16:
//
//						break;
//					case MB_DSFC_RSBC_17:
//
//						break;
					case MB_DSFC_RBCOC_18:
						status = MB_DSF_ReturnBCOC(&mbHandle.diag);
						break;
					case MB_DSFC_COCF_20:
						status = MB_DSF_ClearOCF(&mbHandle.diag);
						break;
					default:
						status = MB_EC_SDV_4;
						break;
				}
				return status;
			}else{
				return MB_EC_ILDV_3;						// if data value of the diagnostics sub-function is not valid, it will returns exception code 3
			}
		}
	}else{
		return MB_EC_ILF_1;						// if diagnostics sub-function code is not valid, it will returns exception code 1
	}
}


/**
  * @brief to Get Comm Event Counter (FC-11) and make the response PDU
  * @param none
  * @retval status code for exception and non exception
  */
uint8_t MB_getCEC(void){


		/*------- Test start---------*/
		uint16_t statusWord = MB_GetBusyStatus() == MB_STATUS_BUSY? MB_MAX_REGISTER_VALUE : 0;
		uint16_t eventCount = mbHandle.diag.comm_event_count;
		if((statusWord == 0 || statusWord == MB_MAX_REGISTER_VALUE)
			&& (eventCount >= 0 && eventCount <= MB_MAX_REGISTER_VALUE)){
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU((uint8_t)(statusWord>>8));
			MB_appendToRespPDU((uint8_t)(statusWord & MB_AND_MASK_16T8));
			MB_appendToRespPDU((uint8_t)(eventCount>>8));
			MB_appendToRespPDU((uint8_t)(eventCount & MB_AND_MASK_16T8));
//			MB_appendToRespPDU_CRC();
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}

}


/**
  * @brief To write the multiple coils(FC-15) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_writeMC(void){

	uint8_t excepCode = MB_checkMCQtyAddrByteCount();		//TODO: an error is found there, have to solve it
	if(excepCode == 0){										// if there is no exception

		/*------- Test start---------*/


		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_CO_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3);
		uint8_t byteCount = (MB_getQuantity(MB_FC_INDEX+3)/8)+((MB_getQuantity(MB_FC_INDEX+3)%8)>0);
		uint16_t startByteIndex = (MB_FC_INDEX+6);
		//uint16_t lastByteIndex = (startByteIndex + byteCount-1);

		for(uint8_t byteIndex = 0; byteIndex < byteCount; byteIndex++){
			uint8_t data = MB_getMCValueInByte((startByteIndex+byteIndex));
			for(uint8_t bitIndex = 0; bitIndex < 8U; bitIndex++){
				//uint8_t bitValue = ((data>>bitIndex)&0x01);
				uint16_t coilAddress = startAddress+(byteIndex*8U)+bitIndex;
				if(coilAddress>=startAddress && coilAddress < endAddress){
					if(MB_setCoil(coilAddress, ((data>>bitIndex)&0x01)) == MB_ERROR){
						return MB_EC_SDV_4;
					}
				}else{
					break;
				}
			}
		}


			uint16_t andMask = 0x00FF;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1)>>8));
			MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1) & andMask));
			MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3)>>8));
			MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3) & andMask));
//			MB_appendToRespPDU_CRC();
		return MB_RS_255;
	}else{													// if exception occurred
		return excepCode;
	}
}

/**
  * @brief To write the multiple register(FC-16) and make the response PDU
  * @param none
  * @retval none
  */
uint8_t MB_writeMHR(void){
	uint8_t excepCode = MB_checkMRQtyAddrByteCount(MB_HRA_MAX);
	if(excepCode == 0){										// if there is no exception

		uint16_t startAddress = MB_getStartAddress(MB_FC_INDEX+1)+MB_HR_ADDR_OFFSET;
		uint16_t endAddress = startAddress + MB_getQuantity(MB_FC_INDEX+3)-1;
//		uint8_t byteCount = MB_getQuantity(MB_FC_INDEX+3);
		//uint16_t startByteIndex = (MB_FC_INDEX+6);
		//uint16_t lastByteIndex = (startByteIndex + byteCount-1);
		uint8_t byteIndex = (MB_FC_INDEX+6);
		for(uint16_t regAddress = startAddress; regAddress <= endAddress; regAddress++){
			uint16_t data = MB_getMRValueIn2Byte((byteIndex));
			byteIndex += 2;
			if(MB_setHRValue(regAddress, data) == MB_ERROR){
				return MB_EC_SDV_4;
			}
		}

		uint16_t andMask = 0x00FF;
		MB_appendToRespPDU_SAFC();
		MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1)>>8));
		MB_appendToRespPDU((uint8_t)(MB_getStartAddress(MB_FC_INDEX+1) & andMask));
		MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3)>>8));
		MB_appendToRespPDU((uint8_t)(MB_getQuantity(MB_FC_INDEX+3) & andMask));
//		MB_appendToRespPDU_CRC();

		if((MB_getSCRAddress()+MB_HR_ADDR_OFFSET) == MB_HRA_MB_SLAVE_ADDRESS){
			mbHandle.txBuffer[MB_SA_INDEX] = MB_getSlaveAddress();
		}

		return MB_RS_255;
	}else{													// if exception occurred
		return excepCode;
	}
}




/**
  * @brief To write mask to the holding register(FC-22) and make the response PDU
  * @param none
  * @retval 2,3,4 & 255, where 2-4 for the Modbus exception code, and 255 for no exception
  */
uint8_t MB_writeHRM(void){
	uint8_t excepCode = MB_checkMWRAddrMask();
	if(excepCode == 0){										// if there is no exception

		if((MB_getMWRRefAddress()+MB_HR_ADDR_OFFSET) != MB_HRA_MB_SLAVE_ADDRESS){	//if the reference address is not slave address, mask write register option is not applicable for slave address

			if(MB_setMaskWHRValue((MB_getMWRRefAddress()+MB_HR_ADDR_OFFSET), MB_getMWRAndMask(), MB_getMWROrMask())){

				memcpy(mbHandle.txBuffer, mbHandle.rxBuffer, mbHandle.rxBufferSize);
				mbHandle.txBufferSize = mbHandle.rxBufferSize;
				//HAL_UART_Transmit(&huart2, mbHandle.rxBuffer, mbHandle.rxBufferSize, MB_TRANS_TIMEOUT);
				return MB_RS_255;
			}else{
				return MB_EC_SDV_4;									// exception - 4 ,  service device failure
			}
		}else{
			return MB_EC_ILDA_2;
		}
	}else{													// if exception occurred
		return excepCode;
	}
}



/**
  * @brief To read/write the multiple register(FC-23) and make the response PDU
  * @param none
  * @retval 2,3,4 & 255, where 2-4 for the Modbus exception code, and 255 for no exception
  */
uint8_t MB_readWriteMHR(void){
	uint8_t excepCode = MB_checkRWMRQtyAddrByteCount();
	if(excepCode == 0){										// if there is no exception
		/*------- Test start---------*/



		uint16_t startAddressRead = MB_getStartAddress(MB_FC_INDEX+1)+MB_HR_ADDR_OFFSET;
		uint8_t quantityRead = MB_getQuantity(MB_FC_INDEX+3);
		uint16_t endAddressRead = startAddressRead + quantityRead-1;

		uint16_t startAddressWrite = MB_getStartAddress(MB_FC_INDEX+5)+MB_HR_ADDR_OFFSET;
		uint16_t endAddressWrite = startAddressWrite + MB_getQuantity(MB_FC_INDEX+7)-1;
		//uint8_t  byteCountWrite = mbHandle.rxBuffer[MB_FC_INDEX+9]/2;
		uint8_t byteIndex = (MB_FC_INDEX+10);

		// write register
		for(uint16_t regAddress = startAddressWrite; regAddress <= endAddressWrite; regAddress++){
			uint16_t data = MB_getMRValueIn2Byte((byteIndex));
			byteIndex += 2;
			if(MB_setHRValue(regAddress, data) == MB_ERROR){
				return MB_EC_SDV_4;
			}
		}

		//  read register
		uint16_t data[quantityRead];
		if(MB_getHRsValue(data, startAddressRead, endAddressRead)){

			uint16_t andMask = 0x00FF;
			uint8_t byteCount = quantityRead*2;
			MB_appendToRespPDU_SAFC();
			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < quantityRead; i++){
				MB_appendToRespPDU((uint8_t)(data[i]>>8));
				MB_appendToRespPDU((uint8_t)(data[i] & andMask));
			}
//			MB_appendToRespPDU_CRC();

			if((MB_getSCRAddress()+MB_HR_ADDR_OFFSET) == MB_HRA_MB_SLAVE_ADDRESS){
				mbHandle.txBuffer[MB_SA_INDEX] = MB_getSlaveAddress();
			}
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

		/*------- Test end---------*/

	}else{													// if exception occurred
		return excepCode;
	}
}


/**
  * @brief To change the slave address(FC-66) and make the response PDU
  * @param none
  * @retval 2,3,4 & 255, where 2-4 for the Modbus exception code, and 255 for no exception
  */
uint8_t MB_changeSA(void){
	uint8_t excepCode = MB_checkNewSlaveAddr();
		if(excepCode == 0){										// if there is no exception
			/*------- Test start---------*/

			if(MB_setSlaveAddress(MB_getNewSlaveAddress())){
				MB_appendToRespPDU(MB_getSlaveAddress());
				MB_appendToRespPDU(MB_getFunctionCode());
				MB_appendToRespPDU(0);
//				MB_appendToRespPDU_CRC();

				return MB_RS_255;
			}else{
				return MB_EC_SDV_4;									// exception - 4 ,  service device failure
			}

			/*------- Test end---------*/

		}else{													// if exception occurred
			return excepCode;
		}
}




void MB_applyMaskToHR(uint16_t *value, uint16_t andMask, uint16_t orMask){
	*value =((*value & andMask) | (orMask & (~andMask)));
}



/*--------- callback function ------------*/
/**
  * @brief callback for Restart Communications Option
  * @param none
  * @retval none
  */
void MB_CallbackRCO(void){
	NVIC_SystemReset();		// to reset the mcu
}







uint8_t MB_getDI(uint16_t address, uint8_t *value){
	return MB_readDICallback(address, value);
}

uint8_t MB_getDIInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress){
	volatile uint8_t byteCount = 0;
	volatile uint8_t dataByte = 0;
	volatile uint8_t bitCount = 0;

	for(uint16_t i = startAddress; i < endAddress; i++){
		uint8_t data = 0;
		if(MB_getDI(i, &data)){
			if(data >=0 && data <=1){
				dataByte |= data<<(bitCount++);
				if(bitCount>=8){
					if(byteCount < size){
						byteArray[byteCount++] = dataByte;
						dataByte = 0;
						bitCount=0;
					}else{
						return MB_ERROR;
					}
				}
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}
	if( bitCount > 0 && bitCount < 8){
		if(byteCount < size){
			byteArray[byteCount++] = dataByte;
		}else{
			return MB_ERROR;
		}
		//byteCount++;
	}

	return MB_OK;
}




/**
  * @brief sets coil output
  * @param address: address of the holding register
  * @retval holding register value
  */
uint8_t MB_setCoil(uint16_t address, uint16_t value){

	return MB_writeCOCallback(address, (value > 0));
	//return (address-MB_C_ADDR_OFFSET >= 0 && address-MB_C_ADDR_OFFSET <= MB_MAX_CR_ADDR);
}


uint8_t MB_getCoil( uint16_t address, uint8_t *value){
	return MB_readCOCallback(address, value);
}
uint8_t MB_getCoilsInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress){
	uint8_t byteCount = 0;
	uint8_t dataByte = 0;
	uint8_t bitCount = 0;

	for(uint16_t i = startAddress; i < endAddress; i++){
		uint8_t data=0;
		if(MB_getCoil(i, &data)){
			if(data >=0 && data <=1){
				dataByte |= data<<(bitCount++);
				if(bitCount>=8){
					if(byteCount < size){
						byteArray[byteCount++] = dataByte;
						dataByte = 0;
						bitCount=0;
					}else{
						return MB_ERROR;
					}
				}
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}
	if( bitCount > 0 && bitCount < 8){
		if(byteCount < size){
			byteArray[byteCount++] = dataByte;
		}else{
			return MB_ERROR;
		}
		//byteCount++;
	}

	return MB_OK;
}


uint8_t MB_getIRValue(uint16_t address, uint16_t *value){
	return MB_readIRCallback(address, value);
}

uint8_t MB_getIRsValue(uint16_t *value, uint8_t size, uint16_t startAddress, uint16_t lastAddresss){
	for(uint16_t i = startAddress; i <= lastAddresss; i++){
		uint16_t data;
		if(MB_getIRValue(i, &data)){
			if(data >= 0x0000 && data <= 0xFFFF){
				value[i-startAddress] = data;
			}else{
				return MB_ERROR;
			}
		}else{
			return MB_ERROR;
		}
	}

	return MB_OK;
}


/**
  * @brief gets single holding register data
  * @param address: address of the holding register
  * @retval holding register value
  */
uint16_t MB_getHRValue(uint16_t address){
	uint16_t value;
	MB_readHRCallback(address, &value);
	return value;
}






/**
  * @brief gets multiple holding register data
  * @param pData: pointer of the data
  * @param startAddress: start address to be read
  * @param lastAddresss: last address of the register
  * @retval return error if an exception happen
  */
uint8_t MB_getHRsValue(uint16_t *pData, uint16_t startAddress, uint16_t lastAddresss){
	//DEBUG_PRINT("getHR_ok\r\n");

	for(uint16_t address = startAddress; address <= lastAddresss; address++){
		uint16_t data = 0U;
		//DEBUG_PRINT_INT("address: ", address, ",\r\n ");
		if(MB_readHRCallback(address, &data)){
			//DEBUG_PRINT_INT(" OK address: ", address, ",\r\n ");
			if(data >= 0x0000 && data <= 0xFFFF){
				pData[address-startAddress] = data;
			}else{
				return MB_ERROR;
			}
		}else{
			//DEBUG_PRINT_INT(" ERROR address: ", address, ",\r\n ");
			return MB_ERROR;
		}
	}

	return MB_OK;
}




/**
  * @brief sets single holding register data
  * @param address: address to be write the register
  * @retval return error if an exception happen
  */
uint8_t MB_setHRValue(uint16_t address, uint16_t data){
	//DEBUG_PRINT("SHR_OK\r\n");
	return MB_writeHRCallback(address, data);
	 //return (mbRegister.HR[address - MB_HR_ADDR_OFFSET] == data);
}


uint8_t MB_setMaskWHRValue(uint16_t address, uint16_t andMask, uint16_t orMask){

	return MB_maskWriteHRCallback(address, andMask, orMask);
	 //return (mbRegister.HR[address - MB_HR_ADDR_OFFSET] == data);
}




uint8_t MB_setSlaveAddress(uint8_t salveAddress){
//	mbHandle.slave_address = salveAddress;
	return MB_changSAcallback(salveAddress);
}




/*------ Functions for the diagnostics ----------*/
/**
  * @brief count Bus Msg counter by 1, max count 65535 times
  */
void MB_CountBusMsg(void){
	mbHandle.diag.bus_msg_count ++;				// increments the bus message counter when a bus message is detected
}


/**
  * @brief count Bus communication error by 1, max count 65535 times
  */
void MB_CountBusComErr(void){
	mbHandle.diag.bus_com_err_count++;
}



/**
  * @brief count Bus exception error by 1, max count 65535 times
  */
void MB_CountBusExpErr(void){
	mbHandle.diag.bus_exp_err_count++;
}

void MB_CountSlaveMsg(void){
	mbHandle.diag.slv_msg_count++;
}

/**
  * @brief count Bus overrun counter by 1, max count 65535 times
  */
void MB_CountBusCharOvrun(void){ 							// count bus char overrun counter by 1
	mbHandle.diag.bus_char_ovrun_count++;
}

void MB_CountCommEvent(void){
	mbHandle.diag.comm_event_count++;
}

/*Functions for busy status*/
uint8_t MB_GetBusyStatus(void){
	return mbHandle.busy_status;
}
void MB_SetBusyStatus(void){
	mbHandle.busy_status = MB_STATUS_BUSY;
}

void MB_ClearBusyStatus(void){
	mbHandle.busy_status = MB_STATUS_OK;
}

/*Functions for listen only mode*/
uint8_t MB_GetListenOnlyMode(void){
	return mbHandle.diag.listen_only_mode;
}
void MB_SetListenOnlyMode(void){
	mbHandle.diag.listen_only_mode = 1U;
}
void MB_ClearListenOnlyMode(void){
	mbHandle.diag.listen_only_mode = 0U;
}


/*Functions for Restart flag for restart communication option*/
uint8_t MB_GetRestartFlag(void){
	return mbHandle.diag.device_restart_flag;
}
void MB_SetRestartFlag(void){
	mbHandle.diag.device_restart_flag = 1U;
}

void MB_CleartRestartFlag(void){
	mbHandle.diag.device_restart_flag = 0u;
}




/**
  * @brief It get the diagnostics register data
  * @param none
  * @retval 0,1 whare, 0 means exception and 1 means no exception
  */
uint16_t MB_GetDiagRegValue(void){
	return mbHandle.diag.diagReg.value;
}

/**
  * @brief It get the diagnostics register data
  * @param none
  * @retval 0,1 whare, 0 means exception and 1 means no exception
  */
void MB_setDiagRegValue(uint8_t bitAddr, uint16_t bitValue){
	uint16_t mask = 0x0001;
	mbHandle.diag.diagReg.value = bitValue > 0U ? (mbHandle.diag.diagReg.value|(mask<<bitAddr)) : (mbHandle.diag.diagReg.value&(~(mask<<bitAddr))) ;
}

/**
  * @brief It set the diagnostics register data
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
void MB_ClearDiagReg(void){

	mbHandle.diag.diagReg.value = 0u;
}



uint8_t MB_GetExcepStatus(void){
	return mbHandle.excepStatus;
}

void MB_SetExcepStatus(uint8_t regAddress, uint8_t bitValue){
	uint8_t mask = 0x01;
	mbHandle.excepStatus = bitValue > 0U ? (mbHandle.excepStatus|(mask<<regAddress)) : (mbHandle.excepStatus&(~(mask<<regAddress))) ;

}

uint8_t MB_ClearExcepStatus(void){
	mbHandle.excepStatus = 0U;
	return mbHandle.excepStatus == 0U;
}




/*------ attach callback----------*/
// to attach callback function for discrete input
void MB_attachDICallback(uint8_t (*callbackReadFunc)( MB_DIAddrEnum address, uint8_t *value)){
	MB_readDICallback = callbackReadFunc;
}

// to attach callback function for coil output
void MB_attachCOCallback(uint8_t (*callbackWriteFunc)(MB_CoilAddrEnum address, uint8_t value), uint8_t (*callbackReadFunc)(MB_CoilAddrEnum address, uint8_t *value)){
	MB_writeCOCallback = callbackWriteFunc;
	MB_readCOCallback = callbackReadFunc;
}

// to attach callback function for input register
void MB_attachIRCallback(uint8_t (*callbackReadFunc)(MB_IRAddrEnum address, uint16_t *value)){
	MB_readIRCallback = callbackReadFunc;
}

// to attach callback function for holding register
void MB_attachHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t value), uint8_t (*callbackReadFunc)(MB_HRAddrEnum address, uint16_t *value)){
	MB_writeHRCallback = callbackWriteFunc;
	MB_readHRCallback = callbackReadFunc;
}

void MB_attachMaskWHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t andMask, uint16_t orMask)){
	MB_maskWriteHRCallback = callbackWriteFunc;
}

void MB_attachChangeSACllback(uint8_t (*callbackFunc)(uint8_t slaveAddress)){
	MB_changSAcallback = callbackFunc;
}


/*--------- Just for testing start ------------*/
//uint8_t getCoilStatus(uint16_t address){
//	uint8_t bitMask = 0b00000001;
//	uint8_t byteAddress = (address-MB_C_ADDR_OFFSET)/8;	        // 8 bit in a byte
//	uint8_t bitIndex = (address - (byteAddress*8))-MB_C_ADDR_OFFSET;   // bit index in a byte;
//	//return (coils[byteAddress]&(bitMask<<bitIndex))>0;
//	return 0;
//}
//
//
//uint8_t getCoilsStatusInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress){
//	volatile uint8_t byteCount = 0;
//	volatile uint8_t dataByte = 0;
//	volatile uint8_t bitCount = 0;
//
//	for(uint16_t i = startAddress; i <= endAddress; i++){
//		uint8_t data = getCoilStatus(i);
//		if(data >=0 && data <=1){
//			dataByte |= data<<(bitCount++);
//			if(bitCount>=8){
//				if(byteCount < size){
//					byteArray[byteCount++] = dataByte;
//					dataByte = 0;
//					bitCount=0;
//				}else{
//					return 0;
//				}
//			}
//		}else{
//			return 0;
//		}
//	}
//	if( bitCount > 0 && bitCount < 8){
//		if(byteCount < size){
//			byteArray[byteCount++] = dataByte;
//		}else{
//			return 0;
//		}
//		//byteCount++;
//	}
//
//	return 1;
//}







uint8_t setSingleRegisterValue(uint16_t address, uint16_t value){
	return (address-MB_HR_ADDR_OFFSET>=0 && address-MB_HR_ADDR_OFFSET<=MB_MAX_CR_ADDR);
}

uint8_t setRegisterValue(uint16_t *data, uint8_t size){
	return (size>0);
}




//void MB_DEBUG(char *data, uint8_t size){
//
//	HAL_UART_Transmit(&huart2, (uint8_t *)data, size, MB_TRANS_TIMEOUT);
//}



/*---------- Just for testing end -------------*/


