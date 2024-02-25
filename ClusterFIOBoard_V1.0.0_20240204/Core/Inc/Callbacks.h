/*
 * OD_Callback.h
 *
 *  Created on: May 16, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_OD_CALLBACK_H_
#define INC_OD_CALLBACK_H_
#include "stdint.h"

/* init function is used to attach all kind of callback
 * */
void CB_Init(void);
void CB_DBG_Tx(uint8_t* data, uint16_t len);

/*Old*/
void CB_InitErrorCallback(void);
void CB_DIO_InitCallback(void);
void CB_MB_InitCallback(void);
void CB_Sock_InitCallback(void);
void CB_MBTCPS_InitCallback(void);

/*Callback functions for the custom function code
 * */
uint8_t CB_MB_CUST_Process109ReqAODO(uint8_t *buff, uint16_t size);
uint8_t CB_MB_CUST_Build107RespAIDI(uint8_t *buff, uint16_t size);

#endif /* INC_OD_CALLBACK_H_ */
