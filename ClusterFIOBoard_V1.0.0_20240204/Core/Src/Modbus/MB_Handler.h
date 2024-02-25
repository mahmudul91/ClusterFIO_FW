/*
 * MB_Handler.h
 *
 *  Created on: Jan 18, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_HANDLER_H_
#define INC_MB_HANDLER_H_

#include "main.h"
#include "MB_CRC.h"

#include "MB_Addresses.h"
#include "string.h"
//#include "stm32f1xx_it.h"
#include "stdlib.h"
#include "MB_RTUSlave.h"
#include "MB_Diagnostics.h"
#include "MB_Config.h"



//#define mb_res_max_size 255						// maximum data unit size for the response


#define MB_CO_ADDR_OFFSET 	 			1U						// coil address offset by 1
#define MB_DI_ADDR_OFFSET 	 			10001U					// discrete input address offset by 10001
#define MB_HR_ADDR_OFFSET 	 			40001U					// holding register address offset by 40001
#define MB_IR_ADDR_OFFSET 	 			30001U					// input register address offset by 30001

#define MB_MAX_CR_ADDR 		 			10000U					// max address for coils and registers
#define MB_MAX_COILS 					2000U					// max coils in the Modbus protocol is 2000
#define MB_MAX_WM_COILS 				1968U					// max multiple coils to write in the Modbus protocol is 1968
#define MB_MAX_REGISTERS 				125U		// max registers in the Modbus protocol is 2000

#define MB_MAX_REGISTER_VALUE 			0xFFFF	// max registers value


#define MB_FC_MAX						127U		// max supported function code
#define MB_EFC_OFFSET					0x80U		// exception function code offset for te exception


#define MB_TRANS_TIMEOUT				300U			//At 9600 baud rate the time to send 1 byte is 1.042 ms, so for 256 bytes, the time will be 266.75 ms, so that, the timeout for transmitting 256 bytes must be greater then 266.75 ms.


#define MB_AND_MASK_16T8				0x00FF		// AND mask to get the lower 8 bits

#define MB_VALUE_MULTIPLIER_10			10.0f
#define MB_VALUE_MULTIPLIER_100			100.0f
#define MB_VALUE_MULTIPLIER_1000		1000.0f



#define MB_DIAG_ES_SCAN_TIM 			10000U	// 10000ms = 10s


typedef enum MB_ErrorStatus{
	MB_ERROR = 0U,
	MB_OK,
}MB_ErrorStatus;

typedef enum MB_Status{
	MB_STATUS_OK = 0U,
	MB_STATUS_BUSY,
	MB_STATUS_READY,
	MB_STATUS_ERROR,
}MB_Status;



//typedef enum{
//	false = 0,
//	true = 1,
//}bool;

/*Coil state value*/
typedef enum{
	LOW = 0x0000,
	HIGH = 0xFF00,
}MB_CoilValue;


typedef enum MB_ExcepStatus{
	MB_ES_RESET = 0U,
	MB_ES_SET,
}MB_ExcepStatus;




/* Function code list */
typedef enum MB_FuncCode{
	MB_FC_RC_01 = 1U,		//01 (0x01), RC-Read Coils
	MB_FC_RDI_02,			//02 (0x02), RDI-Read Discrete Inputs
	MB_FC_RHR_03,			//03 (0x03), RHG-Read Holding Registers
	MB_FC_RIR_04,			//04 (0x04), RIG-Read Input Registers
	MB_FC_WSC_05,			//05 (0x05), WSC-Write Single Coil
	MB_FC_WSR_06,			//06 (0x06), WSR-Write Single Register
	MB_FC_RES_07,			//07 (0x07), RES-Read Exception Status (Serial Line only)
	MB_FC_DIAG_08,			//08 (0x08), DIAG-Diagnostics (Serial Line only)
	MB_FC_GCEC_11 = 11U,		//11 (0x0B), GCEC-Get Communication Event Counter (Serial Line only)
	//MB_FC_GCEL_12,			//12 (0x0C), GCEL-Get Communication Event Log (Serial Line only)
	MB_FC_WMC_15 = 15U,		//15 (0x0F), WMC-Write Multiple Coils
	MB_FC_WMR_16,			//16 (0x10), WMR-Write Multiple registers
	//MB_FC_RSI_17,			//17 (0x11), RSI-Report Server ID (Serial Line only)
	//MB_FC_RFR_20 = 20U,		//20 (0x14), RFR-Read File Record
	//MB_FC_WFR_21,			//21 (0x15), WFR-Write File Record
	MB_FC_MWR_22 = 22U,			//22 (0x16), MWR-Mask Write Register
	MB_FC_RWMR_23,			//23 (0x17), RWMR-Read/Write Multiple registers
	//MB_FC_RFQ_24,			//24 (0x18), RFQ-Read FIFO Queue
//	MB_FC_EIT_43 = 43U,		//43 (0x2B), EIT-Encapsulated Interface Transport

}MB_FuncCode;




