/*
 * IOLink_DL.c
 *
 *  Created on: Oct 28, 2023
 *      Author: wsrra
 */



#include <Debug.h>
#include <IOLink_DL.h>
#include <IOLink_PL.h>
#include <IOLink_Time.h>
#include <string.h>
#include <sys/_stdint.h>
#include "IOLink_SM.h"
#include "IOLink_AL.h"



static IOL_Errors IOL_DL_OD_Req(IOL_Port_t *port, IOL_RWDirection rwDir, IOL_CommChannel comCh, uint8_t addrCtrl, uint8_t length, uint8_t *data);
IOL_Errors IOL_DL_OD_Cnf(IOL_Port_t *port, uint8_t *data, uint8_t length, IOL_Errors errorInfo);
IOL_Errors IOL_DL_PD_Req(IOL_Port_t *port, uint8_t pdInAddr, uint8_t pdInLen, uint8_t *pdOut, uint8_t pdOutAddr, uint8_t pdOutLen);
IOL_Errors IOL_DL_PD_Cnf(IOL_Port_t *port, uint8_t *pdInData, IOL_Errors errorInfo);
IOL_Errors IOL_DL_EventFlag_Ind(IOL_Port_t *port, IOL_EHEventFlag flag);
IOL_Errors IOL_DL_PDInStatus_Ind(IOL_DL_t *dl, IOL_PDINStatus status);
IOL_Errors IOL_DL_MHInfo_Ind(IOL_Port_t *port,  IOL_DL_MHInfor mhInfo);
IOL_Errors IOL_DL_ODTrig_Ind(IOL_Port_t *port, uint8_t dataLen);
IOL_Errors IOL_DL_PDTrig_Ind(IOL_Port_t *port, uint8_t dataLen);



static inline void IOL_DL_MH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_MHCommand cmd);
static inline void IOL_DL_PH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_PDHCommand cmd);
static inline void IOL_DL_OH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_OHCommand cmd);
static inline void IOL_DL_IH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_IHCommand cmd);
static inline void IOL_DL_CH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_CHCommand cmd);
static inline void IOL_DL_EH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_EHCommand cmd);


/* internal extra services------------*/
static inline IOL_Errors IOL_DL_BuildMseqType0_Req(	IOL_DL_t *dl, IOL_RWDirection rwDir,
													uint8_t mc, uint8_t ckt, uint8_t data);
static inline IOL_Errors IOL_DL_BuildMseqType1X_Req(IOL_DL_t *dl, IOL_RWDirection rwDir,
													uint8_t mc, uint8_t ckt,
													uint8_t *data, uint16_t len);
static inline IOL_Errors IOL_DL_BuildMseqType2X_Req(IOL_DL_t *dl, IOL_RWDirection rwDir,
													uint8_t mc, uint8_t ckt,
													uint8_t *pdData, uint16_t pdlen,
													uint8_t *odData, uint16_t odlen);
static inline IOL_Errors IOL_DL_ProcessMseqTypeXX_Rsp(IOL_DL_t *dl, uint8_t *data,  uint16_t len);
static inline uint8_t IOL_DL_CalcChecksum(uint8_t *data, uint16_t len);
static inline uint32_t IOL_DL_CalcTmseq(uint8_t txDataLen, uint8_t rxDataLen, IOL_BaudRate baudrate);
void IOL_DL_MH_CheckTimer(IOL_DL_t *dl);
static inline uint32_t IOL_DL_GetMinRecovTime(IOL_MseqCode mseqCode, IOL_BaudRate baudrate);

/* internal extra services------------*/

/* @brief A.2.2 M-sequence TYPE_0
 * M-sequence TYPE_0 is mandatory for all Devices. It only transmits On-request Data. One
 * octet of user data is read or written per cycle.
 * */
static inline IOL_Errors IOL_DL_BuildMseqType0_Req(
		IOL_DL_t *dl, IOL_RWDirection rwDir,
		uint8_t mc, uint8_t ckt, uint8_t odData)
{
	IOL_LOCK(dl);
	dl->txBuffLen = 0;
	dl->txBuff[dl->txBuffLen++] = mc;			// set m-sequence control octet
	dl->txBuff[dl->txBuffLen++] = (ckt&0xC0);			// set m-seq type and checksum but checksum value is 0
	if(rwDir == IOL_RW_DIR_WRITE){
		dl->txBuff[dl->txBuffLen++] = odData;
	}
	/*Calculate & set the checksum*/
	dl->msgh.ckt.octet = ckt;
	dl->msgh.ckt.field.checksum =
			IOL_DL_CalcChecksum(dl->txBuff, dl->txBuffLen);		// calculate checksum
	dl->txBuff[IOL_OCT_INDX_CKT] = dl->msgh.ckt.octet;			// set checksum
	IOL_UNLOCK(dl);
	return IOL_ERROR_NONE;
}

/* @brief A.2.3 M-sequence TYPE_1_x
 * it builds TYPE_1_1, TYPE_1_2 & TYPE_1_3
 * */
static inline IOL_Errors IOL_DL_BuildMseqType1X_Req(
		IOL_DL_t *dl, IOL_RWDirection rwDir,uint8_t mc,
		uint8_t ckt, uint8_t *data, uint16_t len)
{
	if(len > (IOL_MAX_DL_MSG_SIZE-2)){
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}
	IOL_LOCK(dl);
	dl->txBuffLen = 0;
	dl->txBuff[dl->txBuffLen++] = mc;			// set m-sequence control octet
	dl->txBuff[dl->txBuffLen++] = ckt;			// set m-seq type and checksum but checksum value is 0
	if(rwDir == IOL_RW_DIR_WRITE){
		memcpy(&dl->txBuff[dl->txBuffLen], data, len);
		dl->txBuffLen += len;
	}

	/*Calculate & set the checksum*/
	dl->msgh.ckt.octet = ckt;
	dl->msgh.ckt.field.checksum =
			IOL_DL_CalcChecksum(dl->txBuff, dl->txBuffLen);		// calculate checksum
	dl->txBuff[IOL_OCT_INDX_CKT] = dl->msgh.ckt.octet;			// set checksum
	IOL_UNLOCK(dl);
	return IOL_ERROR_NONE;
}

/* @brief A.2.4 M-sequence TYPE_2_x
 * it builds TYPE_1_1, TYPE_1_2 & TYPE_1_3
 * */
static inline IOL_Errors IOL_DL_BuildMseqType2X_Req(
		IOL_DL_t *dl, IOL_RWDirection rwDir, uint8_t mc, uint8_t ckt,
		uint8_t *pdData, uint16_t pdlen, uint8_t *odData, uint16_t odlen)
{
	if(pdlen > (IOL_MAX_DL_MSG_SIZE-2)){
		return IOL_ERROR_PDOUT_LENGTH;
	}
	if(odlen > (IOL_MAX_DL_MSG_SIZE-2)){
		return IOL_ERROR_ODOUT_LENGTH;
	}
	IOL_LOCK(dl);

	dl->txBuffLen = 0;
	dl->txBuff[dl->txBuffLen++] = mc;
	dl->txBuff[dl->txBuffLen++] = ckt;

	if(pdlen != 0U){
		memcpy(&dl->txBuff[dl->txBuffLen], pdData, pdlen);
		dl->txBuffLen += pdlen;
	}
	if(rwDir == IOL_RW_DIR_WRITE && odlen != 0U){
		memcpy(&dl->txBuff[dl->txBuffLen], odData, odlen);
		dl->txBuffLen += odlen;
	}

	/*Calculate & set the checksum*/
	dl->msgh.ckt.octet = ckt;
	dl->msgh.ckt.field.checksum =
			IOL_DL_CalcChecksum(dl->txBuff, dl->txBuffLen);		// calculate checksum
	dl->txBuff[IOL_OCT_INDX_CKT] = dl->msgh.ckt.octet;			// set checksum
	IOL_UNLOCK(dl);
	return IOL_ERROR_NONE;
}

/* @brief process the received response for all the
 * M-sequence type
 * */
static inline IOL_Errors IOL_DL_ProcessMseqTypeXX_Rsp(IOL_DL_t *dl, uint8_t *data,  uint16_t len){
	if(len > 0U){

		IOL_CKSOctet_t cks;
		cks.octet = data[len-1];		// last octet is the CKS octet
		data[len-1] &= 0xC0;
		if(cks.field.checksum == IOL_DL_CalcChecksum(data, len)){
			IOL_LOCK(&dl->msgh);
			dl->msgh.rxData = data;
			dl->msgh.rxDataLen = len;
			IOL_UNLOCK(&dl->msgh);
		}else{
			dl->msgh.mhInfo = IOL_DL_MHINFO_CHECKSUM_MISMATCH;
		}
		return IOL_ERROR_NONE;
	}else {
		return IOL_ERROR_NO_DATA;
	}
}

/* @brief A.1.6 Calculation of the checksum
 * */
static inline uint8_t IOL_DL_CalcChecksum(uint8_t *data, uint16_t len){
	IOL_BitAccess_t checksum_8;		// 8 bit checksum
	IOL_BitAccess_t checksum_6;		// 6 bit checksum

	/*Calculate the 8 bit checksum*/
	checksum_8.octet = IOL_CHECKSUM_SEED_VALUE;
	data[1] &= 0xC0;	// set 0 to checksum(6) bit 0 to 5
	for(uint16_t i = 0; i < len; i++){
		checksum_8.octet ^= data[i];		// xro octet by octet
	}

	/*onvert 8 bit to 6 bit checksum*/
	checksum_6.bits.b0 = checksum_8.bits.b0 ^ checksum_8.bits.b1;
	checksum_6.bits.b1 = checksum_8.bits.b2 ^ checksum_8.bits.b3;
	checksum_6.bits.b2 = checksum_8.bits.b4 ^ checksum_8.bits.b5;
	checksum_6.bits.b3 = checksum_8.bits.b6 ^ checksum_8.bits.b7;
	checksum_6.bits.b4 = checksum_8.bits.b0 ^ checksum_8.bits.b2 ^
						 checksum_8.bits.b4 ^ checksum_8.bits.b6;
	checksum_6.bits.b5 = checksum_8.bits.b1 ^ checksum_8.bits.b3 ^
						 checksum_8.bits.b5 ^ checksum_8.bits.b7;
	checksum_6.bits.b6 = 0;	// set 0 to 6th bit
	checksum_6.bits.b7 = 0;	// set 0 to 7th bit
	return checksum_6.octet;
}

/* @brief A.5.6 calculate CHKPDU
 * */
static inline uint8_t IOL_DL_CalcChkpdu(uint8_t *data, uint8_t len){
	uint8_t chkpdu = 0;
	data[len-1] = 0;
	for(uint8_t i = 0; i < len; i++){
		chkpdu ^= data[i];
	}
	return chkpdu;
}

/* @brief A.3.6 calculate M-sequence time
 *
 * */
static inline uint32_t IOL_DL_CalcTmseq(uint8_t txDataLen, uint8_t rxDataLen, IOL_BaudRate baudrate){
	uint32_t tA = 0;
	uint32_t t1 = 0;
	uint32_t t2 = 0;
	uint32_t tBit = 0;

	switch (baudrate) {
		case IOL_BAUDRATE_COM1:
			tBit = IOL_TBIT_COM1;//208.33us
			break;
		case IOL_BAUDRATE_COM2:
			tBit = IOL_TBIT_COM2;//26.05
			break;
		case IOL_BAUDRATE_COM3:
			tBit = IOL_TBIT_COM3;//4.34
			break;
		default:
			return 0;
			break;
	}

	tA = tBit * IOL_DEF_TA_MLTPLR; //5*4.34
	t1 = tBit * IOL_DEF_T1_MLTPLR;//1*4.34
	t1 = tBit * IOL_DEF_T2_MLTPLR;//1*4.34

	return ((txDataLen+rxDataLen) * IOL_UART_FRAME_SIZE * tBit +
			tA + (txDataLen-1)*t1+(rxDataLen-1)*t2)+1U;  // 1 offset is added (it is not in spec))
}

void IOL_DL_MH_StartTimer(IOL_DL_t *dl){
	IOL_Timer_Start(&dl->msgh.tmseq);
	IOL_Timer_Start(&dl->msgh.tcyc);
}

void IOL_DL_MH_ResetTimer(IOL_DL_t *dl){
	IOL_Timer_Reset(&dl->msgh.tmseq);
	IOL_Timer_Reset(&dl->msgh.tcyc);
}




/* Check all the timers are elapsed or not*/
void IOL_DL_CheckTimerElapsed(IOL_DL_t *dl){
	/*Check t_m-sequence is elapsed*/
	if(IOL_Timer_IsTimeout(&dl->msgh.tmseq) == IOL_TIMER_TIMEOUT){
		IOL_Timer_Stop(&dl->msgh.tmseq);
		//TODO: to invoke message handler state machine here.
	}

	/*Check t_cyc is elasped*/
	if(IOL_Timer_IsTimeout(&dl->msgh.tcyc) == IOL_TIMER_TIMEOUT){
		IOL_Timer_Stop(&dl->msgh.tcyc);
	}

	/*Check t_cyc is elapsed*/
	if(IOL_Timer_IsTimeout(&dl->isduh.tisdu) == IOL_TIMER_TIMEOUT){
		IOL_Timer_Stop(&dl->isduh.tisdu);
	}
}

/* @brief get the minimum recovery time for startup and preoperate mode
 * @return minimum recovery time in us
 * */
