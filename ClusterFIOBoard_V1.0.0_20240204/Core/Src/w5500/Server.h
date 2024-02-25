/*
 * Server.h
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#ifndef INC_SERVER_H_
#define INC_SERVER_H_
//#include <MB_Msgq.h>
#include "w5500.h"
#include "wizchip_conf.h"
#include "SocketHandler.h"
#include "w5500_chip.h"
#include "MB_Config.h"


#define SERVER_SOCK_MAX			MBTCPS_MAX_CONNS		// Max sockets are used, 1 socket can handle 1 client
#define SOCK_USE_AS_UDP			0xFF	// set a socket number to use as a UDP socket, or set 0xFF to use no socket as a UDP socket

/*
 * 1 socket can handle 1 client
 * so number of socket = number of client
 *
 * */

typedef enum ServerMode{
	SERVER_MODE_NONE = 0,
	SERVER_MODE_TCP,
	SERVER_MODE_UDP,
	SERVER_MODE_MACRAW,
}ServerMode;


typedef struct Server{
	uint8_t sockIndex;				// current socket id
//	wiz_NetInfo netInfo;
//	uint8_t protocol[SOCK_MAX];
//	uint16_t port[SOCK_MAX];
	Socket sock[SERVER_SOCK_MAX];
//	uint8_t flag[SOCK_MAX];
//	W5500_CmnIntr *intr;					// contain socket-0 to 7, conflict, UNREACH, PPPoE & MP interrupt

}Server;



//void ServerInit(Server *server);
void  Server_SockInit(Server *server, uint8_t sockId, uint8_t protocol, uint16_t port, uint8_t flag, uint16_t kpAlvTime);

void Server_Handler(Server *server, ServerMode mode);
//int32_t Server_send(Server *server, uint8_t sn, uint8_t * buf, uint16_t len);
void Server_TCP(Server *server);
void Server_UDP(Server *server);
//void Server_StatusHandler(Server *server, uint8_t status, int8_t opStatus );
//void Server_ReadInterpt(Server *server);
void Server_Disconnect(Server *server);
int32_t Serve_Send(uint8_t sockId, uint8_t *buff, uint16_t size);




#endif /* INC_SERVER_H_ */
