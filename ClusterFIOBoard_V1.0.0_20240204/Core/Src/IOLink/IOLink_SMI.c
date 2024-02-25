#include "IOLink_SMI.h"
#include "string.h"
#include "IOLink_SM.h"
#include "stdio.h"
#include "IOLink_MA.h"
#include "stdlib.h"
#include "IOLink_AL.h"
#include "IOLink_Config.h"
#include <stdio.h>
#include "IOLink_DL.h"
#include "Debug.h"

static uint8_t SMI_AB_JobErrToArray(IOL_SMI_ArgBlock_JobError_t* argBlock,  uint8_t* array);
static uint8_t SMI_AB_VoidblockToArray(uint8_t* array);
inline static uint16_t SMI_ParseABIdFromABArray(uint8_t* argBlock);
inline static uint8_t SMI_IsClientIdInODEJobList(IOL_MA_ODE_t *ode, uint8_t clientId);


/*Internal Services--------------------------------------------------------*/
/* @brief Convert the job error argument block into array
 * */
static uint8_t SMI_AB_JobErrToArray(IOL_SMI_ArgBlock_JobError_t* argBlock,  uint8_t* array){
	if(array == NULL) return 0;
	uint8_t len = 0;
	array[len++] = (uint8_t)(argBlock->argBlockID>>8);
	array[len++] = (uint8_t)(argBlock->argBlockID);
	array[len++] = (uint8_t)(argBlock->expArgBlockID>>8);
	array[len++] = (uint8_t)(argBlock->expArgBlockID);
	array[len++] = (uint8_t)(argBlock->errorCode>>8);
	array[len++] = (uint8_t)(argBlock->errorCode);
	return len;
}

/* Void block as an array
 * */
static uint8_t SMI_AB_VoidblockToArray(uint8_t* array){
	uint8_t len = 0;
	if (array == NULL) return 0;
	array[len++] = (uint8_t)(IOL_ARGBLOCK_ID_VOIDBLOCK>>8);
	array[len++] = (uint8_t)(IOL_ARGBLOCK_ID_VOIDBLOCK);
	return len;
}

/* @brief check the argBlock Id is valid or not
 * */
static uint16_t SMI_CheckABID(uint16_t argBlockId){

	switch (argBlockId) {
		case IOL_ARGBLOCK_ID_MASTERIDENT: 		return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_FS_MASTERACCESS: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_W_MASTERCONFIG: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PDIN: 				return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PDOUT: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PDINOUT: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_SPDUIN: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_SPDUOUT: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PDINIQ: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PDOUTIQ: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_W: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_R: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_DS_DATA: 			return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_DEVICEPARBATCH: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_INDEXLIST: 		return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PORTPOWEROFFON: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PORTCONFIGLIST: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_FS_PORTCONFIGLIST: return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_W_TRACKCONFIGLIST: return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PORTSTATUSLIST: 	return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_FS_PORTSTATUSLIST: return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_W_TRACKSTATUSLIST: return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_W_TRACKSCANRESULT: return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_DEVICEEVENT: 		return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_PORTEVENT: 		return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_VOIDBLOCK: 		return IOL_ERRTYP_NONE; break;
		case IOL_ARGBLOCK_ID_JOBERROR: 			return IOL_ERRTYP_NONE; break;
		default: return IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED; 			   break;
	}
}

/* get argBlock id form argBlock array
 * */
inline static uint16_t SMI_ParseABIdFromABArray(uint8_t* argBlock){
	return (uint16_t)((argBlock[0]<<8) | argBlock[1]);
}

/* check clientId is consist in job request list
 * */
inline static uint8_t SMI_IsClientIdInODEJobList(IOL_MA_ODE_t *ode, uint8_t clientId){
//	 for(uint8_t i = 0; i < IOL_MAX_MA_ODE_JOB; i++){
//		 if(ode->jobList[i].clientId == clientId) return 1;
//	 }
//	 return 0;
	return ode->jobList.clientId == clientId;
 }

/* @brief convey errors to the gateway application
 * */

inline static void SMI_ConveyNegCnf(uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expBlockId, IOL_ArgBlockID refBlockId, uint16_t errorCode,
		void (*cnfFuncCb)(uint8_t clientId, uint8_t portNumber,
			IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock))
{
	IOL_SMI_ArgBlock_JobError_t jobErrorBlock = {
			.argBlockID = IOL_ARGBLOCK_ID_JOBERROR,
			.expArgBlockID = expBlockId,
			.errorCode = errorCode
	};
	uint8_t jobErrArray[IOL_ARGBLOCK_LEN_JOBERROR];
	SMI_AB_JobErrToArray(&jobErrorBlock, jobErrArray);
	cnfFuncCb(clientId, portNumber, refBlockId, IOL_ARGBLOCK_LEN_JOBERROR, jobErrArray);
}

/*ODE internal services------------------------------------------------------*/
/* Create new job
 * */