static inline uint32_t IOL_DL_GetMinRecovTime(IOL_MseqCode mseqCode, IOL_BaudRate baudrate){
	float tbit = 0.0;
	switch (baudrate) {
		case IOL_BAUDRATE_COM1:
			tbit = IOL_TBIT_COM1;
			break;
		case IOL_BAUDRATE_COM2:
			tbit = IOL_TBIT_COM2;
			break;
		case IOL_BAUDRATE_COM3:
			tbit = IOL_TBIT_COM3;
			break;
		default:
			return 0U;
			break;
	}
	if(	mseqCode == IOL_MSEQCODE_TYPE_0 ||
		mseqCode == IOL_MSEQCODE_PREOP_TYPE_1_2)
	{
		return 100U*tbit;
	}else if(mseqCode == IOL_MSEQCODE_PREOP_TYPE_1_V_8){
		return 210U*tbit;
	}else if(mseqCode == IOL_MSEQCODE_PREOP_TYPE_1_V_32){
		return 550U*tbit;
	}else{
		return 0;
	}
}


/*Specified function------------------*/
/*@brief command to change the state of the Message handler state machine
 * */
static inline void IOL_DL_MH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_MHCommand cmd){
	dl->msgh.mhCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_MessageHandler(port);
}

/*@brief command to change the state of the process data handler state machine
 * */
static inline void IOL_DL_PH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_PDHCommand cmd){
	dl->pdh.pdhCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_PDHandler(port);
}

/*@brief command to change the state of the on request data handler state machine
 * */
static inline void IOL_DL_OH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_OHCommand cmd){
	dl->odh.ohCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_ODHandler(port);
}

/*@brief command to change the state of the ISDU handler state machine
 * */
static inline void IOL_DL_IH_Conf(IOL_Port_t *port, IOL_DL_t *dl, IOL_DL_IHCommand cmd){
	dl->isduh.ihCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_ISDUHandler(port);
}

/*@brief command to change the state of the command handler state machine
 * */
static inline void IOL_DL_CH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_CHCommand cmd){
	dl->cmdh.chCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_CommandHandler(port);
}

/*@brief command to change the state of the event handler state machine
 * */
static inline void IOL_DL_EH_Conf(IOL_Port_t *port, IOL_DL_t *dl,  IOL_DL_EHCommand cmd){
	dl->evnth.ehCmd = cmd;
	//TODO: In example code, related state machine is called from here, but why?
	IOL_DL_EventHandler(port);
}


/*DL-B services----------------------------------------*/
/* @brief The DL_ReadParam service is used by the AL to read a parameter value from the Device via
 * the page communication channel.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.2
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_ReadParam_Req(IOL_Port_t *port, uint8_t address){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);


	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_ReadParam_Cnf(port, 0, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}
	//2. check state validity
	if(dl->msgh.state != IOL_DL_MH_STATE_PREOPERATE_6){
		IOL_DL_ReadParam_Cnf(port, 0, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//3. check the address validity
	if(address > IOL_MAX_DIR_PARM_ADDR) return IOL_ERROR_DIR_PARAM_ADDR;

	//4. set address

	IOL_LOCK(&dl->odh);
	dl->odh.address = address;
	IOL_UNLOCK(&dl->odh);

	dl->msgh.rwCmd = IOL_DL_MH_RWCMD_READPARAM;
	return IOL_ERROR_NONE;
}

/* @brief The IOL_DL_ReadParam_Cnf service is used to return the response of IOL_DL_ReadParam_Req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.2
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_ReadParam_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	IOL_AL_DL_ReadParam_Cnf(port, value, errorInfo);
	return IOL_ERROR_NONE;
}

/* @brief The DL_WriteParam service is used by the AL to write a parameter value to the Device via
 * the page communication channel.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.3
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_WriteParam_Req(IOL_Port_t *port, uint8_t address, uint8_t value){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_WriteParam_Cnf(port, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}
	//2. check state validity
	if(dl->msgh.state != IOL_DL_MH_STATE_PREOPERATE_6){
		IOL_DL_WriteParam_Cnf(port, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//1. check address
	if(!(address >= 16U && address <= IOL_MAX_DIR_PARM_ADDR)) return IOL_ERROR_DIR_PARAM_ADDR;

	//2. check the right state
	//TODO: To implement later

	//3. check the communication status
	//TODO: To implement later

	//4. set address & value
	IOL_LOCK(&dl->odh);
	dl->odh.address = address;
	*dl->odh.outData = value;
	dl->odh.outDataLen = 1;
	IOL_UNLOCK(&dl->odh);

	dl->msgh.rwCmd = IOL_DL_MH_RWCMD_WRITEPARAM;
	return IOL_ERROR_NONE;
}

/* @brief The IOL_DL_WriteParam_Cnf service is used to return the response of IOL_DL_WriteParam_Req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.3
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_WriteParam_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	IOL_AL_DL_WriteParam_Cnf(port,errorInfo);
	return IOL_ERROR_NONE;
}

/* @brief The DL_Read.req service is used by System Management to read a Device parameter value via
 * the page communication channel.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.4
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Read_Req(IOL_Port_t *port, uint8_t address){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_ReadParam_Cnf(port, 0, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}
	//2. check state validity
	if(dl->msgh.state != IOL_DL_MH_STATE_STARTUP_2){
		IOL_DL_ReadParam_Cnf(port, 0, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//3. check the address validity
	if(address > 15U) return IOL_ERROR_DIR_PARAM_ADDR;


	//4. set address to be read
	IOL_LOCK(&dl->odh);
	dl->odh.address = address;
	IOL_UNLOCK(&dl->odh);

	dl->msgh.rwCmd = IOL_DL_MH_RWCMD_READ;
	return IOL_ERROR_NONE;
}

/* @brief The DL_Read.cnf service is used to return the response of DL_Read.req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.4
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Read_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo){
	IOL_SM_DL_Read_Cnf(port,value,errorInfo);
	return IOL_ERROR_NONE;
}

/* @brief The DL_Write service is used by System Management to write a Device parameter value to
 * the Device via the page communication channel.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.5
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Write_Req(IOL_Port_t *port, uint8_t address, uint8_t value){
	IOL_DL_t *dl =  (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_Write_Cnf(port, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}
	//2. check state validity
	if( dl->msgh.state != IOL_DL_MH_STATE_STARTUP_2 ||
		dl->msgh.state != IOL_DL_MH_STATE_PREOPERATE_6)
	{
		IOL_DL_Write_Cnf(port, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//1. check address
	if(!(address >= 0U && address <= 15)) return IOL_ERROR_DIR_PARAM_ADDR;

	//2. check the right state
	//TODO: To implement later

	//3. check the communication status
	//TODO: To implement later

	//4. set address & value
	IOL_LOCK(&dl->odh);
	dl->odh.address = address;
	*dl->odh.outData = value;
	dl->odh.outDataLen = 1;
	IOL_UNLOCK(&dl->odh);

	dl->msgh.rwCmd = IOL_DL_MH_RWCMD_WRITE;
	return IOL_ERROR_NONE;
}

/* @brief IOL_DL_Write_Cnf service is used to return the response ofIOL_DL_Write_Req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.5
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Write_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	return IOL_ERROR_NONE;
}


/* @brif The DL_ISDUTransport service is used to transport an ISDU. This service is used by the
 * Master to send a service request from the Master application layer to the Device. It is used by
 * the Device to send a service response to the Master from the Device application layer
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.6
 * */
IOL_Errors IOL_DL_ISDUTransport_Req(IOL_Port_t *port, IOL_ISDUValueList_t *valueList){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_ISDUTransport_Cnf(port, 0, IOL_ISDU_IS_QUAL_NO_SERVICE_0, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}

	//2. check state validity
	if(dl->msgh.state != IOL_DL_MH_STATE_PREOPERATE_6){
		IOL_DL_ISDUTransport_Cnf(port, 0, IOL_ISDU_IS_QUAL_NO_SERVICE_0, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}

	//3. check data validity
	if((valueList->index < 2)){
		IOL_DL_ISDUTransport_Cnf(port, 0, IOL_ISDU_IS_QUAL_NO_SERVICE_0, IOL_ERROR_VALUE_OUT_OF_RANGE);
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}

	uint8_t iService = 0;
	uint16_t length = 0U;
	if(valueList->rwDir == IOL_RW_DIR_READ){
		iService = IOL_ISDU_ISEV_READ_MASK;
	}
	//4. set address & value
	IOL_LOCK(&dl->odh);
	/* Set I-service based on index & subindex range */
	if(valueList->index > 255U){
		iService |= 0x03;
		length = 5;
	}else{
		if(valueList->subindex == 0U){
			iService |= 0x01;
			length = 3;
		}else{
			iService |= 0x02;
			length = 4;
		}
	}

	 length += valueList->dataLen;
	 dl->odh.outDataLen = 0U;
	// insert i-Service & length
	if(length > IOL_MAX_ISDU_EXTLENGTH) {												// out of range
		IOL_DL_ISDUTransport_Cnf(port, 0, IOL_ISDU_IS_QUAL_NO_SERVICE_0, IOL_ERROR_VALUE_OUT_OF_RANGE);
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}else if(length > IOL_MAX_ISDU_LENGTH){
		dl->odh.outData[dl->odh.outDataLen++] = ((iService<<4)&0xF0) | 0x01;			// I-service and length
		dl->odh.outData[dl->odh.outDataLen++] = length;									// ExtLength
	}else{
		dl->odh.outData[dl->odh.outDataLen++] = ((iService<<4)&0xF0) | (length&0x0F);	// I-service and length
	}
	// insert index & sub-index
	if(valueList->index > 255U){
		dl->odh.outData[dl->odh.outDataLen++] = (valueList->index>>8)&0xFF;				// MSB of Index
        dl->odh.outData[dl->odh.outDataLen++] = valueList->index&0xFF;					// LSB of Index
        dl->odh.outData[dl->odh.outDataLen++] = valueList->subindex;					// subindex
	}else{
		dl->odh.outData[dl->odh.outDataLen++] = valueList->index;						// index
		if(valueList->subindex > 0) {
			dl->odh.outData[dl->odh.outDataLen++] = valueList->subindex;				// subindex
		}
	}
	memcpy(&dl->odh.outData[dl->odh.outDataLen], valueList->data, valueList->dataLen);				// Data
	dl->odh.outData[dl->odh.outDataLen++] = 0;
	dl->odh.outData[dl->odh.outDataLen-1] = IOL_DL_CalcChkpdu(dl->odh.outData, dl->odh.outDataLen);	// CHKPDU

	IOL_UNLOCK(&dl->odh);

	dl->msgh.rwCmd = IOL_DL_MH_RWCMD_ISDUTRANS;
	return IOL_ERROR_NONE;
}

/* @brief IOL_DL_ISDUTransport_Cnf service is used to return the response IOL_DL_ISDUTransport_Req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.6
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_ISDUTransport_Cnf(IOL_Port_t *port, uint8_t *data, IOL_IServiceQualifier qualifier, IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	IOL_AL_DL_ISDUTransport_Cnf(port,data,qualifier, errorInfo);
	return IOL_ERROR_NONE;
}

/* @brief The Master’s application layer uses the DL_PDOutputUpdate service to update the output
 * data (Process Data from Master to Device) on the data link layer.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.8
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PDOutputUpdate_Req(IOL_Port_t *port,  uint8_t *data, uint8_t len){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. Check comm status
	if(dl->comStatus == IOL_DL_COM_FAILED){
		IOL_DL_PDOutputUpdate_Cnf(port, IOL_DL_TRANS_NOT_PERMITTED, IOL_ERROR_NO_COMM);
		return IOL_ERROR_NO_COMM;
	}

	//2. check state validity
	if(dl->pdh.state != IOL_DL_PDH_STATE_PDSINGLE_1 ||
			dl->pdh.state != IOL_DL_PDH_STATE_PDOUTINTERLEAVE_3)
	{
		IOL_DL_PDOutputUpdate_Cnf(port, IOL_DL_TRANS_NOT_PERMITTED, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//3. check data length validity
	if(len > IOL_MAX_DL_MSG_SIZE){
		return IOL_ERROR_PDOUT_LENGTH;
	}
	//Lock the process
	IOL_LOCK(&dl->pdh);
	memcpy(dl->pdh.pdOutData, data, len);
	dl->pdh.pdOutLen = len;
	/* for interleave mode, odd-numbered PD length the remaining octets
	 * within the messages are padded with 0x00.
	 * */
	if(dl->msgh.mseqSubType == IOL_MSEQ_SUB_TYPE_1_1 && len&0x01){
		dl->pdh.pdOutData[dl->pdh.pdOutLen] = 0x00;
	}
	//unlock the process
	IOL_UNLOCK(&dl->pdh);
//	// set the Transport Status
//	dl->pdh.transStatus =  (dl->pdh.state == IOL_DL_PDH_STATE_INACTIVE_0) ?
//							IOL_DL_TRANS_NOT_PERMITTED : IOL_DL_TRANS_PERMITTED ;


	return IOL_ERROR_NONE;
}
/* @brief cnf service of DL_PDOutputUpdate service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.8
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PDOutputUpdate_Cnf(IOL_Port_t *port, IOL_DL_TransportStatus tranStatus,  IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	IOL_AL_DL_PDOutputUpdate_Cnf(port, errorInfo);
	return IOL_ERROR_NONE;
}

/* @brief The data link layer on the Master uses the DL_PDInputTransport service to transfer the
 * content of input data (Process Data from Device to Master) to the application layer.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.11
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PDInputTransport_Ind(IOL_Port_t *port, uint8_t *data, uint8_t len){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	IOL_AL_DL_PDInputTransport_Ind(port, data, len);
	return IOL_ERROR_NONE;
}

/* @brief 7.2.1.12 DL_PDCycle
 * The data link layer uses the DL_PDCycle service to indicate the end of a Process Data cycle
 * to the application layer
 * */
IOL_Errors IOL_DL_PDCycle_Ind(IOL_Port_t *port){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	return IOL_ERROR_NONE;
}

