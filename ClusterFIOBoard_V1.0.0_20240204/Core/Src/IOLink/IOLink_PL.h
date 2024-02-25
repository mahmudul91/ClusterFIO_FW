/*
 * IOLink_PL.h
 *
 *  Created on: Oct 28, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_PL_H_
#define SRC_IOLINK_IOLINK_PL_H_
#include "IOLink.h"
#include "IOLink_Define.h"
#include "l6360.h"

#ifndef IOL_HW_L6360
	#define IOL_HW_L6360
#endif

/* TargetMode
 * Ref: IO-Link Interface Spec v1.1.3, section 5.2.2.1,
 * */
typedef enum IOL_PL_TargetMode{
	IOL_PL_MODE_INACTIVE = 0U,		// (C/Q line in high impedance)
	IOL_PL_MODE_DI,					// DI (C/Q line in digital input mode),
	IOL_PL_MODE_DO,					// DO (C/Q line in digital output mode),
	IOL_PL_MODE_SDCI,    			// SDCI (C/Q line in digital), actually SDCI is COM1 or COM2 or COM3
	IOL_PL_MODE_COM1,				// COM1 (C/Q line in COM1 mode),
	IOL_PL_MODE_COM2,				// COM2 (C/Q line in COM2 mode),
	IOL_PL_MODE_COM3,				// COM3 (C/Q line in COM3 mode)
}IOL_PL_TargetMode;


/* Transfer status
 * Ref: IO-Link Interface Spec v1.1.3, section 5.2.2.3,
 * */
typedef enum IOL_PL_ErrorStatus{
	IOL_PL_ERR_STATUS_NONE = 0,			// no error status
	IOL_PL_ERR_STATUS_PARITY_ERROR,		// (UART detected a parity error),
	IOL_PL_ERR_STATUS_FRAMING_ERROR, 	// (invalid UART stop bit detected),
	IOL_PL_ERR_STATUS_OVERRUN, 			// (octet collision within the UART)
}IOL_PL_ErrorStatus;

typedef struct __attribute__((__packed__)) IOL_PL_t{
	IOL_PL_TargetMode mode;
	IOL_PL_ErrorStatus errorStatus;
	//TODO: to implement later

	/*IOLink master chip*/
#ifdef IOL_HW_L6360
	L6360_IOLink_hw_t* l6360;
#endif

}IOL_PL_t;


/*Other services not in spec-----------------------*/
#ifdef IOL_HW_L6360
IOL_Errors IOL_PL_Initialize(uint8_t portNum, L6360_IOLink_hw_t *l6360);
#endif


/*services are in spec*/
void IOL_PL_SetMode_req(IOL_Port_t *port, IOL_PL_TargetMode mode);
void IOL_PL_WakeUp_req(IOL_Port_t *port);
IOL_PL_ErrorStatus IOL_PL_Transfer_req(IOL_Port_t *port, uint8_t *data, uint8_t len);
IOL_Errors IOL_PL_Transfer_ind(IOL_Port_t *port, uint8_t *data, uint8_t len, IOL_PL_ErrorStatus errorStatus);


#endif /* SRC_IOLINK_IOLINK_PL_H_ */
