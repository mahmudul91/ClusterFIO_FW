/*
 * IOLink_DL.h
 *
 *  Created on: Oct 28, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_DL_H_
#define SRC_IOLINK_IOLINK_DL_H_

#include "IOLink_Define.h"
#include "IOLink_Utility.h"
#include "IOLink.h"
#include "IOLink_Time.h"
#include "IOLink_Types.h"



#define IOL_MAX_DL_MSG_SIZE			32U		// sec:7.2.2.2, line: 1286
#define IOL_MAX_DL_ESTCOM_RETRY		3U
#define IOL_MAX_DL_BUFF_SIZE		256U

#define IOL_MAX_DL_MH_RETRY			2U

/* A.1.6 Calculation of the checksum
 * A seed value of 0x52 is used for the checksum calculation across the message.
 * */
#define IOL_CHECKSUM_SEED_VALUE		0x52


//#define IOL_DL_PDINSTATUS_VALID  	0U
//#define IOL_DL_PDINSTATUS_INVALID	1U

typedef enum IOL_DL_Trigger{
   IOL_DL_TRIG_NONE = 0U,
   IOL_DL_TRIG_DEV_MSG,
   IOL_DL_TRIG_MSTR_MSG,
} IOL_DL_Trigger;


/* This indicates whether the data link layer is in a state permitting data to be
 * transferred to the communication partner
 * */
typedef enum IOL_DL_TransportStatus{
	IOL_DL_TRANS_NOT_PERMITTED = 0U,			// data transmission not permitted
	IOL_DL_TRANS_PERMITTED,					// data transmission permitted
}IOL_DL_TransportStatus;



/* IOL communication status*/
typedef enum IOL_DL_CommStatus{
	IOL_DL_COM_FAILED = 0U,
	IOL_DL_COM_OK,
}IOL_DL_CommStatus;



/* Mode handler-----------------------------------*/
/* States of the Master DL-mode handler State machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.2.4, Table 44 – State transition tables of the Master DL-mode handl
 * */
typedef enum IOL_DL_MDHState {
    IOL_DL_MDH_STATE_IDLE_0 = 0U,          	// Waiting on wakeup request from System Management (SM): DL_SetMode (STA)
    IOL_DL_MDH_STATE_ESTABLISHCOMM_1, 		// Perform wakeup procedure (submachine 1)
    IOL_DL_MDH_STATE_STARTUP_2,       		// System Management uses the STARTUP state for Device identification, check, communication configuration (see Figure 71)
    IOL_DL_MDH_STATE_PREOPERATE_3,    		// On-request Data exchange (parameter, commands, Events) without Process Data
    IOL_DL_MDH_STATE_OPERATE_4,        		// Process Data and On-request Data exchange (parameter, commands, Events)
	IOL_DL_MDH_STATE_SM_WURQ_5, 			// SM-Sub-Machine, Create wakeup current pulse: Invoke service PL-Wake-Up (see Figure 12 and 5.3.3.3) and wait TDMT (see Table 42).
	IOL_DL_MDH_STATE_SM_COMREQCOM3_6,	 	// Try test message with transmission rate of COM3 via the message handler: Call MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
	IOL_DL_MDH_STATE_SM_COMREQCOM2_7, 		// Try test message with transmission rate of COM2 via the message handler: Call MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
	IOL_DL_MDH_STATE_SM_COMREQCOM1_8, 		// Try test message with transmission rate of COM1 via the message handler: Call MH_Conf_COMx (see Figure 40) and wait TDMT (see Table 42).
	IOL_DL_MDH_STATE_SM_RETRY_9, 			// Check number of Retries
}IOL_DL_MDHState;



typedef struct IOL_ATTRIB_PACKED IOL_DL_ModeHandler_t{
	IOL_DL_MDHState state;
	IOL_MDHMode mode;
	uint8_t retryEstCom; 			// retry counter for establishing communication
	uint8_t isEstdComx;
}IOL_DL_ModeHandler_t;





/* Message  handler-----------------------------------*/
/* States of the Master message handler State machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.3.4, Table 46 – State transition table of the Master message handler
 * */
