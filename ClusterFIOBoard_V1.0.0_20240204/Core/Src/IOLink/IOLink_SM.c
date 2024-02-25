/*
 * IOLink_SM.c
 *
 *  Created on: Nov 3, 2023
 *      Author: wsrra
 */
#include "stdio.h"
#include "IOLink_SM.h"
#include "IOLink_SMI.h"
#include "IOLink_DL.h"
#include "IOLink_PL.h"
#include "IOLink_AL.h"
#include "Debug.h"

#define SM_MAX_W_ADDR_INDEX		6U
static const uint8_t SM_WriteAddrList[SM_MAX_W_ADDR_INDEX] = {
		IOL_DIR_PARM_ADDR_REV_ID,
		IOL_DIR_PARM_ADDR_DEV_ID_MSB,
		IOL_DIR_PARM_ADDR_DEV_ID_MLSB,
		IOL_DIR_PARM_ADDR_DEV_ID_LSB,
		IOL_DIR_PARM_ADDR_MASTER_CMD,
		IOL_DIR_PARM_ADDR_MIN_CYCL_TIM
};

static inline void SM_SetValueList(IOL_ModeValueList_t *vl, uint8_t mseqTime,
		IOL_MseqSubType mseqType, uint8_t pdInLen, uint8_t pdOutLen, uint8_t odLenPerMsg);
static IOL_Errors SM_PrepareValuelist (IOL_ModeValueList_t *vl, uint8_t mseqCapb,
		uint8_t revid, uint8_t pdin, uint8_t pdout, uint8_t cycTime, IOL_MDHMode mode);
static void SM_ReadParamBatch(IOL_Port_t *port,  uint8_t startAddr, uint8_t endAddr);
static void SM_CheckComp(IOL_Port_t *port);

/*Built in services------------------------------*/
/* @brief 9.2.2.2 SM_SetPortConfig.req
 * The SM_SetPortConfig service is used to set up the requested Device configuration
 * @retval error status
 * */
IOL_Errors IOL_SM_SetPortConfig_Req(IOL_Port_t *port, IOL_SM_ParamList_t paramList){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);

	/*1. check the parameter validity*/
	if( paramList.targetMode != IOL_TARGET_MODE_INACTIVE &&
		paramList.targetMode != IOL_TARGET_MODE_DI &&
		paramList.targetMode != IOL_TARGET_MODE_DO &&
		paramList.targetMode != IOL_TARGET_MODE_CFGCOM &&
		paramList.targetMode != IOL_TARGET_MODE_AUTOCOM )
	{
		IOL_SM_SetPortConfig_Cnf(port, paramList.portNum, IOL_ERROR_PARM_CONFLICT);
		return IOL_ERROR_PARM_CONFLICT;
	}else if(paramList.inspecLevel != IOL_INSPEC_LEVEL_NO_CHECK &&
			 paramList.inspecLevel != IOL_INSPEC_LEVEL_TYPE_COMP &&
			 paramList.inspecLevel != IOL_INSPEC_LEVEL_IDENTICAL )
	{
		IOL_SM_SetPortConfig_Cnf(port, paramList.portNum, IOL_ERROR_PARM_CONFLICT);
		return IOL_ERROR_PARM_CONFLICT;
	}

	sm->configParamList = paramList;

	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.2 SM_SetPortConfig.cnf
 * The IOL_SM_SetPortConfig_Cnf service is used to return the response of IOL_SM_SetPortConfig_Req service
 * @retval error status
 * */
