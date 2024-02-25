/*
 * IOLink_MA.c
 *
 *  Created on: Dec 18, 2023
 *      Author: wsrra
 */

#include "IOLink_MA.h"
#include "IOLink_SMI.h"
#include "IOLink_SM.h"
#include "IOLink_AL.h"
#include <string.h>
#include <stdlib.h>
#include "Debug.h"

/*Private (internal) services prototype----------------------------*/
inline static void DS_Startup(IOL_Port_t *port);
inline static void DS_Ready(IOL_Port_t *port);
inline static void DS_Fault(IOL_Port_t *port, IOL_PortEventCodes eventCode);
inline static void DS_Delete(IOL_Port_t *port);
inline static void DS_Change(IOL_Port_t *port);
inline static void DS_Upload(IOL_Port_t *port);
inline static void OD_Start(IOL_Port_t *port);
inline static void OD_Stop(IOL_Port_t *port);
inline static void OD_Block(IOL_Port_t *port);
inline static void OD_Unblock(IOL_Port_t *port);
inline static void DU_Start(IOL_Port_t *port);
inline static void DU_Stop(IOL_Port_t *port);
inline static IOL_Errors PD_Start(IOL_Port_t *port);
inline static IOL_Errors PD_Stop(IOL_Port_t *port);

/*non-specified Internal services-----------------------*/
inline static void MA_EventObjToArray(IOL_ArgBlockID argBlockId, IOL_EventX_t* event, uint8_t* array);
inline static void MA_DS_DeleteParams(IOL_Port_t *port);
inline static void MA_DS_CheckBackupRestoreState(IOL_Port_t *port);
/*Public services---------------------------------------*/
/*Configuration manager------------*/

/*Data Storage---------------------*/


/*Private (internal) services---------------------------------------*/
/* Data Storage Services---------*/
/* @ref 11.3 Configuration Manager (CM) Table 125 – Internal variables
 * and Events controlling Master applications
 */

/* @brief This variable indicates the Data Storage has been accomplished successfully;
operating mode is CFGCOM or AUTOCOM (see 9.2.2.2)
 * */
inline static void DS_Startup(IOL_Port_t *port){
	IOL_MA_DS_t *ds = ((IOL_MA_DS_t *)IOL_GetDSInstance(port));
	ds->trigger = IOL_DS_TRIG_STARTUP;
	//TODO: Should ds state machine be called
}

/* @brief This variable indicates the Data Storage has been aborted due to a fault.
 * */
inline static void DS_Ready(IOL_Port_t *port){
	IOL_MA_CM_t *cm = ((IOL_MA_CM_t *)IOL_GetCMInstance(port));
	cm->trigger = IOL_CM_TRIG_DS_READY;

	//TODO: Should CM state machine be called
}

/* @brief Any verified change of Device configuration leads to a deletion of the stored
data set in the Data Storage.
 * */
inline static void DS_Fault(IOL_Port_t *port, IOL_PortEventCodes eventCode){
	IOL_MA_t *ma = ((IOL_MA_t *)IOL_GetMAInstance(port));
	ma->cm.trigger = IOL_CM_TRIG_DS_FAULT;
	ma->cm.portEvent.eventCode = eventCode;
	ma->cm.portEvent.eventQual.field.instance = IOL_EVENTINST_APPLICATION;
	ma->cm.portEvent.eventQual.field.source = IOL_EVENTSRC_MASTER;
	ma->cm.portEvent.eventQual.field.type = IOL_EVENTTYPE_ERROR;
	ma->cm.portEvent.eventQual.field.mode = IOL_EVENTMODE_APPEARS;

	//TODO: Should CM state machine be called
}

/* @brief This variable indicates a content change of Data Storage triggered by service
SMI_ParServToDS.
 * */
inline static void DS_Delete(IOL_Port_t *port){
	IOL_MA_DS_t *ds = &((IOL_MA_t *)IOL_GetMAInstance(port))->ds;
	ds->trigger = IOL_DS_TRIG_DELETE;
	//TODO: Should ds state machine be called
}

/* @brief This variable triggers the Data Storage state machine in the Master due to the
special Event "DS_UPLOAD_REQ" from the Device.
 * */
inline static void DS_Change(IOL_Port_t *port){
	IOL_MA_CM_t *cm = ((IOL_MA_CM_t *)IOL_GetCMInstance(port));
	if(cm->reqPortCfgList.portMode == IOL_PORT_MODE_IOL_MANUAL ||
		cm->reqPortCfgList.validBackup == IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_BACKUPRSTOR ||
		cm->reqPortCfgList.validBackup == IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_RSTOR)
	{
		cm->trigger = IOL_CM_TRIG_DS_CHANGE;
	}
}

/* @brief This variable enables On-request Data access via AL_Read and AL_Write.
 * */
inline static void DS_Upload(IOL_Port_t *port){
	((IOL_MA_DS_t *)IOL_GetDSInstance(port))->trigger = IOL_DS_TRIG_UPLOAD;
	IOL_MA_DSHandler(port);
}

/* @brief This variable enables On-request Data access via AL_Read and AL_Write.
 * */
inline static void OD_Start(IOL_Port_t *port){
	((IOL_MA_ODE_t *)IOL_GetODEInstance(port))->trigger = IOL_ODE_TRIG_OD_UNBLOCK;
	IOL_MA_ODEHandler(port);
}

/* @brief This variable indicates that On-request Data access via AL_Read and AL_Write
is acknowledged with a negative response to the gateway application.
 * */
inline static void OD_Stop(IOL_Port_t *port){
	((IOL_MA_ODE_t *)IOL_GetODEInstance(port))->trigger = IOL_ODE_TRIG_OD_UNBLOCK;
	IOL_MA_ODEHandler(port);
}

/* @brief Data Storage upload and download actions disable the On-request Data access
through AL_Read or AL_Write. Access by the gateway application is denied.
 * */
inline static void OD_Block(IOL_Port_t *port){
	((IOL_MA_ODE_t *)IOL_GetODEInstance(port))->trigger = IOL_ODE_TRIG_OD_BLOCK;
	IOL_MA_ODEHandler(port);
}

/* @brief This variable enables On-request Data access via AL_Read or AL_Write.
 * */
inline static void OD_Unblock(IOL_Port_t *port){
((IOL_MA_ODE_t *)IOL_GetODEInstance(port))->trigger = IOL_ODE_TRIG_OD_UNBLOCK;
	IOL_MA_ODEHandler(port);
}

/* @brief This variable enables the Diagnosis Unit to propagate remote (Device) Events
to the gateway application.
 * */
inline static void DU_Start(IOL_Port_t *port){
	((IOL_MA_DU_t *)IOL_GetDUInstance(port))->trigger = IOL_DU_TRIG_START;
	IOL_MA_DUHandler(port);
}

/* @brief This variable indicates that the Device Events are not propagated to the
gateway application and not acknowledged. Available Events are blocked until
the DU is enabled again.
 * */
inline static void DU_Stop(IOL_Port_t *port){
	((IOL_MA_DU_t *)IOL_GetDUInstance(port))->trigger = IOL_DU_TRIG_STOP;
	IOL_MA_DUHandler(port);
}

/* @brief This variable enables the Process Data exchange with the gateway application.
 * */
inline static IOL_Errors PD_Start(IOL_Port_t *port){
	IOL_MA_PDE_t* pde = ((IOL_MA_PDE_t *)IOL_GetPDEInstance(port));

	pde->trigger = IOL_PDE_TRIG_START;
	/*Check, if the previous allocated memory is not free, free the memory first*/
	/*Allocate memory for pdIn*/
	if(pde->pdIn != NULL){
		pde->pdIn = (uint8_t *)malloc(IOL_MAX_PD_SIZE*sizeof(uint8_t));
		return IOL_ERROR_NONE;
	}
	/*Allocate memory for pdOut*/
	if(pde->pdOut != NULL){
		pde->pdOut = (uint8_t *)malloc(IOL_MAX_PD_SIZE*sizeof(uint8_t));
		return IOL_ERROR_NONE;
	}
	return IOL_ERROR_NULL_POINTER;
}

/* @brief This variable disables the Process Data exchange with the gateway application.
 * */
inline static IOL_Errors PD_Stop(IOL_Port_t *port){
	/* 1. set the trigger
	 * 2. d.eallocate memory
	 * */
	IOL_MA_PDE_t* pde = ((IOL_MA_PDE_t *)IOL_GetPDEInstance(port));
	pde->trigger = IOL_PDE_TRIG_STOP;
	/*Deallocate memory for pdIn*/
	free(pde->pdIn);
	pde->pdIn = NULL;
	/*Deallocate memory for pdOut*/
	free(pde->pdOut);
	pde->pdOut = NULL;
	return IOL_ERROR_NONE;
}

/*Services to receive confirmation from other layers----------------------*/
/*SMI-------------*/
/* @brief Response receive service from IOL__SMI_DeviceEvent_Rsp service
 * */
