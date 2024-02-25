/*
 * IOLink_MA.h
 *
 *  Created on: Dec 18, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_MA_H_
#define SRC_IOLINK_IOLINK_MA_H_
/*
 * IOLink master application layer
 * */
#include "IOLink.h"
#include "IOLink_Utility.h"
#include "IOLink_Define.h"
#include "IOLink_Types.h"
#include "IOLink_SM.h"




/*Configuration manager-----------------------------------*/

typedef enum IOL_ATTRIB_PACKED IOL_MA_CMState{
    IOL_MA_CM_STATE_CHECKPORTMODE_0 = 0,
    IOL_MA_CM_STATE_SM_STARTUP_1,
    IOL_MA_CM_STATE_DS_PARAMMANAGER_2,
    IOL_MA_CM_STATE_PORTFAULT_3,
    IOL_MA_CM_STATE_WAITINGONOPERATE_4,
    IOL_MA_CM_STATE_PORT_ACTIVE_5,
    IOL_MA_CM_STATE_PORT_DIDO_6,
    IOL_MA_CM_STATE_CONFIGMANAGER_7,
    IOL_MA_CM_STATE_PORT_DEACTIVATED_8,
    IOL_MA_CM_STATE_LAST_9,
} IOL_MA_CMState;

typedef enum IOL_ATTRIB_PACKED IOL_MA_CMTrigger{
    IOL_CM_TRIG_NONE = 0,
    IOL_CM_TRIG_IOL_MANUAL,    /* T1 */
    IOL_CM_TRIG_IOL_AUTOSTART, /* T2 */
    IOL_CM_TRIG_SM_COMREADY,   /* T3 */
    IOL_CM_TRIG_SM_FAULT,      /* T4 */
    IOL_CM_TRIG_SM_OPERATE,    /* T7 */
    IOL_CM_TRIG_SM_COMLOST,    /* T8 */
	IOL_CM_TRIG_SM_COMLOST_DS, /* T8 */
	IOL_CM_TRIG_DS_CHANGE,     /* T14 */
    IOL_CM_TRIG_DS_READY,      /* T5 */
    IOL_CM_TRIG_DS_FAULT,      /* T6 */
    IOL_CM_TRIG_DI_CQ,			/* T9 */ // TODO merge
    IOL_CM_TRIG_DO_CQ,			/* T10 */                     // TODO merge
    IOL_CM_TRIG_DEACTIVATED,   	/* T11 */
    IOL_CM_TRIG_PORTCNFLIST_CHANGED,    /* T12, T13 and T15 */
    IOL_CM_TRIG_UNKNOWN,       	/* T16 */
    IOL_CM_TRIG_LAST
} IOL_MA_CMTrigger;






typedef struct IOL_ATTRIB_PACKED IOL_MA_CMHandler_t {
	IOL_MA_CMState state;
	IOL_MA_CMTrigger trigger;

	IOL_SMI_ArgBlock_PortConfigList_t reqPortCfgList;
	IOL_SMI_ArgBlock_PortStatusList_t portStatusList;

	IOL_EventX_t portEvent;

//    IOL_SMI_Service_req_t smiReq;
//    IOL_CM_PortConfigList_t cfgList;
//    IOL_DS_FAULT dsFault;
//    IOL_CM_PortInfo_t portInfo;

} IOL_MA_CM_t;


/*Data Storage (DS)-----------------------------------*/

/* States of the ds state machine
 * Ref: 11.4.4 DS state machine
 * */
