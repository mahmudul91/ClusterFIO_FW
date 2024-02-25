/*
 * MB_Custom.c
 *
 *  Created on: Feb 11, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#include <MB_Handler.h>
#include "MB_Custom.h"
#include "MB_Register.h"
#include "MB_CRC.h"
#include "MB_RTUMaster.h"
#include "MB_Tools.h"
#include "math.h"
#include "Debug.h"
#include "MB_RTUSlave.h"
#include "TimeStamp.h"


TimeStamp timStamp;
extern MB_RTUHandler mbHandle;

static MB_CUST_MQ_Queue msgQ;



uint8_t (*MB_CUST_Process109ReqAODOCallback)(uint8_t *buff, uint16_t size);
uint8_t (*MB_CUST_Build107RespAIDICallback)(uint8_t *buff, uint16_t size);

void MB_CUST_Init(MB_Custom *custom){
	custom->numOfSlv = 0;
	custom->slvStartAddr = 0;
	custom->reTransTimer.enable = 0;
	custom->reTransTimer.time = 0;
	custom->rcvdCount = 0;
	custom->isReadyToSend = 0;
	MBRS_CUST_ClearMQ();

}

/*------Modbus Custom functions for Master------------*/

/**
  * @brief Built request to send MB rtu slave for custom function 107-Read Input Register & Discrete input
  * @retval returns size of the buffer
  */
uint16_t MB_CUST_BuildRTUReqRIRDI(MB_Custom *custom, uint8_t *buff){
	/*
	 * Data frame
	 * ------------------------------------------------------------------------------------------------------
	 * Broadcast address | Custom Function Code | Slave Starting Address | Number of Slave (N) |   CRC-16 	|
	 *   	1 byte		 |		 1 bytes	    |			1 byte		 |		1 bytes		   |   2 bytes	|
	 * ------------------------------------------------------------------------------------------------------
	 * */
	uint16_t size = 0;
	buff[size++] = MB_BROADCAST_ADDR;				//  Broadcast address
	buff[size++] = MB_CFC_RIRDI_107;				//  Custom Function Code 107-Read Input Register & Discrete input
	buff[size++] = custom->slvStartAddr;			//Slave Starting Address
	buff[size++] = custom->numOfSlv;				// Number of slaves
	MBTOOL_SplitU16ToBytes(&buff[size+1], &buff[size], MB_CalcCRC16(buff, size));	// calculate CRC-16, and set it to the buffer
	return size+2;

}

/*Process request for 107 packet
 * */
uint16_t MB_CUST_ProcessRTUReqRIRDI(void){

//	MBS_Serial *serial;
//	serial = MBS_GetInstance();
//	MB_RTUSlave *slave;
//	slave = MBRS_GetInstance();

	return 0;
}


/*
 * This sub packet for error response
 * */
uint16_t MB_CUST_BuildEmptySubPackRIRDI(uint8_t *buff, uint8_t slvAddr){
	uint16_t size = 0;
	while(size < MB_CUST_MQ_MSG_SIZE){
		if(size == MB_CUST_MQ_MSG_SIZE-5){
			buff[size++] = 0x7F;
		}else{
			buff[size++] = 0xFF;
		}
	}
	return size;
}

uint16_t MB_CUST_BuildEmptySubPacket(uint8_t *buff, uint8_t status){

	memset(buff, 0xFF,MB_CUST_DATA_BLOCK_SIZE);		// set 0xFF as there is no valid value
	/*now set the status byte*/
	buff[MB_CUST_DATA_BLOCK_SIZE-2] = status;
	return MB_CUST_DATA_BLOCK_SIZE;
}


/**
  * @brief Built request to send MB tcp server for custom function 107-Read Input Register & Discrete input
  * @retval returns size of the buffer
  */
uint16_t MB_CUST_BUILDTCPCReqRIRDI(MB_Custom *custom, uint8_t *tcpBuff, uint8_t *buff,  uint16_t size){
return 0;
}



/*
 * Checks custom function code
 * */