typedef enum IOL_DL_MHState {
    IOL_DL_MH_STATE_INACTIVE_0	= 0U,       // Waiting on demand for a "test" message via MH_Conf_COMx call (see Figure 36 and Table 44) from DL-mode handler.
    IOL_DL_MH_STATE_AWAITREPLY_1,     		// Waiting on response from the Device to the "test" message. Return to Inactive_0 state whenever the time T_M-sequence elapsed without response from the Device or the response to the "test" message could not be decoded. In case of a correct response from the Device, the message handler changes to the Startup_2 state.
    IOL_DL_MH_STATE_STARTUP_2,        		// When entered via transition T2, this state is responsible to control acyclic On-request Data exchange according to conditions specified in Table A.7. Any service DL_Write or DL_Read from System Management causes a transition.
    IOL_DL_MH_STATE_RESPONSE_3,       		// The OD service caused the message handler to send a corresponding message. The submachine in this pseudo state waits on the response and checks its correctness.
    IOL_DL_MH_STATE_SM_AWAITREPLY_4,  		// This state checks whether the time TM-sequence elapsed and the response is correct.
    IOL_DL_MH_STATE_SM_ERRHANDL_5, 	// In case of an incorrect response the message handler will re-send the message after a waiting time T_initcyc. After too many retries the message handler will change to the Inactive_0 state.
    IOL_DL_MH_STATE_PREOPERATE_6,     		// Upon reception of a call MH_Conf_PREOPERATE the message handler changed to this state. The message handler is now responsible to control acyclic On-request Data exchange according to conditions specified in Table A.8. Any service DL_ReadParam, DL_WriteParam, DL_ISDUTransport, DL_Write, or EventFlag causes a transition.
    IOL_DL_MH_STATE_GETOD_7,          		// The message handler used the ODTrig service to acquire OD from the On-request Data handler. The message handler waits on the OD service to send a message after a time T_initcyc.
    IOL_DL_MH_STATE_RESPONSE_8,       		// The OD service caused the message handler to send a corresponding message. The submachine in this pseudo state waits on the response and checks its correctness.
    IOL_DL_MH_STATE_SM_AWAITREPLY_9,  		// This state checks whether the time TM-sequence elapsed and the response is correct.
    IOL_DL_MH_STATE_SM_ERRHANDL_10, 	// In case of an incorrect response, the message handler will re-send the message after a waiting time T_initcyc. After too many retries, the message handler will change to the Inactive_0 state.
    IOL_DL_MH_STATE_CHECKHANDL_11,  		// Some services require several OD acquisition cycles to exchange the OD. Whenever the affected OD, ISDU, or Event handler returned to the idle state, the message handler can leave the OD acquisition loop.
    IOL_DL_MH_STATE_OPERATE_12,       		// Upon reception of a call MH_Conf_OPERATE the message handler changed to this state and, after an initial time T_initcyc, it is responsible to control cyclic Process Data and On-request Data exchange according to conditions specified in Table A.9 and Table A.10. The message handler restarts on its own a new message cycle after the time tCYC elapsed.
    IOL_DL_MH_STATE_GETPD_13,         		// The message handler used the PDTrig service to acquire PD from the Process Data handler. The message handler waits on the PD service and then changes to state GetOD_14.
    IOL_DL_MH_STATE_GETOD_14,         		// The message handler used the ODTrig service to acquire OD from the On-request Data handler. The message handler waits on the OD service to complement the already acquired PD and to send a message with the acquired PD/OD.
    IOL_DL_MH_STATE_RESPONSE_15,      		// The message handler sent a message with the acquired PD/OD. The submachine in this pseudo state waits on the response and checks its correctness.
    IOL_DL_MH_STATE_SM_AWAITREPLY_16, 		// This state checks whether the time TM-sequence elapsed and the response is correct.
    IOL_DL_MH_STATE_SM_ERRHANDL_17 	// In case of an incorrect response the message handler will re-send the message after a waiting time tCYC. After too many retries the message handler will change to the Inactive_0 state.
}IOL_DL_MHState;