typedef enum IOL_ATTRIB_PACKED IOL_MA_DSState{
    IOL_DS_STATE_CHECKACTIVATIONSTATE_0 = 0,	// Check current state of the DS configuration: Independently from communication status, DS_Startup from configuration management or an Event DS_UPLOAD_REQ is expected.
    IOL_DS_STATE_WAITINGONDSACTIVITY_1,			// Waiting for upload request, Device startup, all changes of activation state independent of the Device communication state.
    IOL_DS_STATE_UP_DOWNLOAD_2,					// Submachine for up/download actions and checks
    IOL_DS_STATE_OFF_3,							// Data Storage handling switched off or deactivated
    /* UPLOADEDOWNLOAD_2 */
    IOL_DS_STATE_SM_CHECKIDENTITY_4,			// Check Device identification (DeviceID, VendorID) against parameter set within the Data Storage (see Table G.2). Empty content does not lead to a fault
    IOL_DS_STATE_SM_CHECKMEMSIZE_5,				// Check data set size (Index 3, Subindex 3) against available Master storage size
    IOL_DS_STATE_SM_CHECKUPLOAD_6,				// Check for DS_UPLOAD_FLAG within the DataStorageIndex (see Table B.10)
    IOL_DS_STATE_SM_UPLOAD_7,					// Submachine for the upload actions
    IOL_DS_STATE_SM_CHECKDSVALIDITY_8,			// Check whether stored data within the Master is valid or invalid. A Master could be replaced between upload and download activities. It is the responsibility of a Master designer to implement a validity mechanism according to the chosen use cases
    IOL_DS_STATE_SM_CHECKCHECKSUM_9,			// Check for differences between the data set content and the Device parameter via the "Parameter Checksum" within the DataStorageIndex (see Table B.10)
    IOL_DS_STATE_SM_DWNLOAD_10,					// Submachine for the download actions
    IOL_DS_STATE_SM_DS_READY_11,				// Prepare DS_Ready indication to the Configuration Management (CM)
    IOL_DS_STATE_SM_DS_FAULT_12,				// Prepare DS_Fault indication from "Identification_Fault", "SizeCheck_Fault", "Upload_Fault", and "Download_Fault" to the Configuration Management (CM)
    /* UPLOAD_7 */
    IOL_DS_STATE_SM_DECOMPOSE_IL_13,			// Read Index List within the DataStorageIndex (see Table B.10). Read content entry by entry of the Index List from the Device (see Table B.11).
    IOL_DS_STATE_SM_READPARAMETER_14,			// Wait until read content of one entry of the Index List from the Device is accomplished.
    IOL_DS_STATE_SM_STOREDATASET_15,			// Task of the gateway application: store entire data set according to Table G.1 and Table G.2
    IOL_DS_STATE_SM_UPLOADFAULT_16,				// Prepare Upload_Fault indication from "Device_Error" and "COM_ERROR" as input for the higher-level indication DS_Fault
    /* DOWNLOAD_10 */
    IOL_DS_STATE_SM_DECOMPOSE_SET_17,			// Write parameter by parameter of the data set into the Device according to Table G.1.
    IOL_DS_STATE_SM_WRITE_PARAMETER_18,			// Wait until write of one parameter of the data set into the Device is accomplished.
    IOL_DS_STATE_SM_DOWNLOAD_DONE_19,			// Download completed. Read back "Parameter Checksum" from the DataStorageIndex according to Table B.10. Save this value in the stored data set according to Table G.2
    IOL_DS_STATE_SM_DOWNLOAD_FAULT_20,			// Prepare Download_Fault indication from "Device_Error" and "COM_ERROR" as input for the higher-level indication DS_Fault.

} IOL_MA_DSState;




typedef enum IOL_ATTRIB_PACKED IOL_MA_DSTrigger{
    IOL_DS_TRIG_NONE,
    IOL_DS_TRIG_ENABLED,       /* T1 */
	IOL_DS_TRIG_CLEARED,       /* T11 + T7 */
	IOL_DS_TRIG_DISABLED, 		/* T7 + T12 */
    IOL_DS_TRIG_STARTUP,     /* T2 or T14 */
	IOL_DS_TRIG_DELETE,      /* T9 + T10 */
    IOL_DS_TRIG_READY,       /* T25, T26 or T27; + T3 */
    IOL_DS_TRIG_UPLOAD,      /* T4 or T13 */
	IOL_DS_TRIG_UPLOAD_DONE,    /* T26 */
	IOL_DS_TRIG_DOWNLOAD_DONE,    /* T41 */
//    IOL_DS_TRIG_FAULT_DONE,  /* T5 */
//    IOL_DS_TRIG_FAULT_ID,    /* T15 */
//    IOL_DS_TRIG_FAULT_SIZE,  /* T17 */
//    IOL_DS_TRIG_FAULT_UL,    /* T23 */
//    IOL_DS_TRIG_FAULT_DL,    /* T28 */
//    IOL_DS_TRIG_FAULT_LOCK,  /* T29 */
//    IOL_DS_TRIG_ENABLE_COM,  /* T6 */
//    IOL_DS_TRIG_CLR_DISA,    /* T7 */
//    IOL_DS_TRIG_ENABLE_NO_C, /* T8 */



//    IOL_DS_TRIG_PASSED,      /* T16, T18 or T22 */
//    IOL_DS_TRIG_DO_UPLOAD,   /* T19 or T21 */
//    IOL_DS_TRIG_NO_UPLOAD,   /* T20 or T43 */

//
//    IOL_DS_TRIG_MORE_DATA,  /* T30 or T37 */
//    IOL_DS_TRIG_READ_DONE,  /* T31 */
//    IOL_DS_TRIG_STORE_DATA, /* T35 */

//    IOL_DS_TRIG_DEV_ERR,    /* T32 or T39 */
//    IOL_DS_TRIG_COM_ERR,    /* T33, T34, T36, T40 or T42 */
//
//    IOL_DS_TRIG_WR_DONE, /* T38 */

//    IOL_DS_TRIG_INIT,    /* Not in spec. */
//    IOL_DS_TRIG_LAST,
} IOL_MA_DSTrigger;