uint8_t MB_CUST_CheckCFunCode(uint8_t funCode){
	return (funCode == MB_CFC_RIRDI_107 || funCode == MB_CFC_WMHRCO_109);
}

/*
 * get the packet type
 * 1 = slave response packet on broadcast & custom function code
 * 0 = master request packet on broadcast & custom function code
 * */
uint8_t MB_CUST_GetPacketType(uint8_t *buff){
	return (buff[MB_CUST_STATUS_BYTE_INDEX]&0x20);
}

/**
  * @brief Built request for custom function 108-Read Holding Register & Coil
  * @retval returns size of the buffer
  *
  */
//uint16_t MB_CUST_BuildReqRHRCO(MB_Custom *custom, uint8_t *buff){
//	/*
//	 * Data frame
//	 * ------------------------------------------------------------------------------------------------------
//	 * Broadcast address | Custom Function Code | Slave Starting Address | Number of Slave (N) |   CRC-16 	|
//	 *   	1 byte		 |		 1 bytes	    |			1 byte		 |		1 bytes		   |   2 bytes	|
//	 * ------------------------------------------------------------------------------------------------------
//	 * */
//
//	uint16_t size = 0;
//	buff[size++] = MB_BROADCAST_ADDR;				//  Broadcast address
//	buff[size++] = MB_CFC_RIRDI_108;				//  Custom Function Code 107-Read Input Register & Discrete input
//	buff[size++] = mbCustom->slvStartAddr;			//Slave Starting Address
//	buff[size++] = mbCustom->numOfSlv;				// Number of slaves
//	MBTOOL_SplitU16ToBytes(buff[size+2],buff[size+1], MB_CalcCRC16(buff, size));	// calculate CRC-16, and set it to the buffer
//	return size+2;
//
//}

void MB_CUST_StartTimer(MB_CUST_WaitingTimer *timer){
	timer->enable = 1;
	timer->time = TS_GetUS(&timStamp);
}

void MB_CUST_StopTimer(MB_CUST_WaitingTimer *timer){
	timer->enable = 0;
	timer->time = 0;
	timer->isTimeRst = 0;
}

void MB_CUST_ResetTimer(MB_CUST_WaitingTimer *timer){
	timer->time = TS_GetUS(&timStamp);
	timer->isTimeRst = 1;
}

void MB_CUST_SetTimeout(MB_CUST_WaitingTimer *timer, uint32_t timeout){
	timer->timeout = timeout;
}

uint8_t MB_CUST_IsTimerRunning(MB_CUST_WaitingTimer *timer){
	return timer->enable;
}

uint8_t MB_CUST_IsTimeout(MB_CUST_WaitingTimer *timer){
	if(!timer->enable) return 0;
	if((uint64_t)fabsl((long double)(TS_GetUS(&timStamp) - timer->time)) >= (uint64_t)timer->timeout){
//		timer->time = TS_GetUS(&timStamp);
		timer->isTimeRst = 0;
		return 1;
	}
	return 0;
}


//void MB_CUST_StartTimerUS(MB_CUST_WaitingTimer *timer){
//	timer->enable = 1;
//	timer->time = TS_GetUS(&timStamp);
//}
//
//uint8_t MB_CUST_IsTimeoutUS(MB_CUST_WaitingTimer *timer){
//	if(!timer->enable) return 0;
//	if((uint64_t)fabsl((long double)(TS_GetUS(&timStamp) - timer->time)) >= (uint64_t)timer->timeout){
////		timer->time = HAL_GetTick() ;
//		timer->isTimeRst = 0;
//		return 1;
//	}
//	return 0;
//}


/*------Modbus Custom functions for slave------------*/

uint8_t MBRS_CUST_GetSlvStartAddr(void){
	return mbHandle.rxBuffer[MB_FC_INDEX+1];
}

uint8_t MBRS_CUST_GetNumOfSlv(void){
	return mbHandle.rxBuffer[MB_FC_INDEX+2];
}

/**
  * @brief check the slave start address
  * @param none
  * @retval 0 = no exception, or >0 = exception code
  */