typedef enum IOL_DL_MHCommand{
	IOL_DL_MH_CMD_INACTIVE = 0,
	IOL_DL_MH_CMD_COM1,
	IOL_DL_MH_CMD_COM2,
	IOL_DL_MH_CMD_COM3,
	IOL_DL_MH_CMD_ACTIVE,
	IOL_DL_MH_CMD_STARTUP,
	IOL_DL_MH_CMD_PREOPERATE,
	IOL_DL_MH_CMD_OPERATE,
} IOL_DL_MHCommand;

/* IOLink MHInfor - message handler info
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.2.6,
 * */
typedef enum IOL_DL_MHInfor{
	IOL_DL_MHINFO_NONE = 0U,			// no message handler info
	IOL_DL_MHINFO_COMLOST,				// lost communication
	IOL_DL_MHINFO_ILLEGAL_MESSAGETYPE, // unexpected M-sequence type detected
	IOL_DL_MHINFO_CHECKSUM_MISMATCH,	// Checksum error detected
} IOL_DL_MHInfor;

/*	Read/write command to get the msg from the upper layer from the msg handler
 * it will use with the od trigger
 * */
typedef enum IOL_DL_MHRWCommand{
   IOL_DL_MH_RWCMD_NONE = 0,
   IOL_DL_MH_RWCMD_WRITE,
   IOL_DL_MH_RWCMD_READ,
   IOL_DL_MH_RWCMD_WRITEPARAM,
   IOL_DL_MH_RWCMD_READPARAM,
   IOL_DL_MH_RWCMD_ISDUTRANS,
   IOL_DL_MH_RWCMD_ISDUABORT,
   IOL_DL_MH_RWCMD_WRITEDEVMODE,
} IOL_DL_MHRWCommand;




typedef struct IOL_ATTRIB_PACKED IOL_DL_MsgHandler_t{
	IOL_DL_MHState state;
	IOL_DL_MHCommand mhCmd;
	IOL_DL_MHInfor mhInfo;
	IOL_RWDirection rwDir;			// rw dirrection
	IOL_DL_MHRWCommand rwCmd;
	IOL_MseqCtrlOctet_t mc;			//M-sequence control octet
	IOL_CKTOctet_t ckt;		// M-sequence type and checksum octet
	uint8_t retry;

	IOL_ProcessLock lock;


	uint16_t txDataLen;
	uint16_t rxDataLen;
	uint8_t *txData;
	uint8_t *rxData;

	uint8_t pdInLen;
	uint8_t pdOutLen;
	uint8_t odLenPerMsg;
	IOL_MseqSubType mseqSubType;
	uint8_t mseqTime;
	IOL_Timer_t tmseq;
	IOL_Timer_t tcyc;
	uint32_t minCycTime;
	IOL_DL_INTERLEAVE interleave;
}IOL_DL_MsgHandler_t;


/*Process data handler-----------------------------------*/
/* States of the Master r Process Data handler State machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.4.3,
 * */
typedef enum IOL_DL_PDHState{
	IOL_DL_PDH_STATE_INACTIVE_0 = 0U,			// Waiting for activation
	IOL_DL_PDH_STATE_PDSINGLE_1,				// Process Data communication within one single M-sequence
	IOL_DL_PDH_STATE_PDININTERLEAVE_2,			// Input Process Data communication in interleave mode
	IOL_DL_PDH_STATE_PDOUTINTERLEAVE_3,			// Output Process Data communication in interleave mode
}IOL_DL_PDHState;

typedef enum IOL_DL_PDHCommand{
	IOL_DL_PDH_CMD_INACTIVE = 0U,
	IOL_DL_PDH_CMD_SINGLE,
	IOL_DL_PDH_CMD_INTERLEAVE,
}IOL_DL_PDHCommand;



/* Structure of the the process data handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.4,
 * */
typedef struct IOL_ATTRIB_PACKED IOL_DL_PDHandler_t{
	IOL_ProcessLock lock;
	IOL_DL_PDHState state;
	IOL_DL_PDHCommand pdhCmd;
	IOL_DL_Trigger trigger;
	IOL_DL_TransportStatus transStatus;
	uint8_t pdOutAddr;
	uint8_t pdInAddr;
	uint8_t pdOutLen;
	uint8_t pdInLen;
	uint8_t pdOutData[IOL_MAX_PD_SIZE];
	uint8_t pdInData[IOL_MAX_PD_SIZE];

}IOL_DL_PDHandler_t;