//IOL_ErrorTypes IOL_MA_SMI_DeviceEvent_Rsp( uint8_t clientId, uint8_t portNumber,
//	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock){
//
//}
//
///* @brief Response receive service from IOL_SMI_PortEvent_Rsp service
// * */
//IOL_ErrorTypes IOL_MA_SMI_PortEvent_Rsp( uint8_t clientId, uint8_t portNumber,
//	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock){
//
//}


/*AL servicess----*/
/* @brief Response receive service from IOL_AL_Read_Cnf service
 * */
void IOL_CM_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len,IOL_ErrorTypes errorInfo){
	//TODO: to implement later

}

void IOL_ODE_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len, IOL_ErrorTypes errorInfo){
	//TODO: to implement later

}

void IOL_DS_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len, IOL_ErrorTypes errorInfo){
	//TODO: to implement later
	IOL_MA_DS_t* ds = (IOL_MA_DS_t*) IOL_GetDSInstance(port);
	ds->errInfALCnf = errorInfo;



	if(errorInfo != IOL_ERRTYP_NONE){
		if(errorInfo == IOL_ERRTYPDERIEVD_COMM_ERR){
			ds->trigComX = IOL_DS_TRIG_COMX_ERROR;
		}
		return;
	}
	if( ds->currentIndex == IOL_DEV_PARM_INDX_DATA_STORAGE){
		switch (ds->currentSubindex) {
			case IOL_DEV_DS_SUBINDX_DS_SIZE:
				ds->devDSConfig.storgSize = IOL_GET4BYTES(data, sizeof(ds->devDSConfig.storgSize));
				break;
			case IOL_DEV_DS_SUBINDX_STATE_PROPERTY:
				ds->devDSConfig.statPrpty.octet = data[0];
				break;
			case IOL_DEV_DS_SUBINDX_DS_CMD:
				ds->devDSConfig.cmd = data[0];
				break;
			case IOL_DEV_DS_SUBINDX_PARM_CHECKSUM:
				ds->devDSConfig.parmChksum = IOL_GET4BYTES(data, sizeof(ds->devDSConfig.parmChksum));
				break;
			case IOL_DEV_DS_SUBINDX_INDEX_LIST:
				ds->devDSConfig.ilCount = len/3;	//3 bytes in each indexList item
				memcpy(ds->devDSConfig.indexList, data, len);
				break;
			default:
				break;
		}
	}else{
		ds->alReadCnf.len = len;
		memcpy(ds->alReadCnf.data, data, len);
	}
	ds->trigSrvic = IOL_DS_TRIG_SRVC_ALREAD_CNF;

}

/* @brief Response receive service from IOL_AL_Write_Cnf service
 * */
void IOL_CM_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo){
	//TODO: to implement later
}

void IOL_DS_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo){
	IOL_MA_DS_t* ds = (IOL_MA_DS_t*) IOL_GetDSInstance(port);
	ds->errInfALCnf = errorInfo;
	switch (errorInfo) {
		case IOL_ERRTYPDERIEVD_COMM_ERR:
			ds->trigComX = IOL_DS_TRIG_COMX_ERROR;
			break;
		default:
			break;
	}
	ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_CNF;
}

/* @brief Response receive service from IOL_AL_Event_Ind service
 * */
IOL_Errors IOL_DU_AL_Event_Ind(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event){
	//TODO1: to propagate the event to the gateway application
	//TODO2: does it need to store event into portStatusList diagEntry
	IOL_MA_DU_t* du = (IOL_MA_DU_t*)IOL_GetDUInstance(port);
	du->eventCount = eventCount;
	memcpy(du->events, event, eventCount*sizeof(IOL_EventX_t));
	du->trigger = IOL_DU_TRIG_AL_EVENT_IND;
	return IOL_ERROR_NONE;
}



/*Response receive service from SM cnf service--------------------*/
void IOL_CM_SM_PortMode_Ind(IOL_Port_t *port, uint8_t portNum, IOL_SM_PortMode mode){
	IOL_MA_CM_t *cm = &((IOL_MA_t *)IOL_GetMAInstance(port))->cm;
	switch (mode) {
		case IOL_SM_PORTMODE_COMREADY:
			cm->trigger = IOL_CM_TRIG_SM_COMREADY;
			break;
		case IOL_SM_PORTMODE_COMLOST:
			cm->trigger = IOL_CM_TRIG_SM_COMLOST;
			break;
		case IOL_SM_PORTMODE_REVISION_FAULT:
		case IOL_SM_PORTMODE_COMP_FAULT:
		case IOL_SM_PORTMODE_SERNUM_FAULT:
			cm->trigger = IOL_CM_TRIG_SM_FAULT;
			break;
		default:
			break;
	}
}

/*Others internal services-------------------*/
/*@brief coverts event object to argBlock array for SMI_xEvent services
 * */
inline static void MA_EventObjToArray(IOL_ArgBlockID argBlockId, IOL_EventX_t* event, uint8_t* array){
	array[IOL_ARGPOS_ARGBLOCKID_U16] = (uint8_t)(argBlockId>>8);
	array[IOL_ARGPOS_ARGBLOCKID_U16+1] = (uint8_t)(argBlockId);
	array[IOL_ARGPOS_EVENT_EVENTQUAL_U8] = event->eventQual.octet;
	array[IOL_ARGPOS_EVENT_EVENTCODE_U16] = (uint8_t)(event->eventCode>>8);
	array[IOL_ARGPOS_EVENT_EVENTCODE_U16+1] = (uint8_t)(event->eventCode);
}

/* Delete DS saved parameter set
 * */
inline static void MA_DS_DeleteParams(IOL_Port_t *port){
//	IOL_MA_DS_t *ds = (IOL_MA_DS_t*)IOL_GetDSInstance(port);
	//TODO: delete the Device parameters from the data storage
}

/* Check the backup and restore status
 * Ref: 11.4.4 DS state machine, line-4290 to 4292
 * TODO: Need to be more clear about this checking
 * */
inline static void MA_DS_CheckBackupRestoreState(IOL_Port_t *port){
	//,
	IOL_MA_t* ma = (IOL_MA_t*)IOL_GetMAInstance(port);
	IOL_MA_DS_t *ds = &ma->ds;
	IOL_MA_CM_t *cm = &ma->cm;
	switch (cm->reqPortCfgList.validBackup) {
		case IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_BACKUPRSTOR:
		case IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_RSTOR:
			ds->trigger = IOL_DS_TRIG_ENABLED;
			break;
		case IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1:
			ds->trigger = IOL_DS_TRIG_CLEARED;
			break;
		default:
			ds->trigger = IOL_DS_TRIG_DISABLED;
			break;
	}
}


/*State machines----------------------------------------*/
/*State machine for configuration manager*/