uint8_t MBRS_CUST_CheckSlvStartAddr(uint8_t address){
	return ((address > MB_BROADCAST_ADDR) && (address <= MB_SLAVE_ADDR_MAX))?
			0: MB_ECC_ISSA_12;
}

/**
  * @brief check the  num of slaves
  * @param none
  * @retval 0 = no exception, or >0 = exception code
  */
uint8_t MBRS_CUST_CheckNumOfSlv(uint8_t numOfSlv){
	return ((numOfSlv >= 1) && (numOfSlv <= MB_CUST_NUM_OF_SLAVE_MAX))? 0: MB_ECC_INS_13;
}

/**
  * @brief check the  slave start address & num of slaves
  * @param none
  * @retval 0 = no exception, or >0 = exception code
  */
uint8_t MBRS_CUST_CheckSlvSAAndNum(uint8_t address, uint8_t numOfSlv){

	if(MBRS_CUST_CheckNumOfSlv(numOfSlv)==0){
		if(MBRS_CUST_CheckSlvStartAddr(address)==0 && (numOfSlv+numOfSlv>=1 && numOfSlv+numOfSlv <= MB_SLAVE_ADDR_MAX)){
			return 0;
		}else{
			return MB_ECC_ISSA_12;
		}
	}else{
		return MB_ECC_INS_13;
	}

//	return MBRS_CUST_CheckSlvStartAddr(address) == 0 ? MBRS_CUST_CheckNumOfSlv(numOfSlv): MB_ECC_ISSA_12;

}

/**
  * @brief To read coil(CFC-101) and make the response PDU
  * @param none
  * @retval 2,3,4 & 255, where 2-4 for the Modbus exception code, and 255 for no exception
  */
uint8_t MBRS_CUST_readCO(void){
	return MB_readCoils();
}

/**
  * @brief To read discrete input(CFC-102) and make the response PDU
  */
uint8_t MBRS_CUST_readDI(void){
	return MB_readDI();
}

/**
  * @brief To read Holding Register(CFC-103) and make the response PDU
  */
uint8_t MBRS_CUST_readHR(void){
	return MB_readHR();
}

/**
  * @brief To read input register(CFC-104) and make the response PDU
  */
uint8_t MBRS_CUST_readIR(void){
	return MB_readIR();
}

/**
  * @brief To write multiple coil (CFC-105) and make the response PDU
  */
uint8_t MBRS_CUST_writeMC(void){
	return  MB_writeMC();
}

/**
  * @brief To write multiple register(CFC-106) and make the response PDU
  */
uint8_t MBRS_CUST_writeMR(void){
	return MB_writeMHR();
}



/**
  * @brief To read input register & discrete input(CFC-107) and make the response PDU
  */
uint8_t MBR_CUST_readIRDI(void){




	if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
		static uint8_t excepCode =	0;
		MB_RTUSlave *slave;
		slave = MBRS_GetInstance();
//		DEBUG_SPRINT(" P0.1:%d",slave->custom.respondingSlave);
//		DEBUG_SPRINT("\r\nR%dL%dS%dN%d", MBRS_CUST_GetSlvStartAddr() , slave->slave_address, slave->custom.slvStartAddr, slave->custom.numOfSlv);
		 if(MBRS_CUST_GetSlvStartAddr() > slave->slave_address ){															 // if the response is for this slave
			slave->custom.isReadyToSend = 0;
			return MB_NRS_254;
		}

		if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
			//Check the request packet size
			if(slave->rxBufferSize == MB_CUST_107_REQ_PACK_SIZE){
				slave->custom.numOfSlv = MBRS_CUST_GetNumOfSlv();
				slave->custom.slvStartAddr = MBRS_CUST_GetSlvStartAddr();
				excepCode =	MBRS_CUST_CheckSlvSAAndNum(slave->custom.slvStartAddr,slave->custom.numOfSlv );
				if(excepCode != 0 ){	return excepCode;}
			}
//			DEBUG_SPRINT("\r\nR%dL%dS%dN%d", MBRS_CUST_GetSlvStartAddr() , slave->slave_address, slave->custom.slvStartAddr,MBRS_CUST_GetNumOfSlv());
//			excepCode =	MBRS_CUST_CheckSlvStartAddr(MBRS_CUST_GetSlvStartAddr());
			//excepCode = MBRS_CUST_CheckSlvSAAndNum(MBRS_CUST_GetSlvStartAddr(), MBRS_CUST_GetNumOfSlv());			// is the slave address is valid;
		}