/*On request data handler-----------------------------------*/

/* States of theMaster On-request Data handler State machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.5.2,
 *
 * */
typedef enum IOL_DL_ODHState{
	IOL_DL_ODH_STATE_INACTIVE_0 = 0U,		// Waiting on activation
	IOL_DL_ODH_STATE_ISDU_1,				// Default state of the On-request Data handler (lowest priority)
	IOL_DL_ODH_STATE_COMMAND_2,				// State to control the Device via commands with highest priority
	IOL_DL_ODH_STATE_EVENT_3,				// State to convey Event information (errors, warnings, notifications) with higher priority
}IOL_DL_ODHState;



typedef enum IOL_DL_OHCommand{
	IOL_DL_OH_CMD_INACTIVE = 0U,
	IOL_DL_OH_CMD_ACTIVE,
}IOL_DL_OHCommand;



/*
 * States of theMaster On-request Data handler State machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.5.2,
 * */
typedef struct IOL_ATTRIB_PACKED IOL_DL_ODHandler_t{
	IOL_ProcessLock lock;
	IOL_DL_ODHState state;
	IOL_DL_OHCommand ohCmd;
	IOL_DL_Trigger trigger;
	uint8_t outDataLen;			// out data buffer length
	uint8_t inDataLen;			// out data buffer length
	uint8_t *outData;			// pointer of the out data buffer
	uint8_t *inData;			// pointer of the out data buffer
	uint8_t address;			// address of a single data

//	uint8_t data;				// value of a single data
}IOL_DL_ODHandler_t;


/*Master ISDU handler -----------------------------------*/
typedef enum IOL_DL_ISDUHState{
    IOL_DL_ISDUH_STATE_INACTIVE_0 = 0U,  		// Waiting on activation
    IOL_DL_ISDUH_STATE_IDLE_1,      			// Waiting on transmission of next On-request Data
    IOL_DL_ISDUH_STATE_ISDUREQUEST_2,			// Transmission of ISDU request data
    IOL_DL_ISDUH_STATE_ISDUWAIT_3,   			// Waiting on response from Device. Observe ISDUTime
    IOL_DL_ISDUH_STATE_ISDUERROR_4, 			// Error handling after detected errors: Invoke negative DL_ISDU_Transport response with ISDUTransportErrorInfo
    IOL_DL_ISDUH_STATE_ISDURESPONSE_5  			// Get response data from Device
}IOL_DL_ISDUHState;



typedef enum IOL_DL_IHCommand{
	IOL_DL_IH_CMD_INACTIVE = 0U,
	IOL_DL_IH_CMD_ACTIVE,
}IOL_DL_IHCommand;




/* IOLink ISDU communication channel flow control
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.6.2,
 * */
typedef enum IOL_DL_ISDUFlowCtrl{
	IOL_DL_ISDU_FLOWCTRL_START = 0x10,			// Start of an ISDU I-Service, i.e., start of a request or a response. For the start of a request, any previously incomplete services may be rejected. For a start request associated with a response, a Device shall send “No Service” until its application returns response data (see Table A.12).
	IOL_DL_ISDU_FLOWCTRL_IDLE_1,				// No request for ISDU transmission.
//	IOL_DL_ISDU_FLOWCTRL_IDLE_2,				// Reserved for future use No request for ISDU transmission.
	IOL_DL_ISDU_FLOWCTRL_ABORT = 0x1F,			// Abort entire service. The Master responds by rejecting received response data. The Device responds by rejecting received request data and may generate an abort.
}IOL_DL_ISDUFlowCtrl;




/* State machine of the Master ISDU handler
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.6.3,
 * */