IOL_Errors IOL_SM_SetPortConfig_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_Errors errorInf){
//	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later

	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.3 SM_GetPortConfig.req
 * The SM_GetPortConfig service is used to acquire the real (actual) Device configuration.
 * @retval error status
 * */
IOL_Errors IOL_SM_GetPortConfig_Req(IOL_Port_t *port, uint8_t portNum){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later
	sm->realParamList.portNum = portNum;
	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.3 SM_GetPortConfig.cnf
 * The IOL_SM_GetPortConfig_Cnf service is used to return the response of IOL_SM_GetPortConfig_Req service
 * @retval error status
 * */
IOL_Errors IOL_SM_GetPortConfig_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_SM_ParamList_t realParamList, IOL_Errors errorInf){
//	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.4 SM_PortMode.ind
 * The SM_PortMode service is used to indicate changes or faults of the local communication
 * mode. These shall be reported to the Master application
 * @retval error status
 * */
IOL_Errors IOL_SM_PortMode_Ind(IOL_Port_t *port, uint8_t portNum, IOL_SM_PortMode mode){
//	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
//	IOL_MA_SM_PortMode_Ind(port, portNum, mode);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.5 SM_Operate.req
 * The SM_Operate service prompts System Management to calculate the MasterCycleTime for
 * the ports if the service is acknowledged positively with Result (+). This service is effective at
 * the indicated port.
 * @retval error status
 * */
IOL_Errors IOL_SM_Operate_Req(IOL_Port_t *port, uint8_t portNum){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later
	//1. check state validity
	if(sm->mode == IOL_MDH_MODE_OPERATE ||
			sm->state != IOL_SM_STATE_WAIT_4 ){
		IOL_SM_Operate_Cnf(port, sm->configParamList.portNum, IOL_ERROR_STATE_CONFLICT);
		return IOL_ERROR_STATE_CONFLICT;
	}
	//2. Calculate Master Cycle Time
	//TODO: To implement later. Need to understand deeply
	return IOL_ERROR_NONE;
}

/* @brief 9.2.2.5 SM_Operate.cnf
 * The IOL_SM_Operate_Cnf service is used to return the response of IOL_SM_Operate_Req service
 * @retval error status
 * */
IOL_Errors IOL_SM_Operate_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_Errors errorInf){
//	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/*Response receive service from DL cnf service--------------------*/
/* @brief Response receive service from DL_Read_Cnf service
 * */
void IOL_SM_DL_Read_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	if(errorInfo == IOL_ERROR_NONE){
		switch (sm->paramAddress ) {
			case IOL_DIR_PARM_ADDR_MASTER_CMD:
				//TODO: To implement later
				break;
			case IOL_DIR_PARM_ADDR_MASTER_CYCL_TIM:
				//TODO: To implement later
				break;
			case IOL_DIR_PARM_ADDR_MIN_CYCL_TIM:
				sm->realParamList.cycleTime = value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_M_SEQ_CAPBILITY:
				sm->comParamList.mseqCapb = value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_REV_ID:
				sm->realParamList.revId = value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_PDI:
				sm->comParamList.pdIn = value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_PDO:
				sm->comParamList.pdOut = value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_VEND_ID_MSB:
				sm->realParamList.vendId = ((uint16_t)value<<8);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_VEND_ID_LSB:
				sm->realParamList.vendId |= (uint16_t)value;
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_DEV_ID_MSB:
				sm->realParamList.devId = ((uint32_t)value<<16);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_DEV_ID_MLSB:
				sm->realParamList.devId |= ((uint32_t)value<<8);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_DEV_ID_LSB:
				sm->realParamList.devId |= ((uint32_t)value);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_FUNC_ID_MSB:
				sm->realParamList.funcId = ((uint16_t)value<<8);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_FUNC_ID_LSB:
				sm->realParamList.funcId |= ((uint16_t)value);
				sm->rwStatus = IOL_RW_STATUS_DONE;
				break;
			case IOL_DIR_PARM_ADDR_SYS_CMD:
				//TODO: To implement later
				break;
			default:
				break;
		}
	}
}

/* @brief Response receive service from DL_Write_Cnf service
 * */
void IOL_SM_DL_Write_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	if(errorInfo != IOL_ERROR_NONE){
		/*Check the write operation for restart device*/
		if(sm->state == IOL_SM_STATE_RESTARTDEVICE_24){
			switch (sm->paramAddress) {
				case IOL_DIR_PARM_ADDR_DEV_ID_MSB:
				case IOL_DIR_PARM_ADDR_DEV_ID_MLSB:
				case IOL_DIR_PARM_ADDR_DEV_ID_LSB:
				case IOL_DIR_PARM_ADDR_MASTER_CMD:
				case IOL_DIR_PARM_ADDR_MIN_CYCL_TIM:
					sm->rwStatus = IOL_RW_STATUS_DONE;
					break;
				default:
					break;
			}
		}
	}else if(errorInfo == IOL_ERROR_NO_COMM){
		sm->mode = IOL_MDH_MODE_COMLOST;
	}
	//TODO: To implement other error
}

/* @brief Response receive service from DL_SetMode_Cnf service
 * */
void IOL_SM_DL_SetMode_Cnf(IOL_Port_t *port, IOL_Errors errorInf){
//	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	//TODO: To implement later
}

/* @brief Response receive service from DL_Mode_Ind service
 * */
void IOL_SM_DL_Mode_Ind(IOL_Port_t *port, IOL_MDHMode realMode){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	sm->mode = realMode;
}

void IOL_SM_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, IOL_Errors errorInfo){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	if(sm->configParamList.portNum == portNumber && errorInfo == IOL_ERROR_NONE){
		sm->realParamList.SrlNum = ((((uint16_t)data[0])<<8)&data[1]);
	}
}
/*Extra services-----------------------------------*/


/* @brief IOL_SM_SetValueList is used to set the list of values for mode
 * @retval error status
 * */
static inline void SM_SetValueList(IOL_ModeValueList_t *vl, uint8_t mseqTime,
		IOL_MseqSubType mseqType, uint8_t pdInLen, uint8_t pdOutLen, uint8_t odLenPerMsg)
{
	vl->mseqTime = mseqTime;
	vl->mseqType = mseqType;
	vl->pdInLen = pdInLen;
	vl->pdOutLen = pdOutLen;
	vl->odLenPerMsg = odLenPerMsg;
}


/* @brief modify value list based on different conditions
 * @retval error status
 * */