//		uint8_t slvStartAddr = MBRS_CUST_GetSlvStartAddr();
//		DEBUG_SPRINT(" P0.2:%d",slave->custom.respondingSlave);
		if(slave->custom.waitingTimer.isTimeRst<=0 && MBRS_CUST_GetSlvStartAddr() > 0){
			slave->custom.respondingSlave = MBRS_CUST_GetSlvStartAddr();
//			DEBUG_SPRINT(" P1.1:%d",slave->custom.respondingSlave);
		}

		//check response from other slave address
		if((slave->custom.respondingSlave+1) == slave->slave_address
				&& slave->custom.waitingTimer.isTimeRst<=0
				&& slave->custom.eventType == MB_CUST_EVENT_107_RECVD){		// if the response comes from just in front slave
			if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
				if(slave->custom.numOfSlv == 0 && slave->rxBufferSize == MB_CUST_107_REQ_PACK_SIZE){
					slave->custom.numOfSlv = MBRS_CUST_GetNumOfSlv();
					slave->custom.slvStartAddr = MBRS_CUST_GetSlvStartAddr();
					if(MBRS_CUST_CheckNumOfSlv(slave->custom.numOfSlv ) == MB_ECC_INS_13) {return MB_ECC_INS_13; }
				}
				 if(slave->custom.numOfSlv > 1){
					MB_CUST_StartTimer(&slave->custom.waitingTimer);
					slave->custom.isReadyToSend = 0;
//					DEBUG_PRINT(" P1.2");
				 }
				return MB_NRS_254;
			}else{
//				DEBUG_PRINT(" P1");
				slave->custom.isReadyToSend = 1;
				MB_CUST_StopTimer(&slave->custom.waitingTimer);
			}
		}
		else if((slave->custom.respondingSlave+1) == slave->slave_address
				&& slave->custom.waitingTimer.isTimeRst==1
				&& slave->custom.eventType == MB_CUST_EVENT_107_RECVD){		// if the response comes from just in front slave
//				DEBUG_SPRINT("\nR:%d",slave->custom.respondingSlave);
				slave->custom.isReadyToSend = 1;
				MB_CUST_StopTimer(&slave->custom.waitingTimer);
		}
		else if(slave->custom.respondingSlave < slave->slave_address
				&& slave->custom.eventType == MB_CUST_EVENT_107_RECVD){		// if this device is not the next slave
			slave->custom.isReadyToSend = 0;
			if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
				if(slave->custom.numOfSlv == 0 && slave->rxBufferSize == MB_CUST_107_REQ_PACK_SIZE){
//					DEBUG_PRINT(" P2");
					slave->custom.numOfSlv = MBRS_CUST_GetNumOfSlv();
					slave->custom.slvStartAddr = MBRS_CUST_GetSlvStartAddr();
					if(MBRS_CUST_CheckNumOfSlv(slave->custom.numOfSlv ) == MB_ECC_INS_13) {return MB_ECC_INS_13; }
				}
				if(slave->custom.numOfSlv > 1){
//					DEBUG_PRINT(" P3");

//					DEBUG_SPRINT("\nR:%d",slave->rxBufferSize);
					MB_CUST_StartTimer(&slave->custom.waitingTimer);
					slave->custom.isReadyToSend = 0;
				}
			}else{
//				DEBUG_PRINT(" P3.1");

//				DEBUG_SPRINT("\nR:%d",slave->custom.respondingSlave);
				MB_CUST_ResetTimer(&slave->custom.waitingTimer);
				slave->custom.waitingTimer.isTimeRst = 0;
				slave->custom.respondingSlave++;
				return MB_NRS_254;
			}
		}else if(slave->custom.respondingSlave == slave->slave_address){
//				&& slave->custom.eventType == MB_CUST_EVENT_107_TIMEOUT){	// if this device is the target slave
//			DEBUG_PRINT(" P4");
			if(slave->custom.numOfSlv == 0 && slave->rxBufferSize == MB_CUST_107_REQ_PACK_SIZE){
//				DEBUG_PRINT(" P5");
				slave->custom.numOfSlv = MBRS_CUST_GetNumOfSlv();
				slave->custom.slvStartAddr = MBRS_CUST_GetSlvStartAddr();
				if(MBRS_CUST_CheckNumOfSlv(slave->custom.numOfSlv ) == MB_ECC_INS_13) {return MB_ECC_INS_13; }
			}
			slave->custom.waitingTimer.isTimeRst = 0;
			slave->custom.isReadyToSend = 1;
			MB_CUST_StopTimer(&slave->custom.waitingTimer);
		}

		if((slave->slave_address) >=(slave->custom.slvStartAddr+slave->custom.numOfSlv) ){		// if this device is not in the range of target slaves

			slave->custom.isReadyToSend = 0;
			 if( MBRS_CUST_GetSlvStartAddr() == (slave->custom.slvStartAddr+slave->custom.numOfSlv-1)){
					slave->custom.numOfSlv = 0;
					slave->custom.slvStartAddr = 0;

			 }
//			 DEBUG_PRINT(" P6");
			 MB_CUST_StopTimer(&slave->custom.waitingTimer);
			return MB_NRS_254;
		}
