/*
 * IOLink.c
 *
 *  Created on: Oct 30, 2023
 *      Author: wsrra
 */

#include "IOLink.h"
#include "IOLink_Utility.h"
#include "IOLink_AL.h"
#include "IOLink_DL.h"
#include "IOLink_PL.h"
#include "IOLink_Time.h"

#include "IOLink_SM.h"
#include "IOLink_MA.h"
#include <stdlib.h>
#include "Debug.h"
#include "string.h"

/*IOLink main structure
 * */
struct IOL_Port{
	uint8_t portNum;
	uint8_t currentJobId;
	IOL_Port_Info_t portInfo;		// port configuration

	IOL_MA_t ma;
	IOL_AL_t al;
	IOL_DL_t dl;
	IOL_PL_t pl;
	IOL_SM_t sm;
//	IOL_Timer_t timer;

} ;

IOL_Port_t ports[1] = {0};




//struct IOL_Master
//{
//	uint8_t port_cnt;
//	IOL_Port_t ports[IOL_MAX_PORT];
//};
/** @brief  Initialize master and port
  * @param  master: pointer to the Iolink master structure
  * 		numOfPort: number of port need to be initialize
  * @retval none
  */
void IOL_InitMaster(IOL_Master_t *master, uint8_t numOfPort){

	if(!(numOfPort <= IOL_MAX_PORT)) return;
	for(uint8_t i = 0; i < numOfPort; i++){
		ports[i].portNum = i+1;
		master->ports[i] = &ports[i];//(IOL_Port_t*)malloc(sizeof(IOL_Port_t));
//		*master->ports[i] = *((IOL_Port_t*)&port);
//		DBG_SPRINT_NL("master->ports: %p", master->ports[i]);
	}

}


/* get port. 0 should not be the port number
 * */
void IOL_GetPort(IOL_Master_t *master, uint8_t portnumber, IOL_Port_t** port){
	if(portnumber > 0 && portnumber <= IOL_MAX_PORT){
		*port = master->ports[(portnumber - 1)];
	}


}

uint8_t IOL_GetPortNumber(IOL_Port_t *port){
	return port->portNum;
}

IOL_Port_Info_t *IOL_GetPortInfo(IOL_Port_t *port)
{
	return &port->portInfo;
}

void *IOL_GetMAInstance(IOL_Port_t *port)
{
	return &port->ma;
}

void *IOL_GetALInstance(IOL_Port_t *port)
{
	return &port->al;
}

void *IOL_GetDLInstance(IOL_Port_t *port)
{
	return &port->dl;
}

void *IOL_GetPLInstance(IOL_Port_t *port)
{
	return &port->pl;
}

void *IOL_GetSMInstance(IOL_Port_t *port){
	return &port->sm;
}

//void *IOL_GetTimerInstance(IOL_Port_t *port){
//	return &port->timer;
//}

void *IOL_GetCMInstance(IOL_Port_t *port)
{
	return &port->ma.cm;
}

void *IOL_GetDSInstance(IOL_Port_t *port)
{
	return &port->ma.ds;
}
void *IOL_GetODEInstance(IOL_Port_t *port)
{
	return &port->ma.ode;
}
void *IOL_GetDUInstance(IOL_Port_t *port)
{
	return &port->ma.du;
}
void *IOL_GetPDEInstance(IOL_Port_t *port)
{
	return &port->ma.pde;
}


