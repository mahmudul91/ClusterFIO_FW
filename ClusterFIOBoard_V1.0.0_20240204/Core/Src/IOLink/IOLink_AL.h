

#ifndef SRC_IOLINK_IOLINK_AL_H_
#define SRC_IOLINK_IOLINK_AL_H_
#include "IOLink_Define.h"
#include "IOLink_Types.h"
#include "IOLink_Utility.h"
#include "IOLink.h"

/* Service ID to identify the service call
 * */
typedef enum IOL_ATTRIB_PACKED IOL_AL_ServiceId{
	IOL_AL_SRVID_NONE = 0,
	IOL_AL_SRVID_READ_REQ,
	IOL_AL_SRVID_READ_CNF,
	IOL_AL_SRVID_WRITE_REQ,
	IOL_AL_SRVID_WRITE_CNF,
	IOL_AL_SRVID_ABORT_REQ,
	IOL_AL_SRVID_GETINPUT_REQ,
	IOL_AL_SRVID_GETINPUT_CNF,
	IOL_AL_SRVID_NEWINPUT_IND,
	IOL_AL_SRVID_PDCYCLE_IND,
	IOL_AL_SRVID_SETOUTPUT_REQ,
	IOL_AL_SRVID_SETOUTPUT_CNF,
	IOL_AL_SRVID_EVENT_IND,
	IOL_AL_SRVID_CONTROL_REQ,
	IOL_AL_SRVID_CONTROL_IND,
	IOL_AL_SRVID_ISDUTRANSPORT_CNF,
}IOL_AL_ServiceId;

/*OD handler--------------------------------------*/
/* 8.3.2 On-request Data transfer,
 * Table 74 – States for the OD state machine
 * States for the OD state machine
 * */
typedef enum IOL_ATTRIB_PACKED IOL_AL_ODHState{
	IOL_AL_ODH_STATE_ONREQ_IDLE_0 = 0,       // AL service invocations accepted
	IOL_AL_ODH_STATE_BUILD_DL_SERVICE_1,     // AL service calls checked; DL services created
	IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2,   // AL service call transformed into DL_ReadParam/DL_WriteParam calls
	IOL_AL_ODH_STATE_AWAIT_DL_ISDU_CNF_3,    // AL service call transformed into DL_ISDUTransport service call
	IOL_AL_ODH_STATE_BUILD_AL_CNF_4          // AL service confirmation created based on argument error, DL service confirmation, or AL_Abort
}IOL_AL_ODHState;

typedef struct IOL_ATTRIB_PACKED IOL_AL_ODHandler_t{
	IOL_AL_ODHState state;
	IOL_AL_ServiceId srvId;

	IOL_MseqCapbOctet_t mseqCapb;
	IOL_Errors argError;
	IOL_ErrorTypes errorInfo;
	uint8_t address;
	IOL_IServiceOctet_t iservQual;
	uint16_t index;
	uint8_t subindex;
	IOL_RWDirection rwDir;
	uint8_t dataLen;
	uint8_t nextDataPos;
	uint8_t readData[IOL_MAX_ISDU_BUFF_SIZE];
	uint8_t *writeData;
	uint8_t writeDataLen;
}IOL_AL_ODHandler_t;

/*Event handler-----------------------------------*/
typedef enum IOL_ATTRIB_PACKED IOL_AL_EVTHState{
    IOL_AL_EVTH_STATE_EVENT_INACTIVE_0 = 0,   // AL Event handling of the Master is inactive.
    IOL_AL_EVTH_STATE_EVENT_IDLE_1,            // The Master AL is ready to accept DL_Events (diagnosis information) from the DL.
    IOL_AL_EVTH_STATE_READ_EVENT_SET_2,        // The Master AL received a DL_Event_ind with diagnosis information. After this first DL_Event.ind, the AL collects the complete set (1 to 6) of DL_Events of the current EventTrigger (see 11.6).
    IOL_AL_EVTH_STATE_DU_EVENT_HANDLING_3      // The Master AL remains in this state as long as the Diagnosis Unit (see 11.6) did not acknowledge the AL_Event.ind
}IOL_AL_EVTHState;

/* Transition guard condition of the state machine
 * */
