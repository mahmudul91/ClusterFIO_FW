/*
 * Client.c
 *
 *  Created on: Feb 3, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */
#include "Client.h"
#include "stdint.h"

/*
 *
 * */
void Client_init(Client *client){
	//TODO: to implement leter
}


void Client_TCP(Client *client){
	if (client->sockIndex >= CLIENT_SOCK_MAX) client->sockIndex = 0;
	Socket_Handler(&client->sock[client->sockIndex++]);


}
