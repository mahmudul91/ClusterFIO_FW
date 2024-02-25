/*
 * MB_RTUSlave.c
 *
 *  Created on: Oct 25, 2022
 *      Author: wsrra
 */

#include <MB_Handler.h>
#include "MB_RTUSlave.h"
#include "MB_Diagnostics.h"
#include "Debug.h"
#include "MB_CRC.h"
#include "MB_Custom.h"
#include "MB_Serial.h"

\
static MB_RTUSlave *slv;
void MBRS_Init(MB_RTUSlave *mbSlave){
	slv = mbSlave;
}

MB_RTUSlave *MBRS_GetInstance(void){
	return slv;
}
void MBRS_SetInstance(MB_RTUSlave *mbSlave){
	slv = mbSlave;
}

uint8_t MBRS_GetSlaveAddress(void){
	return slv->slave_address;
}

/**
  * @brief To check the slave address. slave address 0 for broadcast,
  * 1-247 valid slave address, 248-255 reserved slave address
  *
  * @param none
  *
  * @retval 0 & 1, if returns 0 means error, 1 means no error
  */
uint8_t MBRS_CheckSAnCRC(MB_RTUSlave *mbSlave){
	static uint16_t busMsgCount = 0u;
	busMsgCount++;
	//mbHandle->bus_msg_count++;				// increments the bus message counter when a bus message is detected
	if(busMsgCount>=2U){
		MB_CountBusMsg();				// increments the bus message counter when a bus message is detected
		busMsgCount = 0U;
	}
//	if(mbHandle->rxBuffer[MB_SA_INDEX] == MB_BROADCAST_ADDR){
//		//TODO: to write code for broadcast message functionality
//	}else
	if((mbSlave->rxBuffer[MB_SA_INDEX] >= MB_BROADCAST_ADDR)
		&& (mbSlave->rxBuffer[MB_SA_INDEX] <= MB_SLAVE_ADDR_MAX)){				// is the slave address is valid

		if(mbSlave->rxBuffer[MB_SA_INDEX] == mbSlave->slave_address
			|| mbSlave->rxBuffer[MB_SA_INDEX] == MB_BROADCAST_ADDR){			// if slave address is matched
			//mbHandle->rxBufferSize = size;									// to set the received adu size
			//memcpy(mbHandle->rxBuffer, recvd_adu_buffer, size);				// to copy the received PDU to the PDU buffer
			//HAL_UART_Transmit(&huart2, mbHandle->rxBuffer, mbHandle->rxBufferSize, MB_TRANS_TIMEOUT);
			//status = MB_processRequest();											// MB_requestProcess() function will process the nest steps and return the status or exception code
			if(MB_CRCCheck(mbSlave->rxBuffer, mbSlave->rxBufferSize)){
				return 1U;
			}else{
				MB_CountBusComErr();										// counts the quantity of CRC errors encountered by the slave since its last restart
			}

			MB_ClearBusyStatus();
		}

	}

	return 0U;
}



/**
  * @brief To process the request.
  *	Basically, it checks CRC error, function code, data quantity and data address
  *	and if any exception occurs, it returns the corresponding exception code
  *
  * @param none
  *
  * @retval 0 & 1, if returns 0 do nothing, else send the response to the master
  */