static IOL_Errors SM_PrepareValuelist (IOL_ModeValueList_t *vl, uint8_t mseqCapb,
		uint8_t revid, uint8_t pdin, uint8_t pdout, uint8_t cycTime, IOL_MDHMode mode)
{
	/* Interpret octet into bit fields
	 * */
	IOL_MseqCapbOctet_t mseqCap = {mseqCapb};
	IOL_PDInOctet_t pdIn = {pdin};
	IOL_PDOutOctet_t pdOut = {pdout};
	uint8_t odLen = 0;
	IOL_MseqSubType mseqSubType = IOL_MSEQ_SUB_TYPE_NONE;
	vl->mseqType = IOL_MSEQ_SUB_TYPE_NONE;

	switch (mode) {
		case IOL_MDH_MODE_STARTUP:
			SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_0, 0, 0, 1);
			break;
		case IOL_MDH_MODE_PREOPERATE:{
			  switch (mseqCap.field.msqTypProp) {
			  /*A.2.6, Table A.8 – M-sequence types for the PREOPERATE mode*/
				case 0:
					odLen = 1;
					mseqSubType = IOL_MSEQ_SUB_TYPE_0;
					break;
				case 1:
					odLen = 2;
					mseqSubType = IOL_MSEQ_SUB_TYPE_1_2;
					break;
				case 2:
					odLen = 8;
					mseqSubType = IOL_MSEQ_SUB_TYPE_1_V;
					break;
				case 3:
					odLen = 8;
					mseqSubType = IOL_MSEQ_SUB_TYPE_1_V;
					break;
				default:
					odLen = 0;
					mseqSubType = IOL_MSEQ_SUB_TYPE_NONE;
					break;
			}
			  if(odLen != 0 && mseqSubType != IOL_MSEQ_SUB_TYPE_NONE){
				  SM_SetValueList(vl, cycTime, mseqSubType, 0, 0, odLen);
			  }
			}break;
		case IOL_MDH_MODE_OPERATE:{
		  if (revid != IOL_PROTOCOL_REV_ID_V11){
			/* For legacy version compatibility*/
			if ((pdIn.field.len == 0) && (pdOut.field.len == 0)){
				 /* no process data
				  * */
				if(mseqCap.field.msqTypOp == 0){
					mseqSubType = IOL_MSEQ_SUB_TYPE_0;
					odLen = 1;
				}else{
					mseqSubType = IOL_MSEQ_SUB_TYPE_1_2;
					odLen = 2;
				}
				SM_SetValueList(vl, cycTime, mseqSubType, 0, 0, odLen);
			}else {
				 /* Table B.6 – Permitted combinations of BYTE and Length */
				if ((pdIn.field.byte || pdOut.field.byte) &&
					((pdIn.field.len  >= 2U) || (pdOut.field.len >= 2U)))
				{
					/* TODO: to understand which one is correct. see ref- A.2.3, Table A.9 & B.6
					 * 1st one is according to reference code
					 * 2nd one is according to specification
					 *
					 */
					 //SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_1_1, 0, 0, 2); // 1st

					 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_1_1, //2nd
						   pdIn.field.len+1 , pdOut.field.len+1, 2);
				}else {
				   if (pdOut.field.len == 0) {
					  if ((pdIn.field.len >= 1) && (pdIn.field.len <= 8)){
						 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_1, 1, 0, 1);
					  }else if ((pdIn.field.len >= 9) && (pdIn.field.len <= 16)){
						 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_2, 2, 0, 1);
					  }
				   }else if (pdIn.field.len == 0){
					  if ((pdOut.field.len >= 1) && (pdOut.field.len <= 8)){
						 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_3, 0, 1, 1);
					  }else if ((pdOut.field.len >= 9) && (pdOut.field.len <= 16)){
						 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_4, 0, 2, 1);
					  }
				   }else if ((pdIn.field.len >= 1) && (pdIn.field.len <= 8) &&
						   (pdOut.field.len >= 1) && (pdOut.field.len <= 8))
				   {
					  SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_5, 1, 1, 1);
				   }
				}
			}
		  }else{			// for version 1.1 compatibility
			 switch (mseqCap.field.msqTypOp){
				 case 0:
					if ((pdIn.field.len == 0) && (pdOut.field.len == 0)){
					   SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_0, 0, 0, 1);
					}else if (!pdIn.field.byte && !pdOut.field.byte){
					   if (pdOut.field.len == 0){
						  if ((pdIn.field.len >= 1) && (pdIn.field.len <= 8)){
							 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_1, 1, 0, 1);
						  }else if ((pdIn.field.len >= 9) && (pdIn.field.len <= 16)) {
							 SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_2, 2, 0, 1);
						  }
					   }else{
						  if (pdIn.field.len == 0){
							 if ((pdOut.field.len >= 1) && (pdOut.field.len <= 8)){
								SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_3, 0, 1, 1);
							 }
							 else if ((pdOut.field.len >= 9) && (pdOut.field.len <= 16)){
								SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_4, 0, 2, 1);
							 }
						  }else if ((pdIn.field.len >= 1) && (pdIn.field.len <= 8) &&
									(pdOut.field.len >= 1) && (pdOut.field.len <= 8))
						  {
							  SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_5, 1, 1, 1);
						  }else if (  ((pdIn.field.len  >= 9) && (pdIn.field.len  <= 16) &&
									  (pdOut.field.len >= 1) && (pdOut.field.len <= 16)) ||
									  ((pdIn.field.len >= 1) && (pdIn.field.len <= 16) &&
									  (pdOut.field.len >= 9) && (pdOut.field.len <= 16)))
						  {
							  SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_V,
											  ((pdIn.field.len >= 9) ? 2 : 1),
											  ((pdOut.field.len >= 9) ? 2 : 1), 1);
						  }
					   }
					}
					break;
				 case 1:
					if ((pdIn.field.len == 0) && (pdOut.field.len == 0)){
						SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_1_2, 0, 0, 2);
					}
					break;
				 case 4:
					 //TODO: this implementation is different from reference code.
						if ((pdIn.field.byte && (pdIn.field.len >= 2)) ||
							(pdOut.field.byte && (pdOut.field.len >= 2)))
						{
							SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_V,
									pdIn.field.len+1, pdOut.field.len+1, 1);
						}
					break;
				 case 5: /*case 5 to 7 are similar so no break is there*/
				 case 6:
				 case 7:{
						 //TODO: to understand deeply
						odLen = 1<<((2*mseqCap.field.msqTypOp)-9);
						if(pdOut.field.len > 0 || pdOut.field.len > 0){
							if ((pdIn.field.byte && (pdIn.field.len >= 2)) ||
									(!pdIn.field.byte && (pdIn.field.len <= 16)))
							{
								if ((pdOut.field.byte && (pdOut.field.len >= 2)) ||
										(!pdOut.field.byte && (pdOut.field.len <= 16)))
								{
									SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_2_V,
											(pdIn.field.byte ?  (pdIn.field.len+1) : ((pdIn.field.len+7)>>3)),
											(pdOut.field.byte ?  (pdOut.field.len+1) : ((pdOut.field.len+7)>>3)),
											odLen);
								}
							}
						}else if(mseqCap.field.msqTypOp != 5){
							SM_SetValueList(vl, cycTime, IOL_MSEQ_SUB_TYPE_1_V, 0, 0, odLen);
						}
					 }break;
				default:
					break;
				 }
			  }
			}break;
		default:
			break;
	}
	return vl->mseqType == IOL_MSEQ_SUB_TYPE_NONE ? IOL_ERROR_INCORRECT_DATA : IOL_ERROR_NONE;
//	return (valuelist->type == IOLINK_MSEQTYPE_TYPE_NONE)
//			 ? IOLINK_ERROR_INCORRECT_DATA
//			 : IOLINK_ERROR_NONE;
}

/* To read the parameters in batch
 * */