/* @brief The DL uses the DL_Mode service to report to System Management that a certain operating
 * status has been reached
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.14
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_SetMode_Req(IOL_Port_t *port, IOL_MDHMode mode, IOL_ModeValueList_t *valueList){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	//1. check state & parameter validity
	if(dl->mdh.state == IOL_DL_MDH_STATE_ESTABLISHCOMM_1){
		IOL_DL_SetMode_Cnf(port, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}else if(mode != IOL_MDH_MODE_INACTIVE &&
			mode != IOL_MDH_MODE_STARTUP &&
			mode != IOL_MDH_MODE_PREOPERATE &&
			mode != IOL_MDH_MODE_OPERATE )
	{
		IOL_DL_SetMode_Cnf(port, IOL_ERROR_PARM_CONFLICT);
		return IOL_ERROR_PARM_CONFLICT;
	}else if(	valueList->mseqType != IOL_MSEQ_SUB_TYPE_0 &&
				!(valueList->mseqType >= IOL_MSEQ_SUB_TYPE_1_1 && valueList->mseqType <= IOL_MSEQ_SUB_TYPE_1_V) &&
				!(valueList->mseqType >= IOL_MSEQ_SUB_TYPE_2_1 && valueList->mseqType <= IOL_MSEQ_SUB_TYPE_2_V))
	{
		IOL_DL_SetMode_Cnf(port, IOL_ERROR_PARM_CONFLICT);
		return IOL_ERROR_PARM_CONFLICT;
	}


	dl->mdh.mode = mode;		// set mode
	dl->msgh.mseqTime = valueList->mseqTime;		// TODO: to set all meseqTime
	dl->msgh.mseqSubType = valueList->mseqType;
	dl->msgh.pdInLen = valueList->pdInLen;			// TODO: to set all pdInLen
	dl->msgh.pdOutLen = valueList->pdOutLen;		// TODO: to set all pdOutLen
	dl->msgh.odLenPerMsg = valueList->odLenPerMsg;	// TODO: to set all odLenPerMsg

	return IOL_ERROR_NONE;
}

/* @brief IOL_DL_SetMode_Cnf service is used to return the response IOL_DL_SetMode_Req service
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.13
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_SetMode_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	//TODO: To convey msg to the AL
	return IOL_ERROR_NONE;
}

/* @brief The DL uses the DL_Mode service to report to System Management that a certain operating
 * status has been reached
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.14
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Mode_Ind(IOL_Port_t *port, IOL_MDHMode realMode){
	IOL_SM_DL_Mode_Ind(port, realMode);		// Convey msg to SM layer
	return IOL_ERROR_NONE;
}

/* @brief The service DL_Event indicates a pending status or error information. The cause for an Event
 * is located in a Device and the Device application triggers the Event transfer.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.14
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_Event_Ind(IOL_Port_t *port, IOL_EventInstance instance, IOL_EventType type,
		IOL_EventMode mode, IOL_DeviceEventCodes eventCode, uint8_t eventsLeft)
{
//	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
    //TODO: To convey msg to the AL
	IOL_AL_DL_Event_Ind(port, instance, type, mode, eventCode, eventsLeft);
    return IOL_ERROR_NONE;
}

/* @brief The DL_EventConf service confirms the transmitted Events via the Event handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.16
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_EventConf_Req(IOL_Port_t *port){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	dl->evnth.eventConf = 1U;
	return IOL_ERROR_NONE;
}

IOL_Errors IOL_DL_Control_Ind(IOL_Port_t *port, IOL_ControlCode ctrlCode){
	//TODO: propagate control code to AL layer
	return IOL_ERROR_NONE;
}

/*DL-A services----------------------------------------*/
/* @brief The OD service is used to set up the On-request Data for the next message to be sent.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.2
 *
 * @retval error status
 * */
static IOL_Errors IOL_DL_OD_Req(IOL_Port_t *port, IOL_RWDirection rwDir, IOL_CommChannel comCh,
		uint8_t addrCtrl, uint8_t length, uint8_t *data)
{

	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	if(addrCtrl > IOL_MAX_ADDRCTRL) return IOL_ERROR_ADDRCTRL_INVALID;		//check the address or flow control
	if(length > IOL_MAX_OD_SIZE) return IOL_ERROR_ODOUT_LENGTH;				// check the length
	//check the valid communication channel
	if(	comCh == IOL_COMM_CH_DIAGNOSIS || comCh == IOL_COMM_CH_PAGE || comCh == IOL_COMM_CH_ISDU){

		dl->odh.outData[0] = rwDir | comCh | addrCtrl;
		dl->odh.outData[1] = (dl->msgh.mseqSubType & 0x30) << 2;		// set the M Sequence type
		//TODO: Not completed
		if(rwDir == IOL_RW_DIR_READ){

		}else if(rwDir == IOL_RW_DIR_WRITE){

		}

	}else{
		return IOL_ERROR_COMM_CH_INVALID;
	}

	return IOL_ERROR_NONE;
}

/* @brief The confirmation of the od request service convey received devices data to the od handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.2
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_OD_Cnf(IOL_Port_t *port, uint8_t *data, uint8_t length, IOL_Errors errorInfo){
	//TODO: to implement later
	if(errorInfo == IOL_ERROR_NONE){
		if(length > IOL_MAX_OD_SIZE) return IOL_ERROR_ODIN_LENGTH;				// check the length
	}else{

	}
	return IOL_ERROR_NONE;
}

/* @brief The PD service is used to setup the Process Data to be sent through the process
 * communication channel.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.3
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PD_Req(IOL_Port_t *port, uint8_t pdInAddr, uint8_t pdInLen, uint8_t *pdOut, uint8_t pdOutAddr, uint8_t pdOutLen){
	if(pdInLen > IOL_MAX_PD_SIZE) return IOL_ERROR_PDIN_LENGTH; 	// check the length
	if(pdOutLen > IOL_MAX_PD_SIZE) return IOL_ERROR_PDOUT_LENGTH; 	// check the length
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	/*No need to check for the state validity because
	 * this service is called in all the state of pd handler*/
	if(dl->msgh.mseqSubType == IOL_MSEQ_SUB_TYPE_1_1){
		dl->msgh.mc.field.address = pdOutAddr;
		IOL_DL_BuildMseqType1X_Req(dl, dl->msgh.rwDir, dl->msgh.mc.octet, dl->msgh.ckt.octet, pdOut, pdOutLen);
	}

	//TODO: to implement later

	return IOL_ERROR_NONE;
}

/* @brief The confirmation of the pd request service convey received devices data to the pd handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.3
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PD_Cnf(IOL_Port_t *port, uint8_t *pdInData, IOL_Errors errorInfo){
	IOL_DL_PDHandler(port);
	return IOL_ERROR_NONE;
}

/* @brief The service PDInStatus sets and signals the validity qualifier of the input Process Data.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.5
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_PDInStatus_Ind(IOL_DL_t *dl, IOL_PDINStatus status){
	dl->cmdh.pdInStatus = status;
	//TODO: write code here
	return IOL_ERROR_NONE;
}


/* @brief The service MHInfo signals an exceptional operation within the message handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.6
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_MHInfo_Ind(IOL_Port_t *port, IOL_DL_MHInfor mhInfo){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	dl->msgh.mhInfo = mhInfo;
	IOL_DL_ModeHandler(port);
	return IOL_ERROR_NONE;
}

/* @brief The service ODTrig is only available on the Master. The service triggers the On-request Data
 * handler and the ISDU, Command, or Event handler currently in charge to provide the On
 * request Data (via the OD service) for the next Master message.
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.7
 *
 * @retval error status
 * */
IOL_Errors IOL_DL_ODTrig_Ind(IOL_Port_t *port, uint8_t dataLen){
	//TODO: to finishe the implementation
	IOL_DL_ODHandler(port);
	return IOL_ERROR_NONE;
}

IOL_Errors IOL_DL_PDTrig_Ind(IOL_Port_t *port, uint8_t dataLen){
	//TODO: to finishe the implementation
	IOL_DL_PDHandler(port);
	return IOL_ERROR_NONE;
}

/*Extra associative services-----------------------------------------------*/


/*State machines of DL layer--------------------------------------------------*/


/*State machine of the  Master Mode handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.2.4, Figure: 35 & 36, Table: 44
 * */
void IOL_DL_ModeHandler(IOL_Port_t *port){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	char pData[100] = {0};

	DBG_SPRINT_APPEND(pData,"\r\nDL>");
	switch (dl->mdh.state) {
		case IOL_DL_MDH_STATE_IDLE_0:
			/* Waiting on wakeup request from System Management (SM): DL_SetMode (STARTUP) */
			DBG_SPRINT_APPEND(pData,"IDL>");
			dl->comStatus = IOL_DL_COM_FAILED;
			if(dl->mdh.mode == IOL_MDH_MODE_STARTUP){							// T1: state 0 to 1
				DBG_SPRINT_APPEND(pData,"STUP>");
				dl->mdh.retryEstCom = 0;
				dl->mdh.isEstdComx = 0;
				dl->mdh.state = IOL_DL_MDH_STATE_ESTABLISHCOMM_1;
			}else if(dl->mdh.mode != IOL_MDH_MODE_INACTIVE){

				DBG_SPRINT_NL("Error: IOL_PORT_%d{DL>State:IDLE_0>Wrong trigger:%d}",
						IOL_GetPortNumber(port), dl->mdh.mode);
			}
//			DBG_PRINT(pData);
			break;

		case IOL_DL_MDH_STATE_STARTUP_2:											// SM uses the STARTUP state for Device identification
			dl->comStatus = IOL_DL_COM_OK;
			//TODO: Need to know how to fetch Device indent.
			if(dl->mdh.mode == IOL_MDH_MODE_PREOPERATE){							// T6: state 2 to 3
				/* T6: state 2 to 3:
				 * SM requested the PREOPERATE state. Activate On-request Data (call
				 * OH_Conf_ACTIVE in Figure 48), ISDU (call IH_Conf_ACTIVE in Figure
				 * 51), and Event handler (call EH_Conf_ACTIVE in Figure 55). Change
				 * message handler state to PREOPERATE (call MH_Conf_PREOPERATE in
				 * Figure 40). Return DL_Mode.ind (PREOPERATE) to SM.
				 * */
				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_ACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_ACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_ACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_PREOPERATE);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_PREOPERATE);
			}else if(dl->mdh.mode == IOL_MDH_MODE_OPERATE){						// T11: state 2 to 4
				/* T11: state 2 to 4:
				 * SM requested the OPERATE state. Activate the Process Data handler (call
				 * PD_Conf_SINGLE or PD_Conf_INTERLEAVE in Figure 46 according to the
				 * Master port configuration). Activate On-request Data (call
				 * OH_Conf_ACTIVE in Figure 48), ISDU (call IH_Conf_ACTIVE in Figure
				 * 51), and Event handler (call EH_Conf_ACTIVE in Figure 55). Change
				 * message handler state to OPERATE (call MH_Conf_OPERATE in Figure
				 * 40). Return DL_Mode.ind (OPERATE) to SM
				 * */
				if(dl->msgh.mseqSubType & (IOL_MSEQ_SUB_TYPE_2_X & 0x20)){
					IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_SINGLE);
				}else if(dl->msgh.mseqSubType == IOL_MSEQ_SUB_TYPE_1_1){
					IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_INTERLEAVE);
				}

				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_ACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_ACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_ACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_OPERATE);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_OPERATE);
			}else{
				DBG_SPRINT_NL("Error: IOL_PORT_%d{DL>State:STARTUP_2>Wrong trigger:%d}",
						IOL_GetPortNumber(port), dl->mdh.mode);
			}
			break;
		case IOL_DL_MDH_STATE_PREOPERATE_3:
			if(dl->mdh.mode == IOL_MDH_MODE_INACTIVE){
				/* T8: state 3 to 0:
				 * SM requested the SIO mode. Deactivate all handlers (call
				 * xx_Conf_INACTIVE). Return DL_Mode.ind (INACTIVE) to SM. See 7.3.2.3.
				 */
				IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_INACTIVE);
				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_INACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_INACTIVE);
			}else if(dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST){
				/* T9: state 3 to 0:
				 * Message handler informs about lost communication via the DL-A service
				 * MHInfo (COMLOST). Deactivate all handlers (call xx_Conf_INACTIVE).
				 * Return DL_Mode.ind (COMLOST) to SM.
				 */
				IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_INACTIVE);
				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_INACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COMLOST);
			}else if(dl->mdh.mode == IOL_MDH_MODE_STARTUP){
				/* T7: state 3 to 1:
				 * SM requested the STARTUP state. Change message handler state to
				 * STARTUP (call MH_Conf_STARTUP in Figure 40). Deactivate On-request
				 * Data (call OH_Conf_INACTIVE in Figure 48), ISDU (call IH_Conf_INACTIVE in Figure 51), and Event handler (call EH_Conf_INACTIVE in
				 * Figure 55). Return DL_Mode.ind (STARTUP) to SM.
				 */

				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_STARTUP);
				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COMLOST);
			}else if(dl->mdh.mode == IOL_MDH_MODE_OPERATE){
				/* T10: state 3 to 4:
				 * SM requested the OPERATE state. Activate the Process Data handler (call
				 * PD_Conf_SINGLE if M-sequence type = TYPE_2_x, or
				 * PD_Conf_INTERLEAVE if M-sequence type = TYPE_1_1 in Figure 46).
				 * Change message handler state to OPERATE (call MH_Conf_OPERATE in
				 * Figure 40). Return DL_Mode.ind (OPERATE) to SM.
				 */
				// mayby type_2_x mseq checking is implemented in wrong way in example code.
				if(dl->msgh.mseqSubType & (IOL_MSEQ_SUB_TYPE_2_X & 0x20)){
					IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_SINGLE);
				}else if(dl->msgh.mseqSubType == IOL_MSEQ_SUB_TYPE_1_1){
					IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_INTERLEAVE);
				}
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_OPERATE);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_OPERATE);
			}else{
				DBG_SPRINT_NL("Error: IOL_PORT_%d{DL>State:PREOPERATE_3>Wrong trigger:%d}",
						IOL_GetPortNumber(port), dl->mdh.mode);
			}
			break;
		case IOL_DL_MDH_STATE_OPERATE_4:
			if(dl->mdh.mode == IOL_MDH_MODE_INACTIVE){
				/* T13: state 4 to 0:
				 * SM requested the SIO state. Deactivate all handlers (call
				 * xx_Conf_INACTIVE). Return DL_Mode.ind (INACTIVE) to SM. See 7.3.2.3.
				 */


				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_INACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_INACTIVE);
			}else if(dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST){
				/* T14: state 4 to 0:
				 * Message handler informs about lost communication via the DL-A service
				 * MHInfo (COMLOST). Deactivate all handlers (call xx_Conf_INACTIVE).
				 * Return DL_Mode.ind (COMLOST) to SM.
				 */

				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_INACTIVE);
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COMLOST);
			}else if(dl->mdh.mode == IOL_MDH_MODE_STARTUP){
				/* T12: state 4 to 2:
				 * SM requested the STARTUP state. Change message handler state to
				 * STARTUP (call MH_Conf_STARTUP in Figure 40). Deactivate Process
				 * Data (call PD_Conf_INACTIVE in Figure 46), On-request Data (call
				 * OH_Conf_INACTIVE in Figure 48), ISDU (call IH_Conf_INACTIVE in
				 * Figure 51), and Event handler (call EH_Conf_INACTIVE in Figure 55).
				 * Return DL_Mode.ind (STARTUP) to SM.
				 */
				IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_STARTUP);
				IOL_DL_PH_Conf(port, dl, IOL_DL_PDH_CMD_INACTIVE);
				IOL_DL_OH_Conf(port, dl, IOL_DL_OH_CMD_INACTIVE);
				IOL_DL_IH_Conf(port, dl, IOL_DL_IH_CMD_INACTIVE);
				IOL_DL_EH_Conf(port, dl, IOL_DL_EH_CMD_INACTIVE);

				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_STARTUP);
			}else{
				DBG_SPRINT_NL("Error: IOL_PORT_%d{DL>State:OPERATE_4>Wrong trigger:%d}",
						IOL_GetPortNumber(port), dl->mdh.mode);
			}
			break;
		case IOL_DL_MDH_STATE_ESTABLISHCOMM_1:
			/* Goto sub state machine state to invoke wake up sequence
			 * */
			dl->comStatus = IOL_DL_COM_FAILED;
			dl->mdh.state = IOL_DL_MDH_STATE_SM_WURQ_5;
		case IOL_DL_MDH_STATE_SM_WURQ_5:
			/* Create wakeup current pulse:
			 * 1. Invoke service PL-Wake-Up (see Figure 12 and 5.3.3.3)
			 * 2. Start and wait T_DMT (see Table 42).
			 * */
			//1

			DBG_SPRINT_APPEND(pData,"WRQ>");
			if(IOL_Timer_IsTimeout(&dl->tdmt) == IOL_TIMER_TIMEOUT){
				/* T15: state 5 to 6:
				 * Set transmission rate of COM3 mode.
				 */
				DBG_SPRINT_APPEND(pData,"timElpsd:%u>",(uint32_t)IOL_Timer_GetTime());

				IOL_Timer_Stop(&dl->tdmt);
				/*Set the baudrate form com3*/
				IOL_PL_SetMode_req(port, IOL_PL_MODE_COM3);
				dl->mdh.isEstdComx = 0;
				dl->mdh.state = IOL_DL_MDH_STATE_SM_COMREQCOM3_6;
//				goto DL_MDH_STATE_SM_COMREQCOM3_6;
			}else if(IOL_Timer_IsRunning(&dl->tdmt) == IOL_TIMER_NOT_RUNNING){
				/*Start Tsd timer (not clear description in spec)*/
				IOL_Timer_SetTimeout(&dl->tsd, IOL_TSD);
				IOL_Timer_Start(&dl->tsd);
				/*Start Tdwu timer  (not clear description in spec)*/
				IOL_Timer_SetTimeout(&dl->tdwu, IOL_TDWU);
				IOL_Timer_Start(&dl->tdwu);
				/*Start Tdmt timer*/
				IOL_Timer_SetTimeout(&dl->tdmt, 300);//(IOL_TREN + IOL_TDMT_COM3));
				IOL_Timer_Start(&dl->tdmt);
//				DBG_ENABLE();
				DBG_SPRINT_APPEND(pData,"PLRQ:%u>",(uint32_t)IOL_Timer_GetTime());
				IOL_PL_WakeUp_req(port);
				 //Calculate 32 bits time according to baudrate and set to timeout value

			}
