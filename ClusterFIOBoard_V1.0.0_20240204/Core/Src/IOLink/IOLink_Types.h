/*
 * IOLink_Types.h
 *
 *  Created on: Nov 20, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_TYPES_H_
#define SRC_IOLINK_IOLINK_TYPES_H_

#include "IOLink_Utility.h"
#include "IOLink_Define.h"




/*A.1.2 M-sequence Control (MC) octet
 * */
typedef union IOL_MseqCtrlOctet_t{
	uint8_t octet;
	struct {
		uint8_t address 	: 5;	// LSB, Bit 0 to 4: Address
	    uint8_t comCh 		: 2;	// Bit 5 to 6: Communication channel
		uint8_t rw 			: 1;	// MSB, Bit 7: R/W
	}field;
}IOL_MseqCtrlOctet_t;

/* A.1.3 Checksum / M-sequence type (CKT)
 * */
typedef union IOL_CKTOctet_t{
	uint8_t octet;
	struct {
		uint8_t checksum : 6;	//LSB, Bit 0 to 5: Checksum
		uint8_t mseqTyp  : 2;	//MSB, Bit 6 to 7: M-sequence type
	}field;
}IOL_CKTOctet_t;

/* A.1.5 Checksum / status (CKS) */
typedef union IOL_CKSOctet_t{
	uint8_t octet;
	struct {
		uint8_t checksum : 6;	// LSB, Bit 0 to 5: Checksum
		uint8_t pdStatus : 1;	// Bit 6: PD status
		uint8_t evntFlag : 1;	// MSB, Bit 7: Event flag
	}field;
}IOL_CKSOctet_t;

/* A.5.2 I-Service */
typedef union IOL_IServiceOctet_t{
	uint8_t octet;
	struct {
		uint8_t length 		: 4;	// LSB, Bit 0 to 3: lenght
		uint8_t iService 	: 4;	// MSB: Bits 4 to 7: I-Service
	}field;
}IOL_IServiceOctet_t;

/* A.6.2 StatusCode type 1 (no details) */
typedef union IOL_StatusCode1Octet_t{
	uint8_t octet;
	struct {
		uint8_t evtCode 	: 5;	// LSB, Bits 0 to 4: EventCode (type 1)
		uint8_t reserved   	: 1; 	// Bit 5: Reserved
		uint8_t pdInvalid 	: 1;	// Bit 6: Reserved, This bit is used in legacy protocol (see [8]) for PDinvalid indication
		uint8_t evtDtail	: 1;    // MSB, Bit 7: Event Details
	}field;
}IOL_StatusCode1Octet_t;

/* A.6.3 StatusCode type 2 (with details) */
typedef union IOL_StatusCode2Octet_t{
	uint8_t octet;
	struct {
		uint8_t actvtdEvt 	: 6;	// LSB, Bits 0 to 5: Activated Events
		uint8_t reserved   	: 1; 	// Bit 6: Reserved
		uint8_t evtDtail	: 1;    // MSB, Bit 7: Event Details
	}field;
}IOL_StatusCode2Octet_t;

/* A.6.4 EventQualifier */
typedef union IOL_EventQualOctet_t{
	uint8_t octet;
	struct {
		uint8_t instance 	: 3;	// LSB, Bits 0 to 2: INSTANCE
		uint8_t source   	: 1; 	// Bit 3: SOURCE
		uint8_t type		: 2;    // Bits 4 to 5: TYPE
		uint8_t mode		: 2;    // Bits 6 to 7: MODE
	}field;
}IOL_EventQualOctet_t;


/*B.1.3 MasterCycleTime and MinCycleTime
 * */
typedef union IOL_CycTimOctet_t{
	uint8_t octet;
	struct{
		uint8_t multiplier 	: 6; 	// LSB bit 0 to 5
		uint8_t timeBase	: 2; 	// MSB bit 6 & 7
	}field;
}IOL_CycTimOctet_t;

