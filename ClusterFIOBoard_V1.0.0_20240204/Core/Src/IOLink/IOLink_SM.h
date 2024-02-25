/*
 * IOLink_SM.h
 *
 *  Created on: Nov 3, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_SM_H_
#define SRC_IOLINK_IOLINK_SM_H_
#include "IOLink_Utility.h"
#include "IOLink_Define.h"
#include "IOLink.h"
#include "IOLink_Types.h"


/*System management job handler-----------*/
// TODO: not used, to remove
//typedef enum IOL_ATTRIB_PACKED IOL_SM_ReadJobState
//{
//	IOL_SM_RJOB_STATE_INACTIVE_0 = 0,
//	IOL_SM_RJOB_STATE_REQUEST_1,
//	IOL_SM_RJOB_STATE_AWAIT_2,
//	IOL_SM_RJOB_STATE_RESPONSE_3,
//	IOL_SM_RJOB_STATE_CHECK_LEFTREQ_4,
//} IOL_SM_ReadJobState;
//
//typedef enum IOL_ATTRIB_PACKED IOL_SM_ReadJobStatus
//{
//	IOL_SM_RJOB_STATUS_NONE = 0,
//	IOL_SM_RJOB_STATUS_BUSY,
//	IOL_SM_RJOB_STATUS_RESP_REDY,
//	IOL_SM_RJOB_STATUS_DONE,
//	IOL_SM_RJOB_STATUS_TIMEOUT,
//	IOL_SM_RJOB_STATUS_ERROR,
//} IOL_SM_ReadJobStatus;
//
//typedef enum IOL_ATTRIB_PACKED IOL_SM_ReadJOBType
//{
//	IOL_SM_RJOB_TYPE_NONE = 0,	 // no job type
//	IOL_SM_RJOB_TYPE_COMPARAM,	 // read communication parameters from Direct Parameter Page 1 (0x02 to 0x06) via service DL_Read
//	IOL_SM_RJOB_TYPE_IDENTPARAM, // read identification parameters from Direct Parameter Page 1 (0x07 to 0x0D) via service DL_Read
//} IOL_SM_ReadJOBType;
//
//typedef struct IOL_ATTRIB_PACKED IOL_SM_ReadJob_t
//{
//	uint8_t address;
//	IOL_SM_ReadJobState state;
//	uint16_t type;
//	IOL_SM_ReadJobStatus status;
//} IOL_SM_ReadJob_t;

/* States of SM Master state machine
 * Ref: IO-Link Interface Spec v1.1.3, section 9.2.3.2,
 *
 * */
typedef enum IOL_SM_State
{
	IOL_SM_STATE_PORTINACTIVE_0 = 0, // No communication
	IOL_SM_STATE_CHECKCOMPA_1,		 // Port started, revision checked, and Device compatibility verified. See Figure 72.
	IOL_SM_STATE_WAITONDLPREOP_2,	 // Wait until the PREOPERATE state is established and all the On-Request handlers are started. Port is ready to communicate.
	IOL_SM_STATE_CHECKSERNUM_3,		 // SerialNumber is checked depending on the InspectionLevel (IL). See Figure 77.
	IOL_SM_STATE_WAIT_4,			 // Port is ready to communicate and waits on service SM_Operate from CM.
	IOL_SM_STATE_SMOPERATE_5,		 // Port is in state OPERATE and performs cyclic Process Data exchange.
	IOL_SM_STATE_INSPECFAULT_6,		 // Port is ready to communicate. However, cyclic Process Data exchange cannot be performed due to incompatibilities.
	IOL_SM_STATE_WAITONDLOPERATE_7,	 // Wait on the requested state OPERATE in case the Master is connected to a legacy Device. The SerialNumber can be read thereafter.
	IOL_SM_STATE_DIDO_8,			 // Port will be switched into the DI or DO mode (SIO, no communication).
	IOL_SM_STATE_JOINPSEUDOSTATE_9,	 // This pseudo state is used instead of a UML join bar. It allows the execution of individual SM_SetPortConfig services depending on the system status (INACTIVE, CFGCOM, AUTOCOM, DI, or DO).
	IOL_SM_STATE_READCOMPARAM_20,	 // Acquires communication parameters from Direct Parameter Page 1 (0x02 to 0x06) via service DL_Read (see Table B.1).
	IOL_SM_STATE_CHECKCOMPV10_21,	 // Acquires identification parameters from Direct Parameter Page 1 (0x07 to 0x0D) via service DL_Read (see Table B.1). The configured InspectionLevel (IL) defines the decision logic of the subsequent compatibility check "CheckCompV10" with parameters RVID, RDID, and RFID according to Figure 74.
	IOL_SM_STATE_CHECKVXY_22,		 // A check is performed whether the configured revision (CRID) matches the real (actual) revision (RRID) according to Figure 73.
	IOL_SM_STATE_CHECKCOMP_23,		 // Acquires identification parameters from Direct Parameter Page 1 (0x07 to 0x0D) via service DL_Read (see Table B.1). The configured InspectionLevel (IL) defines the decision logic of the subsequent compatibility check "CheckComp" according to Figure 75.
	IOL_SM_STATE_RESTARTDEVICE_24,	 // Writes the compatibility parameters configured protocol revision (CRID) and configured DeviceID (CDID) into the Device depending on the Target Mode of communication CFGCOM or AUTOCOM (see Table 81) according to Figure 76.
	IOL_SM_STATE_JOINPSEUDOSTATE_25, // This pseudo state is used instead of a UML join bar. No guards involved.
	IOL_SM_STATE_READSERNUM_30,		 // Acquires the SerialNumber from the Device via AL_Read.req (Index: 0x0015). A positive response (AL_Read(+)) leads to SReadOK = true. A negative response (AL_Read(-)) leads to SRead- = true.
	IOL_SM_STATE_CHECKSERNUM_31,	 // Optional: SerialNumber checking skipped or checked correctly.
	/*not in spec*/
	IOL_SM_STATE_T3JOINPSEUDSTATE_32, // it is not in spec, I made it to combine all T3 transaction
} IOL_SM_State;