/*
 * Exception Code enumerator
 */
typedef enum MB_ExcepCode{
	MB_EC_NONE_0 = 0,				//0, NE - NO EXCEPTION
	MB_EC_ILF_1,				//01, ILF - ILLEGAL FUNCTION
	MB_EC_ILDA_2,				//02, ILDA - ILLEGAL DATA ADDRESS
	MB_EC_ILDV_3,				//03, ILDV - ILLEGAL DATA VALUE
	MB_EC_SDV_4,				//04, SDV - SERVER DEVICE FAILURE
	MB_EC_ACK_5,				//05, ACK - ACKNOWLEDGE
	MB_EC_SDB_6,				//06, SDB - SERVER DEVICE BUSY
	MB_EC_MPE_8 = 8,			//08, MPE - MEMORY PARITY ERROR
	MB_EC_GPU_10 = 10,			//10, GPU - GATEWAY PATH UNAVAILABLE
	MB_EC_GTDFR_11,				//11, GTDFR - GATEWAY TARGET DEVICE	FAILED TO RESPOND

}MB_ExcepCode;

typedef enum {
	MB_NRS_254 = 254u,			//254, modbus no response status
	MB_RS_255,					//255, modbus response status
}RespStatus;



/*
 *To make the PDU buffer byte index readable
 */
typedef enum{
	MB_SA_INDEX = 0,				// 0, Slave address index
	MB_FC_INDEX,					// 1, Function code index

	MB_RWCR_STA_HB_INDEX,		// 2, Read-write-Coil-Register, starting address High byte index
	MB_RWCR_STA_LB_INDEX,			// 3, Read-write-Coil-Register, starting address Low byte index
	MB_RCR_RQ_HB_INDEX,				// 4, Read-Coil-Register, requested quantity high byte index
	MB_RCR_RQ_LB_INDEX,				// 5, Read-Coil-Register, requested quantity Low byte index
	MB_WSCR_OV_HB_INDEX = 5,		// 5, write-Single-Coil-Register, output value high byte index
	MB_WSCR_OV_LB_INDEX,			// 6, write-Single-Coil-Register, output value low byte index
	MB_WMCR_RQ_HB_INDEX = 5,		// 5, Read-Multiple-Coil-Register, requested quantity high byte index
	MB_WMCR_RQ_LB_INDEX,			// 6, write-Multiple-Coil-Register, requested quantity low byte index
//
//	MB_DIAG_SFC_HB_INDEX = 3, 		//3, Diagnostics, sub function code High byte index
//	MB_DIAG_SFC_LB_INDEX, 			//4, Diagnostics, sub function code Low byte index
//	MB_RFR_BC_INDEX = 3, 			//3, Read File Record, byte count
//	MB_WFR_BC_INDEX = 3,			//3, Write File Record, byte count High byte index
//	MB_MWR_RA_HB_INDEX = 3,			//3, MWR-Mask Write Register, Reference Address High byte index
//	MB_MWR_RA_LB_INDEX,				//4, MWR-Mask Write Register, Reference Address Low byte index
//	MB_RWMR_RSA_HB_INDEX = 3, 		//3, RWMR-Read/Write Multiple registers, Read Starting Address High byte index
//	MB_MB_RWMR_RSA_LB_INDEX, 		//4, RWMR-Read/Write Multiple registers, Read Starting Address byte index
//	MB_RFQ_FPA_HB_INDEX = 3, 		//3, RFQ-Read FIFO Queue, FIFO Pointer Address High byte index
//	MB_RFQ_FPA_LB_INDEX, 			//4, RFQ-Read FIFO Queue, FIFO Pointer Address byte index
//	MB_EIT_MT_INDEX = 3,			//3, EIT-Encapsulated Interface Transport, MEI Type


}MB_PDUBufferIndex;