/*B.1.4 M-sequenceCapability
 * */
typedef union IOL_MseqCapbOctet_t{
	uint8_t octet;
	struct{
		uint8_t isdu 		: 1; 	// LSB bit 0
		uint8_t msqTypOp 	: 3; 	// OPERATE M-sequence type bit 1 to 3
		uint8_t msqTypProp	: 2; 	// PREOPERATE M-sequence type bit 4 to 5
		uint8_t reserved   	: 1; 	// MSB bit 6 & 7
	}field;
}IOL_MseqCapbOctet_t;

/* B.1.5 RevisionID (RID)
 * */
typedef union IOL_RevIDOctet_t{
	uint8_t octet;
	struct{
		uint8_t majorRev 	: 4; 	// LSB bit 0 to 3
		uint8_t minorRev	: 4; 	// MSB bit 4 & 7
	}field;
}IOL_RevIDOctet_t;


/*B.1.6 ProcessDataIn
 * */
typedef union IOL_PDInOctet_t{
	uint8_t octet;
	struct{
		uint8_t len 		: 5; 	// LSB bit 0 to 4
		uint8_t reserved 	: 1; 	// reserved bit 5
		uint8_t sio			: 1; 	// bit 6
		uint8_t byte    	: 1; 	// MSB bit 7
	}field;
}IOL_PDInOctet_t;

/*B.1.6 ProcessDataOut
 * */
typedef union IOL_PDOutOctet_t{
	uint8_t octet;
	struct{
		uint8_t len 		: 5; 	// LSB bit 0 to 4
		uint8_t reserved 	: 2; 	// reserved bits 5 & 6
		uint8_t byte    	: 1; 	// MSB bit 7
	}field;
}IOL_PDOutOctet_t;


/* Value list record for the ISDU transport
 * Ref: IO-Link Spec v1.1.3, section 7.2.16,
 * */
typedef struct 	IOL_ATTRIB_PACKED IOL_ISDUValueList_t{
	uint16_t index;				//Permitted values: 2 to 65535
	uint8_t subindex;			//Permitted values: 0 to 255
	uint8_t *data;				//Parameter type: Octet string
	uint8_t dataLen;   			//Length of the data
	IOL_RWDirection rwDir;
}IOL_ISDUValueList_t;

/* Value list record for the DL mode
 * Ref: IO-Link Spec v1.1.3, section 7.2.1.13
 * */
typedef struct IOL_ATTRIB_PACKED IOL_ModeValueList_t{
   uint8_t mseqTime;
   IOL_MseqSubType mseqType;
   uint8_t pdInLen;
   uint8_t pdOutLen;
   uint8_t odLenPerMsg;
} IOL_ModeValueList_t;

/* Parameter list record for the SM Port Config
 * Ref: IO-Link Spec v1.1.3, section 9.2.2.2
 * */
typedef struct IOL_ATTRIB_PACKED IOL_SM_ParamList_t{
	uint8_t portNum;
	uint8_t cycleTime;
	IOL_TargetMode targetMode;
	IOL_BaudRate baudRate;
	uint8_t revId;
	IOL_InspecLevel inspecLevel;
	uint16_t vendId;
	uint32_t devId;
	uint16_t funcId;
	uint16_t SrlNum;
}IOL_SM_ParamList_t;

typedef struct IOL_ATTRIB_PACKED IOL_EventX_t{
	IOL_EventQualOctet_t eventQual;
	uint16_t eventCode;
}IOL_EventX_t;


/*Annex: E --------------------------------------*/

// Ref: Table E.2 – MasterIdent
typedef union IOL_Features1_t{
	uint8_t octet;
	struct{
		uint8_t parWriteBatch 		: 1;     // Bit 0: DeviceParBatch (SMI_ParamWriteBatch)
		uint8_t parReadBatch 	: 1; // Bit 1: DeviceParReadBatch (SMI_ParamReadBatch)
		uint8_t portPowerOffOn 		: 1;     // Bit 2: PortPowerOffOn (SMI_PortPowerOffOn)
		uint8_t reserved 			: 5;          // Bits 3 to 7: Reserved (= 0)
	}field;
} IOL_SMI_Features1_t;