inline static void SMI_CreateODEJob(IOL_MA_ODE_t* ode, uint8_t jobId,
	IOL_ArgBlockID  argBlockId,	uint8_t clientId, uint16_t argBlockLen,uint8_t *argBlock)
{
	ode->jobList.jobId = jobId;
	ode->jobList.clientId = clientId;
	switch (argBlockId) {
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_R:
			ode->jobList.reqOdeBlock = (IOL_SMI_ArgBlock_OnRequestData_t*)calloc(1,
					sizeof(IOL_SMI_ArgBlock_OnRequestData_t));
			ode->jobList.reqOdeBlock->argBlockID = argBlockId;
			ode->jobList.reqOdeBlock->index = IOL_GET2BYTES(argBlock, IOL_ARGPOS_OD_INDEX_U16);
			ode->jobList.reqOdeBlock->subindex = argBlock[IOL_ARGPOS_OD_SUBINDEX_U8];
			ode->jobCount++;
			break;
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_W:
			ode->jobList.reqOdeBlock = (IOL_SMI_ArgBlock_OnRequestData_t*)calloc(1,
					sizeof(IOL_SMI_ArgBlock_OnRequestData_t));
			ode->jobList.reqOdeBlock->argBlockID = argBlockId;
			ode->jobList.reqOdeBlock->index = IOL_GET2BYTES(argBlock, IOL_ARGPOS_OD_INDEX_U16);
			ode->jobList.reqOdeBlock->subindex = argBlock[IOL_ARGPOS_OD_SUBINDEX_U8];
			memcpy(ode->jobList.reqOdeBlock->data, &argBlock[IOL_ARGPOS_OD_ONREQDATA_N],	(argBlockLen-IOL_ARGPOS_OD_ONREQDATA_N));
			ode->jobCount++;
			break;
		case IOL_ARGBLOCK_ID_DEVICEPARBATCH:

			break;
		case IOL_ARGBLOCK_ID_INDEXLIST:

			break;
		default:
			break;
	}
}

inline static uint8_t SMI_IsODEJoblistFull(IOL_MA_ODE_t* ode){
	return (ode->jobCount > 0);
}

/* Delete the ode job
 * */
inline static uint8_t SMI_DeleteODEJob(IOL_MA_ODE_t* ode, uint8_t currentJobId, IOL_ArgBlockID  argBlockId)
{
	if(ode->jobList.jobId != currentJobId) return 0;
	ode->jobList.jobId = 0;
	ode->jobList.clientId = 0;
	switch (argBlockId) {
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_W:
			memset(ode->jobList.reqOdeBlock->data, 0, IOL_MAX_OD_SIZE);
			//same operation of ONREQUESTDATA W & R
		case IOL_ARGBLOCK_ID_ONREQUESTDATA_R:
			ode->jobList.reqOdeBlock->argBlockID = 0;
			ode->jobList.reqOdeBlock->index = 0;
			ode->jobList.reqOdeBlock->subindex = 0;
			free(ode->jobList.reqOdeBlock);
			ode->jobList.reqOdeBlock = NULL;
			if(ode->jobCount>0)	ode->jobCount--;
			return 1;
			break;
		case IOL_ARGBLOCK_ID_DEVICEPARBATCH:

			break;
		case IOL_ARGBLOCK_ID_INDEXLIST:

			break;
		default:
			break;
	}
	return 0;
}






/*SMI services---------------------------------------------------------------*/

/* @ref 11.2.4 SMI_MasterIdentification
 * @brief an explicit identification of a Master did not have priority in SDCI since gateway
 * applications usually provided hard-coded identification and maintenance information as required
 * by the fieldbus system. Due to the requirement "one Master Tool (PCDT) fits different Master
 * brands", corresponding new Master Tools shall be able to connect to Masters providing an
 * SMI. For that purpose, the SMI_MasterIdentification service has been created. It allows Master
 * Tools to adjust to individual Master brands and types, if a particular fieldbus gateway provides
 * the SMI services in a uniform accessible coding
 * */
IOL_ErrorTypes IOL_SMI_MasterIdent_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_ErrorTypes error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	uint8_t expArgBlockLen = IOL_ARGBLOCK_LEN_MASTERIDENT(IOL_MAX_PORT);
	/*Check errors*/
	if(expArgBlockId != IOL_ARGBLOCK_ID_MASTERIDENT &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= expArgBlockLen))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_MASTERIDENT,
						 error,
						 (void *)&IOL_SMI_MasterIdent_Cnf);
		return error;
	}

	/*set data buffer with master ident argument block*/
	IOL_ArgBlock_MasterIdent_t* ident = &iolMaster.ident;
	uint8_t data[IOL_ARGBLOCK_LEN_MASTERIDENT(IOL_MAX_PORT)] = {0};
	expArgBlockLen = 0;
	IOL_SET2BYTES(data, IOL_ARGPOS_ARGBLOCKID_U16, IOL_ARGBLOCK_ID_MASTERIDENT);
	IOL_SET2BYTES(data, IOL_ARGPOS_MSTRIDENT_VENDORID_U16, ident->vendorID);
	IOL_SET2BYTES(data, IOL_ARGPOS_MSTRIDENT_MASTERID_U32, ident->masterID);
	data[IOL_ARGPOS_MSTRIDENT_MASTERTYPE_U8] = (uint8_t)(ident->masterType);
	data[IOL_ARGPOS_MSTRIDENT_FEATURES1_U8] = (uint8_t)(ident->features1.octet);
	data[IOL_ARGPOS_MSTRIDENT_FEATURES2_U8] = (uint8_t)(ident->features2);
	data[IOL_ARGPOS_MSTRIDENT_MAXPORTS_U8] = (uint8_t)(ident->maxNumberOfPorts);
	memcpy(&data[IOL_ARGPOS_MSTRIDENT_PORTTYPE_N], ident->portTypes, sizeof(ident->portTypes));
	expArgBlockLen = IOL_ARGBLOCK_LEN_MASTERIDENT(IOL_MAX_PORT);


	IOL_SMI_MasterIdent_Cnf( clientId,
							 portNumber,
							 IOL_ARGBLOCK_ID_VOIDBLOCK,
							 expArgBlockLen,
							 data);

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_MasterIdent_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}