void IOL_MA_CMHandler(IOL_Port_t *port){

    IOL_MA_t *ma = (IOL_MA_t *)IOL_GetMAInstance(port);
    IOL_MA_CM_t *cm = &((IOL_MA_t *)IOL_GetMAInstance(port))->cm;
    IOL_SM_ParamList_t paramList = {0};
	IOL_SMI_ArgBlock_PortStatusList_t *pStatList;
	pStatList = &cm->portStatusList;
//    IOL_MA_CMTrigEvent trgEVnt = cm->trgEvnt;
//    IOL_Port_Info_t *portInfo = NULL;
//    IOL_GetPortInfo(port, portInfo);
	char pData[100] = {0};
	DBG_SPRINT_APPEND(pData,"\r\nCM>");
    switch (cm->state)
    {
    case IOL_MA_CM_STATE_CHECKPORTMODE_0:{
    	/*Check "Port Mode" element in parameter "PortConfigList" (see 11.2.5)*/
    	uint8_t delete = 1;
    	switch (cm->reqPortCfgList.portMode) {
			case IOL_PORT_MODE_IOL_AUTOSTART:
				/* T2: state 0 to 7
				 * 1. Invoke DS-Delete.
				 * 2. Invoke SM_SetPortConfig_AUTOCOM
				 * 3. goto state 2
				 * */
				DBG_SPRINT_APPEND(pData,"AtoStat>");

				/*2.  Invoke SM_SetPortConfig_AUTOCOM.*/
				paramList.targetMode = IOL_TARGET_MODE_AUTOCOM;
				/*3. goto state 2*/
				cm->state = IOL_MA_CM_STATE_CONFIGMANAGER_7;
				break;
			case IOL_PORT_MODE_IOL_MANUAL:
				/* T1: state 0 to 7
				 * 1. Invoke DS-Delete if identification (VendorID, DeviceID) within DS is
				 *    different to configured port identification.
				 * 2. SM_SetPortConfig_CFGCOM
				 * 3. goto state 7
				 * */
				/*1. Invoke DS-Delete. */
				DBG_SPRINT_APPEND(pData,"MANUAL>");
				if( cm->reqPortCfgList.vendorID == ma->ds.storedDataSet.vendorId &&
					cm->reqPortCfgList.deviceID == ma->ds.storedDataSet.deviceId )
				{
					delete = 0;
				}
				/*2. SM_SetPortConfig_DO.*/
				paramList.targetMode = IOL_TARGET_MODE_CFGCOM;
				/*3. goto state 7*/
				cm->state = IOL_MA_CM_STATE_CONFIGMANAGER_7;
				break;
			case IOL_PORT_MODE_DEACTIVATED:
				/* T11: state 0 to 8
				 * 1. Invoke DS-Delete.
				 * 2. Invoke SM_SetPortConfig_INACTIVE.
				 * 3. Update parameter elements of "PortStatusList":
				 *    - PortStatusInfo = DEACTIVATED
				 *    - RevisionID = 0
				 *    - Transmission rate = 0
				 *    - VendorID = 0
				 *    - DeviceID = 0
				 *    - Port QualityInfo = invalid (0), valid (1)
				 * 4. Delete DiagEntries
				 * 5. goto state 8
				 * */
				DBG_SPRINT_APPEND(pData,"Deact>");
				/*2. SM_SetPortConfig_INACTIVE.*/
				paramList.targetMode = IOL_TARGET_MODE_INACTIVE;
				/*3. Update parameter elements of "PortStatusList":*/
				pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_DEACTIVATED;
				/*4. Delete DiagEntries*/
				pStatList->numOfDiags = 0;
				memset(pStatList->diagEntries, 0, sizeof(pStatList->diagEntries));
				/*5. goto state 8*/
				cm->state = IOL_MA_CM_STATE_PORT_DEACTIVATED_8;
				break;
			case IOL_PORT_MODE_DI_CQ:
				/* T9: state 0 to 8
				 * 1. Invoke DS-Delete.
				 * 2. Invoke SM_SetPortConfig_DI.
				 * 3. Update parameter elements of "PortStatusList":
				 *    - PortStatusInfo = DI_C/Q
				 *    - RevisionID = 0
				 *    - Transmission rate = 0
				 *    - VendorID = 0
				 *    - DeviceID = 0
				 *    - Port QualityInfo = invalid (0), valid (1)
				 * 4. Delete DiagEntries
				 * 5. goto state 6
				 * */
				DBG_SPRINT_APPEND(pData,"DICQ>");
				/*2. SM_SetPortConfig_DI.*/
				paramList.targetMode = IOL_TARGET_MODE_DI;
				/*3. Update parameter elements of "PortStatusList":*/
				pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_DI_CQ;
				/*4. Delete DiagEntries*/
				pStatList->numOfDiags = 0;
				memset(pStatList->diagEntries, 0, sizeof(pStatList->diagEntries));
				/*5. goto state 6*/
				cm->state = IOL_MA_CM_STATE_PORT_DIDO_6;
				break;
			case IOL_PORT_MODE_DO_CQ:
				/* T10: state 0 to 6
				 * 1. Invoke DS-Delete.
				 * 2. Invoke SM_SetPortConfig_DO.
				 * 3. Update parameter elements of "PortStatusList":
				 *    - PortStatusInfo = DO_C/Q
				 *    - RevisionID = 0
				 *    - Transmission rate = 0
				 *    - VendorID = 0
				 *    - DeviceID = 0
				 *    - Port QualityInfo = invalid (0), valid (1)
				 * 4. Delete DiagEntries
				 * 5. goto state 6
				 * */
				DBG_SPRINT_APPEND(pData,"DOCQ>");
				/*2. SM_SetPortConfig_DO.*/
				paramList.targetMode = IOL_TARGET_MODE_DO;
				/*3. Update parameter elements of "PortStatusList":*/
				pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_DO_CQ;
				/*4. Delete DiagEntries*/
				pStatList->numOfDiags = 0;
				memset(pStatList->diagEntries, 0, sizeof(pStatList->diagEntries));
				/*5. goto state 6*/
				cm->state = IOL_MA_CM_STATE_PORT_DIDO_6;
				break;

			default:
				/* T16: state 0 to 8
				 * 1. Invoke DS-Delete.
				 * 2. Invoke SM_SetPortConfig_INACTIVE.
				 * 3. Update parameter elements of "PortStatusList":
				 *    - PortStatusInfo = DEACTIVATED
				 *    - RevisionID = 0
				 *    - Transmission rate = 0
				 *    - VendorID = 0
				 *    - DeviceID = 0
				 *    - Port QualityInfo = invalid (0), valid (1)
				 * 4. goto state 8
				 * */
				/*2. SM_SetPortConfig_DO.*/
				paramList.targetMode = IOL_TARGET_MODE_INACTIVE;
				/*3. Update parameter elements of "PortStatusList":*/
				pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_DEACTIVATED;
				/*3. goto state 7*/
				cm->state = IOL_MA_CM_STATE_PORT_DEACTIVATED_8;
				break;
		}

    	if(delete)	DS_Delete(port);							//1. Invoke DS-Delete.
    	IOL_SM_SetPortConfig_Req(port, paramList);
    	if(cm->state != IOL_MA_CM_STATE_CONFIGMANAGER_7){
    		DBG_SPRINT_APPEND(pData,"CnfMngr>");
			pStatList->revisionID = 0;
			pStatList->transRate = 0;
			pStatList->vendorID = 0;
			pStatList->deviceID = 0;
			pStatList->portQualInfo.field.valid = 0;
    	}
    }break;
    case IOL_MA_CM_STATE_CONFIGMANAGER_7:
    	/* This superstate handles Port communication operations and allows all states inside to
    	 * react on COMLOST via SM_PortMode service. A Port restart is managed inside the
    	 * superstate triggered by the DS_Change signal (see Table 125)
    	 * */

    	/* This is the state for ConfigManager substate machine,
    	 * So, go to the first state (STARTUP_1) of the substate machine
    	 * */
    case IOL_MA_CM_STATE_SM_STARTUP_1:
    	/* Waiting on an established communication or loss of communication or any of the faults
    	 * REVISION_FAULT, COMP_FAULT, or SERNUM_FAULT (see Table 85)
    	 * */
//    	DBG_SPRINT_APPEND(pData,"Strtup>");
    	if (cm->trigger == IOL_CM_TRIG_SM_COMREADY){
        	/* T3: state 1 to 2
        	 * 1. DS_Startup: The DS state machine is triggered.
        	 * 2. Update parameter elements of "PortStatusList":
        	 *    - PortStatusInfo = PREOPERATE
        	 *    - RevisionID = (real) RRID
        	 *    - Transmission rate = COMx
        	 *    - VendorID = (real) RVID
        	 *    - DeviceID = (real) RDID
        	 *    - MasterCycleTime = value
        	 *    - Port QualityInfo = invalid
        	 * 3. Goto state 2
        	 */
    		DBG_SPRINT_APPEND(pData,"Comrdy>");
        	/*1. DS_Startup: The DS state machine is triggered.*/
        	DS_Startup(port);

        	/*2. Update parameter elements of "PortStatusList"*/
        	IOL_SM_t *sm = (IOL_SM_t*)IOL_GetSMInstance(port);
        	pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_NOT_AVAILLABLE;
			pStatList->revisionID = sm->realParamList.revId;
			pStatList->vendorID = sm->realParamList.vendId;
			pStatList->deviceID = sm->realParamList.devId;
			pStatList->portQualInfo.field.valid = 0;
			pStatList->masterCycTime =  sm->realParamList.cycleTime;
			if(	sm->realParamList.baudRate != IOL_BAUDRATE_NONE &&
				sm->realParamList.baudRate != IOL_BAUDRATE_AUTO)
			{
				pStatList->transRate = sm->realParamList.baudRate-1;
			}else{
				pStatList->transRate = 0;
			}
        	/*3. Goto state 2*/
            cm->state = IOL_MA_CM_STATE_DS_PARAMMANAGER_2;
        }else if (cm->trigger == IOL_CM_TRIG_SM_FAULT){
        	/* T4: state 1 to 3
        	 * 1. Update parameter elements of "PortStatusList":
        	 * - PortStatusInfo = PORT_DIAG: Launch SMI_PortEvent with real fault
        	 * causing this incident
        	 * - RevisionID = (real) RRID
        	 * - Transmission rate = COMx
        	 * - VendorID = (real) RVID
        	 * - DeviceID = (real) RDID
        	 * - Port QualityInfo = invalid
        	 * 2. Launch SMI_PortEvent with real fault causing this incident
        	 * 3. Goto state 2
        	 * */
        	DBG_SPRINT_APPEND(pData,"FAULT>");
        	/*1. Update parameter elements of "PortStatusList"*/
        	IOL_SM_t *sm = (IOL_SM_t*)IOL_GetSMInstance(port);
        	pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_PORT_DIAG;
			pStatList->revisionID = sm->realParamList.revId;
			pStatList->vendorID = sm->realParamList.vendId;
			pStatList->deviceID = sm->realParamList.devId;
			pStatList->portQualInfo.field.valid = 0;
			pStatList->masterCycTime =  sm->realParamList.cycleTime;
			if(	sm->realParamList.baudRate != IOL_BAUDRATE_NONE &&
				sm->realParamList.baudRate != IOL_BAUDRATE_AUTO)
			{
				pStatList->transRate = sm->realParamList.baudRate-1;
			}else{
				pStatList->transRate = 0;
			}
			/*2. Launch SMI_PortEvent with real fault causing this incident*/
			//TODO: incomplete
//			IOL_SMI_PortEvent_Ind(clientId, portNumber, expArgBlockId, argBlockLen, argBlock);
			/*3. Goto state 3*/
            cm->state = IOL_MA_CM_STATE_PORTFAULT_3;
        }else if(cm->trigger == IOL_CM_TRIG_SM_COMLOST ||
        		 cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED )
        {
        	DBG_SPRINT_APPEND(pData,"ComlostT(6)/Cngd(14): %d>",  cm->trigger);
        	/* T8 or T13: state 1 to 0 */
        	cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
        }
        break;
    case IOL_MA_CM_STATE_DS_PARAMMANAGER_2:
    	/* Waiting on accomplished Data Storage startup. Parameter are downloaded into the
    	 * Device or uploaded from the Device.
    	 * */
        if (cm->trigger == IOL_CM_TRIG_DS_FAULT){
        	/* T6: state 2 to 3
        	 *  1. Data Storage failed. Rollback to previous parameter set.
        	 *  2. Update parameter elements of "PortStatusList":
        	 *  - PortStatusInfo = PORT_DIAG: Launch SMI_PortEvent with real fault
        	 *  causing this incident
        	 *  - RevisionID = (real) RRID
        	 *  - Transmission rate = COMx
        	 *  - VendorID = (real) RVID
        	 *  - DeviceID = (real) RDID
        	 *  - Port QualityInfo = invalid
        	 *  3. Launch SMI_PortEvent with real fault causing this incident
        	 *  4. goto state 3
        	 * */

        	/*1. Data Storage failed. Rollback to previous parameter set.*/
        	//TODO: need to understand Data Storage failed. Rollback to previous parameter set
        	/*2. Update parameter elements of "PortStatusList"*/
        	IOL_SM_t *sm = (IOL_SM_t*)IOL_GetSMInstance(port);
        	pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_PORT_DIAG;
			pStatList->revisionID = sm->realParamList.revId;
			pStatList->vendorID = sm->realParamList.vendId;
			pStatList->deviceID = sm->realParamList.devId;
			pStatList->portQualInfo.field.valid = 0;
			pStatList->masterCycTime =  sm->realParamList.cycleTime;
			if(	sm->realParamList.baudRate != IOL_BAUDRATE_NONE &&
				sm->realParamList.baudRate != IOL_BAUDRATE_AUTO)
			{
				pStatList->transRate = sm->realParamList.baudRate-1;
			}else{
				pStatList->transRate = 0;
			}
			/*2. Launch SMI_PortEvent with real fault causing this incident*/
			//TODO: incomplete
			//IOL_SMI_PortEvent_Ind(clientId, portNumber, expArgBlockId, argBlockLen, argBlock);
			/*3. Goto state 3*/
            cm->state = IOL_MA_CM_STATE_PORTFAULT_3;
        }else if (cm->trigger == IOL_CM_TRIG_DS_READY){
        	/* T5: state 2 to 5
        	 * SM_Operate
        	 */
            IOL_SM_Operate_Req(port, IOL_GetPortNumber(port));
            cm->state = IOL_MA_CM_STATE_WAITINGONOPERATE_4;
        }else if (cm->trigger == IOL_CM_TRIG_DS_CHANGE){
        	/* T14: state 2 to 1
        	 * SM_SetPortConfig_CFGCOM
        	 * */
        	paramList.targetMode = IOL_TARGET_MODE_CFGCOM;
        	IOL_SM_SetPortConfig_Req(port, paramList);
            cm->state = IOL_MA_CM_STATE_SM_STARTUP_1;
        }else if(cm->trigger == IOL_CM_TRIG_SM_COMLOST ||
       		 cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED )
		{
			/* T8 or T13: state 1 to 0 */
			cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
		}
        break;
    case IOL_MA_CM_STATE_PORTFAULT_3:
        /* Device in state PREOPERATE (communicating). However, one of the three faults
         * REVISION_FAULT, COMP_FAULT, SERNUM_FAULT, or DS_Fault, or PORT_DIAG occurred.
         * */
		if (cm->trigger == IOL_CM_TRIG_DS_CHANGE){
        	/* T14: state 2 to 1
        	 * SM_SetPortConfig_CFGCOM
        	 * */
        	paramList.targetMode = IOL_TARGET_MODE_CFGCOM;
        	IOL_SM_SetPortConfig_Req(port, paramList);
            cm->state = IOL_MA_CM_STATE_SM_STARTUP_1;
        }else if(cm->trigger == IOL_CM_TRIG_SM_COMLOST ||
        		 cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED )
		{
			/* T8 or T13: state 1 to 0, */
			cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
		}
        break;
    case IOL_MA_CM_STATE_WAITINGONOPERATE_4:
    	/*Waiting on SM to switch to OPERATE*/
		if(cm->trigger == IOL_CM_TRIG_SM_OPERATE){
			/* T4: state 7 to 5
			 * Update parameter elements of "PortStatusList":
			 * - PortStatusInfo = OPERATE
			 * - RevisionID = (real) RRID
			 * - Transmission rate = COMx
			 * - VendorID = (real) RVID
			 * - DeviceID = (real) RDID
			 * - Port QualityInfo = x
			 */
        	IOL_SM_t *sm = (IOL_SM_t*)IOL_GetSMInstance(port);
        	pStatList->portStatusInfo = IOL_PORT_STATUS_INFO_OPERATE;
			pStatList->revisionID = sm->realParamList.revId;
			pStatList->vendorID = sm->realParamList.vendId;
			pStatList->deviceID = sm->realParamList.devId;
			//TODO: to set portQualityInfo
			//pStatList->portQualInfo.field.valid = 0;
			if(	sm->realParamList.baudRate != IOL_BAUDRATE_NONE &&
				sm->realParamList.baudRate != IOL_BAUDRATE_AUTO)
			{
				pStatList->transRate = sm->realParamList.baudRate-1;
			}else{
				pStatList->transRate = 0;
			}

		}else if (cm->trigger == IOL_CM_TRIG_DS_CHANGE){
        	/* T14: state 2 to 1
        	 * SM_SetPortConfig_CFGCOM
        	 * */
        	paramList.targetMode = IOL_TARGET_MODE_CFGCOM;
        	IOL_SM_SetPortConfig_Req(port, paramList);
            cm->state = IOL_MA_CM_STATE_SM_STARTUP_1;
        }else if(cm->trigger == IOL_CM_TRIG_SM_COMLOST ||
		 cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED )
		{
			/* T8 or T13: state 1 to 0, */
			cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
		}

        break;
    case IOL_MA_CM_STATE_PORT_ACTIVE_5:
    	/* Port is in OPERATE mode. The gateway application is exchanging Process Data and
    	 * ready to send or receive On-request Data.
    	 * */
    	if (cm->trigger == IOL_CM_TRIG_DS_CHANGE){
			/* T14: state 2 to 1
			 * SM_SetPortConfig_CFGCOM
			 * */
			paramList.targetMode = IOL_TARGET_MODE_CFGCOM;
			IOL_SM_SetPortConfig_Req(port, paramList);
			cm->state = IOL_MA_CM_STATE_SM_STARTUP_1;
		}else if(cm->trigger == IOL_CM_TRIG_SM_COMLOST ||
		 cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED )
		{
			/* T8 or T13: state 1 to 0, */
			cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
		}else{
			OD_Start(port);
			PD_Start(port);
			DU_Start(port);
		}
        break;
    case IOL_MA_CM_STATE_PORT_DIDO_6:
    	/* Port is in DI or DO mode. The gateway application is exchanging Process Data (DI or DO).
    	 * */
    	/*Similar action of state  PORT_DIDO_6 and PORT_DEACTIVATED_8
    	 * So. fall through
    	 */
    case IOL_MA_CM_STATE_PORT_DEACTIVATED_8:
    	/* Port is in DEACTIVATED mode.*/
        if (cm->trigger == IOL_CM_TRIG_PORTCNFLIST_CHANGED){
        	/* T15: state 8 to 0*/
            cm->state = IOL_MA_CM_STATE_CHECKPORTMODE_0;
        }
        break;
    default:
        break;
    }
//    DBG_PRINT(pData);
}

