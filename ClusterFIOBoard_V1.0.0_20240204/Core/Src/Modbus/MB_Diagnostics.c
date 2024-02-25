/*
 * MB_Diagnostics.c
 *
 *  Created on: Feb 11, 2022
 *      Author: RASEL_EEE
 */
#include <MB_Handler.h>
#include "MB_Diagnostics.h"
#include "Flash_Storag.h"
#include "Storage_Address.h"
#include "Debug.h"


MB_RTUSlave *mbSlave;





void MB_DIAG_BuildResp2byteData(uint16_t value);


uint8_t (*MB_DIAG_SubFuncCallback)(uint16_t subFuncCode, uint16_t value);


void MB_DIAG_SetRTUSlaveInstance(MB_RTUSlave *_mbSlave){
	mbSlave = _mbSlave;
}

/**
  * @brief Diagnostics (FC-08)
  * @param none
  * @retval status code for exception and non exception
  */
uint8_t MB_DIAG_CheckDSFC(uint8_t subFuncCode){
	return ((subFuncCode >= MB_DSFC_RQD_0 && subFuncCode <= MB_DSFC_RDR_2)
			|| (subFuncCode == MB_DSFC_FLOM_4)
			|| (subFuncCode >= MB_DSFC_CCDR_10 && subFuncCode <= MB_DSFC_RSMC_14)
			|| (subFuncCode == MB_DSFC_RBCOC_18) || (subFuncCode == MB_DSFC_COCF_20));
}

/**
  * @brief It checks the sub function code data field
  * @param none
  * @retval 0,1 whare, 0 means exception and 1 means no exception
  */
uint8_t MB_DIAG_CheckData(uint8_t subFuncCode, uint16_t dataValue){

	if((subFuncCode != MB_DSFC_RQD_0) && (subFuncCode != MB_DSFC_CAID_3)){
		if(subFuncCode == MB_DSFC_RCO_1){
			return ((dataValue==LOW) || (dataValue==HIGH));
		}else{
			return (dataValue==LOW);
		}
	}else{
		return 0;
	}
}



/**
  * @brief It processes the diagnostics sub-function 00 Return Query Data,
  * the request data field is to be returned (looped back) in the response
  * @param none
  * @retval 0,1 whare, 0 means exception and 1 means no exception
  */
uint8_t MB_DSF_ReturnQD(MB_RTUDiag *diag){
	DBG_PRINT("\r\nRetQD 1");
	if(MB_getDiagData() >=0x0000 && MB_getDiagData() <=0xFFFF){
		DBG_PRINT("\r\nRetQD 2");
//		memcpy(mbSlave->txBuffer, mbSlave->rxBuffer, mbSlave->rxBufferSize);
//		mbSlave->txBufferSize = mbSlave->rxBufferSize;
		MB_CopyReqToRespPDU();
		DBG_PRINT("\r\nRetQD 3");
		return MB_RS_255;
	}else{
		return MB_EC_ILDV_3;
	}
}


/**
  * @brief It processes the diagnostics sub-function 01 Restart Communications Option
  * @param none
  * @retval 0,1 whare, 0 means exception and 1 means no exception
  */
uint8_t MB_DSF_RestartCO(MB_RTUDiag *diag){

		/*
		 * To initialize and restart serial port
		 * To Clear comm. event counter
		 * To bring the port out of Listen Only Mode
		 * If the port is currently in Listen Only Mode, no response is returned else returns response
		 * These occurs before the restart is executed
		*/
		//mbSlave->listen_only_mode = 0U;		// bring the port out of the Listen Only Mode
		// TODO: to add function for disabling listen only mode
		// TODO: to add function for restart device
		// TODO: To initialize and restart serial port
		diag->comm_event_count = 0;			// Clear comm. event counter
		MB_ClearCounters(diag);					// to clear all the counters
//		mbSlave->device_restart_flage = 1U;
		MB_SetRestartFlag();
		MB_DIAG_SubFuncCallback(MB_DSFC_RCO_1, MB_getDiagData());
		if(diag->listen_only_mode==1U){
			diag->listen_only_mode = 0U;// disable listen only mode

			FS_WriteIntNum(SA_MB_LISTEN_ONLY_MODE, diag->listen_only_mode); // TODO: to take the storing function in callback, store listen only mode flag to flash
			if(!MB_GetRestartFlag() || diag->listen_only_mode){
				return MB_EC_SDV_4;
			}else{
				return MB_NRS_254;
			}
		}else{
			if(!MB_GetRestartFlag()){
				return MB_EC_SDV_4;
			}else{
				MB_CopyReqToRespPDU();
				return MB_RS_255;
			}
		}


}