//			DBG_PRINT(pData);
			break;

		case IOL_DL_MDH_STATE_SM_COMREQCOM3_6:
			/* Try test message with transmission rate of COM3 via the message handler: Call
			 * MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
			 * */
//			DL_MDH_STATE_SM_COMREQCOM3_6:
			DBG_SPRINT_APPEND(pData,"RQCOM3>");
			if(dl->mdh.isEstdComx){
				/* T2: state 1 to 2:
				 * Transmission rate of COM3 successful. Message handler activated and
				 * configured to COM3 (see Figure 40, Transition T2). Activate command
				 * handler (call CH_Conf_ACTIVE in Figure 53). Return DL_Mode.ind
				 * (STARTUP) and DL_Mode.ind (COM3) to SM
				 */
				DBG_SPRINT_APPEND(pData,"Estd>");
//				DBG_PRINT(pData);
				dl->mdh.isEstdComx = 0;
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_ACTIVE);

				/* TODO: need to understand. can  call 2 "IOL_DL_Mode_Ind" function at a time */
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_STARTUP);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COM3);
				dl->mdh.state = IOL_DL_MDH_STATE_STARTUP_2;
			}else{

				if(IOL_Timer_IsTimeout(&dl->tdmt) == IOL_TIMER_TIMEOUT){
					/* T16: state 6 to 7:
					 * Set transmission rate of COM3 mode.
					 */
					DBG_SPRINT_APPEND(pData,"TdmtStp:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);
					IOL_Timer_Stop(&dl->tdmt);
					//TODO: to set baud rate to the assigned MCU UART Port. and also set the isBaudrateSet flag
					/*Set the baudrate form com2*/
					IOL_PL_SetMode_req(port, IOL_PL_MODE_COM2);
					dl->mdh.state = IOL_DL_MDH_STATE_SM_COMREQCOM2_7;
//					goto DL_MDH_STATE_SM_COMREQCOM2_7;
				}else if(IOL_Timer_IsRunning(&dl->tdmt) == IOL_TIMER_NOT_RUNNING){
//					DBG_ENABLE();
					DBG_SPRINT_APPEND(pData,"MHCal_Tdmt:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);


					/*Set & Start Tdmt for com3*/
					IOL_Timer_SetTimeout(&dl->tdmt, IOL_TDMT_COM3);
					IOL_Timer_Start(&dl->tdmt);
					/*Try test message with transmission rate of COM3*/
					IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_COM3);
				}
			}
			break;

		case IOL_DL_MDH_STATE_SM_COMREQCOM2_7:
			/* Try test message with transmission rate of COM2 via the message handler: Call
			 * MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
			 * */
//			DL_MDH_STATE_SM_COMREQCOM2_7:
			DBG_SPRINT_APPEND(pData,"RQCOM2>");
			if(dl->mdh.isEstdComx){			// baud rate set successful
				/* T3: state 1 to 2:
				 * Transmission rate of COM2 successful. Message handler activated and
				 * configured to COM2 (see Figure 40, Transition T2). Activate command
				 * handler (call CH_Conf_ACTIVE in Figure 53). Return DL_Mode.ind
				 * (STARTUP) and DL_Mode.ind (COM1) to SM
				 */
				DBG_SPRINT_APPEND(pData,"Estd>");
//				DBG_PRINT(pData);
				dl->mdh.isEstdComx = 0;
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_ACTIVE);
				/*
				 * TODO: need to understand. can  call 2 "IOL_DL_Mode_Ind" function at a time
				 * */
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_STARTUP);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COM2);
				dl->mdh.state = IOL_DL_MDH_STATE_STARTUP_2;
			}else{
				if(IOL_Timer_IsTimeout(&dl->tdmt) == IOL_TIMER_TIMEOUT){
					/* T17: state 7 to 8:
					 * Set transmission rate of COM3 mode.
					 */
					DBG_SPRINT_APPEND(pData,"TdmtStp:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);
					IOL_Timer_Stop(&dl->tdmt);
					//TODO: to set baud rate to the assigned MCU UART Port
					/*Set the baudrate form com1*/
					IOL_PL_SetMode_req(port, IOL_PL_MODE_COM1);
					dl->mdh.state = IOL_DL_MDH_STATE_SM_COMREQCOM1_8;
//					goto DL_MDH_STATE_SM_COMREQCOM1_8;
				}else if(IOL_Timer_IsRunning(&dl->tdmt) == IOL_TIMER_NOT_RUNNING){
					/*Try test message with transmission rate of COM2*/
//					DBG_ENABLE();
					DBG_SPRINT_APPEND(pData,"2MHCal_Tdmt:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);
					IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_COM2);
					/*Set & Start Tdmt for com2*/
					IOL_Timer_SetTimeout(&dl->tdmt, IOL_TDMT_COM2);
					IOL_Timer_Start(&dl->tdmt);
				}
			}
			break;
		case IOL_DL_MDH_STATE_SM_COMREQCOM1_8:
			/* Try test message with transmission rate of COM1 via the message handler: Call
			 * MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
			 * */
//			DL_MDH_STATE_SM_COMREQCOM1_8:
			DBG_SPRINT_APPEND(pData,"RQCOM1>");
			if(dl->mdh.isEstdComx){			// baud rate set successful
				/* T4: state 1 to 2:
				 * Transmission rate of COM1 successful. Message handler activated and
				 * configured to COM1 (see Figure 40, Transition T2). Activate command
				 * handler (call CH_Conf_ACTIVE in Figure 53). Return DL_Mode.ind
				 * (STARTUP) and DL_Mode.ind (COM1) to SM
				 */
				DBG_SPRINT_APPEND(pData,"Estd>");
//				DBG_PRINT(pData);
				dl->mdh.isEstdComx = 0;
				IOL_DL_CH_Conf(port, dl, IOL_DL_CH_CMD_ACTIVE);
				/*
				 * TODO: need to understand. can  call 2 "IOL_DL_Mode_Ind" function at a time
				 * */
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_STARTUP);
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_COM1);
				dl->mdh.state = IOL_DL_MDH_STATE_STARTUP_2;
			}else{

				if(IOL_Timer_IsTimeout(&dl->tdmt) == IOL_TIMER_TIMEOUT){
					/* T18: state 8 to 9:
					 * Increment Retry
					 */
					DBG_SPRINT_APPEND(pData,"TdmtStp:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);
					IOL_Timer_Stop(&dl->tdmt);
					dl->mdh.retryEstCom++;
					dl->mdh.state = IOL_DL_MDH_STATE_SM_RETRY_9;

				}else if(IOL_Timer_IsRunning(&dl->tdmt) == IOL_TIMER_NOT_RUNNING){
					/*Try test message with transmission rate of COM1*/
//					DBG_ENABLE();
					DBG_SPRINT_APPEND(pData,"1MHCal_Tdmt:%u>",(uint32_t)IOL_Timer_GetTime());
//					DBG_PRINT(pData);
					IOL_DL_MH_Conf(port, dl, IOL_DL_MH_CMD_COM1);

					/*Set & Start Tdmt for com1*/
					IOL_Timer_SetTimeout(&dl->tdmt, IOL_TDMT_COM1);
					IOL_Timer_Start(&dl->tdmt);
				}
			}
			break;
		case IOL_DL_MDH_STATE_SM_RETRY_9:
			/* Check number of Retries*/
			DBG_SPRINT_APPEND(pData,"rtry>");
			if(dl->mdh.retryEstCom < IOL_MAX_DL_ESTCOM_RETRY){
				/* T19: state 9 to 5 */
				DBG_SPRINT_APPEND(pData,"notFinsd:%u>",(uint32_t)IOL_Timer_GetTime());
				if(IOL_Timer_IsTimeout(&dl->tdwu) == IOL_TIMER_TIMEOUT){
//					DBG_ENABLE();
					DBG_SPRINT_APPEND(pData,"tdwuout>");
					IOL_Timer_Stop(&dl->tdwu);
					dl->mdh.state = IOL_DL_MDH_STATE_SM_WURQ_5;
				}
			}else{
				/* T5: state 9 to 0
				 * Return DL_Mode.ind (INACTIVE) to SM.
				 * */
//				DBG_ENABLE();
				DBG_SPRINT_APPEND(pData,"Finshd>");
				dl->mdh.state = IOL_DL_MDH_STATE_IDLE_0;
				IOL_DL_Mode_Ind(port, IOL_MDH_MODE_INACTIVE);
			}
			DBG_SPRINT_APPEND(pData,"\r\n");
//			DBG_PRINT(pData);
			break;
		default:
			break;
	}
	DBG_PRINT(pData);
}



/*State machine of the  Master message handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.3, Figure: 40, Table: 46
 * */