/*State machine for data storage*/
void IOL_MA_DSHandler(IOL_Port_t *port){
	IOL_MA_t* ma = (IOL_MA_t*)IOL_GetMAInstance(port);
	IOL_MA_DS_t *ds = &ma->ds;
//	IOL_MA_CM_t *cm = &ma->cm;
	IOL_Port_Info_t *portInfo = IOL_GetPortInfo(port);
	uint8_t portNum = IOL_GetPortNumber(port);

	switch (ds->state){
	case IOL_DS_STATE_CHECKACTIVATIONSTATE_0:
		/* Check current state of the DS configuration: Independently from communication status,
		 * DS_Startup from configuration management or an Event DS_UPLOAD_REQ is expected.
		 * */

		MA_DS_CheckBackupRestoreState(port);	//TODO: Need to be more clear about this checking
		if (ds->trigger == IOL_DS_TRIG_ENABLED){
			/* T1: State 0 to 1*/
			ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		}else if (ds->trigger == IOL_DS_TRIG_CLEARED || ds->trigger == IOL_DS_TRIG_DISABLED){
			/* T7: State 0 to 1*/
			ds->state = IOL_DS_STATE_OFF_3;
		}

		break;
	case IOL_DS_STATE_WAITINGONDSACTIVITY_1:
		/* Waiting for upload request, Device startup, all changes of activation state independent
		 * of the Device communication state.
		 * */
		MA_DS_CheckBackupRestoreState(port);//TODO: Need to be more clear about this checking
		if (ds->trigger == IOL_DS_TRIG_STARTUP){
			/* T2: State 1 to 2*/
			ds->state = IOL_DS_STATE_UP_DOWNLOAD_2;
		}else if (ds->trigger == IOL_DS_TRIG_UPLOAD){
			/* T4: State 1 to 2
			 * Confirm Event "DS_Upload" (see INTERNAL ITEMS)
			 * Note: No need to confirm here. It is confirmed in DU
			 * */
			ds->state = IOL_DS_STATE_UP_DOWNLOAD_2;
		}else if (ds->trigger == IOL_DS_TRIG_DELETE){
			/* T9: State 1 to 1
			 * Clear saved parameter set (see Table G.1 and Table G.2)
			 */
			/*Clear saved parameter set*/
			MA_DS_DeleteParams(port);
			ds->trigger = IOL_DS_TRIG_NONE;
		}else if (ds->trigger == IOL_DS_TRIG_CLEARED){
			/* T11: State 1 to 3
			 * Clear saved parameter set (see Table G.1 and Table G.2)
			 */
			MA_DS_DeleteParams(port);
			ds->trigger = IOL_DS_TRIG_NONE;
			ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		}else if (ds->trigger == IOL_DS_TRIG_DISABLED){
			/* T12: State 1 to 3
			 * Clear saved parameter set (see Table G.1 and Table G.2)
			 */
			ds->trigger = IOL_DS_TRIG_NONE;
			ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		}
		break;

	case IOL_DS_STATE_OFF_3:
		/* Data Storage handling switched off or deactivated */
		MA_DS_CheckBackupRestoreState(port);//TODO: Need to be more clear about this checking
		if (ds->trigger == IOL_DS_TRIG_ENABLED && ds->trigComX == IOL_DS_TRIG_COMX_OK){
			/* T6: State 3 to 2*/
			ds->trigComX = IOL_DS_TRIG_COMX_NONE;
			ds->state = IOL_DS_STATE_UP_DOWNLOAD_2;
		}else if (ds->trigger == IOL_DS_TRIG_ENABLED && ds->trigComX == IOL_DS_TRIG_COMX_NOTOK){
			/* T8: State 3 to 1*/
			ds->trigger = IOL_DS_TRIG_NONE;
			ds->trigComX = IOL_DS_TRIG_COMX_NONE;
			ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		}else if (ds->trigger == IOL_DS_TRIG_DELETE){
			/* T10: State 3 to 3
			 * Clear saved parameter set (see Table G.1 and Table G.2)
			 */
			MA_DS_DeleteParams(port);
			ds->trigger = IOL_DS_TRIG_NONE;
		}else if(ds->trigger == IOL_DS_TRIG_UPLOAD){
			/* T13: State 3 to 3
			 * Confirm Event "DS_Upload" (see INTERNAL ITEMS); no further action
			 * Note: No need to confirm here. It is confirmed in DU
			 */
			/*Do Nothing*/
		}else if(ds->trigger == IOL_DS_TRIG_STARTUP){
			/* T14: State 3 to 3
			 * DS_Ready to CM
			 */
			DS_Ready(port);
		}
		break;
	case IOL_DS_STATE_UP_DOWNLOAD_2:
		/* Submachine for up/download actions and checks
		 * */
		//fall through state 4
	case IOL_DS_STATE_SM_CHECKIDENTITY_4:
		/* Check Device identification (DeviceID, VendorID) against parameter set within the Data
		 * Storage (see Table G.2). Empty content does not lead to a fault.
		 * */

		if( (ds->storedDataSet.vendorId == 0 || ds->storedDataSet.deviceId == 0) ||
			(ds->storedDataSet.vendorId == portInfo->vendorid &&
			ds->storedDataSet.deviceId == portInfo->deviceid))
		{
			/* T16: State 4 to 5
			 * 1. OD_Block
			 * 2. Read "Data Storage Size" according to Table B.10,
			 * 3. goto sub-state 5
			 */
			OD_Block(port);
			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_SIZE;
			IOL_AL_Read_Req(port,
							portNum,
							ds->currentIndex,
							ds->currentSubindex,
							&IOL_DS_AL_Read_Cnf);
			ds->state = IOL_DS_STATE_SM_CHECKMEMSIZE_5;
		}else{
			/* T15: State 4 to 12
			 * 1. Indicate DS_Fault(Identification_Fault) to the gateway application
			 * 2. goto sub-state 12
			 */

			ds->faultEventCode = IOL_EVTCDE_PORT_IDENTITY_FAULT;
			ds->state = IOL_DS_STATE_SM_DS_FAULT_12;
		}
		break;
	case IOL_DS_STATE_SM_CHECKMEMSIZE_5:
		/* Check data set size (Index 3, Subindex 3) against available Master storage size*/
		if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF){
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			if(ds->devDSConfig.storgSize <= IOL_MAX_DS_STORG_SIZE){
				/* T18: State 5 to 6
				 * 1. Read "DS_UPLOAD_FLAG" according to Table B.10
				 * 2. goto sub-state 6
				 */
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_STATE_PROPERTY;	//DS_Upload_Flag is founded in STATE_Property subindex
				IOL_AL_Read_Req(port,
								portNum,
								ds->currentIndex,
								ds->currentSubindex,
								&IOL_DS_AL_Read_Cnf);
				ds->state = IOL_DS_STATE_SM_CHECKUPLOAD_6;
			}else{
				/* T17: State 5 to 12
				 * 1. Indicate DS_Fault(SizeCheck_Fault) to the gateway application
				 * 2. goto sub-state 12
				 */

				ds->faultEventCode = IOL_EVTCDE_PORT_MEM_OUTOFRANGE;
				ds->state = IOL_DS_STATE_SM_DS_FAULT_12;
			}
		}
		break;
	case IOL_DS_STATE_SM_CHECKUPLOAD_6:
		/* Check for DS_UPLOAD_FLAG within the DataStorageIndex (see Table B.10) */
		if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF){
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			if( ds->devDSConfig.statPrpty.field.uploadFlag == IOL_DS_UPLOAD_REQ_PENDING){
				if(ds->devDSConfig.statPrpty.field.dsState == IOL_DS_STORG_STATE_UPLOAD){
					/* T19: State 6 to 7
					 * 1. DataStorageIndex 3, Subindex 1: "DS_UploadStart" (see Table B.10)
					 * 2. goto state 7
					 */
					ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
					ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
					ds->devDSConfig.cmd = IOL_DS_CMD_UPLOAD_START;
					IOL_AL_Write_Req(port,
									portNum,
									ds->currentIndex,
									ds->currentSubindex,
									&ds->devDSConfig.cmd,
									1,
									&IOL_DS_AL_Write_Cnf);
					ds->state = IOL_DS_STATE_SM_UPLOAD_7;
				}else{
					/* T43: State 6 to 8*/
					ds->state = IOL_DS_STATE_SM_CHECKDSVALIDITY_8;
				}
			}else if(ds->devDSConfig.statPrpty.field.uploadFlag == IOL_DS_UPLOAD_REQ_NONE){
				/* T20: State 6 to 8*/
				ds->state = IOL_DS_STATE_SM_CHECKDSVALIDITY_8;
			}else if(ds->devDSConfig.statPrpty.field.dsState == IOL_DS_STORG_STATE_LOCKED){
				/* T29: State 6 to 12
				 * 1. Indicate DS_Fault(Data Storage locked) to the gateway application
				 * 2. goto sub-state 12
				 */

				ds->faultEventCode = IOL_EVTCDE_PORT_DATASTORG_UNSPECERR;
				ds->state = IOL_DS_STATE_SM_DS_FAULT_12;
			}
		}
		break;
	case IOL_DS_STATE_SM_CHECKDSVALIDITY_8:
		/* Check whether stored data within the Master is valid or invalid. A Master could be
		 * replaced between upload and download activities. It is the responsibility of a Master
		 * designer to implement a validity mechanism according to the chosen use cases
		 * */
		if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF){
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			if(ds->storedDataSet.valid){
				/* T22: State 8 to 9*/
				//TODO: understand how to implement valid/invalid checking
				ds->state = IOL_DS_STATE_SM_CHECKCHECKSUM_9;
			}else{
				/* T21: State 8 to 7
				 * DataStorageIndex 3, Subindex 1: "DS_UploadStart" (see Table B.10)
				 * */
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
				ds->devDSConfig.cmd = IOL_DS_CMD_UPLOAD_START;
				IOL_AL_Write_Req(port,
								portNum,
								ds->currentIndex,
								ds->currentSubindex,
								&ds->devDSConfig.cmd,
								1,
								&IOL_DS_AL_Write_Cnf);
				ds->state = IOL_DS_STATE_SM_UPLOAD_7;
			}
		}
		break;
	case IOL_DS_STATE_SM_CHECKCHECKSUM_9:
		/* Check for differences between the data set content and the Device parameter via the
		 * "Parameter Checksum" within the DataStorageIndex (see Table B.10)
		 * */

		if (ds->devDSConfig.parmChksum != ds->storedDataSet.checksum &&
			ds->devDSConfig.statPrpty.field.dsState == IOL_DS_STORG_STATE_DOWUNLOAD)
		{
			/* T24: State 9 to 10
			 * DataStorageIndex 3, Subindex 1: "DS_DownloadStart" (see Table B.10)
			 * */
			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
			ds->devDSConfig.cmd = IOL_DS_CMD_DOWNLOAD_START;
			IOL_AL_Write_Req(port,
							portNum,
							ds->currentIndex,
							ds->currentSubindex,
							&ds->devDSConfig.cmd,
							1,
							&IOL_DS_AL_Write_Cnf);
			ds->storedDataSet.dataLen = 0;
			ds->state = IOL_DS_STATE_SM_DWNLOAD_10;
		}else if(ds->devDSConfig.parmChksum == ds->storedDataSet.checksum ||
				 ds->devDSConfig.statPrpty.field.dsState != IOL_DS_STORG_STATE_DOWUNLOAD){
			/* T25: State 9 to 11 */
			ds->state = IOL_DS_STATE_SM_DS_READY_11;
		}
		break;
	case IOL_DS_STATE_SM_DS_READY_11:
		/* Prepare DS_Ready indication to the Configuration Management (CM)*/
		/* T3: State 2 & substate 11 to 3,
		 * 1. OD_Unblock;
		 * 2. Indicate DS_Ready to CM
		 * 3. goto state 3
		 * */
		OD_Unblock(port);
		DS_Ready(port);
		ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		break;
	case IOL_DS_STATE_SM_DS_FAULT_12:
		/* Prepare DS_Fault indication from "Identification_Fault", "SizeCheck_Fault",
		 * "Upload_Fault", and "Download_Fault" to the Configuration Management (CM)
		 * */
		/* T5: State 2 & substate 12 to 3,
		 * 1. DS_Break (AL_Write, Index 3, Subindex 1);
		 * 2. clear intermediate data (garbage collection);
		 * 3. rollback to previous parameter state;
		 * 4. DS_Fault (see Figure 98);
		 * 5. OD_Unblock.
		 * 6. goto state 3
		 * */
		/*1. DS_Break (AL_Write, Index 3, Subindex 1);*/
		ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
		ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
		ds->devDSConfig.cmd = IOL_DS_CMD_BREAK;
		IOL_AL_Write_Req(port,
						 portNum,
						 ds->currentIndex,
						 ds->currentSubindex,
						 &ds->devDSConfig.cmd,
						 1,
						 &IOL_DS_AL_Write_Cnf);

		/*2. clear intermediate data (garbage collection);*/
		ds->devDSConfig.cmd = IOL_DS_CMD_NONE;
		ds->devDSConfig.statPrpty.octet = 0;
		ds->devDSConfig.storgSize = 0;
		ds->devDSConfig.parmChksum = 0;
		ds->trigger = IOL_DS_TRIG_NONE;
		ds->trigComX = IOL_DS_TRIG_COMX_NONE;
		ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;

		/*3. rollback to previous parameter state;*/
		//TODO: need to understand

		/*4. DS_Fault (see Figure 98);*/
		DS_Fault(port, ds->faultEventCode);

		/*5. OD_Unblock.*/
		OD_Unblock(port);

		/*6. goto state 3*/
		ds->state = IOL_DS_STATE_WAITINGONDSACTIVITY_1;
		break;
	case IOL_DS_STATE_SM_UPLOAD_7:
		/* Submachine for the upload actions
		 * */
		ds->devDSConfig.ilPos = 0;
		ds->devDSConfig.ilCount = 0;
		ds->storedDataSet.dataLen = 0;
	case IOL_DS_STATE_SM_DECOMPOSE_IL_13:
		/* Read Index List within the DataStorageIndex (see Table B.10). Read content entry by
		 * entry of the Index List from the Device (see Table B.11).
		 * */

		if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T34: State 13 to 16
			 * */
			ds->state = IOL_DS_STATE_SM_UPLOADFAULT_16;
		}else if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALWRITE_CNF){
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			/* Note in spec
			 * */

			/* 1. AL_Read (DataStorageIndex 3, Subindex 5), read index list
			 * 2. stay in this state
			 * */
				//TODO: need to understand where I should read index list
			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_INDEX_LIST;
			IOL_AL_Read_Req(port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 &IOL_DS_AL_Read_Cnf);

		}else if(ds->devDSConfig.ilPos < ds->devDSConfig.ilCount){
			/* T30: State 13 to 14
			 * AL_Read (Index List)
			 * */
			ds->currentIndex = ds->devDSConfig.indexList[ds->devDSConfig.ilPos].index;
			ds->currentSubindex =  ds->devDSConfig.indexList[ds->devDSConfig.ilPos++].subindex;
			IOL_AL_Read_Req(port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 &IOL_DS_AL_Read_Cnf);
			ds->state = IOL_DS_STATE_SM_READPARAMETER_14;
		}else if(ds->devDSConfig.ilPos >= ds->devDSConfig.ilCount){
			/* T35: State 13 to 15 - Read Index List completed
			 * Read "Parameter Checksum" (see Table B.10).
			 * */

			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_PARM_CHECKSUM;
			IOL_AL_Read_Req( port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 &IOL_DS_AL_Read_Cnf);
			ds->trigger = IOL_DS_TRIG_UPLOAD_DONE;
			ds->state = IOL_DS_STATE_SM_STOREDATASET_15;
		}
		break;
	case IOL_DS_STATE_SM_READPARAMETER_14:
		/* Wait until read content of one entry of the Index List from the Device is accomplished*/

		if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF){
			/* T31: State 14 to 13 - Data read*/

			if( ds->alReadCnf.len > 0 && ds->alReadCnf.len < IOL_MAX_ISDU_DATA_SIZE &&
				ds->storedDataSet.dataLen+ds->alReadCnf.len <= IOL_MAX_DS_STORG_SIZE)
			{
				//TODO: rechecks the data parsing is ok or not
				ds->storedDataSet.data[ds->storedDataSet.dataLen++] = (uint8_t)(ds->currentIndex>>8);	// MS Byte of index
				ds->storedDataSet.data[ds->storedDataSet.dataLen++] = (uint8_t)(ds->currentIndex);		// LS Byte of index
				ds->storedDataSet.data[ds->storedDataSet.dataLen++] = (uint8_t)(ds->currentSubindex);	// subindex
				ds->storedDataSet.data[ds->storedDataSet.dataLen++] = ds->alReadCnf.len;
				memcpy(&ds->storedDataSet.data[ds->storedDataSet.dataLen], ds->alReadCnf.data, ds->alReadCnf.len);
				ds->storedDataSet.dataLen += ds->alReadCnf.len;
			}

			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			ds->state = IOL_DS_STATE_SM_DECOMPOSE_IL_13;
		}else if(ds->errInfALCnf & IOL_DEV_ERRCODE_MASK){
			/* T32: State 14 to 16 - Device_Error
			 * Access to Index denied, AL_Read or AL_Write.cnf(-) with ErrorCode 0x80
			 * TODO: Need to understand, Access to Index denied
			 * */
			ds->state = IOL_DS_STATE_SM_UPLOADFAULT_16;
		}else if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T33: State 14 to 16 COMx_Error */
			ds->state = IOL_DS_STATE_SM_UPLOADFAULT_16;
		}

		break;
	case IOL_DS_STATE_SM_STOREDATASET_15:
		/* Task of the gateway application: store entire data set according to Table G.1 and Table G.2
		 * */
		/* TODO: to understand the Task of the gateway application
		 * and also understand how to store the data set
		 */

		if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T36: State 15 to 16 COMx_Error */
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			ds->state = IOL_DS_STATE_SM_UPLOADFAULT_16;
		}else if(ds->trigger == IOL_DS_TRIG_UPLOAD_DONE){
			/* T26: State 7 to 11 Upload_Done
			 * 1. DataStorageIndex 3, Subindex 1: "DS_UploadEnd";
			 * 2. read Parameter Checksum (see Table B.10)
			 * */
			if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF){
				/*1. DataStorageIndex 3, Subindex 1: "DS_UploadEnd";*/
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
				ds->devDSConfig.cmd = IOL_DS_CMD_UPLOAD_END;
				IOL_AL_Write_Req(port,
								 portNum,
								 ds->currentIndex,
								 ds->currentSubindex,
								 &ds->devDSConfig.cmd,
								 1,
								 &IOL_DS_AL_Write_Cnf);
				ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_REQ;
			}else if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALWRITE_CNF){
				/*2. read Parameter Checksum (see Table B.10)*/
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_PARM_CHECKSUM;
				IOL_AL_Read_Req(port,
								 portNum,
								 ds->currentIndex,
								 ds->currentSubindex,
								 &IOL_DS_AL_Read_Cnf);
				ds->trigSrvic = IOL_DS_TRIG_SRVC_ALREAD_REQ;
				ds->state = IOL_DS_STATE_SM_DS_READY_11;
			}
		}
		break;
	case IOL_DS_STATE_SM_UPLOADFAULT_16:
		/* Prepare Upload_Fault indication from "Device_Error" and "COM_ERROR" as input for
		 * the higher-level indication DS_Fault.
		 * */
		/* T23: State 7 & substate 16 to 12 Upload_Done
		 * 1. DataStorageIndex 3, Subindex 1: "DS_Break" (see Table B.10).
		 * 2. Indicate DS_Fault(Upload) to the gateway application
		 * 3. goto state 12
		 * */
		/*1. DataStorageIndex 3, Subindex 1: "DS_Break" (see Table B.10).*/
		if(ds->trigComX != IOL_DS_TRIG_COMX_ERROR){
			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
			ds->devDSConfig.cmd = IOL_DS_CMD_BREAK;
			IOL_AL_Write_Req(port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 &ds->devDSConfig.cmd,
							 1,
							 &IOL_DS_AL_Write_Cnf);
			ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_REQ;
		}

		/*2. Indicate DS_Fault(Upload) to the gateway application*/
		ds->faultEventCode = IOL_EVTCDE_PORT_UPLOAD_FAULT;

		/*3. goto state 12*/
		ds->state = IOL_DS_STATE_SM_DS_FAULT_12;
		break;
	case IOL_DS_STATE_SM_DWNLOAD_10:
		/* Submachine for the download actions
		 * */
		//fall through state 17
	case IOL_DS_STATE_SM_DECOMPOSE_SET_17:
		/* Write parameter by parameter of the data set into the Device according to Table G.1*/
		if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T44: State 17 to 20
			 * Not in spec
			 * */
			ds->state = IOL_DS_STATE_SM_DOWNLOAD_FAULT_20;
		}else if(ds->storedDataSet.dataLen < ds->storedDataSet.storgSize){
			/* T37: State 17 to 18 remaining data
			 * Write parameter via AL_Write
			 * */
			ds->currentIndex = (uint16_t)(ds->storedDataSet.data[ds->storedDataSet.dataLen++]<<8);
			ds->currentIndex &= (uint16_t)(ds->storedDataSet.data[ds->storedDataSet.dataLen++]);
			ds->currentSubindex = ds->storedDataSet.data[ds->storedDataSet.dataLen++];
			uint8_t len = ds->storedDataSet.data[ds->storedDataSet.dataLen++];
			uint8_t* data = &ds->storedDataSet.data[ds->storedDataSet.dataLen++];
			ds->storedDataSet.dataLen += len;
			IOL_AL_Write_Req(port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 data,
							 len,
							 &IOL_DS_AL_Write_Cnf);
			ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_REQ;
			ds->state = IOL_DS_STATE_SM_WRITE_PARAMETER_18;
		}else if(ds->storedDataSet.dataLen >= ds->storedDataSet.storgSize){
			/* T41: State 17 to 20
			 * 1. write DataStorageIndex 3, Subindex 1: "DS_DownloadEnd" (see Table B.10)
			 * 2. Read "Parameter Checksum" (see Table B.10).
			 * 3. goto state 19
			 * */
			if(ds->trigSrvic == IOL_DS_TRIG_SRVC_NONE){
				/*1. write DataStorageIndex 3, Subindex 1: "DS_DownloadEnd" (see Table B.10)*/
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
				ds->devDSConfig.cmd = IOL_DS_CMD_DOWNLOAD_END;
				IOL_AL_Write_Req(port,
								 portNum,
								 ds->currentIndex,
								 ds->currentSubindex,
								 &ds->devDSConfig.cmd,
								 1,
								 &IOL_DS_AL_Write_Cnf);
				ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_REQ;
			}else if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALWRITE_CNF){
				/*2. Read "Parameter Checksum" (see Table B.10).*/
				ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
				ds->currentSubindex = IOL_DEV_DS_SUBINDX_PARM_CHECKSUM;
				IOL_AL_Read_Req(port,
								 portNum,
								 ds->currentIndex,
								 ds->currentSubindex,
								 &IOL_DS_AL_Read_Cnf);
				ds->trigger = IOL_DS_TRIG_DOWNLOAD_DONE;
				ds->trigSrvic = IOL_DS_TRIG_SRVC_ALREAD_REQ;
				ds->storedDataSet.dataLen = 0;
				ds->state = IOL_DS_STATE_SM_DOWNLOAD_DONE_19;
			}
		}
		break;
	case IOL_DS_STATE_SM_WRITE_PARAMETER_18:
		/* Wait until write of one parameter of the data set into the Device is accomplished.*/

		if(ds->trigSrvic == IOL_DS_TRIG_SRVC_ALWRITE_CNF){
			/* T38: State 17 to 20 */
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			ds->state = IOL_DS_STATE_SM_DECOMPOSE_SET_17;
		}else if(ds->errInfALCnf & IOL_DEV_ERRCODE_MASK){
			/* T39: State 18 to 20 - Device_Error
			 * Access to Index denied, AL_Read or AL_Write.cnf(-) with ErrorCode 0x80
			 * TODO: Need to understand, Access to Index denied
			 * */
			ds->state = IOL_DS_STATE_SM_DOWNLOAD_FAULT_20;
		}else if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T40: State 18 to 20 COMx_Error */
			ds->state = IOL_DS_STATE_SM_DOWNLOAD_FAULT_20;
		}
		break;
	case IOL_DS_STATE_SM_DOWNLOAD_DONE_19:
		/* Download completed. Read back "Parameter Checksum" from the DataStorageIndex
		 * according to Table B.10. Save this value in the stored data set according to Table G.2
		 * */
		if(ds->trigComX == IOL_DS_TRIG_COMX_ERROR){
			/* T42: State 15 to 16 COMx_Error */
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			ds->state = IOL_DS_STATE_SM_DOWNLOAD_FAULT_20;
		}else if(ds->trigger == IOL_DS_TRIG_DOWNLOAD_DONE &&
				ds->trigSrvic == IOL_DS_TRIG_SRVC_ALREAD_CNF)
		{
			/* T27: State 7 to 11 Download_Done */
			ds->trigSrvic = IOL_DS_TRIG_SRVC_NONE;
			ds->trigger = IOL_DS_TRIG_COMX_NONE;
			ds->state = IOL_DS_STATE_SM_DS_READY_11;
		}
		break;
	case IOL_DS_STATE_SM_DOWNLOAD_FAULT_20:
		/* Prepare Download_Fault indication from "Device_Error" and "COM_ERROR" as input
		 * for the higher-level indication DS_Fault.
		 * */
		/* T28: State 10 to 12 Download_Failed
		 * 1. DataStorageIndex 3, Subindex 1: "DS_Break" (see Table B.10).
		 * 2. Indicate DS_Fault(Download) to the gateway application.
		 * */
		if(ds->trigComX != IOL_DS_TRIG_COMX_ERROR){
			ds->currentIndex = IOL_DEV_PARM_INDX_DATA_STORAGE;
			ds->currentSubindex = IOL_DEV_DS_SUBINDX_DS_CMD;
			ds->devDSConfig.cmd = IOL_DS_CMD_BREAK;
			IOL_AL_Write_Req(port,
							 portNum,
							 ds->currentIndex,
							 ds->currentSubindex,
							 &ds->devDSConfig.cmd,
							 1,
							 &IOL_DS_AL_Write_Cnf);
			ds->trigSrvic = IOL_DS_TRIG_SRVC_ALWRITE_REQ;
		}

		/*2. Indicate DS_Fault(Upload) to the gateway application*/
		ds->faultEventCode = IOL_EVTCDE_PORT_UPLOAD_FAULT;

		/*3. goto state 12*/
		ds->state = IOL_DS_STATE_SM_DS_FAULT_12;
		break;
	default:
		break;
	}

}