typedef enum IOL_ATTRIB_PACKED IOL_MA_DSTriggerComx{
	IOL_DS_TRIG_COMX_NONE = 0,
	IOL_DS_TRIG_COMX_OK,
	IOL_DS_TRIG_COMX_NOTOK,
	IOL_DS_TRIG_COMX_ERROR,
}IOL_MA_DSTriggerComx;

/*Service trigger-------------*/
typedef enum IOL_ATTRIB_PACKED IOL_MA_DSTriggerSerice{
	IOL_DS_TRIG_SRVC_NONE = 0,
	IOL_DS_TRIG_SRVC_ALREAD_CNF,
	IOL_DS_TRIG_SRVC_ALREAD_REQ,
	IOL_DS_TRIG_SRVC_ALWRITE_CNF,
	IOL_DS_TRIG_SRVC_ALWRITE_REQ,

}IOL_MA_DSTriggerSerice;


/* Data storage configuration parameters
 * Ref: B.2.3 DataStorageIndex
 * The parameter DataStorageIndex 0x0003 contains all the information to be used for the Data
 * Storage handling This parameter is reserved for private exchanges between the Master and
 * the Device; the Master shall block any write access request from a gateway application to this
 * Index (see Figure 5). The parameters within this Index 0x0003 are specified as follows.
 * */
typedef struct IOL_ATTRIB_PACKED IOL_DSIndexList_t{
   uint16_t index;
   uint8_t subindex;
} IOL_DSIndexList_t;

typedef union IOL_ATTRIB_PACKED IOL_DSStateProperty_t{
	uint8_t octet;
	struct{
		uint8_t resvd1		: 1;
		uint8_t dsState		: 2;
		uint8_t resvd2		: 4;
		uint8_t uploadFlag 	: 1;
	}field;
}IOL_DSStateProperty_t;

typedef struct IOL_ATTRIB_PACKED IOL_MA_DSStorageConfig_t{
//	uint16_t index;
//	uint8_t subindex;
	IOL_DSCommand cmd;
	IOL_DSStateProperty_t statPrpty;
	uint32_t storgSize;		// Number of octets for storing all the necessary information for the Device replacement (see 10.4.5). Maximum size is 2 048 octets.
	uint32_t parmChksum;	// Parameter set revision indication: CRC signature or Revision Counter (see 10.4.8)
	uint8_t ilCount;
	uint8_t ilPos;
	IOL_DSIndexList_t indexList[IOL_MAX_INDEX_LIST_ENTRIES+2];	// 2 added for 2-byte Termination_Marker
}IOL_MA_DSStorageConfig_t;

/* Structure of the Data Storage data object
 * REF: Annex G (G1 & G2)
 * */
/*REF: Table G.1 – Structure of the stored DS data object
 * */
typedef struct IOL_ATTRIB_PACKED IOL_MA_DSDataObject_t{
	uint16_t index;
	uint8_t subindex;
	uint8_t len;
	uint8_t* data;
}IOL_MA_DSDataObject_t;

typedef struct IOL_ATTRIB_PACKED IOL_MA_DSStoredData_t{

	/* Table G2
	 * store in non-volatile memory
	 * */
	uint32_t checksum;
    uint16_t vendorId;
    uint32_t deviceId;
    uint16_t functionId;

    /* Table G1
     * store in non-volatile memory
     * */
    uint8_t data[IOL_MAX_DS_STORG_SIZE];

    /* Table B.10- Subindex: 03
     * no need to store in non-volatile memory
     * */
    uint32_t storgSize;		// Number of octets for storing all the necessary information for the Device replacement (see 10.4.5). Maximum size is 2 048 octets.
    uint8_t valid;
    uint32_t dataLen;
//    IOL_MA_DSDataObject_t* dataObj;
}IOL_MA_DSStoredData_t;

