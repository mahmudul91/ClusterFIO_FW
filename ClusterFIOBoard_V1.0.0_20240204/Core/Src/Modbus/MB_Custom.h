/*
 * MB_Custom.h
 *
 *  Created on: Feb 11, 2023
 *      Author: Mahmudul Hasan (Russell)
 *      
 */

#ifndef SRC_MODBUS_MB_CUSTOM_H_
#define SRC_MODBUS_MB_CUSTOM_H_
#include "stdint.h"
#include "MB_Addresses.h"


/*Parameter for the Modbus custom protocol*/
#define MB_CUST_NUM_OF_SLAVE_MAX		24
#define MB_CUST_NUM_OF_IR_AI			4	// value is four because of there are
#define MB_CUST_IR_AI1_ADDR           	MB_IRA_AI1_VALUE_12BIT
#define MB_CUST_IR_AI2_ADDR          	MB_IRA_AI2_VALUE_12BIT
#define MB_CUST_IR_AI3_ADDR          	MB_IRA_AI3_VALUE_12BIT
#define MB_CUST_IR_AI4_ADDR          	MB_IRA_AI4_VALUE_12BIT

#define MB_CUST_NUM_OF_HR_AO			4	// value is four because of there are
#define MB_CUST_HR_AO1_ADDR           	MB_HRA_AO1_DUTY
#define MB_CUST_HR_AO2_ADDR          	MB_HRA_AO2_DUTY
#define MB_CUST_HR_AO3_ADDR          	MB_HRA_AO3_DUTY
#define MB_CUST_HR_AO4_ADDR          	MB_HRA_AO4_DUTY

#define MB_CUST_NUM_OF_DI				8	// value is four because of there are
#define MB_CUST_DI_START_ADDR           MB_DIA_DI1_STATUS
#define MB_CUST_NUM_OF_CO				8	// value is four because of there are
#define MB_CUST_CO_START_ADDR           MB_COA_DO1_ENABLE

#define MB_CUST_MQ_QUE_SIZE				32		// can stor 32 msg
#define MB_CUST_MQ_MSG_SIZE				16U  // msg uffer size

#define MB_CUST_SLV_START_ADDR_INDEX 	2		// slave start address in 107 request packet
#define MB_CUST_NUM_OF_SLV_INDEX		3		// number of slave in 107 request packet
#define MB_CUST_STATUS_BYTE_INDEX		11		// status byte index in the rtu packet
#define MB_CUST_DATA_START_BYTE_INDEX	3		// data start byte index in the rtu packet
#define MB_CUST_DATA_BLOCK_SIZE			10

#define MB_CUST_ERROR_BYTE_INDEX		3
#define MB_CUST_MASTER_DATA_START_INDEX	10

#define MB_CUST_ONLINE					0x80
#define MB_CUST_OFFLINE					0x00
#define MB_CUST_ERROR					0x40

#define MB_CUST_107_REQ_PACK_SIZE		6 		// request packet size for 107 pack form master
#define MB_CUST_107_RSP_PACK_SIZE		15 		// response packet size for 107 pack


typedef enum MB_CUST_FuncCode{
	/*------- Custom Function code--------*/
	MB_CFC_RC_101 = 101U,	//101 (0x65), RC-Read Coils
	MB_CFC_RDI_102,			//102 (0x66), RDI-Read Discrete Inputs
	MB_CFC_RHR_103,			//103 (0x67), RHR-Read Holding Registers
	MB_CFC_RIR_104,			//104 (0x68), RIR-Read Input Registers
	MB_CFC_WMC_105,			//105 (0x69), WMC-Write Multiple Coil
	MB_CFC_WMR_106,			//106 (0x68), WMR-Write Multiple Register
	MB_CFC_RIRDI_107,		//107 (0x69), RMIRDI-Read Input Register & Discrete input
	MB_CFC_RHRCO_108,		//108 (0x6A), RMHRCO-Read Holding Register & Coil output
	MB_CFC_WMHRCO_109,		//109 (0x6B), WMHRCO-Write Multiple Holding Register & Coil output
	MB_CFC_RMHRIRCODI_110,		//110 (0x6B), RMHICD-Read Multiple Holding & Input Register and Coil & Input
}MB_CUST_FuncCode;