/*MasterIdent Argument Block--------*/
/* E.2 MasterIdent
 * Table E.2
 */
typedef struct IOL_ATTRIB_PACKED IOL_ArgBlock_MasterIdent_t{
    IOL_ArgBlockID argBlockID;  		// Unique ID (0x0001)
    uint16_t vendorID;              	// Unique VendorID of the Master (see B.1.8)
    uint32_t masterID;              	// 4 octets long vendor-specific unique identification of the Master
    IOL_MasterType masterType;      	// MasterType (0 to 4)
    IOL_SMI_Features1_t features1; 		// Bitfield for Features_1
    uint8_t features2;              	// Reserved for future use
    uint8_t maxNumberOfPorts;       	// Maximum number (n) of ports of this Master (1 to 255)
    uint8_t portTypes[IOL_MAX_PORT];    // Array indicating port types for all n ports (1 to 6)
} IOL_ArgBlock_MasterIdent_t;



/*PortConfigList Argument Block---------*/
/*
 * Ref: IO-Link Interface Spec v1.1.3 Table E.3 – PortConfigList
 */
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PortConfigList_t{
    IOL_ArgBlockID argBlockId; 				// IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST
    IOL_SMI_PortMode portMode;              // Port Mode (0 to 4, 5 to 48 reserved, 49 to 96 reserved, 97 to 255 manufacturer specific)
    IOL_SMI_ValidBackup validBackup;	 	// Validation & Backup (0 to 4, 5 to 255 reserved)
    IOL_SMI_IQBehavior iQBehavior;          // I/Q Behavior (0 to 4, 5 to 255 reserved)
    uint8_t portCycTime;                    // Port Cycle Time (0 to 255, 0: AFAP, 1 to 255: TIME)
    uint16_t vendorID;                      // VendorID (1 to 0xFFFF)
    uint32_t deviceID;                      // DeviceID (1 to 0xFFFFFF)
} IOL_SMI_ArgBlock_PortConfigList_t;


/*PortStatusList Argument Block-------*/
/* Ref: Table E.4 – PortStatusList */
/* PortQualityInfo, 3
 * */
typedef union IOL_ATTRIB_PACKED IOL_SMI_PortQualInfo_t{
	uint8_t octet;
	struct{
		uint8_t valid 		: 1;    	// Bit0: 0 = VALID, 1 = INVALID
		uint8_t pdOutValid 	: 1; 		// Bit1: 0 = PDOUTVALID, 1 = PDOUTINVALID
		uint8_t reserved 	: 6;   		// Bits 2 to 7: Reserved
	}field;
} IOL_SMI_PortQualInfo_t;



///* PortQualityInfo, octet offset: 16 to n
// * */
//typedef struct IOL_ATTRIB_PACKED IOL_SMI_DiagEntry_t{
//    uint8_t eventQual;
//    IOL_DeviceEventCodes eventCode;
//} IOL_SMI_DiagEntry_t;


/* Table E.4 – PortStatusList */
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PortStatusList_t{
    IOL_ArgBlockID argBlockID;         		// Unique ID (0x9000)
    IOL_PortStatusInfo portStatusInfo;   	// Port Status Info (0 to 255)
    IOL_SMI_PortQualInfo_t portQualInfo;	// Port Quality Info (Bitfield)
    uint8_t revisionID;                    	// Revision ID (0 to 255)
    IOL_TransRate transRate; 				// Transmission Rate (0 to 255)
    uint8_t masterCycTime;               	// Master Cycle Time
    uint8_t inDataLen;               		// Input Data Length (0 to 0x20)
    uint8_t outDataLen;       	       		// Output Data Length (0 to 0x20)
    uint16_t vendorID;                     	// VendorID (1 to 0xFFFF)
    uint32_t deviceID;                     	// DeviceID (1 to 0xFFFFFF)
    uint8_t numOfDiags;                 	// Number of Diagnosis Entries (0 to 255)
    IOL_EventX_t diagEntries[IOL_MAX_DIAG_ENTRIES];      		// Array of Diagnosis Entries
    // TODO: need to understand array length
} IOL_SMI_ArgBlock_PortStatusList_t;

