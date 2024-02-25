/*
 * MB_TCP.c
 *
 *  Created on: Oct 19, 2022
 *      Author: wsrra
 */

#include <MB_Handler.h>
#include "MB_TCP.h"
#include "MB_MsgQ.h"
#include "Debug.h"
#include "MB_Tools.h"


//extern DebugStruct dbug;


/*
 * Sets the TCP net information
 * */
void MBTCP_SetNetInfo(MBTCP_NetInfo *netInfo, uint8_t *ip, uint8_t *mac, uint8_t *sn, uint8_t *gw, uint8_t *dns, uint8_t dhcpMode){
	memcpy(netInfo->ip, ip, (sizeof(netInfo->ip)/sizeof(netInfo->ip[0])));
	memcpy(netInfo->mac, mac, (sizeof(netInfo->mac)/sizeof(netInfo->mac[0])));
	memcpy(netInfo->ip, sn, (sizeof(netInfo->sn)/sizeof(netInfo->sn[0])));
	memcpy(netInfo->ip, gw, (sizeof(netInfo->gw)/sizeof(netInfo->gw[0])));
	memcpy(netInfo->ip, dns, (sizeof(netInfo->dns)/sizeof(netInfo->dns[0])));
	netInfo->dhcpMode = dhcpMode;
}



/**
* @brief Add header to the Modbus TCP packet
* @param buff: pointer of a buffer to be send
* @param header: modbus header struct pointer
* @retval none
*/
void MBTCP_AddHeader(uint8_t *buff, MBTCP_Header *header){
	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_TRANS_ID_HI], &buff[MBTCP_INDX_TRANS_ID_LO], header->transId);
	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_PROTOCOL_ID_HI], &buff[MBTCP_INDX_PROTOCOL_ID_LO], header->protocolId);
	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_LENGTH_HI], &buff[MBTCP_INDX_LENGTH_LO], header->length);
	buff[MBTCP_INDX_UNIT_ID] =  header->unitId;
}


void MBTCP_ParseHeader(uint8_t *buff, MBTCP_Header *header){

	header->transId = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_TRANS_ID_HI], buff[MBTCP_INDX_TRANS_ID_LO]);
	header->protocolId = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_PROTOCOL_ID_HI], buff[MBTCP_INDX_PROTOCOL_ID_LO]);
	header->length = MBTOOL_CombBytesToU16(buff[MBTCP_INDX_LENGTH_HI], buff[MBTCP_INDX_LENGTH_LO]);
	header->unitId = buff[MBTCP_INDX_UNIT_ID];
}



void MBTCP_SetHeader(uint8_t *buff, MBTCP_Header *header){

	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_TRANS_ID_HI], &buff[MBTCP_INDX_TRANS_ID_LO], header->transId);
	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_PROTOCOL_ID_HI], &buff[MBTCP_INDX_PROTOCOL_ID_LO], header->protocolId);
	MBTOOL_SplitU16ToBytes(&buff[MBTCP_INDX_LENGTH_HI], &buff[MBTCP_INDX_LENGTH_LO], header->length);
	buff[MBTCP_INDX_UNIT_ID] = header->unitId;
}


/**
  * @brief To check the function code
  * @param functionCode - received function code
  * @retval 1U or 0U, returns 1U for valid function code, or return 0U for invalid function code
  */
uint8_t MBTCP_isFunCodeValid(uint8_t functionCode){
	// 1-227 is the range of supported function code public + user define function code
	return ((functionCode >= MB_FC_RC_01 && functionCode <= MB_FC_WSR_06)
	|| (functionCode >= MB_FC_WMC_15 && functionCode <= MB_FC_WMR_16)
//	|| (functionCode >= MB_FC_MWR_22 && functionCode <= MB_FC_RWMR_23)
//	|| (functionCode == 66u)
	);
}