typedef enum MB_RTUMode{
	MB_RTU_MODE_SLAVE = 0,
	MB_RTU_MODE_MASTER,
}MB_RTUMode;


// for the Modbus slave properties
//typedef struct MB_Slave
//{
//	uint8_t slave_address;							// 1 byte, slave address of this device
//	uint8_t rxBuffer[MB_PDU_MAX_SIZE];		// PDU (application data unit) buffer for the request
//	uint8_t txBuffer[MB_PDU_MAX_SIZE];		// PDU buffer for the response
//	uint8_t rxBufferSize;			// received adu buffer size
//	uint8_t txBufferSize;			//response adu buffer size
//
//	uint16_t bus_msg_count;					//  Bus Message Count
//	uint16_t bus_com_err_count;				// Bus Communication Error Count
//	uint16_t bus_exp_err_count;				// Bus Exception Error Count
//	uint16_t slv_msg_count;					// Slave Message Count
//	uint16_t bus_char_ovrun_count;			// Bus Character Overrun Count
//	uint16_t comm_event_count;				// 11 (0x0B) Get Comm Event Counter
//	uint8_t busy_status;
//	uint8_t listen_only_mode;
//	uint8_t device_restart_flage;
//
//	uint16_t diag_register;
//	uint8_t excepStatus;
//
//}MB_Slave;

/*Modbus handler for modbus RTU
 * */
typedef struct MB_RTUHandler{
//	uint8_t slave_address;					// 1 byte, slave address of this device
	MB_RTUMode mode;
	uint8_t *rxBuffer;				// pDU (application data unit) buffer for the request
	uint8_t *txBuffer;				// pDU buffer for the response
	uint8_t rxBufferSize;			// received pdu buffer size
	uint8_t txBufferSize;			//response pdu buffer size
	uint8_t is_req_cpy_to_resp;

	MB_RTUDiag diag;						// modbus diagnostics
	uint8_t busy_status;
	uint8_t excepStatus;
	uint8_t isBusy;
}MB_RTUHandler;


/*modbus tcp server hadler
// * */
//typedef struct MB_TCPSHandler{
//	uint8_t *rxBuffer;				// pDU (application data unit) buffer for the request
//	uint8_t *txBuffer;				// pDU buffer for the response
//	uint8_t rxBufferSize;			// received pdu buffer size
//	uint8_t txBufferSize;			//response pdu buffer size
//	uint8_t is_req_cpy_to_resp;
//
//	uint8_t excepStatus;
//}MB_TCPSHandler;
//
//
///*modbus tcp client hadler
// * */
//typedef struct MB_TCPCHandler{
//	uint8_t *rxBuffer;				// pDU (application data unit) buffer for the request
//	uint8_t *txBuffer;				// pDU buffer for the response
//	uint8_t rxBufferSize;			// received pdu buffer size
//	uint8_t txBufferSize;			//response pdu buffer size
//	uint8_t is_req_cpy_to_resp;
//
//	uint8_t excepStatus;
//}MB_TCPCHandler;
//




void MB_init(MB_RTUHandler *_mbHandle);												// to initialize the Modbus RTU slave

void MB_SetHandlerInstance(MB_RTUHandler *_mbHandle);
MB_RTUHandler *MB_GetHandlerInstance(void);
uint8_t MB_IsHandlerBusy(void);
void MB_SetHandlerBusy(uint8_t flag);


void MB_SetRTUMode(MB_RTUMode mode);
MB_RTUMode MB_GetRTUMode(void);

void MB_ClearBuffer(void);
void MB_ClearReqBuffer(void);
void MB_ClearResBuffer(void);

void MB_appendToRespPDU(uint8_t data);
void MB_appendToRespPDU_SAFC(void);
void MB_appendToRespPDU_CRC(void);

void MB_applyMaskToHR(uint16_t *value, uint16_t andMask, uint16_t orMask);
void MB_CopyReqToRespPDU(void);
uint8_t MB_getDI(uint16_t address, uint8_t *value);
uint8_t MB_getDIInByte(uint8_t *byteArray, uint8_t size, uint16_t startAddress, uint16_t endAddress);


