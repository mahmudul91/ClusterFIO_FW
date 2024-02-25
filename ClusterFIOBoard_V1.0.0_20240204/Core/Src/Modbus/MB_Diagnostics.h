/*
 * MB_Diagnostics.h
 *
 *  Created on: Feb 11, 2022
 *      Author: RASEL_EEE
 */

#ifndef INC_MB_DIAGNOSTICS_H_
#define INC_MB_DIAGNOSTICS_H_
#include "main.h"
#include "MB_RTUSlave.h"


//#define MB_DIAG_REG_SET_TIM 5000U	// 5000ms = 5s

/* Sub Function codes for diagnostics function */
typedef enum{
	MB_DSFC_RQD_0 = 0,			//00, Return Query Data
	MB_DSFC_RCO_1,				//01, Restart Communications Option
	MB_DSFC_RDR_2,				//02, Return Diagnostic Register
	MB_DSFC_CAID_3,				//03, Change ASCII Input Delimiter
	MB_DSFC_FLOM_4 = 4,				//04, Force Listen Only Mode
	MB_DSFC_CCDR_10 = 10,		//10, Clear Counters and Diagnostic Register
	MB_DSFC_RBMC_11,			//11, Return Bus Message Count
	MB_DSFC_RBCEC_12,			//12, Return Bus Communication Error Count
	MB_DSFC_RBEEC_13,			//13, Return Bus Exception Error Count
	MB_DSFC_RSMC_14,			//14, Return Server Message Count
	//MB_DSFC_RSNRC_15,			//15, Return Server No Response Count
	//MB_DSFC_RSNC_16,			//16, Return Server NAK Count
	//MB_DSFC_RSBC_17 = 17,			//17, Return Server Busy Count
	MB_DSFC_RBCOC_18 = 18,			//18, Return Bus Character Overrun Count
	MB_DSFC_COCF_20 = 20,		//20, Clear Overrun Counter and Flag
}MB_DiagSubFuncCode;



typedef enum MB_DIAG_RegStatus{
	MB_DIAG_REG_RESET = 0U,
	MB_DIAG_REG_SET,
}MB_DIAG_RegStatus;


typedef union MB_DiagReg{
    struct{
        uint16_t    _0:1,  _1:1,  _2:1,  _3:1,
					_4:1,  _5:1,  _6:1,  _7:1,
                    _8:1,  _9:1,  _10:1, _11:1,
					_12:1, _13:1, _14:1, _15:1;
    }bit;
    uint16_t value;
}MB_DiagReg;


typedef struct MB_RTUDiag{


	uint8_t listen_only_mode;				// listen only mode
	uint8_t device_restart_flag;			// used for restart comm. operation
	uint16_t bus_msg_count;					//  Bus Message Count
	uint16_t bus_com_err_count;				// Bus Communication Error Count
	uint16_t bus_exp_err_count;				// Bus Exception Error Count
	uint16_t slv_msg_count;					// Slave Message Count
	uint16_t bus_char_ovrun_count;			// Bus Character Overrun Count
	uint16_t comm_event_count;				// 11 (0x0B) Get Comm Event Counter

	MB_DiagReg diagReg;						// diagnostic register




}MB_RTUDiag;

/*
 *
 * Modbus IOP Overrun Errors Flag
 * Timer 14 Test Failed
 * Timer 3 Test Failed
 * Timer 15 Test Failed
 *
 * */
typedef enum MB_DIAG_RegisterAddress{
	MB_DIAG_REG0_MB_OVRUN_ERRS = 0U,
	//MB_DIAG_REG1_MB_LSTN_ONLY_MODE,
	MB_DIAG_REG2_TIM14_TEST_FAILED,				// diagnostic reg2 for timer14 test flag
	MB_DIAG_REG3_TIM3_TEST_FAILED,				// diagnostic reg3 for timer3 test flag
	MB_DIAG_REG4_TIM15_TEST_FAILED,				// diagnostic reg3 for timer3 test flag
	MB_DIAG_REG5_ZC_INTRPT_TEST_FAILED,
	MB_DIAG_REG_MAX,
}MB_DIAG_RegisterAddress;

//typedef struct MB_Diag{
//	volatile uint16_t MB_DBM_Count;			//  Bus Message Count
//	volatile uint16_t MB_DBCE_Count;		// Bus Communication Error Count
//	volatile uint16_t MB_DECE_Count;		// Bus Exception Error Count
//}MB_Diag;


void MB_DIAG_SetRTUSlaveInstance(MB_RTUSlave *_mbSlave);

uint8_t MB_DIAG_CheckDSFC(uint8_t subFuncCode);			// to check the diagnostics sub-function code
uint8_t MB_DIAG_CheckData(uint8_t subFuncCode, uint16_t dataValue);			// to check the diagnostics sub-function code



uint8_t MB_DSF_ReturnQD(MB_RTUDiag *diag);			// to return the query data
uint8_t MB_DSF_RestartCO(MB_RTUDiag *diag);			// to Restart Communications Option
uint8_t MB_DSF_ReturnDR(MB_RTUDiag *diag);			// to Return Diagnostic Register
uint8_t MB_DSF_ForceLOM(MB_RTUDiag *diag);			// to Force Listen Only Mode
uint8_t MB_DSF_ClearCDR(MB_RTUDiag *diag);			// to Clear Counters and Diagnostic Register
uint8_t MB_DSF_ReturnBMC(MB_RTUDiag *diag);			// to Return Bus Message Count
uint8_t MB_DSF_ReturnBCEC(MB_RTUDiag *diag);			// to Return Bus Communication Error Count
uint8_t MB_DSF_ReturnBEEC(MB_RTUDiag *diag);			// to Return Bus Exception Error Count
uint8_t MB_DSF_ReturnSMC(MB_RTUDiag *diag);			// to Return Server Message Count
uint8_t MB_DSF_ReturnSBC(MB_RTUDiag *diag);			// to Return Server Busy Count
uint8_t MB_DSF_ReturnBCOC(MB_RTUDiag *diag);			// to Return Bus Character Overrun Count
uint8_t MB_DSF_ClearOCF(MB_RTUDiag *diag);			// to Clear Overrun Counter and Flag



void MB_DIAG_attachCallback(uint8_t (*callbackFunc)(uint16_t subFuncCode, uint16_t value));

void MB_ClearCounters(MB_RTUDiag *diag);									// to reset the counters, events and register on restart
uint8_t MB_CheckCountersClear(MB_RTUDiag *diag);									// to reset the counters, events and register on restart
void MB_appendToRespPDU_SFC(void);

uint16_t MB_getDiagSFC(void);
uint16_t MB_getDiagData(void);

#endif /* INC_MB_DIAGNOSTICS_H_ */