static void SM_ReadParamBatch(IOL_Port_t *port, uint8_t startAddr, uint8_t endAddr){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	if(sm->rwStatus == IOL_RW_STATUS_NONE){
    	sm->paramAddress = startAddr;
		IOL_DL_Read_Req(port, sm->paramAddress);
		sm->rwStatus = IOL_RW_STATUS_BUSY;
	}else if(sm->rwStatus == IOL_RW_STATUS_DONE){
		/* minCycleTime read request is sent in state IOL_SM_STATE_CHECKCOMPABILITY
		 * */
		if( sm->paramAddress > startAddr ||
			sm->paramAddress < endAddr)
		{
			sm->paramAddress ++;
			IOL_DL_Read_Req(port, sm->paramAddress);
			sm->rwStatus = IOL_RW_STATUS_BUSY;
		}else if(sm->paramAddress >= endAddr){	// all reading request is completed
			sm->rwStatus = IOL_RW_STATUS_ALL_DONE;
		}
	}
}


/*   the compatibility check in state "CheckComp" figure 75
 * */
static void SM_CheckComp(IOL_Port_t *port){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	IOL_ModeValueList_t valueList = {0};
	IOL_SM_PortMode mode = 0;
	if( sm->configParamList.cycleTime > 0 &&
		sm->configParamList.cycleTime < sm->realParamList.cycleTime)
	{
		/* D6 > T17: state 1 to 7 > CycTimeFault
		 * SM_PortMode.ind (CYCTIME_FAULT), DL_SetMode.req (PREOPERATE, ValueList)
		 * */
		mode = IOL_SM_PORTMODE_CYCTIME_FAULT;
		sm->status = IOL_SM_STATUS_CYCTIMEFAULT;
		sm->state = IOL_SM_STATE_INSPECFAULT_6;
	}else if(sm->configParamList.inspecLevel == IOL_INSPEC_LEVEL_NO_CHECK){
		/* D7 > T2: state 1 to 2 > CompOK
		 * DL_SetMode.req (PREOPERATE, ValueList)
		 * */
		sm->status = IOL_SM_STATUS_COMPOK;
		sm->state = IOL_SM_STATE_WAITONDLPREOP_2;
	}else if(sm->configParamList.vendId != sm->realParamList.vendId){
		/* D8 > T7: state 1 to 6 > CompFault
		 * SM_PortMode.ind (COMP_FAULT), DL_SetMode.req (PREOPERATE, ValueList)
		 * */
		mode = IOL_SM_PORTMODE_COMP_FAULT;
		sm->status = IOL_SM_STATUS_COMPFAULT;
		sm->state = IOL_SM_STATE_INSPECFAULT_6;
	}else if(sm->configParamList.devId == sm->realParamList.devId){
		/* D8to8 > T2: state 1 to 2 > CompOK
		 * DL_SetMode.req (PREOPERATE, ValueList)
		 * */
		sm->status = IOL_SM_STATUS_COMPOK;
		sm->state = IOL_SM_STATE_WAITONDLPREOP_2;
	}else{	//CDID <> RDID

		if(sm->compRetry == 0){
			/* D9 > T23: state 23 to 24 > CompFault*/
			sm->writeDone = 0;
			sm->compRetry++;	//TODO: not in spec. but it should be. need to understand why it is not in spec
			sm->status = IOL_SM_STATUS_RETRYSTARTUP;
			sm->state = IOL_SM_STATE_RESTARTDEVICE_24;
		}else{
			/* D9 > T7: state 1 to 7 > CycTimeFault
			 * SM_PortMode.ind (COMP_FAULT), DL_SetMode.req (PREOPERATE, ValueList)
			 * */
	    	mode = IOL_SM_PORTMODE_COMP_FAULT;
			sm->status = IOL_SM_STATUS_COMPFAULT;
			sm->state = IOL_SM_STATE_INSPECFAULT_6;
		}
	}
	if(sm->state != IOL_SM_STATE_RESTARTDEVICE_24){
		if(sm->state != IOL_SM_STATE_WAITONDLPREOP_2){
			IOL_SM_PortMode_Ind(port, sm->configParamList.portNum, mode);
		}
		SM_PrepareValuelist(&valueList, sm->comParamList.mseqCapb,
							sm->realParamList.revId, sm->comParamList.pdIn,
							sm->comParamList.pdOut,	sm->configParamList.cycleTime,
							IOL_MDH_MODE_PREOPERATE);
		IOL_DL_SetMode_Req(port, IOL_MDH_MODE_PREOPERATE, &valueList);
	}
}

/* Restart device on fault condition
 * Write compatibility parameters
 * */
static IOL_Errors SM_RestartDevice(IOL_Port_t *port){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	static uint8_t addrIndex = 0;
	if(!sm->writeDone){
//		IOL_DL_Write_Req(port,sm->paramAddress, ));
		if(sm->rwStatus == IOL_RW_STATUS_NONE){
			addrIndex = 0;
	    	sm->paramAddress = SM_WriteAddrList[addrIndex++];
	    	IOL_DL_Write_Req(port, sm->paramAddress, sm->configParamList.revId);
			sm->rwStatus = IOL_RW_STATUS_BUSY;
		}else if(sm->rwStatus == IOL_RW_STATUS_DONE){
			/* minCycleTime read request is sent in state IOL_SM_STATE_CHECKCOMPABILITY
			 * */
			if( addrIndex > 0 || addrIndex < SM_MAX_W_ADDR_INDEX){
				addrIndex++;
				if (sm->configParamList.targetMode == IOL_TARGET_MODE_AUTOCOM){
					addrIndex = 4;		// jump to the MASTER_CMD to write device ident
				}else if(sm->configParamList.targetMode != IOL_TARGET_MODE_CFGCOM){
					return IOL_ERROR_INCORRECT_DATA;
				}
				sm->paramAddress = SM_WriteAddrList[addrIndex];
				switch (sm->paramAddress) {
					case IOL_DIR_PARM_ADDR_DEV_ID_MSB:
						IOL_DL_Write_Req(port, sm->paramAddress, (uint8_t)(sm->configParamList.devId>>16));
						break;
					case IOL_DIR_PARM_ADDR_DEV_ID_MLSB:
						IOL_DL_Write_Req(port, sm->paramAddress, (uint8_t)(sm->configParamList.devId>>8));
						break;
					case IOL_DIR_PARM_ADDR_DEV_ID_LSB:
						IOL_DL_Write_Req(port, sm->paramAddress, (uint8_t)sm->configParamList.devId);
						break;
					case IOL_DIR_PARM_ADDR_MASTER_CMD:
						IOL_DL_Write_Req(port, sm->paramAddress, IOL_MSTR_CMD_DEV_IDENT);
						break;
					case IOL_DIR_PARM_ADDR_MIN_CYCL_TIM:
						IOL_DL_Read_Req(port, sm->paramAddress);
						break;
					default:
						break;
				}

				sm->rwStatus = IOL_RW_STATUS_BUSY;
			}else if(addrIndex >= SM_MAX_W_ADDR_INDEX){	// all reading request is completed
				sm->rwStatus = IOL_RW_STATUS_ALL_DONE;
				sm->writeDone = 1;
			}
		}
	}
	return IOL_ERROR_NONE;
}

