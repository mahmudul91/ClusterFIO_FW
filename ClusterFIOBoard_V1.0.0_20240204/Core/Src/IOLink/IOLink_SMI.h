/*
 * IOL_SMI.h
 *
 *  Created on: Dec 19, 2023
 *      Author: wsrra
 */

#ifndef IOLINK_IOLINK_SMI_H_
#define IOLINK_IOLINK_SMI_H_

#include "IOLink_Utility.h"
// #include "IOLink_Define.h"
#include "IOLink.h"
#include "stdbool.h"
#include "IOLink_Types.h"


#define IOL_MAX_DS_SIZE 				2048
#define IOL_DIR_PARAM_REV_V10 			0x10
#define IOL_DIR_PARAM_REV_V11 			0x11
#define DS_STATE_PROPERTY_STATE_MASK 	(3 << 1)
#define DS_STATE_PROPERTY_STATE_LOCKED 	(3 << 1)
#define DS_STATE_PROPERTY_UPLOAD_REQ 	(1 << 7)
#define IOL_DS_IDX_LIST_MAX_ENTRS 		70
#define NULL 							((void *)0)







typedef enum IOL_DS_FAULT {
    IOL_DS_FAULT_NONE = 0,
    IOL_DS_FAULT_ID,
    IOL_DS_FAULT_SIZE,
    IOL_DS_FAULT_UP,
    IOL_DS_FAULT_DOWN,
    IOL_DS_FAULT_COM_ERR,
    IOL_DS_FAULT_LOCK,
} IOL_DS_FAULT;



//typedef struct IOL_ATTRIB_PACKED
//{
typedef union IOL_SMI_ArgBlock_t {
        IOL_ArgBlock_MasterIdent_t masterIdent;
        IOL_SMI_ArgBlock_PDIn_t pdIn;
        IOL_SMI_ArgBlock_PDOut_t pdOut;
        IOL_SMI_ArgBlock_PDInOut_t pdInOut;
        IOL_SMI_ArgBlock_OnRequestData_t onReqData;
        IOL_SMI_ArgBlock_DsData_t dsData;
        IOL_SMI_ArgBlock_PortConfigList_t portConfigList;
        IOL_SMI_ArgBlock_PortStatusList_t portStatusList;
        IOL_SMI_ArgBlock_DeviceEvent_t deviceEvent;
        IOL_SMI_ArgBlock_PortEvent_t portEvent;
        IOL_SMI_ArgBlock_VoidBlock_t voidBlock;
        IOL_SMI_ArgBlock_JobError_t errorBlock;
} IOL_SMI_ArgBlock_t;
//} IOL_SMI_ArgBlock_t;

//typedef union IOL_SMI_ArgBlockODE_t {
//        IOL_SMI_ArgBlock_OnRequestData_t onReqData;
//        IOL_SMI_ArgBlock_DevParaBatch_t devParBatch;
//        IOL_SMI_ArgBlock_VoidBlock_t voidBlock;
//        IOL_SMI_ArgBlock_JobError_t errorBlock;
//} IOL_SMI_ArgBlockODE_t;

typedef struct IOL_ATTRIB_PACKED IOL_SMI_Service_req_t
{
    IOL_ArgBlockID expArgBlockId;
    uint16_t arg_block_len;
    IOL_SMI_ArgBlock_t *argBlock;
    IOL_ErrorTypes result;
} IOL_SMI_Service_req_t;


/* CM block----------------------------------*/


void IOL_ODE_Init(IOL_Port_t *port);

// PDE
typedef enum IOL_ATTRIB_PACKED
{
    IOL_PDE_STATE_Inactive = 0,
    IOL_PDE_STATE_PDactive,
    IOL_PDE_STATE_LAST
} IOL_PDE_STATE;

typedef struct IOL_ATTRIB_PACKED
{
    IOL_PDE_STATE state;
    bool outputEnable;
} IOL_PDE_Port_t;



/* SMI_services---------------------------------------*/
/*currently used services*/
/*Services for CM--------------------------*/
IOL_ErrorTypes IOL_SMI_MasterIdent_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_MasterIdent_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PortConfig_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PortConfig_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_ReadbackPortConfig_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_ReadbackPortConfig_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PortStatus_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PortStatus_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

/*Services for DS--------------------------*/
IOL_ErrorTypes IOL_SMI_DSToParServ_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_DSToParServ_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_ParServToDS_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_ParServToDS_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

/*Services for ODE--------------------------*/
IOL_ErrorTypes IOL_SMI_DeviceWrite_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_DeviceWrite_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_DeviceRead_Req( uint8_t clientId, uint8_t portNumber,
    IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_DeviceRead_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_ParamWriteBatch_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_ParamWriteBatch_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_ParamReadBatch_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_ParamReadBatch_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

/*Services for ???--------------------------*/
IOL_ErrorTypes IOL_SMI_PortPowerOffOn_Req( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PortPowerOffOn_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

/*Services for DU--------------------------*/
IOL_ErrorTypes IOL_SMI_DeviceEvent_Ind( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_DeviceEvent_Rsp( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PortEvent_Ind( uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PortEvent_Rsp( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);


/*Services for PDE--------------------------*/
IOL_ErrorTypes IOL_SMI_PDIn_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
	void (*cb_pdIn_cnf)(uint8_t clientId, uint8_t portNumber,
			IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock));
IOL_ErrorTypes IOL_SMI_PDIn_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PDOut_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
	void (*cb_pdOut_cnf)(uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock));
IOL_ErrorTypes IOL_SMI_PDOut_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PDInOut_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock,
	void (*cb_pdInOut_cnf)(uint8_t clientId, uint8_t portNumber,
		IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock));
IOL_ErrorTypes IOL_SMI_PDInOut_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

/*Not used, but implemented for future*/
IOL_ErrorTypes IOL_SMI_PDInIQ_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PDInIQ_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PDOutIQ_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PDOutIQ_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);

IOL_ErrorTypes IOL_SMI_PDReadbackOutIIQ_Req( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID expArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);
IOL_ErrorTypes IOL_SMI_PDReadbackOutIIQ_Cnf( uint8_t clientId, uint8_t portNumber,
	IOL_ArgBlockID refArgBlockId, uint16_t argBlockLen, uint8_t *argBlock);


/* IOLink SMI Handler--------------------------*/
void IOL_SMI_Initialize( uint8_t numOfPort);
void IOL_SMI_Handler(uint8_t portNumber);


#endif /*end of IOLINK_IOLINK_SMI_H_*/