//typedef struct IOL_ATTRIB_PACKED IOL_MA_DSStoredInfo_t{
//    uint16_t vendorID;
//    uint32_t deviceID;
//}IOL_MA_DSStoredInfo_t;

typedef struct IOL_ATTRIB_PACKED IOL_MA_DSHandler_t {
	IOL_MA_DSState state;
//	struct{
//		uint8_t enabled  : 1;
//		uint8_t disabled : 1;
//		uint8_t cleared  : 1;
//
//	};
	IOL_MA_DSTrigger trigger;
	IOL_MA_DSTriggerComx trigComX;
	IOL_MA_DSTriggerSerice trigSrvic;
	IOL_PortEventCodes faultEventCode;							// it should be the event code
	IOL_MA_DSStoredData_t storedDataSet;		// stored parameter set
	IOL_MA_DSStorageConfig_t devDSConfig;
	IOL_ErrorTypes errInfALCnf;
	uint16_t currentIndex;
	uint8_t currentSubindex;
	struct{
		uint8_t len;
		uint8_t* data;
	}alReadCnf;
	//	IOL_SMI_ArgBlock_DsData_t reqPortCfgList;
}IOL_MA_DS_t;


/* ODE--------------------------*/
// section 11.5 Ref table: 128
typedef enum IOL_ATTRIB_PACKED IOL_MA_ODEState{
    IOL_ODE_STATE_INACTIVE_0 = 0,
    IOL_ODE_STATE_ODACTIVE_1,
    IOL_ODE_STATE_ODBLOCKED_2,
    IOL_ODE_STATE_LAST // not in doc
} IOL_MA_ODEState;

typedef enum IOL_ATTRIB_PACKED IOL_MA_ODETrigger{
    IOL_ODE_TRIG_NONE = 0,
    IOL_ODE_TRIG_SMI_DEVICEREAD,
	IOL_ODE_TRIG_SMI_DEVICEWRITE,
	IOL_ODE_TRIG_SMI_PARAMREADBATCH,
	IOL_ODE_TRIG_SMI_PARAMWRITEBATCH,
	IOL_ODE_TRIG_OD_START,
    IOL_ODE_TRIG_OD_STOP,
    IOL_ODE_TRIG_OD_BLOCK,
    IOL_ODE_TRIG_OD_UNBLOCK,
    IOL_ODE_TRIG_LAST
} IOL_MA_ODETrigger;

typedef struct IOL_MA_ODEJob_t {
	uint8_t jobId;
	uint8_t clientId;
	union{
	    IOL_SMI_ArgBlock_OnRequestData_t* reqOdeBlock;
	    IOL_SMI_ArgBlock_DevParaBatch_t* reqDevParaBatch;
	};
} IOL_MA_ODEJob_t;



typedef struct IOL_ATTRIB_PACKED IOL_MA_ODE_t{
	IOL_MA_ODEState state;
	IOL_MA_ODETrigger trigger;
	uint8_t currentJobId;
	uint8_t jobCount;
    IOL_MA_ODEJob_t jobList;

//    IOL_SMI_ArgBlock_OnRequestData_t reqOdeBlock;
//    IOL_SMI_ArgBlock_DevParaBatch_t reqDevParaBatch;
} IOL_MA_ODE_t;

/* Diagnosis Unit (DU)------------------------------------------------*/
/*Ref: 11.6 Diagnosis Unit (DU)*/

/*	DU states
 * Not in spec, we made the state machine for ourselves
 * Ref: IOL-Interface-Spec_10002_V113_Jun19_ACL_V1.0, Section: 11.6.7
 * */
typedef enum IOL_ATTRIB_PACKED IOL_MA_DUState{
    IOL_DU_STATE_INACTIVE_0 = 0,            // Waiting for activation
    IOL_DU_STATE_IDLE_1,                	// Waiting for device and port events indication by AL_Event.ind service
    IOL_DU_STATE_EVENTHANDLING_2,       	// Propagating the received device and port events to the gateway application by using SMI_xEvent.ind service and checking for the DS_Upload_Req event. Also, storing events to the diagnosis status that can be retrieved by the SMI_PortStatus service in PortStatusList (see Table E.4 and 11.6.4).
    IOL_DU_STATE_WAITINGFOREVENTACK_3,  	// The Master DU remains in this state as long as the gateway application did not acknowledge the SMI_xEvent.ind.
}IOL_MA_DUState;

