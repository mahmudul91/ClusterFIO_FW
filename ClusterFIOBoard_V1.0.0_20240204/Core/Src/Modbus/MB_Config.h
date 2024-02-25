/*
 * MB_Config.h
 *
 *  Created on: Oct 24, 2022
 *      Author: wsrra
 */

#ifndef SRC_MODBUS_MB_CONFIG_H_
#define SRC_MODBUS_MB_CONFIG_H_

#include "Define.h"

/*
 * to use custom function code and protocol for the TCP
 * */
#define MB_TCP_CUSTOM




// Un-comment to enable the protocol
//Implemented protocol
//#define MB_RTU_SLAVE						// to enable RTU slave protocol
#define MB_TCP_SERVER						// to enable TCP server protocol

//Not Implemented yet, But planning to implement
//#define MB_TCP_CLIENT						// to enable TCP client protocol
//#define MB_RTU_MASTER						// to enable RTU master protocol
//#define MB_ASCII_MASTER					// to enable ASCII master protocol
//#define MB_ASCII_SLAVE					// to enable ASCII slave protocol


/*Modbus common config*/
#define MB_BROADCAST_ADDR					0U		// 0 is the broadcast address for the modbus protocol
#define MB_SLAVE_ADDR_MAX					247U	// 247 is the max individual salve address, but 248 to 255 addresses are reserved

/*Modbus Serial line---------------*/

#define MB_PDU_MAX_SIZE 					256U		// maximum application data unit size for the request
#define MB_PDU_MIN_SIZE 	 				4U		// minimum application data unit size for the request, SA-1byte, FC-1byte, CRC-2bytes
#define MB_RESP_TIIMEOUT 	 				1000		// inms

/*Modbus RTU Master---------------*/

#define MBRM_RETRY_MAX						1
#define MBRM_REPLY_TIMEOUT					1000 // in ms
#define MBRM_RETRANS_TIMEOUT_CUST			100 // in ms

/*Modbus gateway---------------*/
#define MBG_TIMEOUT							1000 // in ms


/*------Modbus TCP configuration------*/
/* Modbus TCP common configurations */
#define MBTCP_PROTOCOL_ID					0x0000	// protocol id for the Modbus TCP protocol is always 0
#define MBTCP_SERVER_UNIT_ID				0xFF    // 0xFF is default unit id for requesting from client to the server
#define MBTCP_BUFF_SIZE						DEF_MBTCP_MAX_BUFF_SIZE
#define MBTCP_PDU_START_INDEX				7U		//

/* Modbus TCP server configurations */
#define MBTCPS_MAX_TRANS					DEF_MBTCPS_MAX_TRANS		// Modbus server can handle only this number of transaction
#define MBTCPS_MAX_CONNS 					DEF_SERVER_MAX_TCP_CONNS

/*Modbus TCP client configuration-------*/

#define MBTCPC_MAX_TRANS					1		// we can set max transaction up to 16






//#define MBTCP_IP							{0,0,0,0}
//#define MBTCP_MAC							{0,0,0,0}

/* Modbus TCP client configurations */

#define MBTCPC_MAX_CONNS				DEF_CLIENT_MAX_TCP_CONNS		// max number of connections are supported


/*For custom protocol*/

#endif /* SRC_MODBUS_MB_CONFIG_H_ */
