/*
 * MB_Register.c
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#include "MB_Register.h"
#include "Debug.h"




static MB_Register REG = {0};


uint8_t (*MB_REG_ChangeDICallback)(uint32_t address, uint16_t value);
uint8_t (*MB_REG_ChangeCOCallback)(uint32_t address, uint16_t value);
uint8_t (*MB_REG_ChangeIRCallback)(uint32_t address, uint16_t value);
uint8_t (*MB_REG_ChangeHRCallback)(uint32_t address, uint16_t value);




void MB_REG_Init(void){
	memset(REG.CO, 0, sizeof(REG.CO));
	memset(REG.DI, 0, sizeof(REG.DI));
	memset(REG.IR_, 0, sizeof(REG.IR_));
	memset(REG.HR, 0, sizeof(REG.HR));
}

/**
  * @brief gets single discrete input register value
  * @retval error
  */
uint8_t MB_REG_GetDI(uint32_t address, uint8_t *value){
	return MB_REG_GetDIM(value, address, 1);
}

/**
  * @brief gets multiple discrete input register value
  * @param reg: pointer to the register
  * @param address: address of the discrete inputs register with the offset of 10001
  * @param quantity: it is the number of register, you want to read
  * @retval error
  */
uint8_t MB_REG_GetDIM(uint8_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_DI_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	startAddress = startAddress - MB_DI_ADDR_OFFSET;
//	DEBUG_SPRINT_NL("SA: %d, qty: %d", startAddress, quantity);
	if(startAddress+quantity <= MB_REG_DI_DATA_MAX){
		for(uint16_t i = 0; i < quantity; i++){
			uint16_t byteIndex = (startAddress+i) / 8;	        // 8 bit in a byte
			uint8_t bitIndex = (startAddress+i) % 8;   // bit index in a byte;
			pData[i/8] |= (((REG.DI[byteIndex] >> (bitIndex)) & mask)<<(i%8));
		}
		return MB_OK;
	}
	return MB_ERROR;
}


/**
  * @brief sets discrete input register
  * @param reg: pointer to the register
  * @param value: value to be set to the register
  * @retval return value of the register
  */


uint8_t MB_REG_SetDI(uint32_t address,  uint16_t value){
	if(address < MB_DI_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	address = address - MB_DI_ADDR_OFFSET;
    if (address < MB_REG_DI_DATA_MAX) {
        int byteIndex = (address) / 8;
        int bitIndex = (address) % 8;
        if (value > 0) {
            REG.DI[byteIndex] |= (mask << bitIndex);
        } else {
        	REG.DI[byteIndex] &= ~(mask << bitIndex);
        }
        MB_REG_ChangeDICallback((address+MB_DI_ADDR_OFFSET), (value>0));
        return MB_OK;
    }
    return MB_ERROR;
}


/**
  * @brief sets multiple discrete input register
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetDIM(uint8_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_DI_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	startAddress = startAddress - MB_DI_ADDR_OFFSET;
	if(startAddress+quantity <= MB_REG_DI_DATA_MAX){
		for (uint16_t i= 0; i < quantity; i++) {
			int byteIndex = (startAddress + i) / 8;
			int bitIndex = (startAddress + i) % 8;
			uint16_t value = pData[i/8] & (mask << i%8);
			if (value > 0) {
				REG.DI[byteIndex] |= (mask << bitIndex);
			} else {
				REG.DI[byteIndex] &= ~(mask << bitIndex);
			}
			MB_REG_ChangeDICallback((startAddress+MB_DI_ADDR_OFFSET+i), (value>0));
		}
		return MB_OK;
	}
	return MB_ERROR;

}


/**
  * @brief gets single coil output register value
  * @retval error
  */
uint8_t MB_REG_GetCO(uint32_t address, uint8_t *value){
	return MB_REG_GetCOM(value, address, 1);
}


/**
  * @brief gets multiple coil output register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @retval error
  */
uint8_t MB_REG_GetCOM(uint8_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_CO_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	startAddress = startAddress - MB_CO_ADDR_OFFSET;
	if(startAddress+quantity <= MB_REG_CO_DATA_MAX){
		for(uint16_t i = 0; i < quantity; i++){
			uint16_t byteIndex = (startAddress+i) / 8;	        // 8 bit in a byte
			uint8_t bitIndex = (startAddress+i) % 8;   // bit index in a byte;
			pData[i/8] |= (((REG.CO[byteIndex] >> (bitIndex)) & mask)<<(i%8));
		}
		return MB_OK;
	}
	return MB_ERROR;
}



/**
  * @brief sets coil register value
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetCO(uint32_t address, uint16_t value){
	if(address < MB_CO_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	address = address - MB_CO_ADDR_OFFSET;
    if (address < MB_REG_CO_DATA_MAX) {
        int byteIndex = (address) / 8;
        int bitIndex = (address) % 8;

        if (value > 0) {
            REG.CO[byteIndex] |= (mask << bitIndex);
        } else {
        	REG.CO[byteIndex] &= ~(mask << bitIndex);
        }
        return MB_REG_ChangeCOCallback((address+MB_CO_ADDR_OFFSET), (value>0));
//        return MB_OK;
    }
    return MB_ERROR;
}

/**
  * @brief sets multiple coil register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetCOM(uint8_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_CO_ADDR_OFFSET){return MB_ERROR;}
	uint8_t mask = 0x01;
	startAddress = startAddress - MB_CO_ADDR_OFFSET;
	if(startAddress+quantity <= MB_REG_CO_DATA_MAX){
		for (uint16_t i= 0; i < quantity; i++) {
			int byteIndex = (startAddress + i) / 8;
			int bitIndex = (startAddress + i) % 8;
			uint16_t value = pData[i/8] & (mask << i%8);
			if (value>0) {
				REG.CO[byteIndex] |= (mask << bitIndex);
			} else {
				REG.CO[byteIndex] &= ~(mask << bitIndex);
			}
			MB_REG_ChangeCOCallback((startAddress+MB_CO_ADDR_OFFSET+i), (value>0));
		}
		return MB_OK;
	}
	return MB_ERROR;
}


/**
  * @brief gets single  input register value
  * @retval error
  */
uint8_t MB_REG_GetIR(uint32_t address, uint16_t *value){
	return MB_REG_GetIRM(value, address, 1);
}



/**
  * @brief gets  multiple input register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @retval error
  */
uint8_t MB_REG_GetIRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_IR_ADDR_OFFSET){return MB_ERROR;}
	startAddress = startAddress - MB_IR_ADDR_OFFSET;
	if(startAddress+quantity >  MB_REG_IR_SIZE){return MB_ERROR;}
	for(uint16_t i = 0; i < quantity; i++){
		pData[i] = REG.IR_[startAddress+1];
	}
	return MB_OK;
}

