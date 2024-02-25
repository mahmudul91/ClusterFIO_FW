/*
 * Client.h
 *
 *  Created on: Feb 3, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_W5500_CLIENT_H_
#define SRC_W5500_CLIENT_H_
#include "stdint.h"
#include "w5500_chip.h"
#include "w5500.h"
#include "SocketHandler.h"

#define CLIENT_SOCK_MAX			1

//#define


typedef struct Client{
	uint8_t sockIndex;				// current socket id
	Socket sock[CLIENT_SOCK_MAX];
//	W5500_CmnIntr *intr;					// contain socket-0 to 7, conflict, UNREACH, PPPoE & MP interrupt

}Client;


void Client_TCP(Client *client);


#endif /* SRC_W5500_CLIENT_H_ */