uint8_t MB_checkSlaveAddress(uint16_t address);
void MB_setReqBuff(uint8_t *buff, uint8_t size);
void MB_setRespBuff(uint8_t *buff);
uint8_t MB_getRespBuffSize(void);
uint8_t MB_getSlaveAddress(void);
uint8_t MB_getFunctionCode(void);								// to get the function code
uint16_t MB_getSCRAddress(void);								// to get the diagnostics sub function
uint16_t MB_getStartAddress(uint8_t firstByteIndex);			// to get the start address
uint16_t MB_get2byteData(uint8_t firstByteIndex);					// to get 2 byte of data from PDU buffer
uint8_t MB_getIRValue(uint16_t address, uint16_t *value);
uint8_t MB_getIRsValue(uint16_t *value, uint8_t size, uint16_t startAddress, uint16_t lastAddresss);
uint8_t MB_getMCValueInByte(uint8_t index);
uint16_t MB_getMRValueIn2Byte(uint8_t index);

uint8_t MB_setHRValue(uint16_t address, uint16_t value);
uint8_t MB_setCoil(uint16_t address, uint16_t value);

uint8_t MB_isFunctionCodeValid(uint8_t functionCode);				// to check the function code is valid or not
uint8_t MB_executeRequest(uint8_t functionCode);				// to execute the request
void MB_excepRespBuilder(uint8_t excepCode);						// to build up the exception response PDU



uint8_t MB_readCoils(void);											// to read coils status
uint8_t MB_readDI(void);											// to read discrete input status
uint8_t MB_readHR(void);											// to read holding register
uint8_t MB_readIR(void);											// to read input register
uint8_t MB_writeSC(void);											// to write single coil
uint8_t MB_writeSHR(void);											// to write single register
uint8_t MB_readES(void);											// to read Exception status


uint8_t MB_writeMC(void);											// to write multiple coils
uint8_t MB_writeMHR(void);											// to write multiple register
uint8_t MB_readWriteMHR(void);										// to read/write multiple register
uint8_t MB_writeHRM(void);											// to write register mask



/*Function for Modbus Diagnostics for RTU only-----------*/
void MB_CountBusMsg(void);							// count bus msg counter by 1
void MB_CountBusComErr(void);
void MB_CountBusExpErr(void);
void MB_CountSlaveMsg(void);
void MB_CountBusCharOvrun(void); 							// count bus char overrun counter by 1
void MB_CountCommEvent(void);
uint8_t MB_GetBusyStatus(void);
void MB_SetBusyStatus(void);
void MB_ClearBusyStatus(void);

uint8_t MB_GetListenOnlyMode(void);
void MB_SetListenOnlyMode(void);
void MB_ClearListenOnlyMode(void);

uint8_t MB_GetRestartFlag(void);
void MB_SetRestartFlag(void);
void MB_CleartRestartFlag(void);


uint16_t MB_GetDiagRegValue(void);
void MB_setDiagRegValue(uint8_t bitAddr, uint16_t bitValue);
void  MB_ClearDiagReg(void);


uint8_t MB_GetExcepStatus(void);
void MB_SetExcepStatus(uint8_t regAddress, uint8_t bitValue);
uint8_t MB_ClearExcepStatus(void);

/*-------Callback function--------*/
void MB_CallbackRCO(void);

void MB_attachDICallback(uint8_t (*callbackReadFunc)(MB_DIAddrEnum address, uint8_t *value));
void MB_attachCOCallback(uint8_t (*callbackWriteFunc)(MB_CoilAddrEnum address, uint8_t value), uint8_t (*callbackReadFunc)(MB_CoilAddrEnum address, uint8_t *value));
void MB_attachIRCallback(uint8_t (*callbackReadFunc)(MB_IRAddrEnum address, uint16_t *value));
void MB_attachHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t value), uint8_t (*callbackReadFunc)(MB_HRAddrEnum address, uint16_t *value));
void MB_attachMaskWHRCallback(uint8_t (*callbackWriteFunc)(MB_HRAddrEnum address, uint16_t andMask, uint16_t orMask));
void MB_attachChangeSACllback(uint8_t (*callbackFunc)(uint8_t slaveAddress));

#endif /* INC_MB_HANDLER_H_ */