/*State machine for on request data*/
void IOL_MA_ODEHandler(IOL_Port_t *port){
	IOL_MA_ODE_t *ode = &((IOL_MA_t *)IOL_GetMAInstance(port))->ode;
	switch (ode->state) {
		case IOL_ODE_STATE_INACTIVE_0:
			/*Waiting for activation*/

			switch (ode->trigger) {
				/* T1: state 0 to 0
				 * Access blocked (inactive): indicates "DEVICE_NOT_ACCESSIBLE" to the
				 * gateway application
				 */
				case IOL_ODE_TRIG_SMI_DEVICEREAD:
					/* Action of this trigger is directly implemented in SMI_DeviceRead_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_DEVICEWRITE:
					/* Action of this trigger is directly implemented in SMI_DeviceWrite_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMREADBATCH:
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMWRITEBATCH:
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_OD_START:
					/* T2: state 0 to 1*/
					ode->state = IOL_ODE_STATE_ODACTIVE_1;
					break;
				default:
					break;
			}

			break;
		case IOL_ODE_STATE_ODACTIVE_1:
			/*On-request Data communication active using AL_Read or AL_Write*/
			switch (ode->trigger) {
				case IOL_ODE_TRIG_OD_STOP:
					/* T2: state 0 to 1*/
					ode->state = IOL_ODE_STATE_INACTIVE_0;
					break;
				case IOL_ODE_TRIG_OD_BLOCK:
					/* T5: state 1 to 2*/
					ode->state = IOL_ODE_STATE_ODBLOCKED_2;
					break;
				case IOL_ODE_TRIG_SMI_DEVICEREAD:
					/* T4: state 1 to 1
					 * AL_Read
					 * */
					/* Action of this trigger is directly implemented in SMI_DeviceRead_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */
//					IOL_AL_Read_Req(port,
//									portNum,
//									ode->jobList.reqOdeBlock->index,
//									ode->jobList.reqOdeBlock->subindex,
//									&IOL_SMI_AL_Read_Cnf);
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_DEVICEWRITE:
					/* T4: state 1 to 1
					 *  AL_Write
					 * */
					/* Action of this trigger is directly implemented in SMI_DeviceWrite_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */

//					IOL_AL_Write_Req(port,
//									 portNum,
//									 ode->jobList.reqOdeBlock->index,
//									 ode->jobList.reqOdeBlock->subindex,
//									 ode->jobList.reqOdeBlock->data,
//									 &IOL_SMI_AL_Read_Cnf);
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMREADBATCH:
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMWRITEBATCH:
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;

				default:
					break;
			}

			break;
		case IOL_ODE_STATE_ODBLOCKED_2:
			/*On-request Data communication blocked*/

			switch (ode->trigger) {
				case IOL_ODE_TRIG_OD_UNBLOCK:
					/* T7: state 2 to 1*/
					ode->state = IOL_ODE_STATE_ODACTIVE_1;
					break;
				case IOL_ODE_TRIG_SMI_DEVICEREAD:
					/* T6: state 2 to 2
					 * Access blocked temporarily: indicates "SERVICE_TEMP_UNAVAILABLE"
					 * to the gateway application
					 * */
					/* Action of this trigger is directly implemented in SMI_DeviceRead_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */

					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_DEVICEWRITE:
					/* T6: state 2 to 2
					 * Access blocked temporarily: indicates "SERVICE_TEMP_UNAVAILABLE"
					 * to the gateway application
					 * */
					/* Action of this trigger is directly implemented in SMI_DeviceWrite_Req service
					 * Ref: 11.5 On-request Data exchange (ODE)
					 * */
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMREADBATCH:
					/* T6: state 2 to 2
					 * Access blocked temporarily: indicates "SERVICE_TEMP_UNAVAILABLE"
					 * to the gateway application
					 * */
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;
				case IOL_ODE_TRIG_SMI_PARAMWRITEBATCH:
					/* T6: state 2 to 2
					 * Access blocked temporarily: indicates "SERVICE_TEMP_UNAVAILABLE"
					 * to the gateway application
					 * */
					//TODO: incomplete to implement later last
					ode->trigger = IOL_ODE_TRIG_NONE;
					break;

				default:
					break;
			}
			break;
		default:
			break;
	}
}