//		DEBUG_PRINT(" P10");
		// Append slave address, function code and responding slave address
		mbHandle.txBufferSize = 0;
		MB_appendToRespPDU_SAFC();						// here the slave address basically is the broadcast address
		MB_appendToRespPDU(slave->slave_address);		// local slave address
//				DEBUG_SPRINT("\r\nR%dL%dS%dN%dT%d", MBRS_CUST_GetSlvStartAddr() , slave->slave_address, slave->custom.slvStartAddr, slave->custom.numOfSlv,slave->custom.respondingSlave);
//		slave->custom.isReadyToSend = 1;
	}else{
		mbHandle.txBufferSize = 0;
	}

	//New Implementation-30/07/2023
	MB_CUST_Build107RespAIDICallback(&mbHandle.txBuffer[mbHandle.txBufferSize], MB_CUST_DATA_BLOCK_SIZE);
	mbHandle.txBufferSize += MB_CUST_DATA_BLOCK_SIZE;

	//Old Implementation
//		/*Read input register----------------------*/
//		uint16_t regAddress[MB_CUST_NUM_OF_IR_AI] = {0};
//		regAddress[0] = MB_CUST_IR_AI1_ADDR;
//		regAddress[1] = MB_CUST_IR_AI2_ADDR;
//		regAddress[2] = MB_CUST_IR_AI3_ADDR;
//		regAddress[3] = MB_CUST_IR_AI4_ADDR;
//		uint16_t dataIR[MB_CUST_NUM_OF_IR_AI] = {0};
//		for(uint16_t i = 0; i < MB_CUST_NUM_OF_IR_AI; i++){
//			if(MB_getIRValue(regAddress[i], &dataIR[i]) == MB_ERROR){
//				return MB_EC_SDV_4;
//			}
//		}
//
//
//	/*Read Discrete input----------------------*/
//
//		/*------- Test start---------*/
//		uint16_t size = MB_CUST_NUM_OF_DI/8 + (MB_CUST_NUM_OF_DI%8>0);
//		uint16_t startAddress = MB_CUST_DI_START_ADDR;
//		uint16_t lastAddress = startAddress+MB_CUST_NUM_OF_DI;
//		uint8_t dataDI[size];
//		if(MB_getDIInByte(dataDI, size, startAddress, lastAddress) != MB_OK){
//			return MB_EC_SDV_4;
//		}
//
//		if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
//			MB_appendToRespPDU_SAFC();						// here the slave address basically is the broadcast address
//			MB_appendToRespPDU(MBRS_GetSlaveAddress());		// local slave address
//		}
//		// set IR data to resp buffer
//		for(uint8_t i = 0; i < MB_CUST_NUM_OF_IR_AI; i++){
//			MB_appendToRespPDU((uint8_t)(dataIR[i]>>8));
//			MB_appendToRespPDU((uint8_t)(dataIR[i] & MB_AND_MASK_16T8));
//		}
//		MB_appendToRespPDU(0); // status byte
//
//		// set DI data to resp buffer
//		for(uint8_t i = 0; i < size; i++){
//			MB_appendToRespPDU(dataDI[i]);
//		}

		return MB_RS_255;				// if there is no exception
}