/**
  * @brief It processes the diagnostics sub-function 02 Return Diagnostic Register,
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnDR(MB_RTUDiag *diag){
	// diagnostic registers are not define

	//uint16_t dataValue = mbSlave->diag_register;
	if(MB_GetDiagRegValue()>=0 && MB_GetDiagRegValue()<=MB_MAX_REGISTER_VALUE){
		//HAL_UART_Transmit(&huart2, mbSlave->rxBuffer,  mbSlave->rxBufferSize, MB_TRANS_TIMEOUT);
		MB_DIAG_BuildResp2byteData(MB_GetDiagRegValue());
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}


/**
  * @brief It processes the diagnostics sub-function 04 Force Listen Only Mode,
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ForceLOM(MB_RTUDiag *diag){

	/*
	 * TODO: to store the settings of listen only modes
	 * TODO: to turn off all the active communications
	 */
	diag->listen_only_mode = 1U;
	if(diag->listen_only_mode){
		MB_DIAG_SubFuncCallback(MB_DSFC_FLOM_4, diag->listen_only_mode);

		return MB_NRS_254;
	}else{
		return MB_EC_SDV_4; //exception code 4

	}
}

/**
  * @brief It processes the diagnostics sub-function 10 (0A Hex) Clear Counters and Diagnostic Register
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ClearCDR(MB_RTUDiag *diag){

	MB_ClearCounters(diag);
	diag->comm_event_count = 0;
	MB_ClearDiagReg();
	if(MB_CheckCountersClear(diag)){
		MB_CopyReqToRespPDU();
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}

}

/**
  * @brief It processes the diagnostics sub-function 11 (0B Hex) return bus message count
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnBMC(MB_RTUDiag *diag){
	if(diag->bus_msg_count>=0 && diag->bus_msg_count<=MB_MAX_REGISTER_VALUE){
		//HAL_UART_Transmit(&huart2, mbSlave->rxBuffer,  mbSlave->rxBufferSize, MB_TRANS_TIMEOUT);
		MB_DIAG_BuildResp2byteData(diag->bus_msg_count);
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}


/**
  * @brief It processes the diagnostics sub-function 12 (0C Hex) Return Bus Communication Error Count
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnBCEC(MB_RTUDiag *diag){
	if(diag->bus_com_err_count>=0 && diag->bus_com_err_count<=MB_MAX_REGISTER_VALUE){
		//HAL_UART_Transmit(&huart2, mbSlave->rxBuffer,  mbSlave->rxBufferSize, MB_TRANS_TIMEOUT);
		MB_DIAG_BuildResp2byteData(diag->bus_com_err_count);
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}

/**
  * @brief It processes the diagnostics sub-function 13 (0D Hex) Return Bus Exception Error Count
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnBEEC(MB_RTUDiag *diag){
	if(diag->bus_exp_err_count>=0 && diag->bus_exp_err_count<=MB_MAX_REGISTER_VALUE){
			//HAL_UART_Transmit(&huart2, mbSlave->rxBuffer,  mbSlave->rxBufferSize, MB_TRANS_TIMEOUT);
			MB_DIAG_BuildResp2byteData(diag->bus_exp_err_count);
			return MB_RS_255;
		}else{
			return MB_EC_SDV_4;
		}
}


/**
  * @brief It processes the diagnostics sub-function 14 (0E Hex) Return Server Message Count
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnSMC(MB_RTUDiag *diag){
	if(diag->slv_msg_count>=0 && diag->slv_msg_count<=MB_MAX_REGISTER_VALUE){
		//HAL_UART_Transmit(&huart2, mbSlave->rxBuffer,  mbSlave->rxBufferSize, MB_TRANS_TIMEOUT);
		MB_DIAG_BuildResp2byteData(diag->slv_msg_count);
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}


/**
  * @brief It processes the diagnostics sub-function 18 (12 Hex) Return Bus Character Overrun Count
  * @param none
  * @retval 1U/0U; where, 1U = no exception, 0U = exception occurred
  */