/* @ref 11.2.5 SMI_PortConfiguration
 * @brief With the help of this service, an SMI client such as a gateway application launches the indi-
 * cated Master port and the connected Device using the elements in parameter PortConfigList.
 * The service shall be accepted immediately and performed without delay. Content of Data
 * Storage for that port will be deleted at each new port configuration via "DS_Delete" (see
 * Figure 99). Table 107 shows the structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_PortConfig_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_ErrorTypes error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK ||
			argBlockId != IOL_ARGBLOCK_ID_PORTCONFIGLIST)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(argBlockLen != IOL_ARGBLOCK_LEN_PORTCONFIGLIST){
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	//TODO: to implement ARGBLOCK_INCONSISTENT error. Need to understand

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PORTCONFIGLIST,
						 error,
						 (void *)&IOL_SMI_PortConfig_Cnf);
		return error;
	}

	IOL_Port_t* port = IOL_GET_PORT(portNumber);


	IOL_MA_CM_t* cm = (IOL_MA_CM_t*) IOL_GetCMInstance(port);

	/*Parse the argBlock content from the argBlock array*/
	cm->reqPortCfgList.argBlockId = argBlockId;
	cm->reqPortCfgList.portMode = argBlock[IOL_ARGPOS_PORTCNFLIST_PORTMODE_U8];
	cm->reqPortCfgList.validBackup = argBlock[IOL_ARGPOS_PORTCNFLIST_VALIDBKUP_U8];
	cm->reqPortCfgList.iQBehavior = argBlock[IOL_ARGPOS_PORTCNFLIST_IQBEHAVIOR_U8];
	cm->reqPortCfgList.portCycTime = argBlock[IOL_ARGPOS_PORTCNFLIST_CYCLETIME_U8];
	cm->reqPortCfgList.vendorID = (uint16_t)IOL_GET2BYTES(argBlock, IOL_ARGPOS_PORTCNFLIST_VENDORID_U16);
	cm->reqPortCfgList.deviceID = (uint32_t)IOL_GET4BYTES(argBlock, IOL_ARGPOS_PORTCNFLIST_DEVICEID_U32);

	cm->trigger = IOL_CM_TRIG_PORTCNFLIST_CHANGED;
	/*Invoke confirmation service*/
	uint8_t voidArgBlock[IOL_ARGBLOCK_LEN_VOIDBLOCK];
	SMI_AB_VoidblockToArray(voidArgBlock);
	IOL_SMI_PortConfig_Cnf( clientId,
							portNumber,
							argBlockId,
							IOL_ARGBLOCK_LEN_VOIDBLOCK,
							voidArgBlock);

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PortConfig_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.6 SMI_ReadbackPortConfiguration
 * @brief This service allows for retrieval of the effective configuration of the indicated Master port.
 * Table 108 shows the structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_ReadbackPortConfig_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_ErrorTypes error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PORTCONFIGLIST &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_VOIDBLOCK))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PORTCONFIGLIST,
						 error,
						 (void *)&IOL_SMI_ReadbackPortConfig_Cnf);
		return error;
	}
	IOL_Port_t* port = IOL_GET_PORT(portNumber);