/*
  * @brief To read Holding register & Coils output(CFC-108) and make the response PDU
  */
uint8_t MBR_CUST_readHRCO(void){

	uint8_t excepCode =	MBRS_CUST_CheckSlvSAAndNum(MBRS_CUST_GetSlvStartAddr(), MBRS_CUST_GetNumOfSlv());			// is the slave address is valid;

	if(excepCode != 0){	return excepCode;}									// if there is no exception

		/*Read input register----------------------*/
		uint8_t size = MB_CUST_NUM_OF_HR_AO;
		uint16_t dataHR[size];
		uint16_t startAddress =  MB_CUST_HR_AO1_ADDR;
		uint16_t lastAddress = startAddress+size-1;

		if(MB_getIRsValue(dataHR, size,  startAddress, lastAddress)>0){
//			uint8_t byteCount = size*2;
			MB_appendToRespPDU_SAFC();						// here the slave address basically is the broadcast address
			MB_appendToRespPDU(MBRS_GetSlaveAddress());		// local slave address
//			MB_appendToRespPDU(byteCount);
			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU((uint8_t)(dataHR[i]>>8));
				MB_appendToRespPDU((uint8_t)(dataHR[i] & MB_AND_MASK_16T8));
			}
//			return MB_RS_255;
		}else{

			return MB_EC_SDV_4;									// exception - 4 ,  service device failure
		}

	/*Read Discrete input----------------------*/

		/*------- Test start---------*/
		size = MB_CUST_NUM_OF_CO/8 + (MB_CUST_NUM_OF_CO%8>0);
		startAddress = MB_CUST_CO_START_ADDR;
		lastAddress = startAddress+MB_CUST_NUM_OF_CO-1;
		uint8_t dataCO[size];
		if(MB_getDIInByte(dataCO, size, startAddress, lastAddress)){
//			MB_appendToRespPDU(size);  // total response byte

			for(uint8_t i = 0; i < size; i++){
				MB_appendToRespPDU(dataCO[i]);
			}
//			MB_appendToRespPDU_CRC();

		}else{
			MB_ClearResBuffer();
			return MB_EC_SDV_4;								// returns exception code 04
		}

		return MB_RS_255;				// if there is no exception
}


/*
  * @brief To write Holding register & Coils output(CFC-109) and make the response PDU
  */