typedef struct IOL_ATTRIB_PACKED IOL_DL_ISDUHandler_t{
	IOL_DL_ISDUHState state;
	IOL_DL_IHCommand ihCmd;
	uint8_t countMseq;
	uint16_t txDataLen;
	uint16_t rxDataLen;
	uint8_t txData[IOL_MAX_ISDU_BUFF_SIZE];
	uint8_t rxData[IOL_MAX_ISDU_BUFF_SIZE];
	IOL_Timer_t tisdu;;
}IOL_DL_ISDUHandler_t;




/*Master Command handler-----------------------------------*/
/* States of the Command Handler state machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.7.2,
 * */
typedef enum IOL_DL_CMDHStates{
	IOL_DL_CMDH_STATE_INACTIVE_0 = 0U,			// Waiting on activation by DL-mode handler
	IOL_DL_CMDH_STATE_IDLE_1,					// Waiting on new command from AL: DL_Control (status of output PD) or from SM: DL_Write (change Device mode, for example to OPERATE), or waiting on PDInStatus.ind service primitive.
	IOL_DL_CMDH_STATE_MASTER_CMD_2,			// Prepare data for OD.req service primitive. Waiting on demand from OD handler (CommandTrig).
}IOL_DL_CMDHStates;

typedef enum IOL_DL_CHCommand{
	IOL_DL_CH_CMD_INACTIVE = 0U,
	IOL_DL_CH_CMD_ACTIVE,
} IOL_DL_CHCommand;


typedef struct IOL_ATTRIB_PACKED IOL_DL_CMDHandler_t{
	IOL_DL_CMDHStates state;
	IOL_ControlCode ctrlCode;
	IOL_DL_CHCommand chCmd;
	IOL_PDINStatus pdInStatus;
	IOL_MasterCommand masterCmd;

}IOL_DL_CMDHandler_t;


/*Master Event handler-----------------------------------*/
/* States of the Event Handler state machine
 * Ref: IO-Link Interface Spec v1.1.3, section 7.3.8.3,
 * */
typedef enum IOL_DL_EventHandlerState{
	IOL_DL_EH_STATE_INACTIVE_0 = 0U,         	// Waiting on activation
	IOL_DL_EH_STATE_IDLE_1,         		 	// Waiting on next Event indication ("EventTrig" through On-request Data handler) or Event confirmation through service DL_EventConf from Master AL.
	IOL_DL_EH_STATE_READEVENT_2,       		 	// Read Event data set from Device message by message through Event memory address. Check StatusCode for number of activated Events (see Table 58).
	IOL_DL_EH_STATE_SIGNALEVENT_3,     			// Analyze Event data and invoke DL_Event indication to Master AL (see 7.2.1.15) for each available Event.
	IOL_DL_EH_STATE_EventConf_4,		// Waiting on Event confirmation transmission via service OD.req to the Device.
}IOL_DL_EventHandlerState;



typedef enum IOL_DL_EHCommand{
   IOL_DL_EH_CMD_INACTIVE = 0U,
   IOL_DL_EH_CMD_ACTIVE,
} IOL_DL_EHCommand;


typedef struct IOL_ATTRIB_PACKED IOL_DL_EventHandler_t{
	IOL_DL_EventHandlerState state;
	IOL_DL_EHCommand ehCmd;
	IOL_EHEventFlag eventFlag;
	uint8_t eventConf;
	uint8_t eventMemAddr;
	uint8_t eventMem[IOL_MAX_EVENT_MEM_ADDRESS];
}IOL_DL_EventHandler_t;


typedef struct IOL_ATTRIB_PACKED IOL_DL_t{

	IOL_DL_ModeHandler_t mdh;
	IOL_DL_MsgHandler_t msgh;
	IOL_DL_PDHandler_t pdh;
	IOL_DL_ODHandler_t odh;
	IOL_DL_ISDUHandler_t isduh;
	IOL_DL_CMDHandler_t cmdh;
	IOL_DL_EventHandler_t evnth;
	IOL_BaudRate baudRate;			// baud rate for com3 or com2 or com1
	uint8_t isBaudRateSet;			// baud rate set success flag
	IOL_DL_CommStatus comStatus;	// used to check Transmission rate of COMX successful or not


	/*Timers*/
	IOL_Timer_t tdmt;				// Master message delay, Ref: Table 42
	IOL_Timer_t tdwu;				// Wake-up retry delay, time duration between two successive wurq
	IOL_Timer_t tsd;				// Device detection time, time duration between 2 wurq request sequences
	uint8_t isTdmtElpsd;			// Master message delay time elapsed status


	IOL_ProcessLock lock;
	uint8_t isDataRecved;
	uint8_t txBuffLen;
	uint8_t rxBuffLen;
	uint8_t txBuff[IOL_MAX_DL_BUFF_SIZE];
	uint8_t rxBuff[IOL_MAX_DL_BUFF_SIZE];
}IOL_DL_t;