void IOL_DL_MessageHandler(IOL_Port_t *port){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
//	IOL_PL_t *pl = (IOL_PL_t*)IOL_GetPLInstance(port);
//	IOL_Timer_t *timer = (IOL_Timer_t*)IOL_GetTimerInstance(port);
	char pData[100] = {0};
//	DBG_DISABLE();
	DBG_SPRINT_APPEND(pData,"\r\nMH>");
	switch (dl->msgh.state) {

		case IOL_DL_MH_STATE_INACTIVE_0:
			/* Waiting on demand for a "test" message via MH_Conf_COMx call (see Figure 36 and
			 * Table 44) from DL-mode handler.
			 * */
//			DBG_SPRINT_APPEND(pData,"inact>");
			if(		dl->msgh.mhCmd == IOL_DL_MH_CMD_COM1 ||
					dl->msgh.mhCmd == IOL_DL_MH_CMD_COM2 ||
					dl->msgh.mhCmd == IOL_DL_MH_CMD_COM3){
				/* T1: state 0 to 1:
				 * Send a message with the requested transmission rate of COMx and with
				 * M-sequence TYPE_0: Read Direct Parameter page 1, address 0x02
				 * ("MinCycleTime"), compiling into an M-sequence control MC = 0xA2 (see
				 * A.1.2). Start timer with TM-sequence
				 * */

				//1. Send a test message with M-sequence TYPE_0
				dl->msgh.mc.field.rw = IOL_RW_DIR_READ;
				dl->msgh.mc.field.comCh = IOL_COMM_CH_PAGE;
				dl->msgh.mc.field.address = IOL_DIR_PARM_ADDR_MIN_CYCL_TIM;
				dl->msgh.ckt.field.mseqTyp = IOL_MSEQ_TYPE_0;
				dl->msgh.ckt.field.checksum = 0;

				switch (dl->msgh.mhCmd) {
					case IOL_DL_MH_CMD_COM1: dl->baudRate = IOL_BAUDRATE_COM1; break;
					case IOL_DL_MH_CMD_COM2: dl->baudRate = IOL_BAUDRATE_COM2; break;
					case IOL_DL_MH_CMD_COM3: dl->baudRate = IOL_BAUDRATE_COM3; break;
					default: break;
				}

				IOL_DL_BuildMseqType0_Req(dl, IOL_RW_DIR_READ, dl->msgh.mc.octet, dl->msgh.ckt.octet, 0);	// set data value 0, no data filed available for read
//				DBG_ENABLE();


				//2. Start timer with T_M-sequence
				uint8_t rxBuffSize = 2; // for this test message it is constant
				uint32_t timeout = IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate); //IOL_DL_CalcTmseq(dl->txBuffLen, rxBuffSize, dl->baudRate)
				DBG_SPRINT_APPEND(pData,"Com%d-%02X%02X, Tmsq:%d>",dl->msgh.mhCmd, dl->txBuff[0], dl->txBuff[1], timeout);
//				DBG_SPRINT(pData);
				//				DBG_SPRINT_APPEND(pData,"len:tx-%d,br-%d tmsq: %d>", dl->txBuffLen, dl->baudRate, timeout);
				IOL_Timer_SetTimeout(&dl->msgh.tmseq, timeout);		// set the m-sequence duration
				IOL_Timer_Start(&dl->msgh.tmseq);		//start the timer
				/*Tx data*/
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);
				//goto next state
				dl->msgh.mhCmd = IOL_DL_MH_CMD_INACTIVE;
				dl->msgh.state = IOL_DL_MH_STATE_AWAITREPLY_1;
			}
			break;
		case IOL_DL_MH_STATE_AWAITREPLY_1:
			/* Waiting on response from the Device to the "test" message.
			 * 1. Return to Inactive_0 state whenever the time TM-sequence elapsed
			 * 	  without response from the Device
			 * 2. or the response to the "test" message could not be decoded.
			 * 3. In case of a correct response from the Device, the message
			 *    handler changes to the Startup_2 state.
			 * */
			DBG_SPRINT_APPEND(pData,"AWRply>");
			if(IOL_Timer_IsTimeout(&dl->msgh.tmseq) == IOL_TIMER_TIMEOUT ||
					dl->msgh.mhInfo != IOL_DL_MHINFO_NONE){
				/* T3 & T4: state 1 to 0:
				 * Reset timer (TM-sequence)
				 */
//				DBG_ENABLE();
				DBG_SPRINT_APPEND(pData,"tmsqOut>");
				IOL_Timer_Stop(&dl->msgh.tmseq);

				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;
			}else if(dl->isDataRecved > 0U){
				/* T2: state 1 to 2:
				 * 1. Return value of "MinCycleTime" via DL_Read service confirmation.
				 * 2. goto startup 2 state
				 */
				DBG_SPRINT_APPEND(pData,"rspd>");
				IOL_DL_Read_Cnf(port, *dl->msgh.rxData, IOL_ERROR_NONE);
				dl->msgh.state = IOL_DL_MH_STATE_STARTUP_2;
			}
//			DBG_SPRINT(pData);
			break;
		case IOL_DL_MH_STATE_STARTUP_2:
			/* When entered via transition T2, this state is responsible to control acyclic On-request
			 * Data exchange according to conditions specified in Table A.7. Any service DL_Write or
			 * DL_Read from System Management causes a transition.
			 * */
			if(		dl->msgh.rwDir == IOL_RW_DIR_READ ||
					dl->msgh.rwDir == IOL_RW_DIR_WRITE ){
				/* T5 & T6: state 2 to 3:
				 * 1. Send message using the established transmission rate, the page
				 * communication channel, and the read access option (see A.1.2).
				 * 2. Start timer with T_M-sequence
				 * 3. goto state 3
				 * */

				/* 1. Send message using the established transmission rate*/
				//build m-sequence type 0 request msg
				IOL_DL_BuildMseqType0_Req(dl, dl->msgh.rwDir, dl->msgh.mc.octet, dl->msgh.ckt.octet, *dl->msgh.txData);
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);

				/* 2. Start timer with T_M-sequence*/
				/*TODO: implement M-seq timer*/