uint8_t MBR_CUST_writeHRCO(void){



//	if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
//		uint8_t excepCode =	0;
//		MB_RTUSlave *slave;
//		slave = MBRS_GetInstance();
//		DEBUG_SPRINT_NL("slave Resp: %d, Local: %d",  MBRS_CUST_GetSlvStartAddr(), slave->slave_address);
////		 if(MBRS_CUST_GetSlvStartAddr() > slave->slave_address){															 // if the response is for this slave
////			slave->custom.isReadyToSend = 0;
////			return MB_NRS_254;
////		}
//
//
//		if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
//			excepCode =	MBRS_CUST_CheckSlvSAAndNum(MBRS_CUST_GetSlvStartAddr(), MBRS_CUST_GetNumOfSlv());			// is the slave address is valid;
//		}else{
////			excepCode = MBRS_CUST_CheckSlvStartAddr(MBRS_CUST_GetSlvStartAddr());
//		}
//		if(excepCode != 0){	return excepCode;}
//
////		MB_RTUSlave *slave;
////		slave = MBRS_GetInstance();
//		slave->custom.respondingSlave = MBRS_CUST_GetSlvStartAddr();
////		DEBUG_SPRINT_NL("slave Resp: %d, Local: %d", slave->custom.respondingSlave, slave->slave_address);
//		//check response from other slave address
//		if((slave->custom.respondingSlave+1) == slave->slave_address){		// if the response comes from just in front slave
////			MB_CUST_StartTimer(&slave->custom.waitingTimer);
//			if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
////				DEBUG_SPRINT_NL("TimSt");
//				MB_CUST_StartTimer(&slave->custom.waitingTimer);
//				slave->custom.isReadyToSend = 0;
//				return MB_NRS_254;
//			}else{
////				DEBUG_SPRINT_NL("TimRest");
//				slave->custom.isReadyToSend = 1;
//				MB_CUST_StopTimer(&slave->custom.waitingTimer);
//
//			}
//		}else if(slave->custom.respondingSlave < slave->slave_address){		// if it is not the next slave
////			MB_CUST_ResetTimer(&slave->custom.waitingTimer);
//			slave->custom.isReadyToSend = 0;
//			if(!MB_CUST_IsTimerRunning(&slave->custom.waitingTimer)){
//				MB_CUST_StartTimer(&slave->custom.waitingTimer);
//			}else{
//				MB_CUST_ResetTimer(&slave->custom.waitingTimer);
//				return MB_NRS_254;
//			}
//		}else if(slave->custom.respondingSlave == slave->slave_address){
//			slave->custom.isReadyToSend = 1;
//		}
//	}

	/* write holding register-------------*/
//		uint16_t startAddress = MB_CUST_HR_AO1_ADDR;
//		uint16_t endAddress = startAddress + MB_CUST_NUM_OF_HR_AO-1;

//		uint8_t byteIndex = 0;
		if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
//			byteIndex = (MB_FC_INDEX+3) + (MBRS_GetSlaveAddress()-MBRS_CUST_GetSlvStartAddr())*MB_CUST_DATA_BLOCK_SIZE;
			mbHandle.rxBufferSize= (MB_FC_INDEX+3) + (MBRS_GetSlaveAddress()-MBRS_CUST_GetSlvStartAddr())*MB_CUST_DATA_BLOCK_SIZE;
		}else{
//			byteIndex = 0;
			mbHandle.rxBufferSize=0;
		}
		//New Implementation-30/07/2023
		MB_CUST_Process109ReqAODOCallback(&mbHandle.rxBuffer[mbHandle.rxBufferSize], MB_CUST_DATA_BLOCK_SIZE);
		mbHandle.rxBufferSize += MB_CUST_DATA_BLOCK_SIZE;

		//old Implementation
//		uint16_t regAddress[MB_CUST_NUM_OF_HR_AO] = {0};
//		regAddress[0] = MB_CUST_HR_AO1_ADDR;
//		regAddress[1] = MB_CUST_HR_AO2_ADDR;
//		regAddress[2] = MB_CUST_HR_AO3_ADDR;
//		regAddress[3] = MB_CUST_HR_AO4_ADDR;
//		for(uint16_t i = 0; i < MB_CUST_NUM_OF_HR_AO; i++){
//			uint16_t data = MB_getMRValueIn2Byte((byteIndex));
//
//
//			if(MB_setHRValue(regAddress[i], data) == MB_ERROR){
////				return MB_EC_SDV_4;
//			}
//			byteIndex += 2;
//		}
//
//		/*Write coil output------------*/
//		uint16_t startAddress = MB_CUST_CO_START_ADDR;
////		uint16_t endAddress = startAddress + MB_CUST_NUM_OF_CO;
//		 if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
//			 byteIndex = (MB_FC_INDEX+3) + (MBRS_GetSlaveAddress()-MBRS_CUST_GetSlvStartAddr())*MB_CUST_DATA_BLOCK_SIZE+9; // 9 is the CO data byte index
//		 } else{		// for the master
//			byteIndex = MB_CUST_DATA_BLOCK_SIZE-1;
//		}
//
//		uint8_t dataCO = MB_getMCValueInByte(byteIndex);
////		DEBUG_SPRINT_NL("HC: %d, bindx: %d", dataCO, byteIndex);
//		for(uint8_t i = 0; i < MB_CUST_NUM_OF_CO;i++){
//			if(MB_setCoil(startAddress+i, ((dataCO>>i)&0x01)) == MB_ERROR){
////				return MB_EC_SDV_4;
//			}
//		}
//		if(MB_GetRTUMode()==MB_RTU_MODE_SLAVE){
//			//Build response
////			MB_ClearResBuffer();
////			MB_appendToRespPDU_SAFC();
////			MB_appendToRespPDU(MBRS_GetSlaveAddress());		// local slave address
//		 }else{
//
//		 }

		return MB_NRS_254;//MB_RS_255;

}


