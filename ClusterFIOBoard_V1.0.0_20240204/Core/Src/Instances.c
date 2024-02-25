/*
 * Instances.c
 *
 *  Created on: Feb 25, 2024
 *      Author: wsrra
 */

#include "Instances.h"
#include "GlobalVars.h"

extern GlobalVar_t gVar;

/*@brif get time stamp instance
 * */
TimeStamp* INST_GetTimeStamp(void){
	return &gVar.ts;
}
