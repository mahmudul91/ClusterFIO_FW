/*
 * IOLink_PL.c
 *
 *  Created on: Oct 28, 2023
 *      Author: wsrra
 */
#include "IOLink_PL.h"
#include  "IOLink_DL.h"
#include "string.h"
#include "IOLink_Utility.h"


/*Other services not in spec-----------------------*/
/**
  * @brief  Initialize PL
  * @param  port: pointer to the port struct
  * @retval none
  */
#ifdef IOL_HW_L6360
IOL_Errors IOL_PL_Initialize(uint8_t portNum, L6360_IOLink_hw_t *l6360){
	IOL_PL_t* pl = (IOL_PL_t*) IOL_GetPLInstance(IOL_GET_PORT(portNum));
	if(pl == NULL && l6360 == NULL) return IOL_ERROR_NULL_POINTER;

	pl->errorStatus = IOL_PL_ERR_STATUS_NONE;
	pl->mode = IOL_PL_MODE_INACTIVE;

	pl->l6360 = l6360;

	return IOL_ERROR_NONE;
}
#endif

/*Services according to specs----------------------*/
/**
  * @brief  5.2.2.1 PL_SetMode
  * The PL-SetMode service is used to setup the electrical characteristics and configurations of
  * the Physical Layer.
  * @param  port: pointer to the port struct
  * @retval none
  */
void IOL_PL_SetMode_req(IOL_Port_t *port, IOL_PL_TargetMode mode){
	IOL_PL_t* pl = (IOL_PL_t*) IOL_GetPLInstance(port);
#ifdef IOL_HW_L6360
	L6360_SetPortMode(pl->l6360, mode);
#endif
}

/**
  * @brief  5.2.2.2 PL_WakeUp
  * The PL-WakeUp service initiates or indicates a specific sequence which prepares the
  * Physical Layer to send and receive communication requests (see 5.3.3.3). This unconfirmed
  * service has no parameters. Its success can only be verified by a Master by attempting to
  * communicate with the Device.
  * @param  port: pointer to the port struct
  * @retval none
  */
void IOL_PL_WakeUp_req(IOL_Port_t *port){
	/* Till now it is not clear that how l6360 will handle wake up sequence
	 * TODO: wake up sequence will be implemented later based on l6360.
	 * */
	IOL_PL_t* pl = (IOL_PL_t*) IOL_GetPLInstance(port);
#ifdef IOL_HW_L6360
	L6360_GenWURQ(pl->l6360);
#endif
}


/* The PL-Transfer service is used to exchange the SDCI data between Data Link Layer and
 * Physical Layer
 * Ref: IO-Link Interface Spec v1.1.3, section 5.2.2.3, Table: 46
 * */
IOL_PL_ErrorStatus IOL_PL_Transfer_req(IOL_Port_t *port, uint8_t *data, uint8_t len){
	/* TODO: to write code for sending data to IOLink master chip
	 * via UART port
	 * */
	IOL_PL_t* pl = (IOL_PL_t*) IOL_GetPLInstance(port);
#ifdef IOL_HW_L6360
	L6360_UartSend(pl->l6360, data, len);
#endif
	return IOL_PL_ERR_STATUS_NONE;
}

IOL_Errors IOL_PL_Transfer_ind(IOL_Port_t *port, uint8_t *data, uint8_t len, IOL_PL_ErrorStatus errorStatus){
	if(errorStatus == IOL_PL_ERR_STATUS_NONE){
		IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
		IOL_LOCK(dl);
		memcpy(dl->rxBuff, data, len);
		dl->rxBuffLen = len;
		dl->isDataRecved = 1U;
		IOL_UNLOCK(dl);
	}else {
		IOL_PL_t *pl = (IOL_PL_t*)IOL_GetPLInstance(port);
		pl->errorStatus = errorStatus;
	}
	return IOL_ERROR_NONE;

}