/* Port Mode
 * Ref: IO-Link Interface Spec v1.1.3, section 9.2.2.4,
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SM_PortMode{
   IOL_SM_PORTMODE_INACTIVE = 0,		// Communication disabled, no DI, no DO
   IOL_SM_PORTMODE_DI,					// Port in digital input mode (SIO)
   IOL_SM_PORTMODE_DO,					// Port in digital output mode (SIO)
   IOL_SM_PORTMODE_COMREADY,			// Communication established and inspection successful
   IOL_SM_PORTMODE_SM_OPERATE,			// Port is ready to exchange Process Data
   IOL_SM_PORTMODE_COMLOST,			// Communication failed, new wake-up procedure required
   IOL_SM_PORTMODE_REVISION_FAULT,		// Incompatible protocol revision
   IOL_SM_PORTMODE_COMP_FAULT,			// Incompatible Device or Legacy-Device according to the Inspection Level
   IOL_SM_PORTMODE_SERNUM_FAULT,		// Mismatching SerialNumber according to the InspectionLevel
   IOL_SM_PORTMODE_CYCTIME_FAULT,		// Device does not support the configured cycle time
} IOL_SM_PortMode;

/* B.1 Direct Parameter page 1 and 2 Table: B.1*/
typedef struct IOL_ATTRIB_PACKED IOL_SM_ComParamList{
	//no need to diclare minCycleTime here because it is in the IOL_SM_ParamList_t
//	uint8_t minCycleTime;    	// Minimum cycle duration supported by a Device.	This is a performance feature of the Device and depends on its technology and implementation.
	uint8_t mseqCapb; 	// Information about implemented options related to M-sequences and physical configuration
	uint8_t	pdIn;				// Type and length of input data (Process Data from Device to Master)
	uint8_t	pdOut;				// Type and length of output data (Process Data from Master to Device)
}IOL_SM_ComParamList;

/* See 9.2.3.2 SM Master state machine & sub state machine internal items
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SM_Status{
	IOL_SM_STATUS_NONE = 0, //
	IOL_SM_STATUS_COMPOK,
	IOL_SM_STATUS_COMPFAULT,
	IOL_SM_STATUS_CYCTIMEFAULT,
	IOL_SM_STATUS_SERNUMOK,
	IOL_SM_STATUS_SERNUMFAULT,
	IOL_SM_STATUS_V10COMPOK,
	IOL_SM_STATUS_V10COMPFAULT,
	IOL_SM_STATUS_V10CYCTIMEFAULT,
	IOL_SM_STATUS_REVISIONOK,
	IOL_SM_STATUS_REVISIONFAULT,
	IOL_SM_STATUS_RETRYSTARTUP,
	IOL_SM_STATUS_WRITEDONE,
	IOL_SM_STATUS_SREADNOTOK,
	IOL_SM_STATUS_SREADOK,
}IOL_SM_Status;

/* Not in spec
 * event for service call
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SM_Event{
	IOL_SM_EVENT_NONE = 0, 	// no event call
	IOL_SM_EVENT_SETPORTCONFIG,			// SM_SetPortConfig service call event
	IOL_SM_EVENT_GETPORTCONFIG,			// SM_GetPortConfig service call event
	IOL_SM_EVENT_PORTMODE,				// SM_PortMode service call event
	IOL_SM_EVENT_OPERATE,				// SM_Operate service call event
}IOL_SM_Event;

typedef struct IOL_ATTRIB_PACKED IOL_SM_t{
	IOL_SM_State state;
	IOL_MDHMode mode;
	IOL_SM_Event event;
	IOL_SM_Status status;
	uint8_t compRetry;
	IOL_SM_ParamList_t configParamList;
	IOL_SM_ParamList_t realParamList;
	IOL_SM_ComParamList comParamList;
	IOL_DirParamAddress paramAddress;
	IOL_RWStatus rwStatus;
	uint8_t writeDone;
	//	IOL_ModeValueList_t valueList;

} IOL_SM_t;

/*System management layer services--------------------*/
IOL_Errors IOL_SM_SetPortConfig_Req(IOL_Port_t *port, IOL_SM_ParamList_t paramList);
IOL_Errors IOL_SM_SetPortConfig_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_Errors errorInf);
IOL_Errors IOL_SM_GetPortConfig_Req(IOL_Port_t *port, uint8_t portNum);
IOL_Errors IOL_SM_GetPortConfig_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_SM_ParamList_t realParamList, IOL_Errors errorInf);
IOL_Errors IOL_SM_PortMode_Ind(IOL_Port_t *port, uint8_t portNum, IOL_SM_PortMode mode);
IOL_Errors IOL_SM_Operate_Req(IOL_Port_t *port, uint8_t portNum);
IOL_Errors IOL_SM_Operate_Cnf(IOL_Port_t *port, uint8_t portNum, IOL_Errors errorInf);

/*Response receive service from DL cnf service--------------------*/
void IOL_SM_DL_Read_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo);
void IOL_SM_DL_Write_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
void IOL_SM_DL_SetMode_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
void IOL_SM_DL_Mode_Ind(IOL_Port_t *port, IOL_MDHMode realMode);
void IOL_SM_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, IOL_Errors errorInfo);

/*System management state machines handler----------------*/
void IOL_SM_Handler(IOL_Port_t *port);


#endif /* SRC_IOLINK_IOLINK_SM_H_ */
