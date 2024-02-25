/*
 * IOLink_AL.c
 *
 *  Created on: Nov 1, 2023
 *      Author: wsrra
 */

#include "IOLink_AL.h"
#include "IOLink_DL.h"
#include "string.h"
#include "IOLink_MA.h"


static uint8_t portNumber = 0;

/*Application Layer services-----------------------*/

/* @ref 8.2.2.1 AL_Read
 * @brief The AL_Read service is used to read On-request Data from a Device connected to a specific port.
 * */
IOL_Errors IOL_AL_Read_Req(IOL_Port_t *port, uint8_t portNumber, uint16_t index, uint8_t subindex,
		void (*alReadCnf_cb)(IOL_Port_t *port, uint8_t portNumber, uint8_t *data,  uint8_t len, IOL_ErrorTypes errorInfo))
{
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);

	al->alReadCnf_cb = alReadCnf_cb;
	//TODO: To implement later
	al->odh.index = index;
	al->odh.subindex = subindex;
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.1 AL_Read
 * @brief The IOL_AL_Read_Cnf service is used to return the response of IOL_AL_Read_Req service
 *
 */
IOL_Errors IOL_AL_Read_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *data,  uint8_t len, IOL_ErrorTypes errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	al->alReadCnf_cb(port, portNumber, data, len, errorInfo);
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.2 AL_Write
 * @brief The AL_Write service is used to write On-request Data to a Device connected to a specific port
 * */