/* On-requestData Argument Block------*/
/*Ref: E.5 On-request_Data*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_OnRequestData_t{
    IOL_ArgBlockID argBlockID; 				// IOLINK_ARG_BLOCK_ID_OD_{WR,RD}
    uint16_t index;							// This element contains the Index to be used for the AL_Write or AL_Read service
    uint8_t subindex;						// This element contains the Subindex to be used for the AL_Write or AL_Read service
    uint8_t data[IOL_MAX_OD_SIZE];							// This element contains the On-request Data for ArgBlock 0x3000 if available.
    // TODO: need to understand array length
} IOL_SMI_ArgBlock_OnRequestData_t;


/* DS_Data Argument Block-----*/
/*Ref: E.6 DS_Data*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_DsData_t{
    IOL_ArgBlockID argBlockID; // IOLINK_ARG_BLOCK_ID_DS_DATA
    uint32_t checksum;
    uint16_t vendorId;
    uint32_t deviceId;
    uint16_t functionId;
    uint8_t data[];	// TODO: need to understand array length
} IOL_SMI_ArgBlock_DsData_t;


/* DeviceParBatch Argument Block-----*/

typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_DevParam_t{
	uint16_t index;
	uint8_t subindex;
	/*Shared memory for length and isduError*/
	union{
		uint8_t length;
		uint8_t isduError;
	};
	/*Shared memory for length and isduError*/
	union{
		uint8_t* data;	//TODO: need to understand array length
		IOL_ErrorTypes isduErrType;
	};
}IOL_SMI_ArgBlock_DevParam_t;

/*Ref: E.7 DeviceParBatch*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_DevParaBatch_t{
	IOL_ArgBlockID argBlockID;
	uint8_t numOfdevPara;
	IOL_SMI_ArgBlock_DevParam_t* devParaBatch;
}IOL_SMI_ArgBlock_DevParaBatch_t;

/* IndexList Argument Block-----*/
/*Ref: E.8 IndexLis*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_IndexListEntry_t{
    uint16_t index;
    uint8_t subIndex;
} IOL_SMI_IndexListEntry_t;

typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_IndexList_t{
	IOL_ArgBlockID argBlockID;
	IOL_SMI_IndexListEntry_t indexList[];	//TODO: need to understand array length
}IOL_SMI_ArgBlock_IndexList_t;



/* PortPowerOffOn Argument Block-----*/
/*Ref: E.9 PortPowerOffOn*/
/*Table E.9 – PortPowerOffOn */
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PortPowerMode_t{
	IOL_ArgBlockID argBlockID;
	IOL_SMI_ArgBlock_portPowerMode mode;// PortPowerMode
	uint16_t powerOffTime;				//Duration of Master port power off (ms).
}IOL_SMI_ArgBlock_PortPowerMode_t;


/* PDIn Argument Block------*/
/*Ref: E.10 PDIn*/
typedef union IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PDIn_t{
	uint16_t argBlockID;      	// Unique ID (0x1001)
	uint8_t portQualInfo;               // Port Qualifier Information
	uint8_t pdiLen;            			// Length of Device's input Process Data
	uint8_t* pdiData;
//	uint8_t pdiData[IOL_MAX_PD_SIZE]; 	// Input Process Data (up to 32 octets)
} IOL_SMI_ArgBlock_PDIn_t;