uint8_t MBRS_processRequest(MB_RTUSlave *mbSlave){

	// if the crc is matched then it will try processes next steps
//	if(MB_crcCheck()){
		/*
		 * When the remote device enters its Listen Only Mode,
		 * all active communication controls are turned off
		 * and  any MODBUS messages addressed to it or broadcast are monitored,
		 * but no actions will be taken and no responses will be sent
		 * */

	MB_setReqBuff(mbSlave->rxBuffer, mbSlave->rxBufferSize);
	MB_setRespBuff(mbSlave->txBuffer);
	uint8_t processStatus = 0u;
	uint8_t funCode = MB_getFunctionCode();

	if((MB_GetListenOnlyMode() == 0U) || (funCode == MB_FC_DIAG_08 && MB_getDiagSFC() == MB_DSFC_RCO_1)){	// check if the device is not in listen only mode
		if( funCode != MB_FC_GCEC_11){										// the busy status will 1U except comm. even counter

			MB_SetBusyStatus();
		}
		MB_CountSlaveMsg();													// counts the quantity of messages addressed to the slave, or broadcast, that the remote device has processed since its last restart
		// check function code

		if( MB_isFunctionCodeValid(funCode)>0U || MB_CUST_CheckCFunCode(funCode)>0){									// if the function code is valid
//			DEBUG_SPRINT("\r\n3.exReq ");
//			DEBUG_SPRINT_NL("MS_3");
//			DEBUG_SPRINT("\nrsp_1:%d", mbSlave->custom.respondingSlave);
//			DEBUG_SPRINT("\nr:%d",mbSlave->custom.respondingSlave);
			processStatus = MB_executeRequest(funCode);						//  MB_executeRequest() will return 0x04 and 0xFF, 0x04 = exception occured and 0xFF = no exception
//			DEBUG_SPRINT("\r\nx.exReq:%d ",processStatus);
		}else{
			processStatus = MB_EC_ILF_1;									// exception code 1 for illegal function code
		}
		if (processStatus != 0u){
			if(processStatus != MB_RS_255 && processStatus != MB_NRS_254){	// for exception response
				MB_CountBusExpErr();										// counts the quantity of MODBUS exception responses returned by the slave since its last restart
				if(MB_CUST_CheckCFunCode(funCode)>0){						// build error packet for custom function
					MB_appendToRespPDU(MB_getSlaveAddress());				// appends the slave address to the response PDU
					MB_appendToRespPDU(MB_EFC_OFFSET + MB_getFunctionCode());// appends the exception function code
					MB_appendToRespPDU(mbSlave->slave_address);		// target slave address
					MB_appendToRespPDU(processStatus);						// appends exception code

				}else{														// error packet for regular function code
					MB_excepRespBuilder(processStatus);						// processStatus = 255 means, no exception and the response has been built
				}
			}else{															// for normal response
				if(funCode!= MB_FC_GCEC_11){
					MB_CountCommEvent();									// Increment communication event counter, nut will not increment for  exception responses, poll commands, or fetch event counter commands
				}
				if(processStatus == MB_NRS_254){
//					MB_CUST_StopTimer(&mbSlave->custom.waitingTimer);
					return 0U;
				}
			}
			// return 1 to send the response, if it is either normal or exception response
			if((MB_GetListenOnlyMode() == 0U) && (mbSlave->rxBuffer[MB_SA_INDEX] != MB_BROADCAST_ADDR)){
				MB_appendToRespPDU_CRC();
				mbSlave->txBufferSize = MB_getRespBuffSize();
				if(funCode == MB_FC_WSC_05 || funCode == MB_FC_WSR_06) {mbSlave->txBufferSize -= 2;}
				return 1U;
			}

			// for custom function code
			if(MB_CUST_CheckCFunCode(funCode) && (mbSlave->txBuffer[MB_SA_INDEX] == MB_BROADCAST_ADDR)){
//				//TODO: To remove
//				for(uint32_t i = 3; i <  13; i++){
//					mbSlave->txBuffer[i] = 0;
//				}
//				//--------------------
				MB_appendToRespPDU_CRC();

				mbSlave->txBufferSize = MB_getRespBuffSize();

				if(mbSlave->custom.isReadyToSend>0){ return 1U;	}
			}
		}
	}
	return 0U;
}


/**
  * @brief To process some task after sending the response
  * @param none
  * @retval none
  */
void MBRS_PostProcess(MB_RTUSlave *mbSlave, uint8_t error){
	if(MB_GetRestartFlag()){	// restart the device
		MB_CallbackRCO();
	}
	MB_ClearBusyStatus();
}


/**process custom functions for the modbus slave
  */
//
//uint8_t MBRS_ProcessCustom(MB_RTUSlave *slave){
//	MBS_Serial *serial;
//	serial = MBS_GetInstance();
//	if(serial->isDataReceived > 0U){
//		serial->isDataReceived = 0U;
//		slave->rxBuffer = &serial->rxBuff[0];
//		slave->txBuffer = &serial->txBuff[0];
//		slave->rxBufferSize = serial->rxBuffSize;
//		slave->txBufferSize = serial->txBuffSize;
//
//		MB_setReqBuff(slave->rxBuffer, slave->rxBufferSize);
//		MB_setRespBuff(slave->txBuffer);
//
//		for(uint8_t i = 0; i < slave->rxBufferSize ; i++){
//			DEBUG_SPRINT("%02X ", slave->rxBuffer[i]);
//		}
//		uint8_t broadcastAddr = serial->rxBuff[MB_SA_INDEX];
//		uint8_t funCode = serial->rxBuff[MB_FC_INDEX];
//		uint8_t slvStartAddr = serial->rxBuff[MB_FC_INDEX+1];
//
//		uint8_t processStatus = 0u;
//		if(slvStartAddr == MB_BROADCAST_ADDR && MB_CUST_CheckCFunCode(funCode)>0){		// for the custom functions
//			processStatus = MB_executeRequest(funCode);						//  MB_executeRequest() will return 0x04 and 0xFF, 0x04 = exception occured and 0xFF = no exception
//		}
//
////
////		if(MB_CUST_CheckCFunCode(funCode) && (slave->txBuffer[MB_SA_INDEX] == MB_BROADCAST_ADDR)){
////							MB_appendToRespPDU_CRC();
////							slave->txBufferSize = MB_getRespBuffSize();
////							if(slave->custom.isReadyToSend>0){
////								return 1U;
////							}
////						}
//	}
//	return 0;
//}
//