uint8_t MB_DSF_ReturnBCOC(MB_RTUDiag *diag){
	if(diag->bus_char_ovrun_count>=0 && diag->bus_char_ovrun_count<=MB_MAX_REGISTER_VALUE){
		MB_DIAG_BuildResp2byteData(diag->bus_char_ovrun_count);
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}


uint8_t MB_DSF_ClearOCF(MB_RTUDiag *diag){
	if(diag->bus_char_ovrun_count>=0 && diag->bus_char_ovrun_count<=MB_MAX_REGISTER_VALUE){
		diag->bus_char_ovrun_count = 0;			// to reset the character overrun counter
		MB_CopyReqToRespPDU();						// it copies requested PDU to response PDU for echoing the requested data
		return MB_RS_255;
	}else{
		return MB_EC_SDV_4;
	}
}


/**
  * @brief It builds the diagnostics sub-function respons for 2 byte data value
  * @param none
  * @retval none
  */
void MB_DIAG_BuildResp2byteData(uint16_t value){
	MB_appendToRespPDU_SAFC();
	MB_appendToRespPDU_SFC();
	MB_appendToRespPDU((uint8_t)(value >> 8u));
	MB_appendToRespPDU((uint8_t)(value & MB_AND_MASK_16T8));
//	MB_appendToRespPDU_CRC();
}


/**
  * @brief It append sub-function code to response PDU
  * @param none
  * @retval none
  */
void MB_appendToRespPDU_SFC(void){
	MB_appendToRespPDU((uint8_t)(MB_getDiagSFC()>>8u));
	MB_appendToRespPDU((uint8_t)(MB_getDiagSFC()&MB_AND_MASK_16T8));
}


/**
  * @brief To get the diagnostics sub-function code from the requested PDU buffer
  * @param none
  * @retval returns the diagnostics sub-function code
  */
uint16_t MB_getDiagSFC(void){
	return MB_get2byteData(MB_FC_INDEX+1);
}


/**
  * @brief To get the diagnostics sub-function data from the requested PDU buffer
  * @param none
  * @retval returns the diagnostics sub-function data
  */
uint16_t MB_getDiagData(void){
	return MB_get2byteData(MB_FC_INDEX+3);
}





/**
  * @brief To reset the modbus settings on restart,
  * @param none
  * @retval none
  */
void MB_ClearCounters(MB_RTUDiag *diag){
	diag->bus_msg_count = 0;				// the bus message counter resets to 0
	diag->slv_msg_count = 0;				// the slave message counter resets to 0
	diag->bus_com_err_count = 0;			// the  Bus Communication Error Count resets to 0
	diag->bus_exp_err_count = 0;			// the  Bus Exception Error Count resets to 0
	diag->bus_char_ovrun_count = 0;		// the  Bus Character Overrun Count resets to 0
	diag->comm_event_count = 0;
}

/**
  * @brief To reset the modbus settings on restart,
  * @param none
  * @retval none
  */
uint8_t MB_CheckCountersClear(MB_RTUDiag *diag){
	return ( diag->bus_msg_count == 0 		&& diag->slv_msg_count == 0 		&&
			diag->bus_com_err_count == 0 	&& diag->bus_exp_err_count == 0 	&&
			diag->bus_char_ovrun_count == 0 && diag->comm_event_count == 0);
}



void MB_DIAG_attachCallback(uint8_t (*callbackFunc)(uint16_t subFuncCode, uint16_t value)){
	MB_DIAG_SubFuncCallback = callbackFunc;
}