/**
  * @brief sets input register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetIR(uint32_t address, uint8_t value){
	if(address < MB_IR_ADDR_OFFSET){return MB_ERROR;}
	address = address - MB_IR_ADDR_OFFSET;
	if(address >  MB_REG_IR_SIZE){return MB_ERROR;}
	REG.IR_[address] = value;
	MB_REG_ChangeIRCallback((address+MB_IR_ADDR_OFFSET), value);
	return MB_OK;
}



/**
  * @brief sets input register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetIRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_IR_ADDR_OFFSET){return MB_ERROR;}
	startAddress = startAddress - MB_IR_ADDR_OFFSET;
	if(startAddress+quantity > MB_REG_IR_SIZE){return MB_ERROR;}
	for (uint16_t i= 0; i < quantity; i++) {
		REG.IR_[startAddress+i] =  pData[i];
//		MB_REG_ChangeIRCallback((startAddress+MB_IR_ADDR_OFFSET+i),  pData[i]);
	}
	return MB_OK;
}




/**
  * @brief gets single  holding register value
  * @retval error
  */
uint8_t MB_REG_GetHR(uint32_t address, uint16_t *value){
	return MB_REG_GetHRM(value, address, 1);
}



/**
  * @brief gets multiple holding register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @retval error
  */
uint8_t MB_REG_GetHRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_HR_ADDR_OFFSET){return 0;}
	startAddress = startAddress - MB_HR_ADDR_OFFSET;
	if(startAddress+quantity >  MB_REG_HR_SIZE){return MB_ERROR;}
	for(uint16_t i = 0; i < quantity; i++){
		pData[i] = REG.HR[startAddress+i];

	}
	return MB_OK;
}




/**
  * @brief sets holding register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetHR(uint32_t address, uint16_t value){
	if(address < MB_HR_ADDR_OFFSET){return MB_ERROR;}
	address = address - MB_HR_ADDR_OFFSET;
	if(address > MB_REG_HR_SIZE){return MB_ERROR;}
//	DEBUG_SPRINT_NL(" { address: %d, value: %d }",address, value);
	REG.HR[address] = value;
//	return MB_REG_ChangeHRCallback((address+MB_HR_ADDR_OFFSET), value);
	return MB_OK;
}



/**
  * @brief sets input register value
  * @param reg: pointer to the register
  * @param address: address of a register
  * @param value: value to be set to the register
  * @retval return value of the register
  */
uint8_t MB_REG_SetHRM(uint16_t *pData, uint32_t startAddress, uint16_t quantity){
	if(startAddress < MB_HR_ADDR_OFFSET){return MB_ERROR;}
	startAddress = startAddress - MB_HR_ADDR_OFFSET;
	if(startAddress+quantity >  MB_REG_HR_SIZE){return MB_ERROR;}
	for (uint16_t i= 0; i < quantity; i++) {
		REG.HR[startAddress+i] =  pData[i];
		MB_REG_ChangeHRCallback((startAddress+MB_HR_ADDR_OFFSET+i),  pData[i]);
	}
	return MB_OK;
}



/*Callback functions if there is any change happened in the register data*/
void MB_REG_AttachChangeDIRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value )){
	MB_REG_ChangeDICallback = cbFunc;
}
void MB_REG_AttachChangeCORegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value )){
	MB_REG_ChangeCOCallback = cbFunc;
}
void MB_REG_AttachChangeIRRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value )){
	MB_REG_ChangeIRCallback = cbFunc;
}
void MB_REG_AttachChangeHRRegCallback(uint8_t (*cbFunc)(uint32_t address, uint16_t value )){
	MB_REG_ChangeHRCallback = cbFunc;
}