/*State machines----------------------------------------------------*/
/* IOlink system management main state machine*/
void IOL_SM_Handler(IOL_Port_t *port){
	IOL_SM_t *sm = (IOL_SM_t *)IOL_GetSMInstance(port);
	char pData[100] = {0};
	DBG_SPRINT_APPEND(pData,"\r\nSM>");
    switch (sm->state) {
        case IOL_SM_STATE_PORTINACTIVE_0:
        	sm->status = IOL_SM_STATUS_NONE;
//        	DBG_SPRINT_APPEND(pData,"portinact-md:%d trgtMd:%d>",sm->mode, sm->configParamList.targetMode);
        	/* No communication */
        	if(sm->mode == IOL_MDH_MODE_STARTUP){
        		/* T1: state 0 to 1*/
        		DBG_SPRINT_APPEND(pData,"Statup>");
        		sm->compRetry = 0;
        		sm->state = IOL_SM_STATE_CHECKCOMPA_1;
        	}else if(sm->configParamList.targetMode != IOL_TARGET_MODE_INACTIVE){
        		/* T14 or T15 or T16: state 0 to 9*/
//        		DBG_SPRINT_APPEND(pData,"mdInact>");
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_9;
        	}
            break;
        case IOL_SM_STATE_CHECKCOMPA_1:
        	/* Port is started and revision and Device compatibility is checked. See Figure 72.*/
        	/* Jump to the submechine state ReadComParameter_20 */
        	DBG_SPRINT_APPEND(pData,"CHKCMPA>");
			sm->rwStatus = IOL_RW_STATUS_NONE;
        	sm->state = IOL_SM_STATE_READCOMPARAM_20;
            break;
        case IOL_SM_STATE_WAITONDLPREOP_2:
        	/* Wait until the PREOPERATE state is established and all the On-Request handlers are
        	 * started. Port is ready to communicate.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 2 to 0
        		 * submachine transition state 20 to 25
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;	// no in spec
        	}else if(sm->mode == IOL_MDH_MODE_PREOPERATE){
        		/* T8: state 2 to 3*/
        		/*TODO: should I check for all the On-Request handlers are started or not*/
        		sm->state = IOL_SM_STATE_CHECKSERNUM_3;
        	}
            break;
        case IOL_SM_STATE_CHECKSERNUM_3:
        	/* SerialNumber is checked depending on the InspectionLevel (IL). See Figure 77. */
			sm->rwStatus = IOL_RW_STATUS_NONE;
			sm->status = IOL_SM_STATUS_NONE;
        	sm->state = IOL_SM_STATE_READSERNUM_30;
            break;
        case IOL_SM_STATE_WAIT_4:
        	/* Port is ready to communicate and waits on service SM_Operate from CM.*/
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 4 to 0
        		 * T3 Joint transition state 4 to 32
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;	// no in spec
        	}else if(sm->event == IOL_SM_EVENT_SETPORTCONFIG){
        		/* T14 or T15 or T16: state 0 to 9*/
        		sm->event = IOL_SM_EVENT_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_9;
        	}else if(sm->event == IOL_SM_EVENT_OPERATE){		// event for SM_Operate device call from configuration manager
        		/*TODO: need to understand the implementation is ok*/
        		IOL_ModeValueList_t vl = {0};
				SM_PrepareValuelist(&vl, sm->comParamList.mseqCapb, sm->configParamList.revId,
									sm->comParamList.pdIn, sm->comParamList.pdOut,
									sm->configParamList.cycleTime, IOL_MDH_MODE_OPERATE);
				IOL_DL_SetMode_Req(port, IOL_MDH_MODE_OPERATE, &vl);
        	}

            break;
        case IOL_SM_STATE_SMOPERATE_5:
        	/* Port is in state OPERATE and performs cyclic Process Data exchange.*/
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 4 to 0
        		 * T3 Joint transition state 4 to 32
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;	// no in spec
        	}else if(sm->event == IOL_SM_EVENT_SETPORTCONFIG){
        		/* T14 or T15 or T16: state 0 to 9*/
        		sm->event = IOL_SM_EVENT_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_9;
        	}else{
        		/*TODO: incomplete. need to understand*/
        		if(sm->mode == IOL_MDH_MODE_OPERATE){

        		}
        	}
            break;
        case IOL_SM_STATE_INSPECFAULT_6:
        	/* Port is ready to communicate. However, cyclic Process Data exchange cannot be
        	 * performed due to incompatibilities.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 6 to 0
        		 * T# joint transition state 6 to 32
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;// no in spec
        	}else{
        		/*TODO: \. need to understand*/
        		if(sm->event == IOL_SM_EVENT_SETPORTCONFIG){
            		/* T14 or T15 or T16: state 6 to 9*/
            		sm->event = IOL_SM_EVENT_NONE;
            		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_9;
        		}
        	}
            break;
        case IOL_SM_STATE_WAITONDLOPERATE_7:
        	/* Wait on the requested state OPERATE in case the Master is connected to a legacy
        	 * Device. The SerialNumber can be read thereafter.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 6 to 0
        		 * T# joint transition state 7 to 32
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;// no in spec
        	}else if(sm->mode == IOL_MDH_MODE_OPERATE){
				/* T9: state 7 to 3
				 *
				 */
        		/*TODO: need to understand*/
				sm->state = IOL_SM_STATE_CHECKSERNUM_3;
			}

            break;
        case IOL_SM_STATE_DIDO_8:
        	/* Port will be switched into the DI or DO mode (SIO, no communication).*/
    		if(sm->event == IOL_SM_EVENT_SETPORTCONFIG){
        		/* T14 or T15 or T16: state 6 to 9*/
    			/*TODO: need to understand*/
        		sm->event = IOL_SM_EVENT_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_9;
    		}
            break;
        case IOL_SM_STATE_JOINPSEUDOSTATE_9:{
        	IOL_ModeValueList_t valueList = {0};
        	/* This pseudo state is used instead of a UML join bar. It allows execution of individual
        	 * SM_SetPortConfig services depending on the system status (INACTIVE, CFGCOM,
        	 * AUTOCOM, DI, or DO)
        	 * */
        	DBG_SPRINT_APPEND(pData,"Jin9>");
        	if(sm->configParamList.targetMode == IOL_TARGET_MODE_CFGCOM ||
        	   sm->configParamList.targetMode == IOL_TARGET_MODE_AUTOCOM)
        	{
        		/* T15: state 0,4,5,6,8 to 0
        		 * 1. DL_SetMode.req (STARTUP, ValueList),
        		 * 2. PL_SetMode.req (SDCI)
        		 * */
        		DBG_SPRINT_APPEND(pData,"AutCom>");
				SM_SetValueList(&valueList, sm->configParamList.cycleTime, IOL_MSEQ_SUB_TYPE_0, 0, 0, 1);
				IOL_DL_SetMode_Req(port, IOL_MDH_MODE_STARTUP, &valueList);
				IOL_PL_SetMode_req(port, IOL_PL_MODE_SDCI);
        		sm->state = IOL_SM_STATE_PORTINACTIVE_0;
        	}else if(	sm->configParamList.targetMode == IOL_TARGET_MODE_DI ||
        				sm->configParamList.targetMode == IOL_TARGET_MODE_DO)
			{
        		/* T16: state 0,4,5,6,8 to 8
        		 * 1. PL_SetMode.req (SIO),
        		 * 2. SM_Mode.ind (DI or DO),
        		 * 3. DL_SetMode.req (INACTIVE)
        		 * */
        		IOL_PL_TargetMode plMode = 0;
        		IOL_SM_PortMode smMode = 0;
        		if(sm->configParamList.targetMode == IOL_TARGET_MODE_DI){
        			plMode = IOL_PL_MODE_DI;
        			smMode = IOL_SM_PORTMODE_DI;
        		}else {
        			plMode = IOL_PL_MODE_DO;
        			smMode = IOL_SM_PORTMODE_DO;
        		}
        		IOL_PL_SetMode_req(port, plMode);
        		IOL_SM_PortMode_Ind(port, sm->configParamList.portNum, smMode);
				IOL_DL_SetMode_Req(port, IOL_MDH_MODE_INACTIVE, &valueList);
        		sm->state = IOL_SM_STATE_DIDO_8;
			}else {
        		/* T14: state 0,4,5,6,8 to 0
        		 * SM_PortMode.ind (INACTIVE), DL_SetMode.req (INACTIVE)
        		 * */
				IOL_SM_PortMode_Ind(port, sm->configParamList.portNum, IOL_SM_PORTMODE_INACTIVE);
				IOL_DL_SetMode_Req(port, IOL_MDH_MODE_INACTIVE, &valueList);
				sm->state = IOL_SM_STATE_PORTINACTIVE_0;
			}
        	}break;
        case IOL_SM_STATE_READCOMPARAM_20:
        	/* Acquires communication parameters from Direct Parameter Page 1 (0x02 to 0x06) via
        	 * service DL_Read (see Table B.1)
        	 * */
        	DBG_SPRINT_APPEND(pData,"RCMPRM20>");
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 1 to 0
        		 * submachine transition state 20 to 25
        		 * */
        		DBG_SPRINT_APPEND(pData,"COMLOST>");
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
        	}else{
        		DBG_SPRINT_APPEND(pData,"COMOK>");
				SM_ReadParamBatch(port, IOL_DIR_PARM_ADDR_MIN_CYCL_TIM, IOL_DIR_PARM_ADDR_PDO);
				if(sm->rwStatus == IOL_RW_STATUS_ALL_DONE){
					if(sm->realParamList.revId == IOL_PROTOCOL_REV_ID_V10){
						/* V10, The legacy protocol version 1.0
						 * Real protocol revision of connected Device is a legacy version
						 * T20: state 20 to 21
						 * */
						sm->rwStatus = IOL_RW_STATUS_NONE;
						sm->state = IOL_SM_STATE_CHECKCOMPV10_21;
					}else if(sm->realParamList.revId != IOL_PROTOCOL_REV_ID_V10){
						/* <>V10 (Not v10), This revision of the standard specifies protocol version 1.1.
						 * Real protocol revision of connected Device is in accordance with this standard
						 * T21: state 20 to 22:
						 * DL_Write (0x00, MCmd_MASTERIDENT), see Table B.2
						 */
						sm->paramAddress = IOL_DIR_PARM_ADDR_MASTER_CMD;
						IOL_DL_Write_Req(port, sm->paramAddress, IOL_MSTR_CMD_MSTR_IDENT);
						sm->rwStatus = IOL_RW_STATUS_NONE;
						sm->state = IOL_SM_STATE_CHECKVXY_22;
					}
				}
        	}
            break;
        case IOL_SM_STATE_CHECKCOMPV10_21:
        	/* Acquires identification parameters from Direct Parameter Page 1 (0x07 to 0x0D) via
        	 * service DL_Read (see Table B.1). The configured InspectionLevel (IL) defines the
        	 * decision logic of the subsequent compatibility check "CheckCompV10" with parameters
        	 * RVID, RDID, and RFID according to Figure 74.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 1 to 0
        		 * submachine transition state 21 to 25
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
        	}else{
				SM_ReadParamBatch(port, IOL_DIR_PARM_ADDR_VEND_ID_MSB, IOL_DIR_PARM_ADDR_FUNC_ID_LSB);
				if(sm->rwStatus == IOL_RW_STATUS_ALL_DONE){
					IOL_ModeValueList_t valueList = {0};
					sm->rwStatus = IOL_RW_STATUS_NONE;
					if( sm->configParamList.cycleTime > 0 &&
						sm->configParamList.cycleTime < sm->realParamList.cycleTime)
					{
						/* D3 > T18: state 1 to 6 > V10CycTimeFault
						 * SM_PortMode.ind (CYCTIME_FAULT), DL_SetMode.req (INACTIVE)
						 * */
						IOL_SM_PortMode_Ind(port, sm->configParamList.portNum,
											IOL_SM_PORTMODE_CYCTIME_FAULT);
						IOL_DL_SetMode_Req(port, IOL_MDH_MODE_INACTIVE, &valueList);
						sm->status = IOL_SM_STATUS_V10CYCTIMEFAULT;
						sm->state = IOL_SM_STATE_INSPECFAULT_6;
					}else if(sm->configParamList.inspecLevel == IOL_INSPEC_LEVEL_NO_CHECK){
						/* D4 > T4: state 1 to 7 > V10CompOK
						 * DL_SetMode.req (OPERATE, ValueList)
						 * */
						SM_PrepareValuelist(&valueList, sm->comParamList.mseqCapb,
											sm->realParamList.revId, sm->comParamList.pdIn,
											sm->comParamList.pdOut,	sm->realParamList.cycleTime,
											IOL_MDH_MODE_OPERATE);
						IOL_DL_SetMode_Req(port, IOL_MDH_MODE_OPERATE, &valueList);
						sm->status = IOL_SM_STATUS_V10COMPOK;
						sm->state = IOL_SM_STATE_WAITONDLOPERATE_7;
					}else if(sm->configParamList.vendId == sm->realParamList.vendId &&
							 sm->configParamList.devId == sm->realParamList.devId &&
							 sm->configParamList.revId == sm->realParamList.revId)
					{
						/* D5 > T4: state 1 to 7 > V10CompOK
						 * DL_SetMode.req (OPERATE, ValueList)
						 * */
						SM_PrepareValuelist(&valueList, sm->comParamList.mseqCapb,
											sm->realParamList.revId, sm->comParamList.pdIn,
											sm->comParamList.pdOut,	sm->realParamList.cycleTime,
											IOL_MDH_MODE_OPERATE);
						IOL_DL_SetMode_Req(port, IOL_MDH_MODE_OPERATE, &valueList);
						sm->status = IOL_SM_STATUS_V10COMPOK;
						sm->state = IOL_SM_STATE_WAITONDLOPERATE_7;
					}else{
						/* D5 > T5: state 1 to 6 > V10CompFault
						 * SM_PortMode.ind (COMP_FAULT), DL_SetMode.req (OPERATE, ValueList)
						 * */
						IOL_SM_PortMode_Ind(port, sm->configParamList.portNum,
											IOL_SM_PORTMODE_COMP_FAULT);
						SM_PrepareValuelist(&valueList, sm->comParamList.mseqCapb,
											sm->realParamList.revId, sm->comParamList.pdIn,
											sm->comParamList.pdOut,	sm->configParamList.cycleTime,
											IOL_MDH_MODE_OPERATE);
						IOL_DL_SetMode_Req(port, IOL_MDH_MODE_OPERATE, &valueList);
						sm->status = IOL_SM_STATUS_V10COMPFAULT;
						sm->state = IOL_SM_STATE_INSPECFAULT_6;
					}
				}
        	}
            break;
        case IOL_SM_STATE_CHECKVXY_22:
        	/* A check is performed whether the configured revision (CRID) matches the real (actual)
        	 * revision (RRID) according to Figure 73
        	 * */
			if(sm->mode == IOL_MDH_MODE_COMLOST){
				/* T3: state 1 to 0
				 * submachine transition state 22 to 25
				 * */
				sm->rwStatus = IOL_RW_STATUS_NONE;
				sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
			}else{
        		if(sm->configParamList.revId == sm->realParamList.revId){
					/* D1 > T22: state 20 to 23 > RevisionOK */
        			sm->status = IOL_SM_STATUS_REVISIONOK;
        			sm->state = IOL_SM_STATE_CHECKCOMP_23;
        		}else{
        			if(sm->compRetry == 0){
        				/* D2(+) > T25: state 22 to 24 > RetryStartup */
        				sm->compRetry++;
        				sm->writeDone = 0;
        				sm->status = IOL_SM_STATUS_RETRYSTARTUP;
        				sm->state = IOL_SM_STATE_RESTARTDEVICE_24;
					}else if(sm->compRetry == 1){
						/* D1(-) > T6: state 1 to 6 > RevisionFault
						 * SM_PortMode.ind (REVISION_FAULT), DL_SetMode.req (PREOPERATE, ValueList)
						 * */
						IOL_ModeValueList_t valueList = {0};
						IOL_SM_PortMode_Ind(port, sm->configParamList.portNum,
											IOL_SM_PORTMODE_REVISION_FAULT);
						SM_PrepareValuelist(&valueList, sm->comParamList.mseqCapb,
											sm->realParamList.revId, sm->comParamList.pdIn,
											sm->comParamList.pdOut,	sm->configParamList.cycleTime,
											IOL_MDH_MODE_PREOPERATE);
						IOL_DL_SetMode_Req(port, IOL_MDH_MODE_PREOPERATE, &valueList);
						sm->status = IOL_SM_STATUS_REVISIONFAULT;
						sm->state = IOL_SM_STATE_INSPECFAULT_6;
					}
        		}
        	}
			//TODO: incomplete, implement later
            break;
        case IOL_SM_STATE_CHECKCOMP_23:
        	/* Acquires identification parameters from Direct Parameter Page 1 (0x07 to 0x0D) via
        	 * service DL_Read (see Table B.1). The configured InspectionLevel (IL) defines the decision logic of the subsequent compatibility check "CheckComp" according to Figure 75.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 1 to 0
        		 * submachine transition state 23 to 25
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
        	}else{
				SM_ReadParamBatch(port, IOL_DIR_PARM_ADDR_VEND_ID_MSB, IOL_DIR_PARM_ADDR_FUNC_ID_LSB);
				if(sm->rwStatus == IOL_RW_STATUS_ALL_DONE){
					sm->rwStatus = IOL_RW_STATUS_NONE;
					SM_CheckComp(port);
				}
        	}
            break;
        case IOL_SM_STATE_RESTARTDEVICE_24:
        	/*Writes the compatibility parameters configured protocol revision (CRID) and configured
        	 * DeviceID (CDID) into the Device depending on the Target Mode of communication
        	 * CFGCOM or AUTOCOM (see Table 81) according to Figure 76.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 1 to 0
        		 * submachine transition state 24 to 25
        		 * */
        		sm->rwStatus = IOL_RW_STATUS_NONE;
        		sm->writeDone = 0;
        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
        	}else{
        		SM_RestartDevice(port);
        		if(sm->writeDone){
            		sm->rwStatus = IOL_RW_STATUS_NONE;
            		sm->writeDone = 0;
            		sm->paramAddress = 0;
            		sm->state = IOL_SM_STATE_READCOMPARAM_20;
        		}
        	}
            break;
        case IOL_SM_STATE_JOINPSEUDOSTATE_25:
        	/* This pseudo state is used instead of a UML join bar. No guards involved*/
    		/* T3: state 1 to 0;  submachine transition state 25 to enex_1
    		 * DL_SetMode.req (INACTIVE) and SM_Mode.ind (COMLOST) due to
    		 * communication fault
    		 * */
        	sm->state = IOL_SM_STATE_T3JOINPSEUDSTATE_32;	// not in spec
        	break;
        case IOL_SM_STATE_READSERNUM_30:
        	/* Acquires the SerialNumber from the Device via AL_Read.req (Index: 0x0015). A
        	 * positive response (AL_Read(+)) leads to SReadOK = true. A negative response
        	 * (AL_Read(-)) leads to SRead- = true.
        	 * */
        	if(sm->mode == IOL_MDH_MODE_COMLOST){
        		/* T3: state 1 to 0
        		 * submachine transition state 30 to enex_12
        		 * */

        		sm->state = IOL_SM_STATE_JOINPSEUDOSTATE_25;
        	}else{
        		/* TODO: Acquires the SerialNumber from the Device via AL_Read.req (Index: 0x0015).
        		 * */
//        		IOL_AL_Read_Req(port, sm->configParamList.portNum,
//        				IOL_DEV_PARM_INDX_SERIAL_NUMBER, 0, 0);
        		if(sm->status == IOL_SM_STATUS_SREADOK){
        			/* T30: state 30 to 31*/
        			sm->state = IOL_SM_STATE_CHECKSERNUM_31;
        		}else if(sm->rwStatus != IOL_RW_STATUS_NONE){ // SRead not ok
        			/* T31: state 30 to 31*/
        			sm->state = IOL_SM_STATE_CHECKSERNUM_31;
        		}
        	}
            break;
        case IOL_SM_STATE_CHECKSERNUM_31:
        	/*Optional: SerialNumber checking skipped or checked correctly*/
        	if(sm->configParamList.inspecLevel != IOL_INSPEC_LEVEL_IDENTICAL){
        		/* D11 > T10: state 3 to 4
        		 * submachine transition state 31 to enex_11 > SerNumOK
        		 * SM_PortMode.ind (COMREADY)
        		 * */
        		sm->status = IOL_SM_STATUS_SERNUMOK;
        		sm->state = IOL_SM_STATE_WAIT_4;
        	}else if (sm->status == IOL_SM_STATUS_SREADNOTOK) {
        		/* D11 > T11: state 3 to 6
        		 * submachine transition state 31 to enex_10 > SerNumFault
        		 * SM_PortMode.ind (SERNUM_FAULT)
        		 * */
        		sm->status = IOL_SM_STATUS_SERNUMFAULT;
        		sm->state = IOL_SM_STATE_INSPECFAULT_6;
			}else if(sm->configParamList.SrlNum != sm->realParamList.SrlNum){
				/* D11 > T11: state 31 to enex_10 > SerNumFault */
        		/* D11 > T11: state 3 to 6
        		 * submachine transition state 31 to enex_10 > SerNumFault
        		 * SM_PortMode.ind (SERNUM_FAULT)
        		 * */
        		sm->status = IOL_SM_STATUS_SERNUMFAULT;
        		sm->state = IOL_SM_STATE_INSPECFAULT_6;
			}else{	// serial number matched
        		/* D11 > T10: state 3 to 4
        		 * submachine transition state31 to enex_11 > SerNumOK
        		 * SM_PortMode.ind (COMREADY)
        		 * */
        		sm->status = IOL_SM_STATUS_SERNUMOK;
        		sm->state = IOL_SM_STATE_WAIT_4;
			}
			if(sm->state != IOL_SM_STATE_CHECKSERNUM_31){
				IOL_SM_PortMode_Ind(port, sm->configParamList.portNum,
									(sm->status == IOL_SM_STATUS_SERNUMOK ?
									IOL_SM_PORTMODE_COMREADY :
									IOL_SM_STATUS_SERNUMFAULT));
			}
            break;
        case IOL_SM_STATE_T3JOINPSEUDSTATE_32:{
        	/*Not in spec, I made it to combine all T3 transaction*/
			IOL_ModeValueList_t valueList = {0};
			IOL_DL_SetMode_Req(port, IOL_MDH_MODE_INACTIVE, &valueList);
			IOL_SM_PortMode_Ind(port, sm->configParamList.portNum, IOL_SM_PORTMODE_COMLOST);
			sm->state = IOL_SM_STATE_PORTINACTIVE_0;
        	}break;
        default:
            break;
    }
//    DBG_PRINT(pData);
}