typedef enum MB_CUST_ExcepCode{
	MB_ECC_ISSA_12 = 12,// Invalid Slave Start Address
	MB_ECC_INS_13,// Invalid number of slave
}MB_CUST_ExcepCode;

typedef struct MB_CUST_WaitingTimer{
	uint8_t enable;
	uint64_t time;
	uint32_t timeout;
	uint32_t isTimeRst;
}MB_CUST_WaitingTimer;

typedef enum MB_CUST_PackType{
	MB_CUST_PAK_TYP_MSTR_REQ = 0, //* 0 = master request packet on broadcast & custom function code
	MB_CUST_PAK_TYP_SLV_RESP,		// 1 = slave response packet on broadcast & custom function code
}MB_CUST_PackType;


typedef struct  MB_CUST_MQ_Msg{
    uint8_t buff[MB_CUST_MQ_MSG_SIZE];
    uint16_t size;
}MB_CUST_MQ_Msg;

typedef struct MB_CUST_MQ_Queue{
//	uint32_t id;
	uint8_t startIndex;
	uint8_t endIndex;
	uint8_t noOfMsg;
	MB_CUST_MQ_Msg msg[MB_CUST_MQ_QUE_SIZE];
}MB_CUST_MQ_Queue;


typedef enum MB_CUST_EventType{
	MB_CUST_EVENT_NONE = 0,
	MB_CUST_EVENT_107_RECVD,
	MB_CUST_EVENT_107_TIMEOUT,			//Receive timeout
}MB_CUST_EventType;

typedef struct MB_Custom{
	uint8_t funCode;
	uint8_t slvStartAddr;
	uint8_t numOfSlv;
	uint8_t respondingSlave;
	uint8_t enableRetrans;
	MB_CUST_WaitingTimer reTransTimer;
	MB_CUST_WaitingTimer waitingTimer;
	uint8_t timeoutCount;
	uint8_t rcvdCount;
	uint8_t isReadyToSend;
	uint8_t eventType;
	uint8_t rcvFailedCount[MB_CUST_NUM_OF_SLAVE_MAX+1];	 // consecutive count for no responding slave
	uint8_t rcvConsecCount[MB_CUST_NUM_OF_SLAVE_MAX+1];   // consecutive healthy packet count

}MB_Custom;

void MB_CUST_Init(MB_Custom *custom);
uint16_t MB_CUST_BuildRTUReqRIRDI(MB_Custom *custom, uint8_t *buff);
uint16_t MB_CUST_ProcessRTUReqRIRDI(void);
uint16_t MB_CUST_BuildEmptySubPackRIRDI(uint8_t *buff, uint8_t slvAddr);
uint16_t MB_CUST_BuildEmptySubPacket(uint8_t *buff, uint8_t status);

uint8_t MB_CUST_CheckCFunCode(uint8_t funCode);

uint8_t MB_CUST_GetPacketType(uint8_t *buff);

void MB_CUST_StartTimer(MB_CUST_WaitingTimer *timer);
void MB_CUST_StopTimer(MB_CUST_WaitingTimer *timer);
void MB_CUST_ResetTimer(MB_CUST_WaitingTimer *timer);
void MB_CUST_SetTimeout(MB_CUST_WaitingTimer *timer, uint32_t timeout);
uint8_t MB_CUST_IsTimerRunning(MB_CUST_WaitingTimer *timer);
uint8_t MB_CUST_IsTimeout(MB_CUST_WaitingTimer *timer);



uint8_t MBR_CUST_readIRDI(void);
uint8_t MBR_CUST_readHRCO(void);
uint8_t MBR_CUST_writeHRCO(void);
void MBR_CUST_BuildExcepResp(uint8_t excepCode);


uint8_t MBRS_CUST_GetSlvStartAddr(void);

uint8_t MBRS_CUST_EnqueMsg(uint8_t *buff, uint16_t size);
uint16_t MBRS_CUST_DequeMsg(uint8_t *buff);
void MBRS_CUST_ClearMQ(void);

/*Callback functions for the custom function code
 * */
void MB_CUST_attachProcess109ReqAODOCallback(uint8_t (*callbackFunc)(uint8_t *buff, uint16_t size));
void MB_CUST_attachBuild107RespAIDICallback(uint8_t (*callbackFunc)(uint8_t *buff, uint16_t size));




#endif /* SRC_MODBUS_MB_CUSTOM_H_ */