//				IOL_Timer_Start(&timer->tmseq);

				/*3. goto state 3*/
				dl->msgh.state = IOL_DL_MH_STATE_RESPONSE_3;

			}else if(dl->msgh.mhCmd == IOL_DL_MH_CMD_PREOPERATE){
				/* T12: state 2 to 6 */
				dl->msgh.state = IOL_DL_MH_STATE_PREOPERATE_6;
			}else if(dl->msgh.mhCmd == IOL_DL_MH_CMD_OPERATE){
				/* T39: state 2 to 12 */
				/*Strat the tinicyc*/
				IOL_Timer_SetTimeout(&dl->msgh.tcyc, IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate));
				IOL_Timer_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				dl->msgh.state = IOL_DL_MH_STATE_OPERATE_12;
			}
			break;
		case IOL_DL_MH_STATE_RESPONSE_3:
			// goto the sub machine
			dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_4;
		case IOL_DL_MH_STATE_SM_AWAITREPLY_4:
			/* This state checks whether the time TM-sequence elapsed and the response is correct.
			 * */
			if(dl->msgh.tmseq.isTimeout == IOL_TIMER_TIMEOUT ||
					dl->msgh.mhInfo != IOL_DL_MHINFO_NONE){
				/* T7 & T8: state 1 to 0:
				 * Reset timer (TM-sequence)
				 */
				/*Clear the guard condition*/
				dl->msgh.tmseq.isTimeout = IOL_TIMER_TIMEOUT_NONE;
				dl->msgh.mhInfo = IOL_DL_MHINFO_NONE;

				/* reset(stop) the t_mseq */
				IOL_Timer_Stop(&dl->msgh.tmseq);

				/*and start t_initcyc with the idle time
				 * Not specified in the IOLink spec, but it is needed
				 * */
				int32_t tidle = (IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate)
								- dl->msgh.tmseq.timeout);

				IOL_Timer_SetTimeout(&dl->msgh.tcyc, tidle > 0 ? (uint32_t)tidle :
				IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate));// Guard condition for the negative value
				IOL_Timer_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				dl->msgh.retry = 0;
				/*goto the stat 5*/
				dl->msgh.state = IOL_DL_MH_STATE_SM_ERRHANDL_5;

			}else if(dl->isDataRecved > 0U && dl->msgh.mhInfo == IOL_DL_MHINFO_NONE){
				/* T10: state 3 to 2:
				 * 1. Return DL_Read service confirmation respectively to System Management.
				 * 2. Return DL_Read service confirmation respectively to System Management.
				 * 3. goto the state 2
				 * */
				// 1 & 2
				dl->isDataRecved = 0;
				dl->msgh.rwDir == IOL_RW_DIR_READ ?
						IOL_DL_Read_Cnf(port, *dl->msgh.rxData, IOL_ERROR_NONE) :
						IOL_DL_Write_Cnf(port, IOL_ERROR_NONE);
				// 3
				dl->msgh.state = IOL_DL_MH_STATE_STARTUP_2;

			}
			break;
		case IOL_DL_MH_STATE_SM_ERRHANDL_5:
			/* 1. In case of an incorrect response the message handler will re-send the message after a
			 * waiting time Tinitcyc.
			 * 2. After too many retries the message handler will change to the Inactive_0 state.
			 * */
			if(dl->msgh.retry >= IOL_MAX_DL_MH_RETRY){
				/* T11: state 5 to 0:
				 * 1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				 * 2. goto the state 0
				 * */
				IOL_DL_MHInfo_Ind(port,  IOL_DL_MHINFO_COMLOST);		//1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;		//2. goto the state 0
			}else if(dl->msgh.tcyc.isTimeout == IOL_TIMER_TIMEOUT){
				/* T9: state 5 to 4:
				 * 1. Resend the last message
				 * 2. Restart timer with T_msequence
				 * 3. goto the state 0
				 * */
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);		//1. Resend the last message
				IOL_Timer_Start(&dl->msgh.tmseq);								//2. Restart timer with T_msequence
				dl->msgh.retry++;
				dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_4;			//3. goto the state 0
			}
			break;
		case IOL_DL_MH_STATE_PREOPERATE_6:
			/* Upon reception of a call MH_Conf_PREOPERATE the message handler changed to this
			 * state. The message handler is now responsible to control acyclic On-request Data
			 * exchange according to conditions specified in Table A.8. Any service DL_ReadParam,
			 * DL_WriteParam, DL_ISDUTransport, DL_Write, or EventFlag causes a transition.
			 * */
			if(dl->msgh.mhCmd == IOL_DL_MH_CMD_INACTIVE){
				/* T36: state 6 to 0:
				 * Message handler changes to state Inactive_0 and returns MH_Info
				 * (COMLOST) to the DL-mode handler
				 */
				dl->msgh.mhInfo = IOL_DL_MHINFO_COMLOST;
				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;
			}else if(dl->msgh.mhCmd == IOL_DL_MH_CMD_STARTUP){
				/* T37: state 6 to 0*/
				dl->msgh.state = IOL_DL_MH_STATE_STARTUP_2;
			}else if(dl->msgh.mhCmd == IOL_DL_MH_CMD_OPERATE){
				/* T26: state 6 to 0:
				 * Message handler changes to state Operate_12.
				 * */
				/*Strat the tinicyc*/
				IOL_Timer_SetTimeout(&dl->msgh.tcyc, IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate));
				IOL_Timer_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				dl->msgh.state = IOL_DL_MH_STATE_OPERATE_12;
			}else if(dl->msgh.rwCmd == IOL_DL_MH_RWCMD_READPARAM ||			// T13,
					 dl->msgh.rwCmd == IOL_DL_MH_RWCMD_WRITEPARAM ||		//T14
					 dl->msgh.rwCmd == IOL_DL_MH_RWCMD_ISDUTRANS ||			//T15
					 dl->msgh.rwCmd == IOL_DL_MH_RWCMD_WRITE ||				//T17
//					 dl->mdh.mode == IOL_DL_MDH_MOD_PREOPERATE ||			//T17
					 dl->evnth.eventFlag == IOL_EH_EVENT					//T16
					 ){
				/* T13, T14, T15, T16 & T17: state 6 to 7:
				 * The Message handler invokes the ODTrig service for the On-request
				 * handler (see Figure 48), which is in state "ISDU_1". In this state it causes
				 * the ISDU handler to provide the OD service in correspondence to the
				 * service (see Figure 51, Transition T13).
				 * */
				/*Strat the tinicyc*/
				IOL_Timer_SetTimeout(&dl->msgh.tcyc, IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate));
				IOL_Timer_Start(&dl->msgh.tcyc);				// t_cyc is using as a T_initcyc timer
				dl->odh.trigger = IOL_DL_TRIG_MSTR_MSG;
				IOL_DL_ODTrig_Ind(port, IOL_MAX_DL_MSG_SIZE);
				dl->msgh.state = IOL_DL_MH_STATE_GETOD_7;
			}
			break;
		case IOL_DL_MH_STATE_GETOD_7:
			/* The message handler used the ODTrig service to aquire OD from the On-request Data
			 * handler. The message handler waits on the OD service to send a message after a time
			 * Tinitcyc.
			 * */
			if(dl->msgh.tcyc.isTimeout == IOL_TIMER_TIMEOUT){
				/* T18: state 7 to 8
				 * 1. Send message after a recovery time Tinitcyc caused by the OD.req service.
				 * 2. Start timer with T_M-sequence
				 * 3. goto state 8
				 * */
				//TODO: to build message. in DL_read, DL_write etc function
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);		//1
				IOL_Timer_Start(&dl->msgh.tmseq);								//2
				dl->msgh.state = IOL_DL_MH_STATE_RESPONSE_8;
			}
			break;
		case IOL_DL_MH_STATE_RESPONSE_8:
			// goto the sub machine
			dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_9;
		case IOL_DL_MH_STATE_SM_AWAITREPLY_9:
			/* This state checks whether the time TM-sequence elapsed and the response is correct.
			 * */
			if(dl->msgh.tmseq.isTimeout == IOL_TIMER_TIMEOUT ||
					dl->msgh.mhInfo != IOL_DL_MHINFO_NONE){
				/* T19 & T20: state 9 to 10:
				 * Reset timer (TM-sequence)
				 */
				/*Clear the guard condition*/
				dl->msgh.tmseq.isTimeout = IOL_TIMER_TIMEOUT_NONE;
				dl->msgh.mhInfo = IOL_DL_MHINFO_NONE;

				/* reset(stop) the t_mseq */
				IOL_Timer_Stop(&dl->msgh.tmseq);

				/*and start t_initcyc with the idle time
				 * Not specified in the IOLink spec, but it is needed
				 * */
				int32_t tidle = (IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate)
								- dl->msgh.tmseq.timeout);

				IOL_Timer_SetTimeout(&dl->msgh.tcyc, tidle > 0 ? (uint32_t)tidle :
						IOL_DL_GetMinRecovTime(IOL_MSEQCODE_TYPE_0, dl->baudRate));// Guard condition for the negative value
				IOL_Timer_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				dl->msgh.retry = 0;

				/*goto the stat 5*/
				dl->msgh.state = IOL_DL_MH_STATE_SM_ERRHANDL_10;
			}else if(dl->isDataRecved > 0U && dl->msgh.mhInfo == IOL_DL_MHINFO_NONE){
				/* T23: state 8 to 1 */
				dl->isDataRecved = 0;
				dl->msgh.state = IOL_DL_MH_STATE_CHECKHANDL_11;			// 2
			}
			break;
		case IOL_DL_MH_STATE_SM_ERRHANDL_10:
			/* 1. In case of an incorrect response the message handler will re-send the message after a
			 * waiting time Tinitcyc.
			 * 2. After too many retries the message handler will change to the Inactive_0 state.
			 * */
			if(dl->msgh.retry >= IOL_MAX_DL_MH_RETRY){
				/* T22: state 8 to 0:
				 * 1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				 * 2. goto the state 0
				 * */
				IOL_DL_MHInfo_Ind(port,  IOL_DL_MHINFO_COMLOST);		//1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;		//2. goto the state 0
			}else if(dl->msgh.tcyc.isTimeout == IOL_TIMER_TIMEOUT){
				/* T21: state 10 to 9:
				 * 1. Resend the last message
				 * 2. Restart timer with T_msequence
				 * 3. goto the state 0
				 * */
				//TODO: to build message. in DL_read, DL_write etc function
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);		//1. Resend the last message
				IOL_Timer_Start(&dl->msgh.tmseq);								//2. Restart timer with T_msequence
				dl->msgh.retry++;
				dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_9;			//3. goto the state 0
			}
			break;
		case IOL_DL_MH_STATE_CHECKHANDL_11:
			/* Some services require several OD acquisition cycles to exchange the OD. Whenever
			 * the affected OD, ISDU, or Event handler returned to the idle state, the message
			 * handler can leave the OD acquisition loop.
			 * */
			if(		dl->isduh.state != IOL_DL_ISDUH_STATE_IDLE_1 ||
					dl->evnth.state != IOL_DL_EH_STATE_IDLE_1)
			{
				/* T24: state 11 to 7:
				 * Acquire OD through invocation of the ODTrig service to the On-request
				 * Data handler, which in turn triggers the current handler in charge via the
				 * ISDU or EventTrig call
				 * */
				dl->odh.trigger = IOL_DL_TRIG_MSTR_MSG;
				IOL_Timer_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				IOL_DL_ODTrig_Ind(port, IOL_MAX_DL_MSG_SIZE);

				dl->msgh.state = IOL_DL_MH_STATE_GETOD_7;
			}else{
				/* T25: state 11 to 6:
				 * Return result via service primitive OD.cnf
				 * */
				IOL_DL_OD_Cnf(port, dl->msgh.rxData, dl->rxBuffLen, IOL_ERROR_NONE);	//// 1
				dl->msgh.state = IOL_DL_MH_STATE_PREOPERATE_6;
			}
			break;
		case IOL_DL_MH_STATE_OPERATE_12:
			/* Upon reception of a call MH_Conf_OPERATE the message handler changed to this
			 * state and after an initial time Tinitcyc, it is responsible to control cyclic Process Data
			 * and On-request Data exchange according to conditions specified in Table A.9 and
			 * Table A.10. The message handler restarts on its own a new message cycle after the
			 * time tCYC elapsed.
			 * */
			if(dl->msgh.mhCmd == IOL_DL_MH_CMD_INACTIVE){
				/* T35: state 12 to 0:
				 * Message handler changes to state Inactive_0 and returns MH_Info
				 * (COMLOST) to the DL-mode handler.
				 */
				IOL_DL_MHInfo_Ind(port,  IOL_DL_MHINFO_COMLOST);
				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;
			}else if(dl->msgh.mhCmd == IOL_DL_MH_CMD_STARTUP){
				/* T38: state 12 to 2 */
				dl->msgh.state = IOL_DL_MH_STATE_STARTUP_2;
			}else if(dl->msgh.tcyc.isTimeout == IOL_TIMER_TIMEOUT){
				/* T27: state 12 to 13
				 * Start the tCYC -timer. Acquire PD through invocation of the PDTrig service
				 * to the Process Data handler (see Figure 46).
				 * */
				dl->msgh.tmseq.isTimeout = IOL_TIMER_TIMEOUT_NONE;
//				TIMER_SetTimeout(&dl->msgh.tcyc, dl->msgh.minCycTime);// Guard condition for the negative value
//				TIMER_Start(&dl->msgh.tcyc);		// t_cyc is using as a T_initcyc timer
				dl->pdh.trigger = IOL_DL_TRIG_MSTR_MSG;

				dl->msgh.state = IOL_DL_MH_STATE_GETPD_13;
			}
			break;
		case IOL_DL_MH_STATE_GETPD_13:
			/* The message handler used the PDTrig service to aquire PD from the Process Data
			 * handler. The message handler waits on the PD service and then changes to state
			 * GetOD_14.
			 * */
			//Invoke pd trigger to get the PD data
			IOL_DL_PDTrig_Ind(port, IOL_MAX_DL_MSG_SIZE); //TODO: need to understand about the value of datalen
			/* T28: state 13 to 14:
			 * Acquire OD through invocation of the ODTrig service to the On-request
			 * Data handler (see Figure 48).
			 * */
			dl->odh.trigger = IOL_DL_TRIG_MSTR_MSG;
			dl->msgh.state = IOL_DL_MH_STATE_GETOD_14;
			break;
		case IOL_DL_MH_STATE_GETOD_14:
			/* The message handler used the ODTrig service to aquire OD from the On-request Data
			 * handler. The message handler waits on the OD service to complement the already
			 * acquired PD and to send a message with the acquired PD/OD.
			 * */
			IOL_DL_ODTrig_Ind(port, IOL_MAX_DL_MSG_SIZE);	//TODO: need to understand about the value of datalen
			/* T30 & T31: state 16 to 17:
			 * PD and OD ready through PD.req service from PD handler and OD.req
			 * service via the OD handler. Message handler sends message. Start timer
			 * with T_M-sequence.
			 */
			//TODO: incomplete
			IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);
			IOL_Timer_SetTimeout(&dl->msgh.tmseq, IOL_DL_CalcTmseq(dl->msgh.txDataLen, dl->msgh.rxDataLen, dl->baudRate));
			IOL_Timer_Start(&dl->msgh.tmseq);
			dl->msgh.state = IOL_DL_MH_STATE_RESPONSE_15;
			break;
		case IOL_DL_MH_STATE_RESPONSE_15:
			/* The message handler sent a message with the acquired PD/OD. The submachine in
			 * this pseudo state waits on the response and checks its correctness.
			 * */
			// goto the sub machine
			dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_16;
		case IOL_DL_MH_STATE_SM_AWAITREPLY_16:
			/* This state checks whether the time TM-sequence elapsed and the response is correct.
			 * */
			if(dl->msgh.tmseq.isTimeout == IOL_TIMER_TIMEOUT ||
					dl->msgh.mhInfo != IOL_DL_MHINFO_NONE){
				/* T30 & T31: state 16 to 17:
				 * Reset timer (TM-sequence)
				 */
				/*Clear the guard condition*/
				dl->msgh.tmseq.isTimeout = IOL_TIMER_TIMEOUT_NONE;
				dl->msgh.mhInfo = IOL_DL_MHINFO_NONE;

				/* reset(stop) the t_mseq */
				IOL_Timer_Stop(&dl->msgh.tmseq);

				/*and start t_initcyc with the idle time
				 * Not specified in the IOLink spec, but it is needed
				 * */

				IOL_Timer_SetTimeout(&dl->msgh.tcyc, dl->msgh.minCycTime);// Guard condition for the negative value
				IOL_Timer_Start(&dl->msgh.tcyc);
				dl->msgh.retry = 0;

				/*goto the stat 5*/
				dl->msgh.state = IOL_DL_MH_STATE_SM_ERRHANDL_10;
			}else if(dl->isDataRecved > 0U && dl->msgh.mhInfo == IOL_DL_MHINFO_NONE){
				/* T34: state 15 to 12
				 * Device response message is correct. Return PD via service PD.cnf and via
				 * call PDTrig to the PD handler (see Table 48). Return OD via service
				 * OD.cnf and via call ODTrig to the On-request Data hander, which redirects
				 * it to the ISDU (see Table 53), Command (see Table 56), or Event handler
				 * (see Table 59) in charge.
				 * */
				//TODO: incomplete
				dl->pdh.trigger = IOL_DL_TRIG_DEV_MSG;
				IOL_DL_PD_Cnf(port, dl->rxBuff, IOL_ERROR_NONE);
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				dl->odh.trigger = IOL_DL_TRIG_DEV_MSG;
				//TODO: what should be the OD len, need to understand
				IOL_DL_OD_Cnf(port, dl->rxBuff, dl->rxBuffLen, IOL_ERROR_NONE);
				dl->odh.trigger = IOL_DL_TRIG_NONE;
				dl->msgh.state = IOL_DL_MH_STATE_OPERATE_12;
			}
			break;
		case IOL_DL_MH_STATE_SM_ERRHANDL_17:
			/* In case of an incorrect response the message handler will re-send the message after a
			 * waiting time tCYC. After too many retries the message handler will change to the
			 * Inactive_0 state.
			 * */
			if(dl->msgh.retry >= IOL_MAX_DL_MH_RETRY){
				/* T33: state 15 to 0:
				 * 1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				 * 2. goto the state 0
				 * */
				IOL_DL_MHInfo_Ind(port,  IOL_DL_MHINFO_COMLOST);		//1. Message handler returns MH_Info (COMLOST) to DL-mode handler.
				dl->msgh.state = IOL_DL_MH_STATE_INACTIVE_0;		//2. goto the state 0
			}else if(dl->msgh.tcyc.isTimeout == IOL_TIMER_TIMEOUT){
				/* T32: state 10 to 9:
				 * 1. Resend the last message
				 * 2. Restart timer with T_msequence
				 * 3. goto the state 0
				 * */
				//TODO: to build message. in DL_read, DL_write etc function
				IOL_PL_Transfer_req(port, dl->txBuff, dl->txBuffLen);		//1. Resend the last message
				IOL_Timer_Start(&dl->msgh.tmseq);								//2. Restart timer with T_msequence
				dl->msgh.retry++;
				dl->msgh.state = IOL_DL_MH_STATE_SM_AWAITREPLY_16;			//3. goto the state 0
			}
			break;
		default:
			break;
	}
	DBG_PRINT(pData);
}



/*State machine of the  Master Process Data handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.4.3, Figure: 46, Table: 48
 * */