/* State machine for data storage
 * not in spec, we made it
 * Ref: IOL-Interface-Spec_10002_V113_Jun19_ACL_V1.0, Section: 11.6.7 Diagnosis Unit (DU) State machine
 * */
void IOL_MA_DUHandler(IOL_Port_t *port){
	IOL_MA_DU_t *du = ((IOL_MA_DU_t *)IOL_GetDUInstance(port));
	switch (du->state) {
		case IOL_DU_STATE_INACTIVE_0:
			/* Waiting for activation*/
			if(du->trigger == IOL_DU_TRIG_START){
				/* T1: state 0 to 1
				 * Goto state 1
				 * */
				du->eventAck = IOL_DU_EVENTACK_NONE;
				du->eventCount = 0;
				du->numOfEvents = 0;
				du->state = IOL_DU_STATE_IDLE_1;
			}
			break;
		case IOL_DU_STATE_IDLE_1:
			/* Waiting for device and port events indication by AL_Event.ind service*/
			if(du->trigger == IOL_DU_TRIG_STOP){
				/* T2: state 1 to 0
				 * Goto state 0
				 * */
				du->state = IOL_DU_STATE_INACTIVE_0;
			}else if(du->trigger == IOL_DU_TRIG_AL_EVENT_IND){
				/* T3: state 1 to 2
				 * Goto state 2
				 * */
				du->numOfEvents = du->eventCount;
				du->eventAck = IOL_DU_EVENTACK_NONE;
				du->state = IOL_DU_STATE_EVENTHANDLING_2;
			}
			break;
		case IOL_DU_STATE_EVENTHANDLING_2:
			/* Propagating the received device and port events to the gateway application by using
			 * SMI_xEvent.ind service and checking for the DS_Upload_Req event. Also, storing events
			 * to the diagnosis status that can be retrieved by the SMI_PortStatus service in
			 * PortStatusList (see Table E.4 and 11.6.4).
			 * */
			if(du->trigger == IOL_DU_TRIG_STOP){
				/* T4: state 2 to 0
				 * Goto state 0
				 * */
				du->state = IOL_DU_STATE_INACTIVE_0;
			}else if(du->trigger == IOL_DU_TRIG_AL_EVENT_IND){
				/* T5: state 2 to 2
				 * Propagate events to the gateway application through SMI_xEvent.ind. If the
				 * special DS_UPLOAD_REQ Event (see 10.4 and Table D.1) of a Device shall be
				 * redirected to the common Master application Data Storage. That Event is
				 * acknowledged by the DU itself and not propagated via SMI_DeviceEvent to
				 * the gateway. Store the events in the in diagnosis status
				 * */
				if(du->eventCount > 0U){
					/* event lefts
					 * check the event source
					 * */
					uint8_t portNum = IOL_GetPortNumber(port);
					IOL_EventX_t * event = &du->events[du->numOfEvents-du->eventCount--];
					uint8_t argBlock[IOL_ARGBLOCK_LEN_DEVICEEVENT] = {0}; 	// device & port event length are equal
					if(event->eventQual.field.source == IOL_EVENTSRC_DEVICE){
						/*For device events*/
						if(event->eventCode == IOL_EVTCDE_DEV_DS_UPLOAD_REQ){
							DS_Upload(port);
						}else{
							MA_EventObjToArray(IOL_ARGBLOCK_ID_DEVICEEVENT, event, argBlock);
							IOL_SMI_DeviceEvent_Ind(IOL_BROADCAST_CLIENT_ID,
													portNum ,
													IOL_ARGBLOCK_ID_VOIDBLOCK,
													IOL_ARGBLOCK_LEN_DEVICEEVENT,
													argBlock);
						}

					}else{
						/*For port (master) events*/
						MA_EventObjToArray(IOL_ARGBLOCK_ID_PORTEVENT, event, argBlock);
						IOL_SMI_PortEvent_Ind(IOL_BROADCAST_CLIENT_ID,
												portNum ,
												IOL_ARGBLOCK_ID_VOIDBLOCK,
												IOL_ARGBLOCK_LEN_PORTEVENT,
												argBlock);
					}
				}else{
					/* T4: state 2 to 0
					 * if there is no event left
					 * Goto state 3
					 * */
					du->state = IOL_DU_STATE_WAITINGFOREVENTACK_3;
				}

			}
			break;
		case IOL_DU_STATE_WAITINGFOREVENTACK_3:
			/* The Master DU remains in this state as long as the gateway application did not
			 * acknowledge the SMI_xEvent.ind.
			 * */
			if(du->trigger == IOL_DU_TRIG_STOP){
				/* T9: state 3 to 0
				 * Goto state 0
				 * */
				du->state = IOL_DU_STATE_INACTIVE_0;
			}else if(du->eventAck != IOL_DU_EVENTACK_NONE){
				/* T8 & T7: state 3 to 1
				 * Goto state 1
				 * */
				if(du->eventAck & IOL_DU_EVENTACK_DEVICE){
					IOL_AL_Event_Rsp(port, IOL_GetPortNumber(port));
				}
				du->eventAck = IOL_DU_EVENTACK_NONE;
				du->state = IOL_DU_STATE_IDLE_1;
			}
			break;
		default:
			break;
	}

}
