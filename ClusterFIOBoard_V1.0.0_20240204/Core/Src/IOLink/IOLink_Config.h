/*
 * IOL_Config.h
 *
 *  Created on: Oct 26, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_CONFIG_H_
#define SRC_IOLINK_IOLINK_CONFIG_H_
#include "IOLink_Define.h"
#include <stdint.h>
/*Client info--------------*/



/*Master Identification--------------------------*/
#define  IOL_CNF_MAX_PORTS					1U

#define  IOL_CNF_MSTRIDNT_ARGBLOCK_ID		IOL_ARGBLOCK_ID_MASTERIDENT	// 2-bytes IOLink specific
#define  IOL_CNF_MSTRIDNT_VENDOR_ID 		0x0001	//2-bytes unique vendor id, provided by IOLink community
#define  IOL_CNF_MSTRIDNT_MASTER_ID 		0xFF000001	//4-bytes unique vendor specific master id
#define  IOL_CNF_MSTRIDNT_MASTER_TYPE		IOL_MASTER_TYPE_ACC_TO_SPEC	// 1-byte, Master according to this specification or later
#define  IOL_CNF_MSTRIDNT_FEATURES_1		(IOL_MSTR_FEATURES_1_DEVWRITEPARBATCH | \
											IOL_MSTR_FEATURES_1_DEVREADPARBATCH   | \
											IOL_MSTR_FEATURES_1_PORTPWROFFON )			// 1-byte, control to support three services i.e SMI_ParamWriteBatch, SMI_ParamReadBatch, SMI_PortPowerOffOn
#define  IOL_CNF_MSTRIDNT_FEATURES_2		0	// 1-byte, Reserved for future use (= 0)
#define  IOL_CNF_MSTRIDNT_MAX_PORTS			IOL_MAX_PORT	// 1-byte,  Maximum number (n) of ports of this Master
const uint8_t  IOL_CNF_MSTRIDNT_PORTTYPES[IOL_MAX_PORT] = {
	IOL_MSTR_PORTTYP_CLASS_A_PORTPWROFFON,
	IOL_MSTR_PORTTYP_CLASS_A_PORTPWROFFON,
	IOL_MSTR_PORTTYP_CLASS_A_PORTPWROFFON,
	IOL_MSTR_PORTTYP_CLASS_A_PORTPWROFFON
};


#endif /* SRC_IOLINK_IOLINK_CONFIG_H_ */