/*DL-A services----------------------------------------*/

/*DL-B services----------------------------------------*/
IOL_Errors IOL_DL_ReadParam_Req(IOL_Port_t *port, uint8_t address);
IOL_Errors IOL_DL_ReadParam_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo);
IOL_Errors IOL_DL_WriteParam_Req(IOL_Port_t *port, uint8_t address, uint8_t value);
IOL_Errors IOL_DL_WriteParam_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
IOL_Errors IOL_DL_Read_Req(IOL_Port_t *port, uint8_t address);
IOL_Errors IOL_DL_Read_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo);
IOL_Errors IOL_DL_Write_Req(IOL_Port_t *port, uint8_t address, uint8_t value);
IOL_Errors IOL_DL_Write_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
IOL_Errors IOL_DL_ISDUTransport_Req(IOL_Port_t *port, IOL_ISDUValueList_t *valueList);
IOL_Errors IOL_DL_ISDUTransport_Cnf(IOL_Port_t *port, uint8_t *data, IOL_IServiceQualifier qualifier, IOL_Errors errorInfo);
IOL_Errors IOL_DL_ISDUAbort_Req(IOL_Port_t *port);
IOL_Errors IOL_DL_ISDUAbort_Cnf(IOL_Port_t *port);
IOL_Errors IOL_DL_PDOutputUpdate_Req(IOL_Port_t *port,  uint8_t *data, uint8_t len);
IOL_Errors IOL_DL_PDOutputUpdate_Cnf(IOL_Port_t *port, IOL_DL_TransportStatus tranStatus,  IOL_Errors errorInfo);
IOL_Errors IOL_DL_PDInputTransport_Ind(IOL_Port_t *port, uint8_t *data, uint8_t len);
IOL_Errors IOL_DL_PDCycle_Ind(IOL_Port_t *port);
IOL_Errors IOL_DL_SetMode_Req(IOL_Port_t *port, IOL_MDHMode mode, IOL_ModeValueList_t *valueList);
IOL_Errors IOL_DL_SetMode_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
IOL_Errors IOL_DL_Mode_Ind(IOL_Port_t *port, IOL_MDHMode realMode);
IOL_Errors IOL_DL_Event_Ind(IOL_Port_t *port, IOL_EventInstance instance, IOL_EventType type, IOL_EventMode mode, IOL_DeviceEventCodes eventCode, uint8_t eventsLeft);
IOL_Errors IOL_DL_EventConf_Req(IOL_Port_t *port);
IOL_Errors IOL_DL_Control_Req(IOL_Port_t *port, IOL_ControlCode ctrlCode);
IOL_Errors IOL_DL_Control_Ind(IOL_Port_t *port, IOL_ControlCode ctrlCode);

/*DataLink layer state machine handlers----------------*/
void IOL_DL_ModeHandler(IOL_Port_t *port);
void IOL_DL_MessageHandler(IOL_Port_t *port);
void IOL_DL_PDHandler(IOL_Port_t *port);
void IOL_DL_ODHandler(IOL_Port_t *port);
void IOL_DL_ISDUHandler(IOL_Port_t *port);
void IOL_DL_CommandHandler(IOL_Port_t *port);
void IOL_DL_EventHandler(IOL_Port_t *port);

/*Main handler for the DL-------------------------*/
void IOL_DL_Initialize(IOL_Port_t *port);
void IOL_DL_Handler(IOL_Port_t *port);

#endif /* SRC_IOLINK_IOLINK_DL_H_ */