/* Trigger for the states */
typedef enum IOL_ATTRIB_PACKED IOL_MA_DUTrigger{
    IOL_DU_TRIG_NONE = 0,
    IOL_DU_TRIG_START,
	IOL_DU_TRIG_STOP,
	IOL_DU_TRIG_AL_EVENT_IND,
} IOL_MA_DUTrigger;

/*	bit 0: device even
 *  bit 1: port event
 *  bit 2 to 7: reserved
 * */
typedef enum IOL_ATTRIB_PACKED IOL_MA_DUEventAck{
	IOL_DU_EVENTACK_NONE = 0x00,
	IOL_DU_EVENTACK_DEVICE = 0x01,
	IOL_DU_EVENTACK_PORT = 0x02,
}IOL_MA_DUEventAck;

/* Struct of diagnosis unit */
typedef struct IOL_ATTRIB_PACKED IOL_MA_DU_t{
	IOL_MA_DUState state;
	IOL_MA_DUTrigger trigger;
	uint8_t eventAck; 				/* bit 0: device & bit 1: port event, bit 2 to 7: reserved */
	uint8_t numOfEvents;
	uint8_t eventCount;
	IOL_EventX_t events[IOL_MAX_EVENT];
}IOL_MA_DU_t;

/* PDE-------------------------------------------------------------------*/
/* Trigger for the states */
typedef enum IOL_ATTRIB_PACKED IOL_MA_PDETrigger{
    IOL_PDE_TRIG_NONE = 0,
    IOL_PDE_TRIG_START,
	IOL_PDE_TRIG_STOP,

} IOL_MA_PDETrigger;



typedef struct IOL_ATTRIB_PACKED IOL_MA_PDE_t{
	IOL_MA_PDETrigger trigger;
	uint8_t pdInLen;
	uint8_t pdOutLen;
	uint8_t* pdIn;
	uint8_t* pdOut;
	void (*cb_pdIn_cnf)(
			uint8_t clientId,
			uint8_t portNumber,
			IOL_ArgBlockID refArgBlockId,
			uint16_t argBlockLen,
			uint8_t *argBlock);
	void (*cb_pdOut_cnf)(
			uint8_t clientId,
			uint8_t portNumber,
			IOL_ArgBlockID refArgBlockId,
			uint16_t argBlockLen,
			uint8_t *argBlock);
	void (*cb_pdInOut_cnf)(
			uint8_t clientId,
			uint8_t portNumber,
			IOL_ArgBlockID refArgBlockId,
			uint16_t argBlockLen,
			uint8_t *argBlock);
}IOL_MA_PDE_t;

/* IOLink master application layer --------------------------------------*/
typedef struct IOL_ATTRIB_PACKED IOL_MA_t{
	IOL_MA_CM_t cm;
	IOL_MA_DS_t ds;
	IOL_MA_ODE_t ode;
	IOL_MA_DU_t du;
	IOL_MA_PDE_t pde;
}IOL_MA_t;

/*Services----------------------------------------------------------------*/



/*Services to receive confirmation from other layers----------------------*/

/*AL services----*/

void IOL_CM_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len, IOL_ErrorTypes errorInfo);
void IOL_ODE_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len, IOL_ErrorTypes errorInfo);
void IOL_DS_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len,  IOL_ErrorTypes errorInfo);


void IOL_CM_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo);
void IOL_ODE_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo);
void IOL_DS_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo);
IOL_Errors IOL_DU_AL_Event_Ind(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event);

/*SM cnf service---------------------------------------------------------*/
void IOL_CM_SM_PortMode_Ind(IOL_Port_t *port, uint8_t portNum, IOL_SM_PortMode mode);

/*State machines---------------------------------------------------------*/
void IOL_MA_CMHandler(IOL_Port_t *port);
void IOL_MA_DSHandler(IOL_Port_t *port);
void IOL_MA_ODEHandler(IOL_Port_t *port);
void IOL_MA_DUHandler(IOL_Port_t *port);		// not in spec, we made it

#endif /* SRC_IOLINK_IOLINK_MA_H_ */