//	IOL_GetPort(&iolMaster, portNumber, &port);
	IOL_MA_CM_t* cm = (IOL_MA_CM_t*) IOL_GetCMInstance(port);
	uint8_t data[IOL_ARGBLOCK_LEN_PORTCONFIGLIST] = {0};

	/*Insert port configuration list parameter in the data array*/
	IOL_SET2BYTES(data, IOL_ARGPOS_ARGBLOCKID_U16, IOL_ARGBLOCK_ID_PORTCONFIGLIST);
	data[IOL_ARGPOS_PORTCNFLIST_PORTMODE_U8] = cm->reqPortCfgList.portMode;
	data[IOL_ARGPOS_PORTCNFLIST_VALIDBKUP_U8] = cm->reqPortCfgList.validBackup;
	data[IOL_ARGPOS_PORTCNFLIST_IQBEHAVIOR_U8] = cm->reqPortCfgList.iQBehavior;
	data[IOL_ARGPOS_PORTCNFLIST_CYCLETIME_U8] = cm->reqPortCfgList.portCycTime;
	IOL_SET2BYTES(data, IOL_ARGPOS_PORTCNFLIST_VENDORID_U16, cm->reqPortCfgList.vendorID);
	IOL_SET4BYTES(data, IOL_ARGPOS_PORTCNFLIST_DEVICEID_U32, cm->reqPortCfgList.deviceID);

	/*Invoke confirmation service*/
	IOL_SMI_ReadbackPortConfig_Cnf( clientId,
									portNumber,
									IOL_ARGBLOCK_ID_VOIDBLOCK,
									IOL_ARGBLOCK_LEN_PORTCONFIGLIST,
									data);

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service/
 * */
IOL_ErrorTypes IOL_SMI_ReadbackPortConfig_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}



/* @ref 11.2.7 SMI_PortStatus
 * @brief This service allows for retrieval of the effective status of the indicated Master port. Table 109
 * shows the structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_PortStatus_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_ErrorTypes error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PORTSTATUSLIST &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_VOIDBLOCK))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PORTSTATUSLIST,
						 error,
						 (void *)&IOL_SMI_PortStatus_Cnf);
		return error;
	}
	IOL_Port_t* port = IOL_GET_PORT(portNumber);
//	IOL_GetPort(&iolMaster, portNumber, port);
	IOL_MA_CM_t* cm = (IOL_MA_CM_t*) IOL_GetCMInstance(port);
	uint8_t data[IOL_ARGBLOCK_LEN_PORTSTATUSLIST(IOL_MAX_DIAG_ENTRIES)] = {0};
	/*Insert portstatus argBlock parameter into argBlock array*/
	IOL_SET2BYTES(data, IOL_ARGPOS_ARGBLOCKID_U16, IOL_ARGBLOCK_ID_PORTSTATUSLIST);
	data[IOL_ARGPOS_PORTSTATLIST_PORTSTATINFO_U8] = cm->portStatusList.portStatusInfo;
	data[IOL_ARGPOS_PORTSTATLIST_PORTQUALINFO_U8] = cm->portStatusList.portQualInfo.octet;
	data[IOL_ARGPOS_PORTSTATLIST_REVISIONID_U8] = cm->portStatusList.revisionID;
	data[IOL_ARGPOS_PORTSTATLIST_TRANSRATE_U8] = cm->portStatusList.transRate;
	data[IOL_ARGPOS_PORTSTATLIST_MSTRCYCLTIM_U8] = cm->portStatusList.masterCycTime;
	data[IOL_ARGPOS_PORTSTATLIST_INDATALEN_U8] = cm->portStatusList.inDataLen;
	data[IOL_ARGPOS_PORTSTATLIST_OUTDATALEN_U8] = cm->portStatusList.outDataLen;
	IOL_SET2BYTES(data, IOL_ARGPOS_PORTSTATLIST_VENDORID_U16, cm->portStatusList.vendorID);
	IOL_SET4BYTES(data, IOL_ARGPOS_PORTSTATLIST_DEVICEID_U32, cm->portStatusList.deviceID);
	data[IOL_ARGPOS_PORTSTATLIST_NUMOFDIAGS_U8] = cm->portStatusList.numOfDiags;
	for(uint8_t i = 0; i < cm->portStatusList.numOfDiags; i++){
		data[IOL_ARGPOS_PORTSTATLIST_DIAGENTRYX_U24_N+i] = cm->portStatusList.diagEntries[i].eventQual.octet;
		IOL_SET2BYTES(data, IOL_ARGPOS_PORTSTATLIST_DIAGENTRYX_U24_N+1+i, cm->portStatusList.diagEntries[i].eventCode);
	}

	/*Invoke confirmation service*/
	IOL_SMI_PortStatus_Cnf( clientId,
									portNumber,
									IOL_ARGBLOCK_ID_VOIDBLOCK,
									IOL_ARGBLOCK_LEN_PORTSTATUSLIST(cm->portStatusList.numOfDiags),
									data);

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PortStatus_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.8 SMI_DSToParServ
 * @brief With the help of this service, an SMI client such as a gateway application is able to retrieve
 * the technology parameter set of a Device from Data Storage and back it up within an upper
 * level parameter server
 * */
IOL_ErrorTypes IOL_SMI_DSToParServ_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_ErrorTypes error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PORTSTATUSLIST &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_VOIDBLOCK))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PORTSTATUSLIST,
						 error,
						 (void *)&IOL_SMI_PortStatus_Cnf);
		return error;
	}