typedef enum IOL_ATTRIB_PACKED IOL_AL_EVTHCommand{
	IOL_AL_EVTH_CMD_NONE = 0,
	IOL_AL_EVTH_CMD_DEACTIVATE,
	IOL_AL_EVTH_CMD_ACTIVATE,
	IOL_AL_EVTH_CMD_AL_EVENT_REQ,
	IOL_AL_EVTH_CMD_DL_EVENT_RSP,
	IOL_AL_EVTH_CMD_DL_EVENT_IND,
	IOL_AL_EVTH_CMD_EVENT_DONE,
}IOL_AL_EVTHCommand;


typedef struct IOL_ATTRIB_PACKED IOL_AL_EventHandler_t{
	IOL_AL_EVTHState state;
	IOL_AL_EVTHCommand cmd;
	uint8_t eventCount;
	IOL_EventX_t events[IOL_MAX_EVENT];
	uint8_t eventsLeft;		//This parameter indicates the number of unprocessed Events.
}IOL_AL_EventHandler_t;

/*Application layer main Structure----------------*/
typedef struct IOL_ATTRIB_PACKED IOL_AL_t{
	uint8_t portNum;
	IOL_AL_ODHandler_t odh;
	IOL_AL_EventHandler_t evth;

	void (*alReadCnf_cb)(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len,  IOL_ErrorTypes errorInfo);
	void (*alWriteCnf_cb)(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo);
}IOL_AL_t;


/*Services-----------------------------------*/
IOL_Errors IOL_AL_Read_Req(IOL_Port_t *port, uint8_t portNumber, uint16_t index, uint8_t subindex,
		void (*alReadCnf_cb)(IOL_Port_t *port, uint8_t portNumber, uint8_t *data,  uint8_t len, IOL_ErrorTypes errorInfo));
IOL_Errors IOL_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data,  uint8_t len, IOL_ErrorTypes errorInfo);
IOL_Errors IOL_AL_Write_Req(IOL_Port_t *port, uint8_t portNumber, uint16_t index, uint8_t subindex, uint8_t *data, uint8_t len,
		void (*alWriteCnf_cb)(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo));
IOL_Errors IOL_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo);
IOL_Errors IOL_AL_Abort_Req(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_GetInput_Req(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_GetInput_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *inputData, IOL_Errors errorInfo);
IOL_Errors IOL_AL_NewInput_Ind(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_PDCycle_Ind(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_SetOutput_Req(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len);
IOL_Errors IOL_AL_SetOutput_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_Errors errorInfo);
IOL_Errors IOL_AL_Event_Req(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event);
IOL_Errors IOL_AL_Event_Ind(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event);
IOL_Errors IOL_AL_Event_Rsp(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_Event_Cnf(IOL_Port_t *port, uint8_t portNumber);
IOL_Errors IOL_AL_Control_Req(IOL_Port_t *port, uint8_t portNumber, IOL_ControlCode controlCode);
IOL_Errors IOL_AL_Control_Ind(IOL_Port_t *port, uint8_t portNumber, IOL_ControlCode controlCode);

/*Services to receive confirmation of DL services------------------*/
void IOL_AL_DL_ReadParam_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo);
void IOL_AL_DL_WriteParam_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
void IOL_AL_DL_ISDUTransport_Cnf(IOL_Port_t *port, uint8_t *data, IOL_IServiceQualifier Qualifier, IOL_Errors errorInfo);
void IOL_AL_DL_ISDUAbort_Cnf(IOL_Port_t *port);
void IOL_AL_DL_PDOutputUpdate_Cnf(IOL_Port_t *port, IOL_Errors errorInfo);
void IOL_AL_DL_PDInputTransport_Ind(IOL_Port_t *port, uint8_t *data, uint8_t len);
void IOL_AL_DL_Event_Ind(IOL_Port_t *port, IOL_EventInstance instance, IOL_EventType type, IOL_EventMode mode, IOL_DeviceEventCodes eventCode, uint8_t eventsLeft);


/*State machines-------------------------*/
void IOL_AL_ODHandler(IOL_Port_t *port);
void IOL_AL_EventHandler(IOL_Port_t *port);
#endif /* SRC_IOLINK_IOLINK_AL_H_ */