/* PDOut Argument Block-----*/
/*Ref: E.11 PDOut*/
typedef union IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PDOut_t{
	uint16_t argBlockID;       	// Unique ID (IOL_SMI_ArgBlockID_PDOut)
	uint8_t enable;                		// Output Enable
	uint8_t pdoLen;            			// Length of output Process Data
	uint8_t* pdoData; 	// Output Process Data (up to 32 octets)
//	uint8_t pdoData[IOL_MAX_PD_SIZE]; 	// Output Process Data (up to 32 octets)
} IOL_SMI_ArgBlock_PDOut_t;

/* PDInOut Argument Block-----*/
/*Ref: E.12 PDInOut*/
typedef union IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PDInOut_t{
	uint16_t argBlockID;      	// Unique ID (Table E.12)
	uint8_t portQualInfo;               // Port Qualifier Information
	uint8_t pdoEnable;       	        // Output Enable
	uint8_t pdiLen;  			        // Length of Device's input Process Data
	uint8_t* pdiData;  					// Input Process Data (up to 32 octets)
//	uint8_t pdiData[IOL_MAX_PD_SIZE];  	// Input Process Data (up to 32 octets)
	uint8_t pdoLen;           			// Length of output Process Data
	uint8_t pdoData;				 	// Output Process Data (up to 32 octets)
//	uint8_t pdoData[IOL_MAX_PD_SIZE]; 	// Output Process Data (up to 32 octets)
} IOL_SMI_ArgBlock_PDInOut_t;


/* PDInIQ Argument Block------*/
/*Ref: E.13 PDInIQ*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PDInIQ_t{
	IOL_ArgBlockID argBlockID; 			// Unique ID (Table E.13)
    uint8_t pdi0;    					// Input Process Data I/Q signal (octet 0)
} IOL_SMI_ArgBlock_PDInIQ_t;


/* PDOutIQ Argument Block-----*/
/*Ref: E.14 PDOutIQ*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PDOutIQ_t{
	IOL_ArgBlockID argBlockID; 			// Unique ID (Table E.14 - 0x1FFF)
    uint8_t pdo0;   					// Output Process Data I/Q signal (octet 0)
} IOL_SMI_ArgBlock_PDOutIQ_t;


/* DeviceEvent Argument Block-----*/
/*Ref: E.15 DeviceEvent*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_DeviceEvent_t{
	IOL_ArgBlockID argBlockID;			// Unique ID (Table E.15 - 0xA000)
    IOL_EventQualOctet_t qualifier; 	// EventQualifier: Bit 0:2 - INSTANCE, Bit 3-SOURCE, Bit 4:5 - TYPE
    IOL_DeviceEventCodes code;     		// EventCode according to Table D.1
} IOL_SMI_ArgBlock_DeviceEvent_t;

/* PortEvent Argument Block-----*/
/*Ref: E.16 PortEvent*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_PortEvent_t{
	IOL_ArgBlockID argBlockID; 			// Unique ID (Table E.15 - 0xA000)
	IOL_EventQualOctet_t qualifier;     // EventQualifier: Bit 0:2 - INSTANCE, Bit 3-SOURCE, Bit 4:5 - TYPE
    IOL_PortEventCodes code;  			// EventCode according to Table D.2
} IOL_SMI_ArgBlock_PortEvent_t;


/* VoidBlock Argument Block-----*/
/*Ref: E.17 VoidBlock*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_VoidBlock_t{
	IOL_ArgBlockID argBlockID;
} IOL_SMI_ArgBlock_VoidBlock_t;


/* JobError Argument Block----*/
/*Ref:  E.18 JobError*/
typedef struct IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_JobError_t{
	IOL_ArgBlockID argBlockID;
	IOL_ArgBlockID expArgBlockID;
	IOL_ErrorTypes errorCode;
} IOL_SMI_ArgBlock_JobError_t;


#endif /* SRC_IOLINK_IOLINK_TYPES_H_ */