//	IOL_Port_t* port = IOL_GET_PORT(iolMaster, portNumber);
//	IOL_GetPort(&iolMaster, portNumber, port);
//	IOL_MA_CM_t* cm = (IOL_MA_CM_t*) IOL_GetCMInstance(port);
//	uint8_t data[IOL_ARGBLOCK_LEN_PORTSTATUSLIST(IOL_MAX_DIAG_ENTRIES)] = {0};
	/*Insert portstatus argBlock parameter into argBlock array*/
	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_DSToParServ_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.9 SMI_ParServToDS
 * @brief With the help of this service, an SMI client such as a gateway application is able to restore
 * the technology parameter set of a Device within Data Storage from an upper level parameter
 * server (see Figure 95, clauses 11.4, and 13.4.2). Table 111 shows the structure of the ser-
 * vice.
 *
 * In case of DI or DO on this Port, content of Data Storage is cleared. The same applies if Data
 * Storage is not enabled for this Port.
 * */
IOL_ErrorTypes IOL_SMI_ParServToDS_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_ParServToDS_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.10 SMI_DeviceWrite
 * @brief This service allows for writing On-request Data (OD) for propagation to the Device.
 * */
uint16_t IOL_SMI_DeviceWrite_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	uint16_t error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK &&
			argBlockId != IOL_ARGBLOCK_ID_ONREQUESTDATA_W)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_ONREQUESTDATA_W(IOL_MAX_OD_SIZE)))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}


	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_ONREQUESTDATA_W,
						 error,
						 (void *)&IOL_SMI_DeviceWrite_Cnf);
		return error;
	}

	IOL_MA_ODE_t* ode = (IOL_MA_ODE_t*)IOL_GetODEInstance(iolMaster.ports[portNumber-1]);
	//	else if(!SMI_IsClientIdInODEJobList(ode, clientId) || ode->jobCount > 0){
	//		/*	Master busy, if client is already made a request or there is an incomplete job
	//		 * */
	//		error = IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE;
	//	}
	if(ode->state == IOL_ODE_STATE_INACTIVE_0 ){
		/*Device not communicating*/
		error = IOL_ERRTYPSMI_DEVICE_NOT_ACCESSIBLE;
	}else if(ode->state == IOL_ODE_STATE_ODBLOCKED_2){
		/*Device not communicating*/
		error = IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_ONREQUESTDATA_W,
						 error,
						 (void *)&IOL_SMI_DeviceWrite_Cnf);
		return error;
	}

	/*Create job*/
	SMI_CreateODEJob(ode, 1, argBlockId, clientId, argBlockLen, argBlock);
	ode->trigger = IOL_ODE_TRIG_SMI_DEVICEWRITE;
	uint8_t len = argBlockLen - (sizeof(argBlockId)
					+ sizeof(ode->jobList.reqOdeBlock->index)
					+ sizeof(ode->jobList.reqOdeBlock->subindex));

	IOL_AL_Write_Req(iolMaster.ports[portNumber-1],
					portNumber,
					ode->jobList.reqOdeBlock->index,
					ode->jobList.reqOdeBlock->subindex,
					ode->jobList.reqOdeBlock->data,
					len,
					&IOL_ODE_AL_Write_Cnf);

	// Call the ode handler
	IOL_MA_ODEHandler(iolMaster.ports[portNumber-1]);
	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
uint16_t IOL_SMI_DeviceWrite_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{

	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.11 SMI_DeviceRead
 * @brief This service allows for reading On-request Data (OD) from the Device via the Master.
 * */
uint16_t IOL_SMI_DeviceRead_Req( uint8_t clientId, uint8_t portNumber,
    IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	uint16_t error = IOL_ERRTYP_NONE;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_ONREQUESTDATA_W &&
			argBlockId != IOL_ARGBLOCK_ID_ONREQUESTDATA_R)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_ONREQUESTDATA_R))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_ONREQUESTDATA_R,
						 error,
						 (void *)&IOL_SMI_DeviceRead_Cnf);
		return error;
	}

	IOL_MA_ODE_t* ode = (IOL_MA_ODE_t*)IOL_GetODEInstance(iolMaster.ports[portNumber-1]);

	//	else if(!SMI_IsClientIdInODEJobList(ode, clientId) || ode->jobCount > 0){
	//		/*	Master busy, if client is already made a request or there is an incomplete job
	//		 * */
	//		error = IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE;
	//	}
	if(ode->state == IOL_ODE_STATE_INACTIVE_0 ){
		/*Device not communicating*/
		error = IOL_ERRTYPSMI_DEVICE_NOT_ACCESSIBLE;
	}else if(ode->state == IOL_ODE_STATE_ODBLOCKED_2){
		/*Device not communicating*/
		error = IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE;
	}
	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_ONREQUESTDATA_R,
						 error,
						 (void *)&IOL_SMI_DeviceRead_Cnf);
		return error;
	}


	/*Create job*/
	SMI_CreateODEJob(ode, 1, argBlockId, clientId, argBlockLen, argBlock);
	ode->trigger = IOL_ODE_TRIG_SMI_DEVICEREAD;
	IOL_AL_Read_Req(iolMaster.ports[portNumber-1],
					portNumber,
					ode->jobList.reqOdeBlock->index,
					ode->jobList.reqOdeBlock->subindex,
					&IOL_ODE_AL_Read_Cnf);
	IOL_MA_ODEHandler(iolMaster.ports[portNumber-1]);
	return IOL_ERRTYP_NONE;
}