void IOL_DL_PDHandler(IOL_Port_t *port){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);
	switch (dl->pdh.state) {
		case IOL_DL_PDH_STATE_INACTIVE_0:
			/*Waiting for activation*/
			if(dl->pdh.trigger != IOL_DL_TRIG_NONE){
				/* T1: state 0 to 0:
				 * Invoke PD.req with no Process Data
				 */
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				IOL_DL_PD_Req(port, 0, 0, 0, 0, 0);
			}else if(dl->pdh.pdhCmd == IOL_DL_PDH_CMD_SINGLE){
				/*T2: state 0 to 1 */
				dl->pdh.state = IOL_DL_PDH_STATE_PDSINGLE_1;
			}else if(dl->pdh.pdhCmd == IOL_DL_PDH_CMD_INTERLEAVE){
				/*T4: state 0 to 2*/
				dl->pdh.state = IOL_DL_PDH_STATE_PDININTERLEAVE_2;
			}else{
				//TODO: IOL>DL>PD: write code here
			}
			break;
		case IOL_DL_PDH_STATE_PDSINGLE_1:
			/*Process Data communication within one single M-sequence*/
			if(dl->pdh.pdhCmd == IOL_DL_PDH_CMD_INACTIVE){
				/*T9: state 1 to 0*/
				dl->pdh.state = IOL_DL_PDH_STATE_INACTIVE_0;
			}else if(dl->pdh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/*T3: state 1 to 1
				 * 1. take data from DL_PDOutputUpdate service and invoke PD.req
				 *  to propagate output PD to the message handler
				 * 2. Take data from PD.cnf and invoke DL_PDInputTransport.ind and
				 * DL_PDCycle.ind to propagate input PD to the AL.
				 * */
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				//1
				IOL_DL_PD_Req(port, dl->pdh.pdInAddr, dl->pdh.pdInLen, dl->pdh.pdOutData, dl->pdh.pdOutAddr, dl->pdh.pdOutLen);

			}else if(dl->pdh.trigger == IOL_DL_TRIG_DEV_MSG){			//T3: state 1 to 1
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				/* the DL_PDCycle service is called to indicate the end of a Process Data cycle
				 * to the application layer.
				 * */

				//4. invoke DL_PDInputTransport.ind and
				IOL_DL_PDInputTransport_Ind(port, dl->pdh.pdInData, dl->pdh.pdInLen);
				IOL_DL_PDCycle_Ind(port);

			}
			break;
		case IOL_DL_PDH_STATE_PDININTERLEAVE_2:
			/*Input Process Data communication in interleave mode*/
			if(dl->pdh.pdhCmd == IOL_DL_PDH_CMD_INACTIVE){
				/*T10: state 2 to 0*/
				dl->pdh.state = IOL_DL_PDH_STATE_INACTIVE_0;
			}else if(dl->pdh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T5: state 2 to 2:
				 * Invoke PD.req and use PD.cnf to prepare DL_PDInputTransport.ind
				 */

				 dl->pdh.trigger = IOL_DL_TRIG_NONE;
				 IOL_DL_PD_Req(port, dl->pdh.pdInAddr, dl->pdh.pdInLen, dl->pdh.pdOutData, dl->pdh.pdOutAddr, dl->pdh.pdOutLen);


			}else if(dl->pdh.trigger == IOL_DL_TRIG_DEV_MSG){			//T5: state 2 to 2
				//TODO: to understand this implementation
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				 IOL_DL_PD_Cnf(port, dl->pdh.pdInData, IOL_ERROR_NONE);
			}else if(dl->pdh.trigger == IOL_DL_TRIG_DEV_MSG && dl->pdh.pdInLen > 0){		// if PD input data is available
				/* T6: state 3 to 2:
				 * Invoke DL_PDInputTransport.ind and DL_PDCycle.ind to propagate input
				 * PD to the AL
				 * */

				IOL_DL_PDInputTransport_Ind(port, dl->pdh.pdInData, dl->pdh.pdInLen);
				IOL_DL_PDCycle_Ind(port);
			}
			break;
		case IOL_DL_PDH_STATE_PDOUTINTERLEAVE_3:
			/* Output Process Data communication in interleave mode*/
			if(dl->pdh.pdhCmd == IOL_DL_PDH_CMD_INACTIVE){
				/*T11: state 3 to 0*/
				dl->pdh.state = IOL_DL_PDH_STATE_INACTIVE_0;

			}else if(dl->pdh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T7: state 3 to 3
				 * Take data from DL_PDOutputUpdate service and invoke PD.req to
				 * propagate output PD to the message handler.
				 * */
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				IOL_DL_PD_Req(port, dl->pdh.pdInAddr, dl->pdh.pdInLen, dl->pdh.pdOutData, dl->pdh.pdOutAddr, dl->pdh.pdOutLen);
			}else if(dl->pdh.trigger == IOL_DL_TRIG_DEV_MSG){		//TODO: is this right? if no, what will be the condition, if PD input data is available
				/* T8: state 3 to 2. if output data updating is completed
				 * Invoke DL_PDCycle.ind to indicate end of Process Data cycle to the AL
				 * */
				dl->pdh.trigger = IOL_DL_TRIG_NONE;
				IOL_DL_PDCycle_Ind(port);
			}
			break;
		default:
			break;
	}
}



/*State machine of the On-request Data handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.5.2, Figure: 48, Table: 50
 * */
void IOL_DL_ODHandler(IOL_Port_t *port){
	IOL_DL_t *dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	switch (dl->odh.state) {
		case IOL_DL_ODH_STATE_INACTIVE_0:
			/*Waiting on activation*/
			if(dl->odh.ohCmd == IOL_DL_OH_CMD_ACTIVE){
				/* T1: state 0 to 1*/
				dl->odh.state = IOL_DL_ODH_STATE_ISDU_1;
			}else if(dl->odh.ohCmd != IOL_DL_OH_CMD_INACTIVE){
				DBG_SPRINT_NL("Error: PORT_%d{DL>State:Inactive_0>Unknown trigger:%d}", IOL_GetPortNumber(port), dl->odh.ohCmd);
			}
			break;
		case IOL_DL_ODH_STATE_ISDU_1:
			/*Default state of the On-request Data handler (lowest priority)*/
			if(dl->odh.ohCmd == IOL_DL_OH_CMD_INACTIVE){
				/* T13: state 1 to 0*/
				dl->odh.state = IOL_DL_ODH_STATE_INACTIVE_0;
			}else if(dl->cmdh.ctrlCode != IOL_CONTROLCODE_NONE &&
					dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T3: state 1 to 2*/
				//IOL_DL_CommandHandler(port);		//TODO: may change the implementation
				dl->odh.state = IOL_DL_ODH_STATE_COMMAND_2;
				//dl->cmdh.ctrlCode = IOL_CONTROLCODE_NONE;
			}else if(dl->evnth.eventFlag == IOL_EH_EVENT){
				/* T5: state 1 to 3*/
				//IOL_DL_EventHandler(port);			//TODO: may change the implementation
				dl->odh.state = IOL_DL_ODH_STATE_EVENT_3;
				//dl->evnth.eventFlag == IOL_EH_EVENT;
			}else if(dl->odh.trigger != IOL_DL_TRIG_NONE){
				/* T2: state 1 to 1
				 * On-request Data handler propagates the ODTrig.ind service now named
				 * ISDUTrig to the ISDU handler (see Figure 51). In case of DL_Read,
				 * DL_Write, DL_ReadParam, or DL_WriteParam services, the ISDU handler
				 * will use a separate transition (see Figure 51, T13).
				 */
				IOL_DL_ISDUHandler(port);
				dl->odh.trigger = IOL_DL_TRIG_NONE;
			}else{
				DBG_SPRINT_NL("Error: PORT_%d{DL>State:ISDU_1>Unknown trigger:%d}", IOL_GetPortNumber(port), dl->odh.trigger);
			}
			break;
		case IOL_DL_ODH_STATE_COMMAND_2:
			/*State to control the Device via commands with highest priority*/
			if(dl->odh.ohCmd == IOL_DL_OH_CMD_INACTIVE){
				/* T11: state 2 to 0*/
				dl->odh.state = IOL_DL_ODH_STATE_INACTIVE_0;
			}else if(dl->odh.trigger != IOL_DL_TRIG_NONE){
				/* T9: state 2 to 2:
				 * On-request Data handler propagates the ODTrig.ind service now named
				 * CommandTrig to the command handler (see Figure 53)
				 * */
				IOL_DL_CommandHandler(port);
				dl->odh.trigger = IOL_DL_TRIG_NONE;
				if(dl->cmdh.ctrlCode == IOL_CONTROLCODE_NONE){
					//TODO: baseCode have a function, but on in spec:  write_master_command (port, IOLINK_STATUS_NO_ERROR);
					if(dl->evnth.eventFlag == IOL_EH_EVENT){
						/* T8: state 2 to 3*/
						dl->odh.state = IOL_DL_ODH_STATE_EVENT_3;
					}else{
						/* T4: state 2 to 1*/
						dl->odh.state = IOL_DL_ODH_STATE_ISDU_1;
					}
				}
			}else{
				DBG_SPRINT_NL("Error: PORT_%d{DL>State:Command_2>Unknown trigger:%d}", IOL_GetPortNumber(port));
			}
			break;
		case IOL_DL_ODH_STATE_EVENT_3:
			/*State to convey Event information (errors, warnings, notifications) with higher priority*/
			if(dl->odh.ohCmd == IOL_DL_OH_CMD_INACTIVE){
				/* T12: state 3 to 0*/
				dl->odh.state = IOL_DL_ODH_STATE_INACTIVE_0;
			}else if(dl->cmdh.ctrlCode != IOL_CONTROLCODE_NONE &&
					dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T7: state 3 to 2*/
				//IOL_DL_CommandHandler(port);
				dl->odh.state = IOL_DL_ODH_STATE_COMMAND_2;
			}else if(dl->evnth.eventFlag == IOL_EH_NO_EVENT){
				/* T6: state 3 to 1*/
				dl->odh.state = IOL_DL_ODH_STATE_ISDU_1;
			}else if(dl->odh.trigger != IOL_DL_TRIG_NONE){
				/* T10: state 3 to 3
				 * On-request Data handler propagates the ODTrig.ind service now named
				 * EventTrig to the Event handler (see Figure 55)
				 * */
				IOL_DL_EventHandler(port);
				dl->odh.trigger = IOL_DL_TRIG_NONE;
			}else{
				DBG_SPRINT_NL("Error: PORT_%d{DL>State:Event_3>Unknown trigger:%d}", IOL_GetPortNumber(port));
			}
			break;
		default:
			break;
	}
}



/* State machine of the  Master ISDU handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.6.3, Figure: 51, Table: 53
 *
 * */
