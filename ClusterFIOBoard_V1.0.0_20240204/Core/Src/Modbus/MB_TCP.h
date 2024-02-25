/*
 * MB_TCP.h
 *
 *  Created on: Oct 19, 2022
 *      Author: wsrra
 */

#ifndef INC_MB_TCP_H_
#define INC_MB_TCP_H_
#include <MB_Handler.h>
#include "stdint.h"
#include "MB_Config.h"
#include "MB_MsgQ.h"
#include "MB_ACL.h"


/*
 *  (MBAP) Modbus application protocol header
 *  Size: 7 bytes
 *  -------------------------------------------------------
 *  | Field Name| TransId | protocolId | length  | uintId |
 *  |------------------------------------------------------
 *  | Size		| 2 bytes | 2 bytes	   | 2 bytes | 1-byte |
 *  -------------------------------------------------------
 * */





typedef enum MBTCP_Status{
	MBTCP_STATUS_ERROR = 0U,
	MBTCP_STATUS_OK,
	MBTCP_STATUS_BUSY,
	MBTCP_STATUS_INVALID_TRANS_ID,
	MBTCP_STATUS_INVALID_PROTOCOL_ID,
	MBTCP_STATUS_INVALID_LENGTH,
	MBTCP_STATUS_INVALID_UNIT_ID,
	MBTCP_STATUS_INVALID_FUNC_CODE,
	MBTCP_STATUS_INVALID_RESPONSE,
	MBTCP_STATUS_EXCED_MAX_TRANS,
	MBTCP_STATUS_NO_TRANS,
	MBTCP_STATUS_RESP_TIMEOUT,
	MBTCP_STATUS_EXCED_MAX_RETRY,
	MBTCP_STATUS_MAX,
}MBTCP_Status;


/*
 * Modbus TCP Header index of the ADU buffer for the following fields
 * */
typedef enum MB_TCPHeaderIndx{
	MBTCP_INDX_TRANS_ID_HI = 0,			// buffer index for the transaction id High byte
	MBTCP_INDX_TRANS_ID_LO,			// buffer index for the transaction id low byte
	MBTCP_INDX_PROTOCOL_ID_HI,			// buffer index for the protocol id
	MBTCP_INDX_PROTOCOL_ID_LO,			// buffer index for the protocol id
	MBTCP_INDX_LENGTH_HI,				// buffer index for the pdu length id
	MBTCP_INDX_LENGTH_LO,				// buffer index for the pdu length id
	MBTCP_INDX_UNIT_ID,				// buffer index for the unit id (slave id)
	MBTCP_INDX_HEADER_MAX,
}MB_TCPHeaderIndx;


/*
 * FIFO type circular msg queue
 * */
typedef struct MBTCP_MsgQue{
	uint32_t id;
	MBMQ_Queue que;
}MBTCP_MsgQue;


typedef struct MBTCP_NetInfo{
   uint8_t mac[6];  ///< Mac Address
   uint8_t ip[4];   ///< IP Address
   uint8_t sn[4];   ///< Subnet Mask
   uint8_t gw[4];   ///< Gateway IP Address
   uint8_t dns[4];  ///< DNS server IP Address
   uint8_t dhcpMode;  ///< 1 - Static, 2 - DHCP
}MBTCP_NetInfo;


/*Transaction info*/
typedef struct MBTCP_TransInfo{
	uint32_t id;
	uint16_t transId;		//	Transaction Identifier: Identification of a MODBUS Request / Response transaction
}MBTCP_TransInfo;




typedef struct MBTCP_Header{
	uint16_t transId;		//	Transaction Identifier: Identification of a MODBUS Request / Response transaction
	uint16_t protocolId;	//	Protocol Identifier: Always 0x0000, (0 = MODBUS protocol)
	uint16_t length;		//	Length: Number of bytes of the following fields, including the Unit Identifier and data fields.
	uint8_t	unitId;			//	Unit Identifier: Identification of a remote slave connected on a serial line or on other buses.
}MBTCP_Header;


void MBTCP_SetNetInfo(MBTCP_NetInfo *netInfo, uint8_t *ip, uint8_t *mac, uint8_t *sn, uint8_t *gw, uint8_t *dns, uint8_t dhcpMode);
void MBTCP_AddHeader(uint8_t *buff, MBTCP_Header *header);
void MBTCP_ParseHeader(uint8_t *buff, MBTCP_Header *header);
void MBTCP_SetHeader(uint8_t *buff, MBTCP_Header *header);
uint8_t MBTCP_isFunCodeValid(uint8_t functionCode);


#endif /* INC_MB_TCP_H_ */