/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
uint16_t IOL_SMI_DeviceRead_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.12 SMI_ParamWriteBatch
 * @brief This service allows for the "push" transfer of a large number of consistent Device objects via
 * multiple ISDUs. Table 114 shows the structure of the service. The following rules apply:
 * • The service transfers the ArgBlock "DeviceParBatch" to the Master that conveys the
 *   content object by object to the Device via AL_Write (ISDU).
 *   The same ArgBlock structure is returned as Result (+). However, a value "0x0000"
 *   indicates success of a particular AL_Write or an ISDU ErrorType of a failed AL_Write
 *   instead of a parameter record.
 * • Result (-) is only returned in case of a failing service via "JobError".
 *
 * NOTE1 This service supposes use of Block Parameterization and sufficient buffer ressources
 * NOTE2 This service may have unexpected duration
 *
 * This service is optional. Availability is indicated via Master identification
 * */
IOL_ErrorTypes IOL_SMI_ParamWriteBatch_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: incomplete to implement later last
	uint16_t error = IOL_ERRTYP_NONE;
	IOL_MA_ODE_t* ode= NULL;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!iolMaster.ident.features1.field.parWriteBatch){
		/*Service unknown*/
		error = IOL_ERRTYPSMI_SERVICE_NOT_SUPPORTED;
	}else if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK &&
			argBlockId != IOL_ARGBLOCK_ID_DEVICEPARBATCH)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_DEVPARBATCH(IOL_MAX_ISDU_DATA_SIZE*IOL_MAX_DEVPARBATCH_LEN)))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}else{
		ode = (IOL_MA_ODE_t*)IOL_GetODEInstance(iolMaster.ports[portNumber-1]);
		if(!SMI_IsClientIdInODEJobList(ode, clientId) || ode->jobCount > 0){

			/*	Master busy, if client is already made a request or there is an incomplete job
			 * */
			error = IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE;
		}else if(ode->state == IOL_ODE_STATE_INACTIVE_0 ){
			/*Device not communicating*/
			error = IOL_ERRTYPSMI_DEVICE_NOT_ACCESSIBLE;
		}
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_ONREQUESTDATA_W,
						 error,
						 (void *)&IOL_SMI_DeviceWrite_Cnf);
		return error;
	}


	/*Create job*/
	SMI_CreateODEJob(ode, 1, argBlockId, clientId, argBlockLen, argBlock);
	ode->trigger = IOL_ODE_TRIG_SMI_DEVICEWRITE;

	// Call the ode handler
	IOL_MA_ODEHandler(iolMaster.ports[portNumber-1]);
	return IOL_ERRTYP_NONE;

}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_ParamWriteBatch_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{//TODO: incomplete to implement later last

	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.13 SMI_ParamReadBatch
 * @brief This service allows for the "pull" transfer of a large number of consistent Device parameters
 * via multiple ISDUs. Table 114 shows the structure of the service. The following rules apply:
 * • The service transfers the ArgBlock "IndexList" to the Master that transforms the content
 * 	 entry by entry into AL_Read (ISDU) to the Device.
 * • The corresponding ArgBlock "DeviceParBatch is returned as Result (+). In case of a
 *   successful AL_Read of an object, the corresponding parameter record or an ISDU
 *   ErrorType of a failed AL_Read instead of a parameter record is returned.
 * • Result (-) is only returned in case of a failing service via "JobError".
 *
 * NOTE1 This service supposes use of Block Parameterization and sufficient buffer ressources
 * NOTE2 This service may have unexpected duration
 *
 * This service is optional. Availability is indicated via Master identification
 * */
IOL_ErrorTypes IOL_SMI_ParamReadBatch_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{//TODO: incomplete to implement later last

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_ParamReadBatch_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: incomplete to implement later last
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.14 SMI_PortPowerOffOn
 * @brief This service allows for switching Power 1 of a particular port off and on (see 5.4.1). It returns
 * upon elapsed time provided within the ArgBlock.
 * */
IOL_ErrorTypes IOL_SMI_PortPowerOffOn_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PortPowerOffOn_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.15 SMI_DeviceEvent
 * @brief This service allows for signaling a Master Event created by the Device.
 * */
IOL_ErrorTypes IOL_SMI_DeviceEvent_Ind( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to convey msg to gateway application
	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_DeviceEvent_Rsp( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{

	IOL_Port_t* port = iolMaster.ports[ portNumber-1];
	IOL_MA_DU_t* du = (IOL_MA_DU_t*)IOL_GetDUInstance(port);
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		return IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(refArgBlockId != IOL_ARGBLOCK_ID_DEVICEEVENT &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		return IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_VOIDBLOCK))
	{
		/*(incorrect ArgBlock length*/
		return IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	du->eventAck |= IOL_DU_EVENTACK_DEVICE;

	//Run the DU state machine
	IOL_MA_DUHandler(port);
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.16 SMI_PortEvent
 * @brief This service allows for signaling a Master Event created by the Port.
 * */
IOL_ErrorTypes IOL_SMI_PortEvent_Ind( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to convey msg to gateway application
	return IOL_ERRTYP_NONE;
}


/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PortEvent_Rsp( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	IOL_Port_t* port = iolMaster.ports[portNumber-1];
	IOL_MA_DU_t* du = (IOL_MA_DU_t*)IOL_GetDUInstance(port);
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		return IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(refArgBlockId != IOL_ARGBLOCK_ID_PORTEVENT &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		return IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_VOIDBLOCK))
	{
		/*(incorrect ArgBlock length*/
		return IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}

	du->eventAck |= IOL_DU_EVENTACK_PORT;
	//Run the DU state machine
	IOL_MA_DUHandler(port);
	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.17 SMI_PDIn
 * @brief This service allows for cyclically reading input Process Data from an InBuffer (see 11.7.2.1).
 * Table 119 shows the structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_PDIn_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
		void (*cb_pdIn_cnf)(uint8_t clientId, uint8_t portNumber,
				IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock))
{
	uint16_t error = IOL_ERRTYP_NONE;
	IOL_Port_t* port = NULL;
	IOL_MA_PDE_t* pde = NULL;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors----------*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PDIN &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_PDIN(IOL_MAX_PD_SIZE)))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}else{
		port = iolMaster.ports[portNumber-1];
		pde = (IOL_MA_PDE_t*) IOL_GetPDEInstance(port);

		if(pde->trigger == IOL_PDE_TRIG_STOP){
			/*Device not communicating*/
			error = IOL_ERRTYPSMI_DEVICE_NOT_IN_OPERATE;
		}
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PDIN,
						 error,
						 (void *)&IOL_SMI_PDIn_Cnf);
		return error;
	}

	pde->cb_pdIn_cnf = cb_pdIn_cnf;

	IOL_AL_GetInput_Req(port, portNumber);

	/*
	 * 1. Get pdin data from the buffer
	 * 2. Invoke AL_GetInput.req
	 * */
	// convey the pd input data
	IOL_SMI_PDIn_Cnf(clientId,
					 portNumber,
					 IOL_ARGBLOCK_ID_VOIDBLOCK,
					 pde->pdInLen,
					 pde->pdIn);
	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PDIn_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	//TODO: to implement later
	((IOL_MA_PDE_t*) IOL_GetPDEInstance(iolMaster.ports[portNumber-1]))->cb_pdIn_cnf(
			clientId,
			portNumber,
			refArgBlockId,
			argBlockLen,
			argBlock);

	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.18 SMI_PDOut
 * @brief This service allows for cyclically writing output Process Data to an OutBuffer (see 11.7.3.1).
 * Table 120 shows the structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_PDOut_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
		void (*cb_pdOut_cnf)(uint8_t clientId, uint8_t portNumber,
				IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock))
{
	uint16_t error = IOL_ERRTYP_NONE;
	IOL_Port_t* port = NULL;
	IOL_MA_PDE_t* pde = NULL;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors----------*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PDOUT &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_PDOUT(IOL_MAX_PD_SIZE)))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}else{
		port = iolMaster.ports[portNumber-1];
		pde = (IOL_MA_PDE_t*) IOL_GetPDEInstance(port);

		if(pde->trigger == IOL_PDE_TRIG_STOP){
			/*Device not communicating*/
			error = IOL_ERRTYPSMI_DEVICE_NOT_IN_OPERATE;
		}
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PDOUT,
						 error,
						 (void *)&IOL_SMI_PDOut_Cnf);
		return error;
	}

	uint8_t voidBlock[IOL_ARGBLOCK_LEN_VOIDBLOCK];
	/*Get void argblock id*/
	SMI_AB_VoidblockToArray(voidBlock);
	/*Set the callback*/
	pde->cb_pdOut_cnf = cb_pdOut_cnf;

	/*Set pdOut data to pdOut buffer*/
	pde->pdOutLen = argBlockLen;
	memcpy(pde->pdOut, argBlock, pde->pdOutLen);

	// convey cnf msg
	IOL_SMI_PDOut_Cnf(clientId,
					 portNumber,
					 IOL_ARGBLOCK_ID_PDOUT,
					 IOL_ARGBLOCK_LEN_VOIDBLOCK,
					 voidBlock);

	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PDOut_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{
	((IOL_MA_PDE_t*) IOL_GetPDEInstance(iolMaster.ports[portNumber-1]))->cb_pdOut_cnf(
			clientId,
			portNumber,
			refArgBlockId,
			argBlockLen,
			argBlock);

	return IOL_ERRTYP_NONE;
}