IOL_Errors IOL_AL_Write_Req(IOL_Port_t *port, uint8_t portNumber, uint16_t index, uint8_t subindex, uint8_t *data, uint8_t len,
		void (*alWriteCnf_cb)(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo))
{
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	al->alWriteCnf_cb = alWriteCnf_cb;
	//TODO: To implement later
	if(index == 0){
		IOL_AL_Write_Cnf(port, portNumber, IOL_ERRTYPISDU_VAL_LENUNDRUN);
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}else if(subindex > IOL_MAX_ISDU_DATA_SIZE){
		IOL_AL_Write_Cnf(port, portNumber, IOL_ERRTYPISDU_VAL_LENOVRRUN);
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}
	al->odh.index = index;
	al->odh.subindex = subindex;
	al->odh.writeData = data;
	al->odh.writeDataLen = len;
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.2 AL_Write
 * @brief The IOL_AL_Write_Cnf service is used to return the response of IOL_AL_Write_Req service
 */
IOL_Errors IOL_AL_Write_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_ErrorTypes errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	al->alWriteCnf_cb(port, portNumber, errorInfo);
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.3 AL_Abort
 * @brief The AL_Abort service is used to abort a current AL_Read or AL_Write service on a specific
 * port. Invocation of this service abandons the response to an AL_Read or AL_Write service in
 * progress on the Master.
 * */
IOL_Errors IOL_AL_Abort_Req(IOL_Port_t *port, uint8_t portNumber){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.4 AL_GetInput
 * @brief The AL_GetInput service reads the input data within the Process Data provided by the data
 * link layer of a Device connected to a specific port.
 * */
IOL_Errors IOL_AL_GetInput_Req(IOL_Port_t *port, uint8_t portNumber){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.4 AL_GetInput
 * @brief The IOL_AL_GetInput_Cnf service is used to return the response of IOL_AL_GetInput_Req service
 */
IOL_Errors IOL_AL_GetInput_Cnf(IOL_Port_t *port, uint8_t portNumber, uint8_t *inputData, IOL_Errors errorInfo){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.5 AL_NewInput
 * @brief The AL_NewInput local service indicates the receipt of updated input data within the Process
 * Data of a Device connected to a specific port
 * */
IOL_Errors IOL_AL_NewInput_Ind(IOL_Port_t *port, uint8_t portNumber){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.7 AL_PDCycle
 * @brief The AL_PDCycle local service indicates the end of a Process Data cycle. The Device
 * application can use this service to transmit new input data to the application layer via
 * AL_SetInput
 * */
IOL_Errors IOL_AL_PDCycle_Ind(IOL_Port_t *port, uint8_t portNumber){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.10 AL_SetOutput
 * @brief The AL_SetOutput local service updates the output data within the Process Data of a Master
 * */
IOL_Errors IOL_AL_SetOutput_Req(IOL_Port_t *port, uint8_t portNumber, uint8_t *data, uint8_t len){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	IOL_DL_PDOutputUpdate_Req(port, data, len);
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.10 AL_SetOutput
 * @brief The IOL_AL_SetOutput_Cnf service is used to return the response of IOL_AL_SetOutput_Req service
 */
IOL_Errors IOL_AL_SetOutput_Cnf(IOL_Port_t *port, uint8_t portNumber, IOL_Errors errorInfo){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}



/* @ref 8.2.2.11 AL_Event
 * @brief The AL_Event service indicates up to 6 pending status or error messages. The source of one
 * Event can be local (Master) or remote (Device). The Event can be triggered by a
 * communication layer or by an application.
 * */

IOL_Errors IOL_AL_Event_Req(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event){

	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	if(eventCount >= IOL_MAX_EVENT){
		return IOL_ERROR_VALUE_OUT_OF_RANGE;
	}

	al->evth.eventCount = eventCount;
	al->evth.events[eventCount] = event[eventCount];
	al->evth.cmd = IOL_AL_EVTH_CMD_AL_EVENT_REQ;
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.11 AL_Event
 * @brief propagate the event to the smi
 * */
IOL_Errors IOL_AL_Event_Ind(IOL_Port_t *port, uint8_t portNumber, uint8_t eventCount, IOL_EventX_t *event){
	/*Convey the event to the smi*/
	IOL_DU_AL_Event_Ind(port, portNumber, eventCount, event);
	return IOL_ERROR_NONE;
}


IOL_Errors IOL_AL_Event_Rsp(IOL_Port_t *port, uint8_t portNumber){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	al->evth.cmd = IOL_AL_EVTH_CMD_DL_EVENT_RSP;
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.12 AL_Control
 * @brief The AL_Control service contains the Process Data qualifier status information transmitted to
 * and from the Device application. This service shall be synchronized with AL_GetInput and
 * AL_SetOutput respectively (see 11.7.2.1).
 * */
IOL_Errors IOL_AL_Control_Req(IOL_Port_t *port, uint8_t portNumber, IOL_ControlCode controlCode){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/* @ref 8.2.2.12 AL_Control
 * @brief The IOL_AL_Control_Ind service is used to receive as an indication of IOL_AL_Control_Req service
 * */
IOL_Errors IOL_AL_Control_Ind(IOL_Port_t *port, uint8_t portNumber, IOL_ControlCode controlCode){
//	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	return IOL_ERROR_NONE;
}

/*Services to receive confirmation of DL services------------------*/
/* IOL_AL_DL_ReadParam_Cnf service is used to receive response of IOL_DL_ReadParam_Cnf service
 * */
void IOL_AL_DL_ReadParam_Cnf(IOL_Port_t *port, uint8_t value, IOL_Errors errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	al->odh.readData[al->odh.nextDataPos++] = value;
	switch (errorInfo) {
		case IOL_ERROR_NONE:
			al->odh.errorInfo = IOL_ERRTYP_NONE;

			break;
		case IOL_ERROR_NO_COMM:
			al->odh.errorInfo = IOL_ERRTYPDERIEVD_COMM_ERR;
			break;
		case IOL_ERROR_STATE_CONFLICT:
			al->odh.errorInfo = IOL_ERRTYPISDU_SERV_NOTAVAIL_LOCCTRL;
			break;
		default:
			break;
	}
}

/* IOL_AL_DL_WriteParam_Cnf service is used to receive response of IOL_DL_WriteParam_Cnf service
 * */
void IOL_AL_DL_WriteParam_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	switch (errorInfo) {
		case IOL_ERROR_NONE:
			al->odh.errorInfo = IOL_ERRTYP_NONE;
			break;
		case IOL_ERROR_NO_COMM:
			al->odh.errorInfo = IOL_ERRTYPDERIEVD_COMM_ERR;
			break;
		case IOL_ERROR_STATE_CONFLICT:
			al->odh.errorInfo = IOL_ERRTYPISDU_SERV_NOTAVAIL_LOCCTRL;
			break;
		default:
			break;
	}
}

/* IOL_AL_DL_ISDUTransport_Cnf service is used to receive response of IOL_DL_ISDUTransport_Cnf service
 * */
void IOL_AL_DL_ISDUTransport_Cnf(IOL_Port_t *port, uint8_t *data, IOL_IServiceQualifier Qualifier, IOL_Errors errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
	if(errorInfo == IOL_ERROR_NONE){
		memset(al->odh.readData,0,IOL_MAX_ISDU_BUFF_SIZE);
	}else{

	}
}

/* IOL_AL_DL_PDOutputUpdate_Cnf service is used to receive response of IO_DL_PDOutputUpdate_Cnf service
 * */
void IOL_AL_DL_PDOutputUpdate_Cnf(IOL_Port_t *port, IOL_Errors errorInfo){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	IOL_AL_SetOutput_Cnf(port, al->portNum, errorInfo);
}

/* IOL_AL_DL_PDInputTransport_Ind service is used to receive response of IOL_DL_PDInputTransport_Ind service
 * */
void IOL_AL_DL_PDInputTransport_Ind(IOL_Port_t *port, uint8_t *data, uint8_t len){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	IOL_AL_NewInput_Ind(port, al->portNum);
}

/* IOL_AL_DL_ISDUAbort_Cnf service is used to receive response of IOL_DL_ISDUAbort_Cnf service
 * */
void IOL_AL_DL_ISDUAbort_Cnf(IOL_Port_t *port){
	//TODO: To implement later
}

/* IOL_AL_DL_Event_Ind service is used to receive response of IOL_DL_Event_Ind service
 * */
void IOL_AL_DL_Event_Ind(IOL_Port_t *port, IOL_EventInstance instance, IOL_EventType type,
		IOL_EventMode mode, IOL_DeviceEventCodes eventCode, uint8_t eventsLeft)
{
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	if(al->evth.eventCount < IOL_MAX_EVENT){
		IOL_EventX_t *event = &((IOL_AL_t *)IOL_GetALInstance(port))->evth.events[al->evth.eventCount++];
		event->eventCode = eventCode;
		event->eventQual.field.instance = instance;
		event->eventQual.field.source = IOL_EVENTSRC_DEVICE;
		event->eventQual.field.type = type;
		event->eventQual.field.mode = mode;
		al->evth.eventsLeft = eventsLeft;
		al->evth.cmd = IOL_AL_EVTH_CMD_DL_EVENT_IND;
		//Propagate to the MA
		IOL_AL_Event_Ind(port, IOL_GetPortNumber(port), al->evth.eventCount, event);
	}


}

/*State machines--------------------------------------------*/
/*@brief State machine for on request data handler state
 * */
void IOL_AL_ODHandler(IOL_Port_t *port){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	switch (al->odh.state){
		case IOL_AL_ODH_STATE_ONREQ_IDLE_0:
			/* AL service invocations from the Master applications or from the SM Portx handler (see
			 * Figure 57) can be accepted within this state.
			 * */
			if( al->odh.srvId == IOL_AL_SRVID_READ_REQ ||
				al->odh.srvId == IOL_AL_SRVID_WRITE_REQ)
			{
				/* T1: state 0 to 1
				 * Memorize the port number "Portx"
				 * */
				portNumber = al->portNum;

				al->odh.nextDataPos = 0;
				al->odh.state = IOL_AL_ODH_STATE_BUILD_DL_SERVICE_1;
			}else if(al->odh.srvId == IOL_AL_SRVID_ABORT_REQ){
				/*T17: state 0 to 0
				 * Return negative AL service confirmation with port number "Portx"
				 * */
				//TODO: to understand

			}
			break;
		case IOL_AL_ODH_STATE_BUILD_DL_SERVICE_1:
			/* Within this state AL service calls are checked, and corresponding DL services are
			 * created within the subsequent states. In case of an error in the arguments of the AL
			 * service a negative AL confirmation is created and returned.
			 * */
			if(al->odh.argError != IOL_ERROR_NONE){
				/* T2: state 1 to 4
				 * Prepare negative AL service confirmation
				 * */
				//TODO: to implement later
				switch (al->odh.argError) {
					case IOL_ERROR_VALUE_OUT_OF_RANGE:
						al->odh.errorInfo = IOL_ERRTYPISDU_PAR_VALOUTOFRNG;
						break;
					default:
						break;
				}
				al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
			}else if(al->odh.srvId == IOL_AL_SRVID_READ_REQ){
				al->odh.rwDir = IOL_RW_DIR_READ;
				if(al->odh.index <= 1){
					/* T3: state 1 to 2
					 * Prepare DL_ReadParam for Index 0 or 1
					 * */
					//TODO: incomplete, to understand
					al->odh.address = (al->odh.index*IOL_PARAM_PAGE2_START_ADDR) +
							(al->odh.subindex>0 ? al->odh.subindex -1 : 0);
					al->odh.dataLen = (al->odh.subindex>0 ? 1 :IOL_PARAM_PAGE_MAX_LEN);
					IOL_DL_ReadParam_Req(port, al->odh.address);
					al->odh.nextDataPos++;
					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2;
				}else if(al->odh.mseqCapb.field.isdu == IOL_MSEQCAPB_ISDU_SUPPORTED){	// index >= 2 & isdu supported
					/* T6: state 1 to 3
					 *
					 * */
					al->odh.dataLen = 0;
					IOL_ISDUValueList_t vl={	.index = al->odh.index,
												.subindex = al->odh.subindex,
												.dataLen = al->odh.dataLen,
												.data = 0,
												.rwDir = al->odh.rwDir,
												};
					IOL_DL_ISDUTransport_Req(port, &vl);
					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_ISDU_CNF_3;
				}
			}else if(al->odh.srvId == IOL_AL_SRVID_WRITE_REQ){
				al->odh.rwDir = IOL_RW_DIR_WRITE;
				if (al->odh.index == 1) {
					/* T4: state 1 to 2
					 * Prepare DL_WriteParam for Index 1
					 * */
					//TODO: incomplete, to understand

					al->odh.address = IOL_PARAM_PAGE2_START_ADDR +
							(al->odh.subindex>0 ? al->odh.subindex -1 : 0);
					al->odh.dataLen = (al->odh.subindex>0 ? 1 :IOL_PARAM_PAGE_MAX_LEN);
					IOL_DL_WriteParam_Req(port, al->odh.address, al->odh.writeData[al->odh.nextDataPos++]);

					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2;
				}else if(al->odh.index >= 2 &&
						al->odh.mseqCapb.field.isdu == IOL_MSEQCAPB_ISDU_SUPPORTED)
				{
					/* T7: state 1 to 3
					 *
					 * */
					//TODO: incomplete, to understand, specialy writeDataLen
//					al->odh.writeDataLen = (al->odh.subindex>0 ? 1 :IOL_PARAM_PAGE_MAX_LEN);
					IOL_ISDUValueList_t vl={	.index = al->odh.index,
												.subindex = al->odh.subindex,
												.dataLen = al->odh.dataLen,
												.data = al->odh.writeData,
												.rwDir = al->odh.rwDir,
												};
					IOL_DL_ISDUTransport_Req(port, &vl);
					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_ISDU_CNF_3;
				}else if(al->odh.index == 2 &&
						al->odh.mseqCapb.field.isdu == IOL_MSEQCAPB_ISDU_NOT_SUPPORTED)
				{
					/* T5: state 1 to 2
					 * Prepare DL_Write for Address 0x0F if the Device does not support ISDU.
					 * */
					//TODO: incomplete, to understand

					al->odh.address = IOL_PARAM_PAGE2_START_ADDR +
							(al->odh.subindex>0 ? al->odh.subindex -1 : 0);
					al->odh.dataLen = (al->odh.subindex>0 ? 1 :IOL_PARAM_PAGE_MAX_LEN);
					IOL_DL_WriteParam_Req(port, al->odh.address, al->odh.writeData[al->odh.nextDataPos++]);

					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2;
				}else if(al->odh.index == 0 && al->odh.subindex == 16  &&
						al->odh.mseqCapb.field.isdu == IOL_MSEQCAPB_ISDU_NOT_SUPPORTED)
				{
					/* T18: state 1 to 2
					 *
					 * */
					//TODO: incomplete, to understand

					al->odh.address = IOL_PARAM_PAGE2_START_ADDR;
					al->odh.dataLen = 1;
					IOL_DL_WriteParam_Req(port, al->odh.address, al->odh.writeData[al->odh.nextDataPos++]);

					al->odh.state = IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2;
				}
			}
			break;
		case IOL_AL_ODH_STATE_AWAIT_DL_PARAM_CNF_2:
			/* Within this state the AL service call is transformed in a sequence of as many
			 * DL_ReadParam or DL_WriteParam calls as needed (Direct Parameter page access;
			 * see page communication channel in Figure 7). All asynchronously occurred AL service
			 * invocations except AL_Abort are rejected (see 3.3.7).
			 * */
			if(al->odh.srvId == IOL_AL_SRVID_ABORT_REQ){
				/* T9: state 2 to 4
				 * All current DL service actions are abandoned, and a negative AL service
				 * confirmation is prepared.
				 * */
				//TODO: incomplete, to understand, how to abandoned current DL service actions

				al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
			}else if(al->odh.srvId == IOL_AL_SRVID_READ_CNF){

				if(al->odh.errorInfo != IOL_ERRTYP_NONE){
					/* T8: state 2 to 2
					 * Return negative AL_Read service confirmation on this asynchronous service call.
					 * */

					IOL_AL_Read_Cnf(port, al->portNum, 0, 0, al->odh.errorInfo);

				}else if(al->odh.dataLen > al->odh.nextDataPos){
					/* T10: state 2 to 2
					 * Call next DL_ReadParam or DL_WriteParam service if not all OD are transferred.
					 * */


					al->odh.address++;
					al->odh.nextDataPos++;
					IOL_DL_ReadParam_Req(port, al->odh.address);
				}else{
					/* T13: state 2 to 4
					 * Prepare positive AL_Read service confirmation.
					 * */
					//TODO: implement , to understand
					al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
				}
			}else if(al->odh.srvId == IOL_AL_SRVID_WRITE_CNF){
				if(al->odh.errorInfo != IOL_ERRTYP_NONE){
					/* T8: state 2 to 2
					 * Return negative AL_Write service confirmation on this asynchronous service call.
					 * */

					IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
				}else if(al->odh.dataLen > al->odh.nextDataPos){
					/* T10: state 2 to 2
					 * Call next DL_ReadParam or DL_WriteParam service if not all OD are transferred.
					 * */
					//TODO: incomplete , to understand
					al->odh.address++;
					IOL_DL_WriteParam_Req(port, al->odh.address,al->odh.writeData[al->odh.nextDataPos++]);
				}else{

					/* T14: state 2 to 4
					 * Prepare positive AL_Write service confirmation.
					 * */
					//TODO: implement later, to understand
					al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
				}
			}


			break;
		case IOL_AL_ODH_STATE_AWAIT_DL_ISDU_CNF_3:
			/* Within this state the AL service call is transformed in a DL_ISDUTransport service call
			 * (see ISDU communication channel in Figure 7). All asynchronously occurred AL service
			 * invocations except AL_Abort are rejected (see 3.3.7).
			 * */
			if(al->odh.srvId == IOL_AL_SRVID_ABORT_REQ){
				/* T11: state 3 to 4
				 * All current DL service actions are abandoned, and a negative AL service
				 * confirmation is prepared.
				 * */

				IOL_DL_ISDUAbort_Req(port);
				al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
			}else if(al->odh.iservQual.field.iService == IOL_ISDU_IS_QUAL_DEV_WRSP_N_4 ||
					al->odh.iservQual.field.iService == IOL_ISDU_IS_QUAL_DEV_RRSP_N_12){
				/* T12: state 3 to 3
				 * Return negative AL_Read service confirmation on this asynchronous service call.
				 * */
				//TODO: to understand
				if(al->odh.rwDir == IOL_RW_DIR_READ){
					IOL_AL_Read_Cnf(port, al->portNum, 0, 0,  al->odh.errorInfo);
				}else{
					IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
				}
			}else if(al->odh.iservQual.field.iService == IOL_ISDU_IS_QUAL_DEV_WRSP_P_5 ||
					al->odh.iservQual.field.iService == IOL_ISDU_IS_QUAL_DEV_RRSP_P_13){
				/* T15: state 3 to 4
				 * Prepare positive AL service confirmation
				 * */
				//TODO: implement later, to understand
				al->odh.state = IOL_AL_ODH_STATE_BUILD_AL_CNF_4;
			}
			break;
		case IOL_AL_ODH_STATE_BUILD_AL_CNF_4:
			/* Within this state an AL service confirmation is created depending on an argument error,
			 * the DL service confirmation, or an AL_Abort
			 * */
			if(al->odh.argError != IOL_ERROR_NONE){
				if(al->odh.rwDir == IOL_RW_DIR_READ){
					IOL_AL_Read_Cnf(port, al->portNum, 0, 0, al->odh.errorInfo);
				}else{
					IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
				}
			}else if(al->odh.srvId == IOL_AL_SRVID_ABORT_REQ){
				//TODO: implement later, to understand
				if(al->odh.rwDir == IOL_RW_DIR_READ){
					IOL_AL_Read_Cnf(port, al->portNum, 0, 0, al->odh.errorInfo);
				}else{
					IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
				}

			}else{	// For successfully completed request
				if(al->odh.mseqCapb.field.isdu == IOL_MSEQCAPB_ISDU_SUPPORTED){

					/* ISDU communication channel
					 *
					 * */
					//TODO: to the data for isdu
					if(al->odh.rwDir == IOL_RW_DIR_READ){
						IOL_AL_Read_Cnf(port, al->portNum, al->odh.readData, al->odh.dataLen, al->odh.errorInfo);
					}else{
						IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
					}
				}else{
					if(al->odh.rwDir == IOL_RW_DIR_READ){
						IOL_AL_Read_Cnf(port, al->portNum, al->odh.readData,al->odh.dataLen, al->odh.errorInfo);
					}else{
						IOL_AL_Write_Cnf(port, al->portNum, al->odh.errorInfo);
					}
				}

			}
			al->odh.state = IOL_AL_ODH_STATE_ONREQ_IDLE_0;
			break;
		default:
			break;
	}
}


/*@brief State machine for event handler state
 * */
void IOL_AL_EventHandler(IOL_Port_t *port){
	IOL_AL_t *al = (IOL_AL_t *)IOL_GetALInstance(port);
	//TODO: To implement later
    switch (al->evth.state) {
        case IOL_AL_EVTH_STATE_EVENT_INACTIVE_0:
        	/* The AL Event handling of the Master is inactive. */
        	if(al->evth.cmd == IOL_AL_EVTH_CMD_ACTIVATE){
        		/* T1: state 0 to 1*/
        		al->evth.eventsLeft = 0;
        		al->evth.eventCount = 0;
        		memset(al->evth.events,0,sizeof(al->evth.events));
        		al->evth.state = IOL_AL_EVTH_STATE_EVENT_IDLE_1;
        	}

            break;
        case IOL_AL_EVTH_STATE_EVENT_IDLE_1:
        	/* The Master AL is ready to accept DL_Events (diagnosis information) from the DL. */
        	if(al->evth.cmd == IOL_AL_EVTH_CMD_DEACTIVATE){
        		/* T2: state 0 to 1*/
        		al->evth.state = IOL_AL_EVTH_STATE_EVENT_INACTIVE_0;
        	}else if(al->evth.cmd == IOL_AL_EVTH_CMD_DL_EVENT_IND){
        		/* T3: state 1 to 2*/
        		if(al->evth.eventCount > 0){
        			al->evth.state = IOL_AL_EVTH_STATE_READ_EVENT_SET_2;
        		}
        	}else if(al->evth.cmd == IOL_AL_EVTH_CMD_AL_EVENT_REQ){
        		/* T7: state 1 to 1
        		 * invoke AL_Event.ind
        		 * */
        		IOL_AL_Event_Ind(port, al->portNum, al->evth.eventCount, al->evth.events);
        	}

            break;
        case IOL_AL_EVTH_STATE_READ_EVENT_SET_2:
        	/* The Master AL received a DL_Event_ind with diagnosis information. After this first
        	 * DL_Event.ind, the AL collects the complete set (1 to 6) of DL_Events of the current
        	 * EventTrigger (see 11.6).
        	 * */
        	if(al->evth.cmd == IOL_AL_EVTH_CMD_DL_EVENT_IND){
				if(al->evth.eventsLeft > 0){
					/* This parameter indicates the number of unprocessed Events.
					 * T4: state 2 to 2
					 * */
				}else{
					/* T5: state 2 to 3
					 * if there is no event left
					 * */
					IOL_AL_Event_Ind(port, al->portNum, al->evth.eventCount, al->evth.events);
					al->evth.state = IOL_AL_EVTH_STATE_DU_EVENT_HANDLING_3;
				}
        	}
            break;
        case IOL_AL_EVTH_STATE_DU_EVENT_HANDLING_3:
        	/* The Master AL remains in this state as long as the Diagnosis Unit (see 11.6) did not
        	 * acknowledge the AL_Event.ind.
        	 * */
        	if(al->evth.cmd == IOL_AL_EVTH_CMD_DL_EVENT_RSP){
				/* T6: state 3 to 1
				 * DL_EventConf.req
				 * */
        		IOL_DL_EventConf_Req(port);
        		al->evth.state = IOL_AL_EVTH_STATE_EVENT_IDLE_1;
        	}

            break;
        default:
        	break;
    }
}