/*
 * Exception response for the custom function code
 * */
void MBR_CUST_BuildExcepResp(uint8_t excepCode){
	MB_ClearResBuffer();
	for(uint8_t i = 0; i < 8; i++){		// 8bytes for 4 analog IOs
		MB_appendToRespPDU(0xFF);
	}
	MB_appendToRespPDU(MB_CUST_ONLINE|MB_CUST_ERROR|excepCode);
	MB_appendToRespPDU(0xFF);
}




void MB_CUST_attachProcess109ReqAODOCallback(uint8_t (*callbackFunc)(uint8_t *buff, uint16_t size)){

	MB_CUST_Process109ReqAODOCallback = callbackFunc;


}
void MB_CUST_attachBuild107RespAIDICallback(uint8_t (*callbackFunc)(uint8_t *buff, uint16_t size)){
	MB_CUST_Build107RespAIDICallback = callbackFunc;
}


/**
  * @brief  enque the msg queue
  * @param  buff: Pointer to msg buff
  * @retval error status, 1 = no error, 0 = error
  */
uint8_t MBRS_CUST_EnqueMsg(uint8_t *buff, uint16_t size){
  if (msgQ.noOfMsg < MB_CUST_MQ_QUE_SIZE && size <= MB_CUST_MQ_MSG_SIZE){
    if(msgQ.endIndex >= MB_CUST_MQ_QUE_SIZE){
    	msgQ.endIndex = 0U;
    }
    memcpy(msgQ.msg[msgQ.endIndex].buff, buff, size);
    msgQ.msg[msgQ.endIndex].size = size;
    msgQ.endIndex++;
    msgQ.noOfMsg++;
    return 1U;
  }else{
    return 0U;
  }
}

/**
  * @brief  deque the msg queue, it will remove the msg
  * @param  buff: Pointer to msg buff
  * @retval 0 = error, >0= buffer size
  */
uint16_t MBRS_CUST_DequeMsg(uint8_t *buff){
  if (msgQ.noOfMsg > 0U){
    uint16_t sz;
    memcpy(buff, msgQ.msg[msgQ.endIndex].buff, msgQ.msg[msgQ.endIndex].size);

    /*Now cleare the queue*/

    memset(msgQ.msg[msgQ.startIndex].buff, 0U, MB_CUST_MQ_MSG_SIZE);
    sz = msgQ.msg[msgQ.startIndex].size;
    msgQ.msg[msgQ.startIndex].size = 0U;


    msgQ.startIndex = (msgQ.startIndex+1) % MB_CUST_MQ_QUE_SIZE;
    msgQ.noOfMsg--;
    return sz;
  }else{
    return 0;
  }
}

/**
  * @brief  clear the msg queue
  * @param  que: Pointer to msg queue
  * @retval none
  */
void MBRS_CUST_ClearMQ(void){

	 msgQ.startIndex = 0U;
	 msgQ.endIndex = 0U;
	 msgQ.noOfMsg = 0U;
	for (uint16_t i = 0U; i < MB_CUST_MQ_QUE_SIZE; i++){


		 msgQ.msg[i].size = 0u;
		memset( msgQ.msg[i].buff, 0U, MB_CUST_MQ_MSG_SIZE);
	}
}