void IOL_DL_ISDUHandler(IOL_Port_t *port){
	IOL_DL_t *dl =  (IOL_DL_t*)IOL_GetDLInstance(port);
	switch (dl->isduh.state) {
		case IOL_DL_ISDUH_STATE_INACTIVE_0:
			/*Waiting on activation*/
			if(dl->isduh.ihCmd == IOL_DL_IH_CMD_ACTIVE){
				/* T1: state 0 to 1*/
				dl->isduh.state = IOL_DL_ISDUH_STATE_IDLE_1;
			}else{
				//TODO: DL>ISDU>INACTIVE_0: to handle unknown command
			}
			break;
		case IOL_DL_ISDUH_STATE_IDLE_1:
			/*Waiting on transmission of next On-request Data*/
			if(dl->isduh.ihCmd == IOL_DL_IH_CMD_ACTIVE){
				/* T16: state 1 to 0*/
				dl->isduh.state = IOL_DL_ISDUH_STATE_IDLE_1;
			}else if(dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				switch (dl->msgh.rwCmd) {
					case IOL_DL_MH_RWCMD_ISDUTRANS:
						/* T2: State 1 to 2
						 * Invoke OD.req with ISDU write start condition: OD.req (W, ISDU, flowCtrl =
						 * START, data)
						 * */
						IOL_DL_OD_Req(	port,
										IOL_RW_DIR_WRITE,
										IOL_COMM_CH_ISDU,
										IOL_DL_ISDU_FLOWCTRL_START,
										dl->isduh.txDataLen,
										dl->isduh.txData);
						dl->isduh.countMseq = 1;
						dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUREQUEST_2;
						break;
					case IOL_DL_MH_RWCMD_READPARAM:
						/* T13: State 1 to 1:
						 * Invoke OD.req with appropriate data  for WriteParam.req service
						 * */
						//TODO: ReadParam service is used for page comm channel why we use ISDU channel
						IOL_DL_OD_Req(	port,
										IOL_RW_DIR_READ,
										IOL_COMM_CH_PAGE,
										dl->odh.address,
										0,
										0);
						break;
					case IOL_DL_MH_RWCMD_WRITEPARAM:
						/* T13: State 1 to 1:
						 * Invoke OD.req with appropriate data for WriteParam.req service
						 * */
						//TODO: WriteParam service is used for page comm channel why we use ISDU channel
						IOL_DL_OD_Req(	port,
										IOL_RW_DIR_WRITE,
										IOL_COMM_CH_PAGE,
										dl->odh.address,
										1,
										dl->odh.outData);
						break;
					default:
						/* T13: State 1 to 1:
						 * Invoke OD.req with idle message: OD.req (R, ISDU, flowCtrl = IDLE)
						 */
						IOL_DL_OD_Req(	port,
										IOL_RW_DIR_READ,
										IOL_COMM_CH_ISDU,
										IOL_DL_ISDU_FLOWCTRL_IDLE_1,
										0,
										0);
						break;
				}
			}else if(dl->odh.trigger == IOL_DL_TRIG_DEV_MSG){
				/* T13: State 1 to 1:
				 * Invoke positive DL_ReadParam/DL_WriteParam response via .cnf services
				 * */
				switch (dl->msgh.rwCmd) {
					case IOL_DL_MH_RWCMD_READPARAM:
						IOL_DL_ReadParam_Cnf(port, dl->rxBuff[0], IOL_ERROR_NONE);
						dl->msgh.rwCmd = IOL_DL_MH_RWCMD_NONE;
						break;
					case IOL_DL_MH_RWCMD_WRITEPARAM:
						IOL_DL_WriteParam_Cnf(port, IOL_ERROR_NONE);
						dl->msgh.rwCmd = IOL_DL_MH_RWCMD_NONE;
						break;
					default:
						break;
				}
			}
			break;
		case IOL_DL_ISDUH_STATE_ISDUREQUEST_2:
			/* Transmission of ISDU request data*/
			if(		dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST ||
					dl->msgh.rwCmd == IOL_DL_MH_RWCMD_ISDUABORT )
			{
				/*T12: State 2 to 4 */
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
			}else if(dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T3: State 2 to 2:
				 * Invoke OD.req with ISDU data write and FlowCTRL under conditions of Table 52
				 * */
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_WRITE,
								IOL_COMM_CH_ISDU,
								dl->isduh.countMseq++,
								dl->isduh.txDataLen,
								dl->isduh.txData);

				if(dl->isduh.countMseq > IOL_MAX_ISDU_COUNT){
					dl->isduh.countMseq = 0;
					//TODO: to generate overflow event according to spec: Tab;e: 52, but how?
				}

			}else if(dl->odh.trigger == IOL_DL_TRIG_DEV_MSG){
				/* T4: State 2 to 3:
				 * Start timer (ISDUTime)
				 * */
				IOL_Timer_SetTimeout(&dl->isduh.tisdu, IOL_MAX_ISDU_RESP_TIME);
				IOL_Timer_Start(&dl->isduh.tisdu);
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUWAIT_3;
			}
			break;
		case IOL_DL_ISDUH_STATE_ISDUWAIT_3:
			/*Waiting on response from Device. Observe ISDUTime*/
			if(dl->msgh.rwCmd == IOL_DL_MH_RWCMD_ISDUABORT){
				/*T17: State 3 to 4:*/
				IOL_Timer_Stop(&dl->isduh.tisdu);
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
			}else if (	dl->isduh.tisdu.isTimeout == IOL_TIMER_TIMEOUT ||
						dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST)
			{
				/*T9: State 3 to 4:*/
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
			}else if(dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T5: State 3 to 3:
				 * Invoke OD.req with ISDU read start condition: OD.req (R, ISDU, flowCtrl = START)
				 * */
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_READ,
								IOL_COMM_CH_ISDU,
								IOL_DL_ISDU_FLOWCTRL_START,
								dl->isduh.txDataLen,
								dl->isduh.txData);
			}else if (dl->odh.trigger == IOL_DL_TRIG_DEV_MSG) {
				/* T6: State 3 to 5:
				 * Stop timer (ISDUTime)
				 * */
				IOL_Timer_Stop(&dl->isduh.tisdu);
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
				//TODO: incomplete
			}
			break;
		case IOL_DL_ISDUH_STATE_ISDUERROR_4:
			/* Error handling after detected errors: Invoke negative DL_ISDU_Transport response
			 * with ISDUTransportErrorInfo
			 * */
			if(dl->isduh.ihCmd == IOL_DL_IH_CMD_INACTIVE){
				/* T18: State 5 to 4:
				 * In case of lost communication, the message handler informs the DL_Mode
				 * handler which in turn uses the administrative call IH_Conf_INACTIVE. No
				 * actions during this transition required.
				 * */
				dl->isduh.state = IOL_DL_ISDUH_STATE_INACTIVE_0;
			}else if(dl->msgh.rwCmd == IOL_DL_MH_RWCMD_ISDUABORT){
				/* T11: State 4 to 1:
				 * Invoke OD.req with ISDU abortion: OD.req (R, ISDU, flowCtrl = ABORT).
				 * Invoke negative DL_ISDUTransport confirmation
				 */
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_READ,
								IOL_COMM_CH_ISDU,
								IOL_DL_ISDU_FLOWCTRL_ABORT,
								0,
								0);
				dl->isduh.countMseq = 0;
				IOL_DL_ISDUTransport_Cnf(	port,
											dl->isduh.rxData,
											IOL_ISDU_IS_QUAL_DEV_RRSP_N_12,
											IOL_ERROR_ISDU_ABORT);
				dl->msgh.rwCmd = IOL_DL_MH_RWCMD_NONE;
			}else if (dl->isduh.tisdu.isTimeout == IOL_TIMER_TIMEOUT){

				dl->isduh.tisdu.isTimeout = IOL_TIMER_TIMEOUT_NONE;
				IOL_DL_ISDUTransport_Cnf(	port,
											dl->isduh.rxData,
											IOL_ISDU_IS_QUAL_DEV_RRSP_N_12,
											IOL_ERROR_ISDU_TIMEOUT);
			}else if (dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST)	{
				dl->msgh.mhInfo = IOL_DL_MHINFO_NONE;
				IOL_DL_ISDUTransport_Cnf(	port,
											dl->isduh.rxData,
											IOL_ISDU_IS_QUAL_DEV_RRSP_N_12,
											IOL_ERROR_NO_COMM);
			}
			//TODO: incomplete
			break;
		case IOL_DL_ISDUH_STATE_ISDURESPONSE_5:
			/*Get response data from Device*/
			if(dl->msgh.rwCmd == IOL_DL_MH_RWCMD_ISDUABORT){
				/*T18: State 5 to 4:*/
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
			}else if (	dl->isduh.tisdu.isTimeout == IOL_TIMER_TIMEOUT ||
						dl->msgh.mhInfo == IOL_DL_MHINFO_COMLOST)
			{
				/*T10: State 5 to 4:*/
				dl->isduh.state = IOL_DL_ISDUH_STATE_ISDUERROR_4;
			}else if (dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG) {
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_WRITE,
								IOL_COMM_CH_ISDU,
								dl->isduh.countMseq++,
								dl->isduh.txDataLen,
								dl->isduh.txData);
				if(dl->isduh.countMseq > IOL_MAX_ISDU_COUNT){
					dl->isduh.countMseq = 0;
					//TODO: to generate overflow event according to spec: Tab;e: 52, but how?
				}
			}else if (dl->odh.trigger == IOL_DL_TRIG_DEV_MSG) {
				/* T8: State 5 to 1:
				 * OD.req (R, ISDU, flowCtrl = IDLE)
				 * Invoke positive DL_ISDUTransport confirmation
				 * */
				//TODO: how do I know transmission is completed
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_READ,
								IOL_COMM_CH_ISDU,
								IOL_DL_ISDU_FLOWCTRL_IDLE_1,
								0,
								0);
				IOL_DL_ISDUTransport_Cnf(	port,
											dl->isduh.rxData,
											IOL_ISDU_IS_QUAL_DEV_RRSP_P_13,
											IOL_ERROR_NONE);
			}
			break;
		default:
			break;
	}
}


/* State machine of the  Master command handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.7.2, Figure: 53, Table: 56
 *
 * */
void IOL_DL_CommandHandler(IOL_Port_t *port){
	IOL_DL_t *dl =  (IOL_DL_t*)IOL_GetDLInstance(port);
	switch (dl->cmdh.state) {
		case IOL_DL_CMDH_STATE_INACTIVE_0:
			/* Waiting on activation by DL-mode handler */
			if(dl->cmdh.chCmd == IOL_DL_CH_CMD_ACTIVE){
				/* T1: state 0 to 1 */
				dl->cmdh.state = IOL_DL_CMDH_STATE_IDLE_1;
			}
			break;
		case IOL_DL_CMDH_STATE_IDLE_1:
			/*  Waiting on new command from AL: DL_Control (status of output PD)
			 *  or from SM: DL_Write (change Device mode, for example to OPERATE),
			 *  or waiting on PDInStatus.ind service primitive.
			 * */

			if(dl->cmdh.chCmd == IOL_DL_CH_CMD_INACTIVE){
				/* T6: state 1 to 0*/
				dl->cmdh.state = IOL_DL_CMDH_STATE_INACTIVE_0;
			}else if(dl->cmdh.ctrlCode == IOL_CONTROLCODE_PDOUTVALID ||
					dl->cmdh.ctrlCode == IOL_CONTROLCODE_PDOUTINVALID){
				/* T3: state 1 to 2
				 * 1. If service DL_Control.req = PDOUTVALID invoke OD.req (WRITE, PAGE,
				 * 0, 1, MasterCommand = 0x98).
				 * 2. If service DL_Control.req = PDOUTINVALID invoke OD.req (WRITE,
				 * PAGE, 0, 1, MasterCommand = 0x99). See Table B.2.
				 * */
				if(dl->cmdh.ctrlCode == IOL_CONTROLCODE_PDOUTVALID){
					dl->cmdh.masterCmd = IOL_MSTR_CMD_PDO_OPERATE;	//0x98
				}else if(dl->cmdh.ctrlCode == IOL_CONTROLCODE_PDOUTINVALID){
					dl->cmdh.masterCmd = IOL_MSTR_CMD_DEV_OPERATE;	//0x99
				}

				dl->cmdh.state = IOL_DL_CMDH_STATE_MASTER_CMD_2;
			}else if(dl->cmdh.ctrlCode == IOL_CONTROLCODE_DEVICEMODE){
				/* T4: state 1 to 2------------
				 * The services DL_Write_DEVICEMODE translate into:
				 * INACTIVE: OD.req (WRITE, PAGE, 0, 1, MasterCommand = 0x5A)
				 * STARTUP: OD.req (WRITE, PAGE, 0, 1, MasterCommand = 0x97)
				 * PREOPERATE: OD.req (WRITE, PAGE, 0, 1, MasterCommand = 0x9A)
				 * OPERATE: OD.req (WRITE, PAGE, 0, 1, MasterCommand = 0x99)
				 * Thats mean, in DL_Write service, the masterCommand variable will
				 * be set with appropriate master command
				 */
				dl->cmdh.state = IOL_DL_CMDH_STATE_MASTER_CMD_2;
			}else if(dl->cmdh.pdInStatus !=  IOL_PDIN_STATUS_NONE){
				/* T2: state 1 to 1:
				 * 1. If service PDInStatus.ind = VALID invoke DL_Control.ind (VALID)
				 * 	  to signal valid input Process Data to AL.
				 * 2. If service PDInStatus.ind = INVALID invoke DL_Control.ind (INVALID)
				 *    to signal invalid input Process Data to AL.
				 * */
				IOL_DL_Control_Ind(port, dl->cmdh.pdInStatus == IOL_PDIN_STATUS_VALID ?
											IOL_CONTROLCODE_VALID : IOL_CONTROLCODE_INVALID);
			}
			break;
		case IOL_DL_CMDH_STATE_MASTER_CMD_2:
			/*Prepare data for OD.req service primitive. Waiting on demand from OD handler]
			 * (CommandTrig).
			 * */

			//TODO: to implement later. need to understand deeply
			if(dl->odh.trigger ==IOL_DL_TRIG_MSTR_MSG){
				/* T5: state 2 to 1:
				 * A call CommandTrig from the OD handler causes the command handler to
				 * invoke the OD.req service primitive and subsequently the message handler
				 * to send the appropriate MasterCommand to the Device.
				 */
				IOL_DL_OD_Req(
								port,
								IOL_RW_DIR_WRITE,
								IOL_COMM_CH_PAGE,
								0,
								1,
								(uint8_t*)&dl->cmdh.masterCmd);
				dl->cmdh.state = IOL_DL_CMDH_STATE_IDLE_1;
			}

			break;
		default:
			break;
	}
}


/* State machine of the  Event handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.8.3, Figure: 55, Table: 59
 * */
void IOL_DL_EventHandler(IOL_Port_t *port){
	IOL_DL_t *dl =  (IOL_DL_t*)IOL_GetDLInstance(port);

	switch (dl->evnth.state) {
		case IOL_DL_EH_STATE_INACTIVE_0:
			/*Waiting on activation*/
			if(dl->evnth.ehCmd == IOL_DL_EH_CMD_ACTIVE){
				/* T1: state 0 to 1*/
				dl->evnth.state = IOL_DL_EH_STATE_IDLE_1;
			}
			break;
		case IOL_DL_EH_STATE_IDLE_1:
			/* Waiting on next Event indication ("EventTrig" through On-request Data handler) or
			 * Event confirmation through service DL_EventConf from Master AL.
			 * */
			if(dl->evnth.ehCmd == IOL_DL_EH_CMD_INACTIVE){
				/* T10: state 1 to 0*/
				dl->evnth.state = IOL_DL_EH_STATE_INACTIVE_0;
			}else if(dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T2: state 1 to 2:
				 * Read Event StatusCode octet via service OD.req (R, DIAGNOSIS, Event
				 * memory address = 0, 1)
				 * */
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_READ,
								IOL_COMM_CH_DIAGNOSIS,
								0,
								1,
								0);
				dl->evnth.eventMemAddr = 0;
				dl->odh.trigger = IOL_DL_TRIG_NONE;
				dl->evnth.state = IOL_DL_EH_STATE_READEVENT_2;
			}else if(dl->evnth.eventConf){
				/* T7: state 1 to 4 */
				dl->evnth.eventConf = 0;
				dl->evnth.state = IOL_DL_EH_STATE_EventConf_4;
			}
			//TODO: incomplete this state
			break;
		case IOL_DL_EH_STATE_READEVENT_2:
			/* Read Event data set from Device message by message through Event memory
			 * address. Check StatusCode for number of activated Events (see Table 58)
			 * */
			if(dl->mdh.mode == IOL_MDH_MODE_COMLOST){
				/* T6: state 2 to 0*/
				dl->evnth.state = IOL_DL_EH_STATE_INACTIVE_0;
			}else if(dl->odh.trigger == IOL_DL_TRIG_MSTR_MSG){
				/* T3: state 2 to 2:
				 * Read octets from Event memory via service OD.req (R, DIAGNOSIS,
				 * incremented Event memory address, 1)
				 * */
				//TODO: to implement later
				IOL_DL_OD_Req(	port,
								IOL_RW_DIR_READ,
								IOL_COMM_CH_DIAGNOSIS,
								dl->evnth.eventMemAddr++,
								1,
								0);
				dl->odh.trigger = IOL_DL_TRIG_NONE;
			}else if(dl->evnth.eventMemAddr > IOL_MAX_EVENT_MEM_ADDRESS){
				/* T4: state 2 to 3 */
				dl->evnth.state = IOL_DL_EH_STATE_SIGNALEVENT_3;
			}
			//TODO: incomplete this state
			break;
		case IOL_DL_EH_STATE_SIGNALEVENT_3:
			/* Analyze Event data and invoke DL_Event indication to Master AL (see 7.2.1.15) for
			 * each available Event.
			 * */
			//TODO: incomplete this state
			break;
		case IOL_DL_EH_STATE_EventConf_4:
			/*Waiting on Event confirmation transmission via service OD.req to the Device*/
			break;
		default:
			break;
	}
}



/*Initialize DL*/
void IOL_DL_Initialize(IOL_Port_t *port){
	IOL_DL_t* dl = (IOL_DL_t*)IOL_GetDLInstance(port);

	IOL_Timer_Init(&dl->tdmt);
	IOL_Timer_Init(&dl->tdwu);
	IOL_Timer_Init(&dl->tsd);
}

/*Main handler for the DL-------------------------*/
void IOL_DL_Handler(IOL_Port_t *port){
	IOL_DL_ModeHandler(port);
	IOL_DL_MessageHandler(port);
}