/* @ref 11.2.19 SMI_PDInOut
 * @brief This service allows for periodically reading input from an InBuffer (see 11.7.2.1) and periodi-
 * cally reading output Process Data from an OutBuffer (see 11.7.3.1). Table 121 shows the
 * structure of the service.
 * */
IOL_ErrorTypes IOL_SMI_PDInOut_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
		void (*cb_pdInOut_cnf)(uint8_t clientId, uint8_t portNumber,
				IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock))
{
	uint16_t error = IOL_ERRTYP_NONE;
	IOL_Port_t* port = NULL;
	IOL_MA_PDE_t* pde = NULL;
	uint16_t argBlockId = SMI_ParseABIdFromABArray(argBlock);
	/*Check errors----------*/
	if(!(portNumber > 0U && portNumber < IOL_MAX_PORT)){
		/*incorrect Port number*/
		error = IOL_ERRTYPSMI_PORT_NUM_INVALID;
	}else if(expArgBlockId != IOL_ARGBLOCK_ID_PDINOUT &&
			argBlockId != IOL_ARGBLOCK_ID_VOIDBLOCK)
	{
		/*ArgBlock unknown*/
		error = IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED;
	}else if(!(argBlockLen >= IOL_ARGBLOCK_LEN_MIN &&
			argBlockLen <= IOL_ARGBLOCK_LEN_PDINOUT(IOL_MAX_PD_SIZE,IOL_MAX_PD_SIZE)))
	{
		/*(incorrect ArgBlock length*/
		error = IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID;
	}else{
		port = iolMaster.ports[portNumber-1];
		pde = (IOL_MA_PDE_t*) IOL_GetPDEInstance(port);

		if(pde->trigger == IOL_PDE_TRIG_STOP){
			/*Device not communicating*/
			error = IOL_ERRTYPSMI_DEVICE_NOT_IN_OPERATE;
		}
	}

	/*Convey the negative confirmation to the gateway application*/
	if(error != IOL_ERRTYP_NONE){
		SMI_ConveyNegCnf(clientId,
						 portNumber,
						 expArgBlockId,
						 IOL_ARGBLOCK_ID_PDINOUT,
						 error,
						 (void *)&IOL_SMI_PDInOut_Cnf);
		return error;
	}

	/*Set the callback*/
	pde->cb_pdInOut_cnf = cb_pdInOut_cnf;

	uint8_t* pdInOutBlock = (uint8_t *)malloc((pde->pdInLen + pde->pdOutLen)*sizeof(uint8_t));
	uint8_t pdINOutLen = 0;
	uint8_t pdInDataLen = pde->pdIn[3];						// copy the pdIn len for only data buffer
	uint8_t pdOutDataLen = pde->pdOut[3];					// copy the pdIn len for only data buffer
	pdInOutBlock[pdINOutLen++] = (uint8_t)(IOL_ARGBLOCK_ID_PDINOUT>>8);
	pdInOutBlock[pdINOutLen++] = (uint8_t)(IOL_ARGBLOCK_ID_PDINOUT);
	pdInOutBlock[pdINOutLen++] = pde->pdIn[2];							// copy PQI
	pdInOutBlock[pdINOutLen++] = pde->pdOut[2];   						// copy output enable byte
	pdInOutBlock[pdINOutLen++] = pdInDataLen;   						// copy pdin len
	memcpy(&pdInOutBlock[pdINOutLen], &pde->pdIn[4], pdInDataLen);	// copy 1 len byte + PDin data
	pdINOutLen += pdInDataLen;
	pdInOutBlock[pdINOutLen++] = pdOutDataLen;   // copy pdout len
	memcpy(&pdInOutBlock[pdINOutLen], &pde->pdOut[4], pdOutDataLen);	// copy 1 len byte + PDOut data
	pdINOutLen += pdOutDataLen;
	// convey the pd input data
	IOL_SMI_PDInOut_Cnf(clientId,
					 portNumber,
					 IOL_ARGBLOCK_ID_VOIDBLOCK,
					 pdINOutLen,
					 pdInOutBlock);

	free(pdInOutBlock);
	pdInOutBlock = NULL;
	return IOL_ERRTYP_NONE;
}

/* @brief IOL_SMI_MasterIdent_Cnf service is used by the upper layer to receive the response of
 * IOL_SMI_MasterIdent_Req service
 * */
IOL_ErrorTypes IOL_SMI_PDInOut_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock)
{

	((IOL_MA_PDE_t*) IOL_GetPDEInstance(iolMaster.ports[portNumber-1]))->cb_pdInOut_cnf(
			clientId,
			portNumber,
			refArgBlockId,
			argBlockLen,
			argBlock);
	//TODO: to implement later
	return IOL_ERRTYP_NONE;
}



/* IOLink SMI Handler--------------------------*/
/**
  * @brief  Initialize SMI with number of ports
  * @param  none
  * @retval none
  */
void IOL_SMI_Initialize( uint8_t numOfPort){
	IOL_InitMaster(&iolMaster, numOfPort);
	for(uint8_t i = 0; i < numOfPort; i++){
		IOL_DL_Initialize(IOL_GET_PORT(numOfPort));
	}


}

/*SMI handler*/
void IOL_SMI_Handler(uint8_t portNum){
	IOL_Port_t* port = IOL_GET_PORT(portNum);

	IOL_MA_CMHandler(port);
	IOL_SM_Handler(port);
	IOL_DL_Handler(port);
}

