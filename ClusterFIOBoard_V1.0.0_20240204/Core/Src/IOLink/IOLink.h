/*
 * IOLink.h
 *
 *  Created on: Oct 30, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_H_
#define SRC_IOLINK_IOLINK_H_
#include "stdint.h"
#include "IOLink_Define.h"
#include "IOLink_Types.h"

#define IOL_GET_PORT(portNum) (iolMaster.ports[portNum-1])

/*IOL port structure----------*/
typedef struct IOL_Port IOL_Port_t;




/*IOL Master structure*/
typedef struct IOL_Master{
	IOL_ArgBlock_MasterIdent_t ident;
	uint8_t port_cnt;
	IOL_Port_t *ports[IOL_MAX_PORT];
//	uint64_t time;
}IOL_Master_t;

IOL_Master_t iolMaster;

/* Port information
 * */
typedef struct IOL_ATTRIB_PACKED IOL_Port_Info_t{
    uint32_t deviceid;
    uint16_t vendorid;
    uint16_t functionid;
    uint8_t revisionid;
    uint8_t cycletime;
    uint8_t portQualityInfo;
    IOL_PortStatusInfo portStatusInfo;
    IOL_TransRate transRate;
    uint8_t serialnumber[16];
} IOL_Port_Info_t;


void IOL_InitMaster(IOL_Master_t *master, uint8_t numOfPort);
void IOL_GetPort(IOL_Master_t *master, uint8_t portnumber, IOL_Port_t** port);
uint8_t IOL_GetPortNumber(IOL_Port_t *port);
IOL_Port_Info_t *IOL_GetPortInfo(IOL_Port_t *port);

void *IOL_GetMAInstance(IOL_Port_t *port);
void *IOL_GetALInstance(IOL_Port_t *port);
void *IOL_GetDLInstance(IOL_Port_t *port);
void *IOL_GetPLInstance(IOL_Port_t *port);
void *IOL_GetSMInstance(IOL_Port_t *port);
//void *IOL_GetTimerInstance(IOL_Port_t *port);
void *IOL_GetCMInstance(IOL_Port_t *port);
void *IOL_GetDSInstance(IOL_Port_t *port);
void *IOL_GetODEInstance(IOL_Port_t *port);
void *IOL_GetDUInstance(IOL_Port_t *port);
void *IOL_GetPDEInstance(IOL_Port_t *port);
void *IOL_GetALInstance(IOL_Port_t *port);




#endif /* SRC_IOLINK_IOLINK_H_ */
