/*
 * IOL_Define.h
 *
 *  Created on: Oct 26, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_DEFINE_H_
#define SRC_IOLINK_IOLINK_DEFINE_H_

#include "stdint.h"
#include "IOLink_Utility.h"


#ifndef IOL_ATTRIB_PACKED
#define IOL_ATTRIB_PACKED  			__attribute__((__packed__))
#endif

/*B.1.5 RevisionID (RID):
 * This revision of the standard specifies protocol version 1.1.
 * size 8bits.
 * Minor Revision = bit 0 to 3
 * Major Revision = bit 4 to 7
 * */
#define IOL_PROTOCOL_REV_ID_V11			0x11	//<>V10 currently implemented revision version
#define IOL_PROTOCOL_REV_ID_V10			0x10	//V10 legacy device revision version

#define IOL_MAX_PORT					4U		// maximum number of supported port of the master device

#define IOL_BROADCAST_CLIENT_ID			0U
#define IOL_MAX_CLIENTS					IOL_CNF_MAX_CLIENTS
#define IOL_MAX_CLIENT_ID				4
#define IOL_BROADCAST_PORTNUM			0U

//#define IOL_MAX_MA_ODE_JOB			4

#define IOL_MAX_PD_SIZE					32U
#define IOL_MAX_OD_SIZE        			32U		// TODO: to remove sec:7.2.2.2, line: 1286
#define IOL_MAX_ADDRCTRL				31U		// the address or flow control value
#define IOL_MAX_DIR_PARM_ADDR			31U		// the address or flow control value
#define IOL_RXTX_BUFF_SIZE 				64U



#define IOL_MAX_ISDU_DATA_SIZE 			238U
#define IOL_MAX_ISDU_BUFF_SIZE 			(IOL_MAX_ISDU_DATA_SIZE + 3) /* I-Service and len + ExtLen + CHKPDU = 3 */

#define IOL_MAX_ISDU_COUNT				15U
#define IOL_MAX_ISDU_RESP_TIME			5000000U	//in us, 5000ms, See, 10.8.7 Protocol constants Table 102 – first column
#define IOL_MAX_ISDU_LENGTH				15U			// A.5.3 Extended length (ExtLength)
#define IOL_MAX_ISDU_EXTLENGTH			238U		// A.5.3 Extended length & A.5.7 ISDU examples
#define IOL_ISDU_ISEV_READ_MASK			0x08
#define IOL_DEV_ERRCODE_MASK			0x8000		// Device(ISDU) error code mask

#define IOL_MAX_EVENT_MEM_ADDRESS		18U		//
#define IOL_MAX_EVENT					6U
/*SMI-------*/

#define IOL_MAX_DEVPARBATCH_LEN			8U	 	// according ot spec it 70, Ref: Annex G.1, B.10, B11
#define IOL_MAX_DIAG_ENTRIES			64U	// Ref: B.2.21 DetailedDeviceStatus

#define IOL_COM1_BARDRATE				4800U	//4.8Kbit/s
#define IOL_COM2_BARDRATE				38400U	//38.4Kbit/s
#define IOL_COM3_BARDRATE				230400U	//230.4Kbit/s

#define IOL_TBIT_COM1					208.33F	//in us, at 4.8bit/s
#define IOL_TBIT_COM2					26.05F	//in us, at 4.8bit/s
#define IOL_TBIT_COM3					4.34F	//in us, at 4.8bit/s


/*DS index list--*/
#define IOL_MAX_DS_STORG_SIZE			2048U	// maximum data storage size
#define IOL_MAX_INDEX_LIST_ENTRIES		70U	// Each Index_List can carry up to 70 entries
#define IOL_INDEX_LIST_SIZE(n)			(n*3U + 2U)	//Size of Index List of parameter indices to be saved (REF: B.2.3 DataStorageIndex


/*
 * ISDU
 * Ref: IO-Link Interface Spec v1.1.3, section B.1.4
 * */
#define IOL_MSEQCAP_ISDU_NOT_SUPPORTED 	0U
#define IOL_MSEQCAP_ISDU_SUPPORTED 		1U

/*
 * SIO Mode
 * Ref: IO-Link Interface Spec v1.1.3, section B.1.6
 * */
#define IOL_PDI_SIO_MOD_NOT_SUPPORTED 	0U
#define IOL_PDI_SIO_MOD_SUPPORTED 		1U

#define IOL_COM1_BARDRATE 				4800U	  // 4.8Kbit/s
#define IOL_COM2_BARDRATE 				38400U  // 38.4Kbit/s
#define IOL_COM3_BARDRATE 				230400U // 230.4Kbit/s

#define IOL_TBIT_COM1 					208.33F // in us, at 4.8bit/s
#define IOL_TBIT_COM2 					26.05F  // in us, at 4.8bit/s
#define IOL_TBIT_COM3 					4.34F	  // in us, at 4.8bit/s

/* A.3.2 Bit Time
 * It should be in the range of 27 <= T_bit <= 37
 * */
#define IOL_MAX_NUM_OF_TBIT 			37U //
#define IOL_MIN_NUM_OF_TBIT 			27U //
#define IOL_NUM_OF_TBIT 				32U // default

/*A.3.3 UART frame transmission delay of Master (ports)
 * It should be in the range of 0 <= T1 <= 1 Tbit
 * */
#define IOL_MAX_T1_MLTPLR 				1U
#define IOL_MIN_T1_MLTPLR 				0U
#define IOL_DEF_T1_MLTPLR 				1U // default

/*A.3.4 UART frame transmission delay of Devices
 * It should be in the range of 0 <= T2 <= 3 Tbit
 * */
#define IOL_MAX_T2_MLTPLR 				3U
#define IOL_MIN_T2_MLTPLR 				0U
#define IOL_DEF_T2_MLTPLR 				1U // default

/*A.3.5 Response time of Devices
 * It should be in the range of 1 Tbit <= TA <= 10 Tbit
 * */
#define IOL_MAX_TA_MLTPLR 				10U
#define IOL_MIN_TA_MLTPLR 				1U
#define IOL_DEF_TA_MLTPLR 				5U // default

/*IOLink Times---------------------------------*/
#define IOL_WAKEUP_RETRY				2
/*T_REN receive enable delay for the Device, Table 10*/
#define IOL_TREN_MAX					500U			// us
#define IOL_TREN_MIN					0U				// us
#define IOL_TREN						IOL_TREN_MAX	// us
/*Master message delay, Table 42 */
#define IOL_TDMT_MAX(tbit)				(tbit*IOL_MAX_NUM_OF_TBIT)
#define IOL_TDMT_MIN(tbit)				(tbit*IOL_MIN_NUM_OF_TBIT)

//#define IOL_TDMT_WURQ 				(IOL_DEF_NUM_OF_TBIT * IOL_TBIT_COM1) //	in us, formula T_bit*1000000/bardrate
#define IOL_TDMT_COM1 					(IOL_NUM_OF_TBIT * IOL_TBIT_COM1) //	in us, formula T_bit*1000000/bardrate
#define IOL_TDMT_COM2 					(IOL_NUM_OF_TBIT * IOL_TBIT_COM2) //	in us, formula T_bit*1000000/bardrate
#define IOL_TDMT_COM3 					(IOL_NUM_OF_TBIT * IOL_TBIT_COM3) // 	in us, formula T_bit*1000000/bardrate

/*Wake-up retry delay, After TDWU the Master
 * repeats the wake-up request, Table 42
 * */
#define IOL_TDWU_MAX					50000U 	//us, 50ms
#define IOL_TDWU_MIN					30000U 	//us, 30ms
#define IOL_TDWU						40000U	//us, 40ms

/*Device detection time Time between 2 wake-up request sequences Table 42*/
#define IOL_TSD_MAX						1000000U	//us, 1s
#define IOL_TSD_MIN						500000U		//us, 0.5s
#define IOL_TSD							750000U		//us

/*IOL Frame-------*/
#define IOL_UART_FRAME_SIZE 			11U // bits/frame

/* A.3.7 Cycle time,
 * Minimum Cycle time
 * */
#define IOL_MIN_CYCLE_TIME_COM1 		18000U // in us
#define IOL_MIN_CYCLE_TIME_COM2 		2300U  // in us
#define IOL_MIN_CYCLE_TIME_COM3 		400U   // in us

#define IOL_PORT_CYC_TIME_AFAP			0U	//AFAP: (As fast as possible ‒ SM: FreeRunning -> Port cycle timing is not restricted. Default value in port mode IOL_MANUAL)

//#define IOL_PORT_QUALITY_INFO_BIT 			0
//#define IOL_PORT_QUALITY_INFO_VALID 		0
//#define IOL_PORT_QUALITY_INFO_INVALID 		BIT(IOL_PORT_QUALITY_INFO_BIT)
///* PDOut bit1 */
//#define IOL_PORT_QUALITY_INFO_PDO_BIT 		1
//#define IOL_PORT_QUALITY_INFO_PDO_VALID 	0
//#define IOL_PORT_QUALITY_INFO_PDO_INVALID 	BIT(IOL_PORT_QUALITY_INFO_PDO_BIT)
//#define IOL_PORT_QUALIFIER_INFO_PQ_BIT 		7
//#define IOL_PORT_QUALIFIER_INFO_PQ_VALID 	0
//#define IOL_PORT_QUALIFIER_INFO_PQ_INVALID 	BIT(IOL_PORT_QUALIFIER_INFO_PQ_BIT)

#define IOL_PARAM_PAGE1_START_ADDR		0U
#define IOL_PARAM_PAGE2_START_ADDR		16U
#define IOL_PARAM_PAGE_MAX_LEN			16U  // length of each page

/*ArgBlock length-----------------*/
/*Ref: Annex- E */
#define IOL_ARGBLOCK_LEN_MIN				2U
#define IOL_ARGBLOCK_LEN_PD_OFFSET			(4U)	//REF: E.10,

#define IOL_ARGBLOCK_LEN_MASTERIDENT(n)		(13U + n)	//REF: E.2, n is the number of port,
#define IOL_ARGBLOCK_LEN_PORTCONFIGLIST		(12U)		//REF: E.3,
#define IOL_ARGBLOCK_LEN_PORTSTATUSLIST(n)	(16U + 3*n)	//REF: E.4, 3 byte of each n elements
#define IOL_ARGBLOCK_LEN_ONREQUESTDATA_W(n)	(5U + n)	//REF: E.5,
#define IOL_ARGBLOCK_LEN_ONREQUESTDATA_R	(4U)		//REF: E.5,
#define IOL_ARGBLOCK_LEN_DSDATA(n)			(2U + n)	//REF: E.6,
#define IOL_ARGBLOCK_LEN_DEVPARBATCH(n)		(2U + n)	//REF: E.7,
#define IOL_ARGBLOCK_LEN_INDEXLIST(n)		(2U + n)	//REF: E.8,
#define IOL_ARGBLOCK_LEN_PORTPWROFFON		(5)			//REF: E.9,
#define IOL_ARGBLOCK_LEN_PD(n)				(IOL_ARGBLOCK_LEN_PD_OFFSET + n)	//REF: E.10,
#define IOL_ARGBLOCK_LEN_PDIN(n)			IOL_ARGBLOCK_LEN_PD(n)	//REF: E.10,
#define IOL_ARGBLOCK_LEN_PDOUT(n)			IOL_ARGBLOCK_LEN_PD(n)	//REF: E.11,
#define IOL_ARGBLOCK_LEN_PDINOUT(n,m)		(6U + n + m)//REF: E.12,
#define IOL_ARGBLOCK_LEN_PDINIQ				(3U)		//REF: E.13,
#define IOL_ARGBLOCK_LEN_PDOUTIQ			(3U)		//REF: E.14,
#define IOL_ARGBLOCK_LEN_DEVICEEVENT		(5U)		//REF: E.15,
#define IOL_ARGBLOCK_LEN_PORTEVENT			(5U)		//REF: E.16,
#define IOL_ARGBLOCK_LEN_VOIDBLOCK			(2U)		//REF: E.17,
#define IOL_ARGBLOCK_LEN_JOBERROR			(6U)		//REF: E.18,

/*Octet position of argBlock---------*/
#define IOL_ARGPOS_ARGBLOCKID_U16			0U

/*	E.2 MasterIdent
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_MasterIdentARGPosition{
	IOL_ARGPOS_MSTRIDENT_VENDORID_U16	= 2,
	IOL_ARGPOS_MSTRIDENT_MASTERID_U32 	= 4,
	IOL_ARGPOS_MSTRIDENT_MASTERTYPE_U8 	= 8,
	IOL_ARGPOS_MSTRIDENT_FEATURES1_U8 	= 9,
	IOL_ARGPOS_MSTRIDENT_FEATURES2_U8 	= 10,
	IOL_ARGPOS_MSTRIDENT_MAXPORTS_U8 	= 11,
	IOL_ARGPOS_MSTRIDENT_PORTTYPE_N 	= 12,
}IOL_SMI_MasterIdentARGPosition;


/*	E.3 PortConfigList
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_PortConfListARGPosition{
	IOL_ARGPOS_PORTCNFLIST_PORTMODE_U8 		= 2,
	IOL_ARGPOS_PORTCNFLIST_VALIDBKUP_U8 	= 3,
	IOL_ARGPOS_PORTCNFLIST_IQBEHAVIOR_U8 	= 4,	// 5 to n
	IOL_ARGPOS_PORTCNFLIST_CYCLETIME_U8 	= 5,
	IOL_ARGPOS_PORTCNFLIST_VENDORID_U16 	= 6,
	IOL_ARGPOS_PORTCNFLIST_DEVICEID_U32 	= 8,
}IOL_SMI_PortConfListARGPosition;

/*	E.4 – PortStatusList
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_PortStatusListARGPosition{
	IOL_ARGPOS_PORTSTATLIST_PORTSTATINFO_U8 = 2,
	IOL_ARGPOS_PORTSTATLIST_PORTQUALINFO_U8	= 3,
	IOL_ARGPOS_PORTSTATLIST_REVISIONID_U8	= 4,
	IOL_ARGPOS_PORTSTATLIST_TRANSRATE_U8	= 5,
	IOL_ARGPOS_PORTSTATLIST_MSTRCYCLTIM_U8	= 6,
	IOL_ARGPOS_PORTSTATLIST_INDATALEN_U8	= 7,
	IOL_ARGPOS_PORTSTATLIST_OUTDATALEN_U8	= 8,
	IOL_ARGPOS_PORTSTATLIST_VENDORID_U16	= 9,
	IOL_ARGPOS_PORTSTATLIST_DEVICEID_U32	= 11,
	IOL_ARGPOS_PORTSTATLIST_NUMOFDIAGS_U8	= 15,
	IOL_ARGPOS_PORTSTATLIST_DIAGENTRYX_U24_N= 16,	// n number of 3-bytes

}IOL_SMI_PortStatusListARGPosition;


/*	REf: E.5 On-request_Data
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_ODARGPosition{
	IOL_ARGPOS_OD_INDEX_U16 	= 2,
	IOL_ARGPOS_OD_SUBINDEX_U8 	= 4,
	IOL_ARGPOS_OD_ONREQDATA_N 	= 5,	// 5 to n
}IOL_SMI_ODARGPosition;

/*	REf: E.15 DeviceEvent & 2 E.16 PortEvent
 * */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_EventsARGPosition{
	IOL_ARGPOS_EVENT_EVENTQUAL_U8 = 2,
	IOL_ARGPOS_EVENT_EVENTCODE_U16,
}IOL_SMI_EventsARGPosition;


/* M-sequence octet index for the buffer
 * */
typedef enum IOL_MseqOctetIndex
{
	IOL_OCT_INDX_MC = 0,	 // M-sequence control octet
	IOL_OCT_INDX_CKT,		 // M-sequence type & checksum octet
	IOL_OCT_INDX_DATA_START, // Data start octet
} IOL_MseqOctetIndex;

/* IOLink Events
 * Ref: IO-Link Interface Spec v1.1.3, section A.3.8.1, Table 58 – Event memory
 * */
typedef enum IOL_EventMemAddress
{
	IOL_EVNTMEM_ADDR_STATUSCODE = 0, // Summary of status and error information. Also used to control read access for individual messages.
	IOL_EVNTMEM_ADDR_EVNTQUAL_1,	 // Type, mode and source of the Event
	IOL_EVNTMEM_ADDR_EVNTCODE_1_MSB, // MSB of 16-bit EventCode of the Event
	IOL_EVNTMEM_ADDR_EVNTCODE_1_LSB, // LSB of 16-bit EventCode of the Event
	IOL_EVNTMEM_ADDR_EVNTQUAL_2,	 //
	IOL_EVNTMEM_ADDR_EVNTCODE_2_MSB, //
	IOL_EVNTMEM_ADDR_EVNTCODE_2_LSB, //
	IOL_EVNTMEM_ADDR_EVNTQUAL_3,	 //
	IOL_EVNTMEM_ADDR_EVNTCODE_3_MSB, //
	IOL_EVNTMEM_ADDR_EVNTCODE_3_LSB, //
	IOL_EVNTMEM_ADDR_EVNTQUAL_4,	 //
	IOL_EVNTMEM_ADDR_EVNTCODE_4_MSB, //
	IOL_EVNTMEM_ADDR_EVNTCODE_4_LSB, //
	IOL_EVNTMEM_ADDR_EVNTQUAL_5,	 //
	IOL_EVNTMEM_ADDR_EVNTCODE_5_MSB, //
	IOL_EVNTMEM_ADDR_EVNTCODE_5_LSB, //
	IOL_EVNTMEM_ADDR_EVNTQUAL_6,	 //
	IOL_EVNTMEM_ADDR_EVNTCODE_6_MSB, //
	IOL_EVNTMEM_ADDR_EVNTCODE_6_LSB, //
									 // 0x13 to 0x1F Reserved for future use
} IOL_EventMemAddress;

/* IOLink DL mode
 * Ref: IO-Link Interface Spec v1.1.3, section 7.2.1.13,
 * */
typedef enum IOL_MDHMode{
	IOL_MDH_MODE_INACTIVE = 0U,		// handler shall change to INACTIVE state
	IOL_MDH_MODE_COM1,				// COM1 mode established
	IOL_MDH_MODE_COM2,				// COM2 mode established
	IOL_MDH_MODE_COM3,				// COM3 mode established
	IOL_MDH_MODE_COMLOST,				// Lost communication
	IOL_MDH_MODE_ESTABCOM,			// Handler changed to the EstablishCom state
	IOL_MDH_MODE_STARTUP,				// handler shall change to STARTUP state
	IOL_MDH_MODE_PREOPERATE,			// handler shall change to PREOPERATE state
	IOL_MDH_MODE_OPERATE,				// handler shall change to OPERATE state
}IOL_MDHMode;



/*SMI----------------------------------------------*/

/* IO-Link ArgBlock IDs
 * Table E.1 – ArgBlock types and their ArgBlockIDs
 */
typedef enum IOL_ArgBlockID{
    IOL_ARGBLOCK_ID_MASTERIDENT = 0x0001,               // Used by SMI_MasterIdentification (see 11.2.4)
    IOL_ARGBLOCK_ID_FS_MASTERACCESS = 0x0100,          // Used by ...
    IOL_ARGBLOCK_ID_W_MASTERCONFIG = 0x0200,           // Used by ...
    IOL_ARGBLOCK_ID_PDIN = 0x1001,                     // Used by SMI_PDIn (see 11.2.17)
    IOL_ARGBLOCK_ID_PDOUT = 0x1002,                    // Used by SMI_PDOut (see 11.2.18)
    IOL_ARGBLOCK_ID_PDINOUT = 0x1003,                 // Used by SMI_PDInOut (see 11.2.19)
    IOL_ARGBLOCK_ID_SPDUIN = 0x1101,                   // Used by ...
    IOL_ARGBLOCK_ID_SPDUOUT = 0x1102,                  // Used by ...
    IOL_ARGBLOCK_ID_PDINIQ = 0x1FFE,                  // Used by SMI_PDInIQ (see 11.2.20)
    IOL_ARGBLOCK_ID_PDOUTIQ = 0x1FFF,                 // Used by SMI_PDOutIQ (see 11.2.21)
    IOL_ARGBLOCK_ID_ONREQUESTDATA_W = 0x3000,           // Used by SMI_DeviceWrite (see 11.2.10)
    IOL_ARGBLOCK_ID_ONREQUESTDATA_R = 0x3001,           // Used by SMI_DeviceRead (see 11.2.11)
    IOL_ARGBLOCK_ID_DS_DATA = 0x7000,                   // Used by SMI_DSToParServ (see 11.2.8)
    IOL_ARGBLOCK_ID_DEVICEPARBATCH = 0x7001,          // Used by SMI_ParamWriteBatch (see 11.2.12)
    IOL_ARGBLOCK_ID_INDEXLIST = 0x7002,                // Used by SMI_ParamReadBatch (see 11.2.13)
    IOL_ARGBLOCK_ID_PORTPOWEROFFON = 0x7003,         // Used by SMI_PortPowerOffOn (see 11.2.14)
    IOL_ARGBLOCK_ID_PORTCONFIGLIST = 0x8000,          // Used by SMI_PortConfiguration (see 11.2.5)
    IOL_ARGBLOCK_ID_FS_PORTCONFIGLIST = 0x8100,       // Used by ...
    IOL_ARGBLOCK_ID_W_TRACKCONFIGLIST = 0x8200,        // Used by ...
    IOL_ARGBLOCK_ID_PORTSTATUSLIST = 0x9000,          // Used by SMI_PortStatus (see 11.2.7)
    IOL_ARGBLOCK_ID_FS_PORTSTATUSLIST = 0x9100,       // Used by ...
    IOL_ARGBLOCK_ID_W_TRACKSTATUSLIST = 0x9200,        // Used by ...
    IOL_ARGBLOCK_ID_W_TRACKSCANRESULT = 0x9201,        // Used by ...
    IOL_ARGBLOCK_ID_DEVICEEVENT = 0xA000,              // Used by SMI_DeviceEvent (see 11.2.15)
    IOL_ARGBLOCK_ID_PORTEVENT = 0xA001,                // Used by SMI_PortEvent (see 11.2.16)
    IOL_ARGBLOCK_ID_VOIDBLOCK = 0xFFF0,                // Used by SMI service management
    IOL_ARGBLOCK_ID_JOBERROR = 0xFFFF                  // Used by SMI service management
} IOL_ArgBlockID;


/*PortConfigList Argument Block-----*/

/* Define an enumeration for PortMode -Ref: Table E.3
 * used in smi and master application
 */
typedef enum IOL_SMI_PortMode {
    IOL_PORT_MODE_DEACTIVATED = 0, // 0: DEACTIVATED
    IOL_PORT_MODE_IOL_MANUAL,      // 1: IOL_MANUAL
    IOL_PORT_MODE_IOL_AUTOSTART,   // 2: IOL_AUTOSTART
    IOL_PORT_MODE_DI_CQ,           // 3: DI_C/Q (Pin 4 at M12)
    IOL_PORT_MODE_DO_CQ            // 4: DO_C/Q (Pin 4 at M12)
} IOL_SMI_PortMode;


// Define an enumeration for Validation&Backup-Ref: Table E.3
typedef enum IOL_SMI_ValidBackup{
    IOL_VALIDBACKUP_NO_DEVICE_CHECK = 0,               	// 0: No Device check
    IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_0,               	// 1: Type compatible Device V1.0
    IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1,               	// 2: Type compatible Device V1.1
    IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_BACKUPRSTOR, 	// 3: Type compatible Device V1.1, Backup + Restore
    IOL_VALIDBACKUP_TYPECOMPA_DEV_V1_1_RSTOR,       	 // 4: Type compatible Device V1.1, Restore
} IOL_SMI_ValidBackup;

/* This element defines the behavior of the I/Q
 * signal (Pin 2 at M12 connector)
 * */
typedef enum IOL_SMI_IQBehavior{
    IOL_IQBEHAVIOR_NOT_SUPPORTED = 0,
    IOL_IQBEHAVIOR_DI = 1,
    IOL_IQBEHAVIOR_DO = 2,
	/*3 to 4 reserved*/
    IOL_IQBEHAVIOR_POWER2 = 5,
	/*6 to 255 reserved*/
} IOL_SMI_IQBehavior;

/* PortStatusInfo, Table E.4 – PortStatusList
 * This element contains status information of the Port.
 * */
typedef enum IOL_PortStatusInfo{
	IOL_PORT_STATUS_INFO_NO_DEVICE = 0,		// No communication (COMLOST). However, Port configuration IOL_MANUAL or IOL_AUTOSTART was set (see Table E.3)
	IOL_PORT_STATUS_INFO_DEACTIVATED,		// Port configuration DEACTIVATED was set (see Table E.3).
	IOL_PORT_STATUS_INFO_PORT_DIAG,			// This value to be set If a DiagEntry indicates an upcoming diagnosis of the Port during startup, validation, and Data Storage (group error). Device is in PREOPERATE and DiagEntry contains the diagnosis cause. .
	/*According to CR_216, it is not used now*/
//	IOL_PORT_STATUS_INFO_PREOPERATE,		//  This is only an intermediate state leading to OPERATE or PORT_DIAG
	IOL_PORT_STATUS_INFO_OPERATE,			// This value to be set if the Device is in OPERATE, even in case of Device error.
	IOL_PORT_STATUS_INFO_DI_CQ,			// Port configuration "DI" was set (see Table E.3).
	IOL_PORT_STATUS_INFO_DO_CQ,			// Port configuration "DO" was set (see Table E.3).
	/*Reserved for IO-Link Safety [10]*/
	IOL_PORT_STATUS_INFO_POWER_OFF = 254,	// SMI_PortPowerOffOn (see 11.2.14) caused the communication system to stop. All indications become obsolete according to the state machine in Figure 101 and shall be reported according to state transition Table 126.
	IOL_PORT_STATUS_INFO_NOT_AVAILLABLE,	// PortStatusInfo currently not available
} IOL_PortStatusInfo;

/*Transmission rate*/
typedef enum IOL_TransRate {
	IOL_TRANS_RATE_NOT_DETECTED = 0,
	IOL_TRANS_RATE_COM1 = 1,
	IOL_TRANS_RATE_COM2 = 2,
	IOL_TRANS_RATE_COM3 = 3,
} IOL_TransRate;


/* PortPowerOffOn Argument Block----*/
/*Ref: E.9 PortPowerOffOn*/
/*Octet offset 2, PortPowerMode */
typedef enum IOL_ATTRIB_PACKED IOL_SMI_ArgBlock_portPowerMode{
	IOL_PORTPWRMODE_ONETIME_OFF = 0,		// 0: One time switch off (PowerOffTime)
	IOL_PORTPWRMODE_PERMANENT_OFF,			// 1: Switch PortPowerOff (permanent)
	IOL_PORTPWRMODE_PERMANENT_ON,			// 2: Switch PortPowerOn (permanent)
}IOL_SMI_ArgBlock_portPowerMode;


/*Annex: A -----------------------*/

/*IOLink communication channel
 * Ref: IO-Link Interface Spec v1.1.3, section A.1.2, Table A.1 – Values of communication channel
 * */
typedef enum IOL_CommChannel
{
	IOL_COMM_CH_PROCESS = 0U, // Process
	IOL_COMM_CH_PAGE,		  // Page
	IOL_COMM_CH_DIAGNOSIS,	  // Diagnosis
	IOL_COMM_CH_ISDU,		  // ISDU
} IOL_CommChannel;

/*IOLink read write access
 * Ref: IO-Link Interface Spec v1.1.3, section A.1.2, Table A.2 – Values of R/W
 * */
typedef enum IOL_RWDirection
{
	IOL_RW_DIR_WRITE = 0U, // Write access
	IOL_RW_DIR_READ,	   // Read access
} IOL_RWDirection;

/* IOLink M-sequence type
 * Ref: IO-Link Interface Spec v1.1.3, section A.1.3, Table A.3 – Values of M-sequence types
 * */
// typedef enum IOL_MSequenceTypes{
//	IOL_MSEQTYP_TYPE0 = 0,
//	IOL_MSEQTYP_TYPE1,
//	IOL_MSEQTYP_TYPE2, // type 2 have some Sub-types depend on PD configuration and PD direction.
//	IOL_MSEQTYP_TYPE3, // type 3, resereved
// } IOL_MSequenceTypes;

/* IOLink PD status
 * Ref: IO-Link Interface Spec v1.1.3, section A.1.5, Table A.5 – Values of PD status
 * */
typedef enum IOL_PDINStatus
{
	IOL_PDIN_STATUS_NONE = 0,
	IOL_PDIN_STATUS_VALID,
	IOL_PDIN_STATUS_INVALID,
} IOL_PDINStatus;

/* IOLink Event flag
 * Ref: IO-Link Interface Spec v1.1.3, section A.1.5, Table A.6 – Values of Event flag
 * */
typedef enum IOL_EHEventFlag
{
	IOL_EH_NO_EVENT = 0,
	IOL_EH_EVENT,
} IOL_EHEventFlag;

/* A.1.3 M-sequence type (CKT)*/
typedef enum IOL_MseqType
{
	IOL_MSEQ_TYPE_0 = 0,
	IOL_MSEQ_TYPE_1,
	IOL_MSEQ_TYPE_2,
	IOL_MSEQ_TYPE_3,		// reserved
}IOL_MseqType;

/* M-sequence sub types
 * Ref: IO-Link Interface Spec v1.1.3, section A.2,
 *
 * */
typedef enum IOL_MseqSubType
{
	IOL_MSEQ_SUB_TYPE_NONE = 0x00,
	IOL_MSEQ_SUB_TYPE_0 = 0x01,
	IOL_MSEQ_SUB_TYPE_1_1 = 0x11,
	IOL_MSEQ_SUB_TYPE_1_2 = 0x12,
	IOL_MSEQ_SUB_TYPE_1_V = 0x1E,
	IOL_MSEQ_SUB_TYPE_1_X = 0x1F,
	IOL_MSEQ_SUB_TYPE_2_1 = 0x21,
	IOL_MSEQ_SUB_TYPE_2_2 = 0x22,
	IOL_MSEQ_SUB_TYPE_2_3 = 0x23,
	IOL_MSEQ_SUB_TYPE_2_4 = 0x24,
	IOL_MSEQ_SUB_TYPE_2_5 = 0x25,
	IOL_MSEQ_SUB_TYPE_2_V = 0x2E,
	IOL_MSEQ_SUB_TYPE_2_X = 0x2F,
} IOL_MseqSubType;

/* M-sequence codes
 * Ref: IO-Link Interface Spec v1.1.3, section A.2.6,
 * */
typedef enum IOL_MseqCode{
	IOL_MSEQCODE_TYPE_0 = 0,	   // this code is used for startup and preoperate mode
	IOL_MSEQCODE_PREOP_TYPE_1_2,   // only for preoperate mode
	IOL_MSEQCODE_PREOP_TYPE_1_V_8, // only for preoperate mode for 8 octets
	IOL_MSEQCODE_PREOP_TYPE_1_V_32, // only for preoperate mode for 32 octets
} IOL_MseqCode;

typedef enum IOL_DL_INTERLEAVE{
   IOL_DL_INTERLEAVE_NONE = 0,
   IOL_DL_INTERLEAVE_PD,
   IOL_DL_INTERLEAVE_OD,
} IOL_DL_INTERLEAVE;;





/*Annex: B -----------------------*/
/*
 * Direct Parameter page 1 and 2 address,
 * Ref: IO-Link Interface Spec v1.1.3, section B.1.1
 * Page1:
 * 1. Communication control - 0x00 to 0x06
 * 2. Identification -  0x07 to 0x0E
 * 3. Application control - 0x0F
 *
 * */
typedef enum IOL_DirParamAddress{
	IOL_DIR_PARM_ADDR_MASTER_CMD = 0x00, // Master command to switch to operating states
	IOL_DIR_PARM_ADDR_MASTER_CYCL_TIM,	 // Actual cycle duration used by the Master to address the Device. Can be used as a parameter to monitor Process Data transfer.
	IOL_DIR_PARM_ADDR_MIN_CYCL_TIM,		 // Minimum cycle duration supported by a Device. This is a performance feature of the Device and depends on its technology and implementation.
	IOL_DIR_PARM_ADDR_M_SEQ_CAPBILITY,	 // Information about implemented options related to M-sequences and physical configuration
	IOL_DIR_PARM_ADDR_REV_ID,			 // ID of the used protocol version for implementation (shall be set to 0x11)
	IOL_DIR_PARM_ADDR_PDI,				 // Type and length of input data (Process Data from Device to Master)
	IOL_DIR_PARM_ADDR_PDO,				 // Type and length of output data (Process Data from Master to Device)
	IOL_DIR_PARM_ADDR_VEND_ID_MSB,		 // MSB, Unique vendor identification
	IOL_DIR_PARM_ADDR_VEND_ID_LSB,		 // LSB, Unique vendor identification
	IOL_DIR_PARM_ADDR_DEV_ID_MSB,		 // MSB, Unique Device identification allocated by a vendor
	IOL_DIR_PARM_ADDR_DEV_ID_MLSB,		 // MLSB, Unique Device identification allocated by a vendor
	IOL_DIR_PARM_ADDR_DEV_ID_LSB,		 // LSB, Unique Device identification allocated by a vendor
	IOL_DIR_PARM_ADDR_FUNC_ID_MSB,		 // Reserved
	IOL_DIR_PARM_ADDR_FUNC_ID_LSB,		 // Reserved
	/*0x0# Reserved*/					 // Reserved
	IOL_DIR_PARM_ADDR_SYS_CMD = 0x0F,	 // Command interface for end user applications only and Devices without ISDU support (see NOTE)
	/*0x10 to 0x1F Vendor specific*/	 // Device specific parameters
} IOL_DirParamAddress;

/* List of Master Commands,
 * Ref: IO-Link Interface Spec v1.1.3, section B.1.2
 * */
typedef enum IOL_MasterCommand
{
	/*0x00 to 0x59 Reserved*/
	IOL_MSTR_CMD_FALLBACK = 0x5A, // Transition from communication to SIO mode. The Device shall execute this transition after 3 Master-CycleTimes and before 500 ms elapsed after the MasterCommand.
	/*0x5B to 0x94 Reserved*/
	IOL_MSTR_CMD_MSTR_IDENT = 0x95, // Indicates a Master revision higher than 1.0
	IOL_MSTR_CMD_DEV_IDENT,			// Start check of Direct Parameter page for changed entries
	IOL_MSTR_CMD_DEV_STARTUP,		// Switches the Device from OPERATE or PREOPERATE to STARTUP
	IOL_MSTR_CMD_PDO_OPERATE,		// Process output data valid
	IOL_MSTR_CMD_DEV_OPERATE,		// Process output data invalid or not available. Switches the Device from STARTUP or PREOPERATE to OPERATE
	IOL_MSTR_CMD_DEV_PREOPERATE,	// Switches the Device from STARTUP to state PREOPERATE
									/*0x9B to 0xFF Reserved*/
} IOL_MasterCommand;

/* Time base is used to calculate the minimum cycle time for the master cycle time
 *  Ref: IO-Link Interface Spec v1.1.3, section B.1.3
 *
 * */
// const float IOL_TimeBase[4] = {0.1f, 0.4f, 1.6f, 0.0f};		//4th position is reserved

/* Predefined Device parameters
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.1
 * */
typedef enum IOL_DeviceParamIndex
{
	IOL_DEV_PARM_INDX_DIR_PARM_PAGE_1 	= 0x0000,	// Redirected to the page communication channel,
	IOL_DEV_PARM_INDX_DIR_PARM_PAGE_2 	= 0x0001,	// Redirected to the page communication channel,
	IOL_DEV_PARM_INDX_SYS_CMD 			= 0x0002,	// Command Code Definition
	IOL_DEV_PARM_INDX_DATA_STORAGE 		= 0x0003,	// Set of data objects for storage
	/*0x0004 to 0x000B Reserved*/
	IOL_DEV_PARM_INDX_DEV_ACC_LOCK 		= 0x000C,  	// Standardized Device locking functions
	IOL_DEV_PARM_INDX_PROFIL_CHARISTIC	= 0x000D, 	// Reserved for Common Profile
	IOL_DEV_PARM_INDX_PDI_DESCRIPTION 	= 0x000E,	// Reserved for Common Profile
	IOL_DEV_PARM_INDX_PDO_DESCRIPTION 	= 0x000F,	// Reserved for Common Profile
	IOL_DEV_PARM_INDX_VENDOR_NAME 		= 0x0010,	// Vendor information
	IOL_DEV_PARM_INDX_VENDOR_TEXT 		= 0x0011,	// Additional vendor information
	IOL_DEV_PARM_INDX_PRODUCT_NAME 		= 0x0012,	// Detailed product or type name
	IOL_DEV_PARM_INDX_PRODUCT_ID 		= 0x0013,	// Product or type identification
	IOL_DEV_PARM_INDX_PRODUCT_TEXT 		= 0x0014,	// Description of Device function or characteristic
	IOL_DEV_PARM_INDX_SERIAL_NUMBER 	= 0x0015,	// Vendor specific serial number
	IOL_DEV_PARM_INDX_HARDWARE_REV 		= 0x0016,	// Vendor specific format
	IOL_DEV_PARM_INDX_FIRMWARE_REV 		= 0x0017,	// Vendor specific format
	IOL_DEV_PARM_INDX_APP_SPEC_TAG 		= 0x0018,	// Tag defined by user
	IOL_DEV_PARM_INDX_FUNCTION_TAG 		= 0x0019,	// Reserved for Common Profile
	IOL_DEV_PARM_INDX_LOCATION_TAG 		= 0x001A,	// Reserved for Common Profile
	/*0x001B - 0x001F Reserved*/
	IOL_DEV_PARM_INDX_ERROR_COUNT 		= 0x0020, 	// Errors since power-on or reset
	/*0x0021 - 0x0023 Reserved*/
	IOL_DEV_PARM_INDX_DEV_STATUS 		= 0x0024, 	// Contains current status of the Device
	IOL_DEV_PARM_INDX_DETILD_DEVSTATUS  = 0x0025, 	//
	/*0x0026 - 0x0027 Reserved*/
	IOL_DEV_PARM_INDX_PDI				= 0x0028, 	// Read last valid Process Data from PDin channel (
	IOL_DEV_PARM_INDX_PDO 				= 0x0029,	// Read last valid Process Data from PDout channel
	/*0x002 - 0x002F Reserved*/
	IOL_DEV_PARM_INDX_OFFSET_TIME 		= 0x0030, 	// Synchronization of Device application timing to M-sequence timing
	/*0x0031 - 0xFFFF*/
}IOL_DeviceParamIndex;

/* System Command for ISDU
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.2
 * */
typedef enum IOL_DeviceSystemCommand
{
	IOL_DEV_SYSCMD_PARM_UPLOAD_START = 0x00,
	IOL_DEV_SYSCMD_PARM_UPLOAD_END,
	IOL_DEV_SYSCMD_PARM_DOWNLOAD_START,
	IOL_DEV_SYSCMD_PARM_DOWNLOAD_END,
	IOL_DEV_SYSCMD_PARM_DOWNLOAD_STORE,
	IOL_DEV_SYSCMD_PARM_BREAK,
	/*0x07 to 0x7F Reserved*/
	IOL_DEV_SYSCMD_DEVICE_RESET = 0x80,
	IOL_DEV_SYSCMD_APP_RESET,
	IOL_DEV_SYSCMD_RESTORE_FACTORY_SETTINGS,
	IOL_DEV_SYSCMD_BACK_TO_BOX,
	/*0x84 to 0x9F Reserved*/
	/*0xA0 to 0xFF Vendor specific*/
} IOL_DeviceSystemCommand;

/* Data Storage Index for ISDU
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.3
 * */
typedef enum IOL_DeviceDataStorageIndex{
	IOL_DEV_DS_SUBINDX_DS_CMD = 0x01,
	IOL_DEV_DS_SUBINDX_STATE_PROPERTY,
	IOL_DEV_DS_SUBINDX_DS_SIZE,
	IOL_DEV_DS_SUBINDX_PARM_CHECKSUM,
	IOL_DEV_DS_SUBINDX_INDEX_LIST,
} IOL_DeviceDataStorageIndex;

/* DS commands in Data Storage Index (ox0003)
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.3
 * */
typedef enum IOL_DSCommand{
	IOL_DS_CMD_NONE		= 0,
	IOL_DS_CMD_UPLOAD_START,
	IOL_DS_CMD_UPLOAD_END,
	IOL_DS_CMD_DOWNLOAD_START,
	IOL_DS_CMD_DOWNLOAD_END,
	IOL_DS_CMD_BREAK,
	/*0x06 to 0xFF: Reserved*/
}IOL_DSCommand;

/* State of Data Storage in Data Storage Index (ox0003)
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.3
 * */
typedef enum IOL_DSStorageState{
	IOL_DS_STORG_STATE_INACTIVE	= 0,
	IOL_DS_STORG_STATE_UPLOAD,
	IOL_DS_STORG_STATE_DOWUNLOAD,
	IOL_DS_STORG_STATE_LOCKED,

}IOL_DSStorageState;

/* Upload Request in Data Storage Index (ox0003)
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.3
 * */
typedef enum IOL_DSUploadReq{
	IOL_DS_UPLOAD_REQ_NONE	= 0,	// no upload request
	IOL_DS_UPLOAD_REQ_PENDING,		// upload request pending
}IOL_DSUploadReq;

typedef enum IOL_MseqCapability{
	IOL_MSEQCAPB_ISDU_NOT_SUPPORTED = 0,
	IOL_MSEQCAPB_ISDU_SUPPORTED,
}IOL_MseqCapability;

/* Device Status
 * Ref: IO-Link Interface Spec v1.1.3, section B.2.20 Table B.13
 * */
typedef enum IOL_DeviceStatus
{
	IOL_DEV_STATUS_OPERAT_PROPERLY = 0U, // Device is operating properly
	IOL_DEV_STATUS_MAINT_REQRD,				// Maintenance Required
	IOL_DEV_STATUS_OUT_OF_SPEC,				// Out of Specification
	IOL_DEV_STATUS_FUNC_CHECK,				// Functional Check
	IOL_DEV_STATUS_FAILURE,					// Failure
} IOL_DeviceStatus;

/*Annex: C -----------------------*/
/*ErrorTypes----------------------*/
/*
 * Device Application related ErrorTypes
 * Ref: IO-Link Interface Spec v1.1.3, section C.2.1,
 * */
typedef enum IOL_ErrorTypes{
	/*Table C.1 – ISDU(Device) ErrorTypes*/
	IOL_ERRTYP_NONE							= 0x0000,	// no error
	IOL_ERRTYPISDU_APP_DEV 					= 0x8000,	// Device application error - no details
	IOL_ERRTYPISDU_IDX_NOTAVAIL 			= 0x8011,	// Index not available
	IOL_ERRTYPISDU_SUBIDX_NOTAVAIL			= 0x8012,	// Subindex not available
	IOL_ERRTYPISDU_SERV_NOTAVAIL 			= 0x8020,	// Service temporarily not available
	IOL_ERRTYPISDU_SERV_NOTAVAIL_LOCCTRL	= 0x8021,	// Service temporarily not available – local control
	IOL_ERRTYPISDU_SERV_NOTAVAIL_DEVCTRL	= 0x8022,	// Service temporarily not available – Device control
	IOL_ERRTYPISDU_IDX_NOT_ACCESSIBLE		= 0x8023,	// Access denied
	IOL_ERRTYPISDU_PAR_VALOUTOFRNG 			= 0x8030, 	// Parameter value out of range
	IOL_ERRTYPISDU_PAR_VALGTLIM 			= 0x8031,	// Parameter value above limit
	IOL_ERRTYPISDU_PAR_VALLTLIM 			= 0x8032,	// Parameter value below limit
	IOL_ERRTYPISDU_VAL_LENOVRRUN 			= 0x8033,	// Parameter length overrun
	IOL_ERRTYPISDU_VAL_LENUNDRUN 			= 0x8034,	// Parameter length underrun
	IOL_ERRTYPISDU_FUNC_NOTAVAIL 			= 0x8035,	// Function not available
	IOL_ERRTYPISDU_FUNC_UNAVAILTEMP 		= 0x8036,	// Function temporarily unavailable
	IOL_ERRTYPISDU_PAR_SETINVALID 			= 0x8040,	// Invalid parameter set
	IOL_ERRTYPISDU_PAR_SETINCONSIST			= 0x8041,	// Inconsistent parameter set
	IOL_ERRTYPISDU_APP_DEVNOTRDY 			= 0x8082,	// Application not ready
	/* Vendor specific (range 0x8100 to 0x81FF)*/
	/*Table C.2 – Derived(Master) ErrorTypes*/
	IOL_ERRTYPDERIEVD_COMM_ERR 				= 0x1000,	// Master – Communication error
	IOL_ERRTYPDERIEVD_I_SERVICE_TIMEOUT 	= 0x1100, 	// Master – ISDU timeout
	IOL_ERRTYPDERIEVD_M_ISDU_CHECKSUM 		= 0x5600,   // Master – ISDU checksum error
	IOL_ERRTYPDERIEVD_M_ISDU_ILLEGAL 		= 0x5700,	// Master – ISDU illegal service primitive

	/*Table C.3 – SMI related ErrorTypes*/
	IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED 	= 0x4001,	// ArgBlock unknown
	IOL_ERRTYPSMI_ARGBLOCK_INCONSISTENT 	= 0x4002,	// Incorrect ArgBlock content type
	IOL_ERRTYPSMI_DEVICE_NOT_ACCESSIBLE 	= 0x4003,	// Device not communicating
	IOL_ERRTYPSMI_SERVICE_NOT_SUPPORTED 	= 0x4004,	// Service unknown
	IOL_ERRTYPSMI_DEVICE_NOT_IN_OPERATE 	= 0x4005,	// Process Data not accessible
	IOL_ERRTYPSMI_MEMORY_OVERRUN 			= 0x4006,	// Insufficient memory
	IOL_ERRTYPSMI_PORT_NUM_INVALID 			= 0x4011,	// Incorrect Port number
	IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID 	= 0x4034,  	// Incorrect ArgBlock length
	IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE 	= 0x4036, 	// Master busy
	IOL_ERRTYPSMI_PROPAGATED_ERROR 			= 0xEEAA,	// Device / Master error (Propagated error)
} IOL_ErrorTypes;

///*
// * Derived ErrorTypes generated in the Master AL and are caused by internal incidents or 5662 those received from the Device.
// * Ref: IO-Link Interface Spec v1.1.3, section C.3.1, Table C.2 – Derived ErrorTypes
// * */
//typedef enum IOL_ErrorTypesDerived{
//	IOL_ERRTYPDERIEVD_COMM_ERR = 0x1000,		   // Master – Communication error
//	IOL_ERRTYPDERIEVD_I_SERVICE_TIMEOUT = 0x1100, // Master – ISDU timeout
//	IOL_ERRTYPDERIEVD_M_ISDU_CHECKSUM = 0x5600,   // Master – ISDU checksum error
//	IOL_ERRTYPDERIEVD_M_ISDU_ILLEGAL = 0x5700,	   // Master – ISDU illegal service primitive
//} IOL_ErrorTypesDerived;
//
///*
// * The Master returns SMI related ErrorTypes within a negative response (Result (-) while 5692 performing an SMI service
// * Ref: IO-Link Interface Spec v1.1.3, section C.4.1, Table C.C.3 – SMI related ErrorTypes
// * */
//typedef enum IOL_ErrorTypeSMI{
//	IOL_ERRTYPSMI_NONE 						= 0x0000,	// No error
//	IOL_ERRTYPSMI_ARGBLOCK_NOT_SUPPORTED 	= 0x4001,	// ArgBlock unknown
//	IOL_ERRTYPSMI_ARGBLOCK_INCONSISTENT 	= 0x4002,	// Incorrect ArgBlock content type
//	IOL_ERRTYPSMI_DEVICE_NOT_ACCESSIBLE 	= 0x4003,	// Device not communicating
//	IOL_ERRTYPSMI_SERVICE_NOT_SUPPORTED 	= 0x4004,	// Service unknown
//	IOL_ERRTYPSMI_DEVICE_NOT_IN_OPERATE 	= 0x4005,	// Process Data not accessible
//	IOL_ERRTYPSMI_MEMORY_OVERRUN 			= 0x4006,	// Insufficient memory
//	IOL_ERRTYPSMI_PORT_NUM_INVALID 			= 0x4011,	// Incorrect Port number
//	IOL_ERRTYPSMI_ARGBLOCK_LENGTH_INVALID 	= 0x4034,  	// Incorrect ArgBlock length
//	IOL_ERRTYPSMI_SERVICE_TEMP_UNAVAILABLE 	= 0x4036, 	// Master busy
//	IOL_ERRTYPSMI_PROPAGATED_ERROR 			= 0xEEAA,	// Device / Master error (Propagated error)
//	/* Vendor specific (range 0x4080 to 0x40FF)*/
//} IOL_ErrorTypeSMI;


/*Annex: D -----------------------*/
/*EventCodes (diagnosis information)----------------*/
/* EventCodes for Devices
 * Ref: IO-Link Interface Spec v1.1.3, section D.2, Table D.1 – EventCodes for Devices
 * */
typedef enum IOL_DeviceEventCodes{
	IOL_EVTCDE_DEV_NO_MALFUNC 				= 0x0000,	// No malfunction - Notification
	IOL_EVTCDE_DEV_GENRL_MALFUNC 			= 0x1000, 	// General malfunction – unknown error - Error
	IOL_EVTCDE_NO_DEV 						= 0x1800,	// No device founded
	IOL_EVTCDE_DEV_TEMP_FAULT 				= 0x4000,	// Temperature fault – Overload - Error
	IOL_EVTCDE_DEV_TEMP_OVRUN  				= 0x4210,	// Device temperature overrun – Clear source of heat - Warning
	IOL_EVTCDE_DEV_TEMP_UNDRUN  			= 0x4220,	// Device temperature underrun – Insulate Device - Warning
	IOL_EVTCDE_DEV_DEV_HW_FAULT  			= 0x5000,	// Device hardware fault – Device exchange - Error
	IOL_EVTCDE_DEV_COMPO_MALFUNC  			= 0x5010,	// Component malfunction – Repair or exchange - Error
	IOL_EVTCDE_DEV_NON_VOLA_MEMLOSS  		= 0x5011,	// Non-volatile memory loss – Check batteries - Error
	IOL_EVTCDE_DEV_BATTERIES_LOW  			= 0x5012,	// Batteries low – Exchange batteries - Warning
	IOL_EVTCDE_DEV_GENRL_PWR_SPLY_FAULT  	= 0x5100,	// General power supply fault – Check availability - Error
	IOL_EVTCDE_DEV_FUSE_BLOWN_OPEN 			= 0x5101,	// Fuse blown/open – Exchange fuse - Error
	IOL_EVTCDE_DEV_PRIMARY_SPLYVOLT_OVRUN 	= 0x5110,	// Primary supply voltage overrun – Check tolerance - Warning
	IOL_EVTCDE_DEV_PRIMARY_SPLYVOLT_UNDRUN  = 0x5111,	// Primary supply voltage underrun – Check tolerance - Warning
	IOL_EVTCDE_DEV_SECONDARY_SPLYVOLT_FAULT = 0x5112,	// Secondary supply voltage fault (Port Class B) – Check tolerance - Warning
	IOL_EVTCDE_DEV_DEVE_SW_FAULT 			= 0x6000,	// Device software fault – Check firmware revision - Error
	IOL_EVTCDE_DEV_PARAM_ERROR 				= 0x6320,	// Parameter error – Check data sheet and values - Error
	IOL_EVTCDE_DEV_PARAM_MISSING 			= 0x6321,	// Parameter missing – Check data sheet - Error
	IOL_EVTCDE_DEV_WIRE_BREAK  				= 0x7700,	// Wire break of a subordinate device – Check installation - Error
	IOL_EVTCDE_DEV_WIRE_BREAK_SUB1  		= 0x7701,	// Wire break of subordinate device 1 …device 15 – Check installation - Error
	IOL_EVTCDE_DEV_SHORT_CIRCUIT  			= 0x7710,	// Short circuit – Check installation - Error
	IOL_EVTCDE_DEV_GROUND_FAULT  			= 0x7711,	// Ground fault – Check installation - Error
	IOL_EVTCDE_DEV_TECH_SPEC_APP_FAULT 		= 0x8C00,	// Technology specific application fault – Reset Device - Error
	IOL_EVTCDE_DEV_SIMULATION_ACTIVE 		= 0x8C01,	// Simulation active – Check operational mode - Warning
	IOL_EVTCDE_DEV_PROCESS_VAR_RNG_OVRUN 	= 0x8C10,	// Process variable range overrun – Process Data uncertain - Warning
	IOL_EVTCDE_DEV_MEASUR_RNG_EXCED 		= 0x8C20,	// Measurement range exceeded – Check application - Error
	IOL_EVTCDE_DEV_PROCESS_VAR_RNG_UNDRUN 	= 0x8C30,	// Process variable range underrun – Process Data uncertain - Warning
	IOL_EVTCDE_DEV_MAINT_CLEANING 			= 0x8C40,	// Maintenance required – Cleaning - Warning
	IOL_EVTCDE_DEV_MAINT_REFILL  			= 0x8C41,	// Maintenance required – Refill - Warning
	IOL_EVTCDE_DEV_MAINT_WEAR_TEAR  		= 0x8C42,	// Maintenance required – Exchange wear and tear parts - Warning
	IOL_EVTCDE_DEV_DS_UPLOAD_REQ		 	= 0xFF91,	// Data Storage upload request ("DS_UPLOAD_REQ") – internal, not visible to the user - Notification (single shot)
} IOL_DeviceEventCodes;

/* EventCodes for Ports
 * Ref: IO-Link Interface Spec v1.1.3, section D.3, Table D.2 – EventCodes for Ports
 * */
typedef enum IOL_PortEventCodes
{
	IOL_EVTCDE_PORT_NONE	 			= 0x0000,	// no port error
	IOL_EVTCDE_PORT_NO_DEV_COM 			= 0x1800,	// No Device (communication) Trigger: SMI_PortEvent (0x1800) by SM_PortMode (COMLOST) - Error
	IOL_EVTCDE_PORT_STARTUP_PARM_ERR 	= 0x1801,	// Startup parametrization error – check parameter - Error
	IOL_EVTCDE_PORT_INCORRECT_VENID 	= 0x1802,	// Incorrect VendorID – Inspection Level mismatch Trigger: SM_PortMode (COMP_FAULT) - Error
	IOL_EVTCDE_PORT_INCORRECT_DEVID 	= 0x1803,	// Incorrect DeviceID – Inspection Level mismatch Trigger: SM_PortMode (COMP_FAULT) - Error
	IOL_EVTCDE_PORT_SHORT_CKT_CQ 		= 0x1804,	// Short circuit at C/Q – check wire connection - Error
	IOL_EVTCDE_PORT_PHY_OVRTEMP 		= 0x1805,	// PHY overtemperature – check Master temperature and load - Error
	IOL_EVTCDE_PORT_SHORT_CKT_LPLUS 	= 0x1806,	// Short circuit at L+ – check wire connection - Error
	IOL_EVTCDE_PORT_OVRCURRENT_LPLUS 	= 0x1807,	// Overcurrent at L+ – check power supply (e.g. L1+) - Error
	IOL_EVTCDE_PORT_DEV_EVNT_OVRFLOW 	= 0x1808,	// Device Event overflow - Error
	IOL_EVTCDE_PORT_MEM_OUTOFRANGE 		= 0x1809,	// Backup inconsistency – memory out of range (2048 octets) Trigger: SMI_PortEvent (0x1809) by DS_Fault (SizeCheck_Fault) - Error
	IOL_EVTCDE_PORT_IDENTITY_FAULT 		= 0x180A,	// Backup inconsistency – identity fault Trigger: SMI_PortEvent (0x180A) by DS_Fault (Identification_Fault) - Error
	IOL_EVTCDE_PORT_DATASTORG_UNSPECERR = 0x180B,	// Backup inconsistency – Data Storage unspecific error Trigger: SMI_PortEvent (0x180B) by DS_Fault (All other incidents) - Error
	IOL_EVTCDE_PORT_UPLOAD_FAULT 		= 0x180C,	// Backup inconsistency – upload fault - Error
	IOL_EVTCDE_PORT_DOWNLOAD_FAULT 		= 0x180D,	// Parameter inconsistency – download fault - Error
	IOL_EVTCDE_PORT_P24_MIS_OR_UNDRVOLT = 0x180E, 	// P24 (Class B) missing or undervoltage - Error
	IOL_EVTCDE_PORT_SHORT_CKT_P24 		= 0x180F,	// Short circuit at P24 (Class B) – check wire connection (e.g. L2+) - Error
	IOL_EVTCDE_PORT_SHORT_CKT_IQ 		= 0x1810,	// Short circuit at I/Q – check wiring - Error
	IOL_EVTCDE_PORT_SHORT_CKT_CQ_DIGI	= 0x1811,	// Short circuit at C/Q (if digital output) – check wiring - Error
	IOL_EVTCDE_PORT_OVRCURRENT_IQ 		= 0x1812,	// Overcurrent at I/Q – check load - Error
	IOL_EVTCDE_PORT_OVRCURRENT_CQ_DIGI 	= 0x1813,	// Overcurrent at C/Q (if digital output) – check load - Error
	IOL_EVTCDE_PORT_INVALID_CYCLE_TIME 	= 0x6000,	// Invalid cycle time Trigger: SM_PortMode (CYCTIME_FAULT) - Error
	IOL_EVTCDE_PORT_REVISION_FAULT 		= 0x6001,	// Revision fault – incompatible protocol version Trigger: SM_PortMode (REVISION_FAULT) - Error
	IOL_EVTCDE_PORT_ISDU_BATCH_FAILED 	= 0x6002,	// ISDU batch failed – parameter inconsistency? - Error
	IOL_EVTCDE_PORT_DEV_PLUGGED_IN		= 0xFF21,	// DL: Device plugged in ("NEW_SLAVE") – PD stop Trigger: SM_PortMode (COMREADY); see Figure 71 (T10) - Notification
	IOL_EVTCDE_PORT_DEV_COM_LOST 		= 0xFF22,	// Device communication lost ("DEV_COM_LOST") Trigger: see Figure 101 (T9) - Notification
	IOL_EVTCDE_PORT_DS_IDENT_MISMATCH 	= 0xFF23,	// Data Storage identification mismatch ("DS_IDENT_MISMATCH") Trigger: see Figure 104 (T15) - Notification
	IOL_EVTCDE_PORT_DS_BUFFER_OVRFLOW 	= 0xFF24,	// Data Storage buffer overflow ("DS_BUFFER_OVERFLOW") Trigger: see Figure 104 (T17) - Notification
	IOL_EVTCDE_PORT_DS_ACCESS_DENIED 	= 0xFF25,	// Data Storage parameter access denied ("DS_ACCESS_DENIED") Trigger: see Figure 104 (T29), Figure 105 (T32), Figure 107 (T39) - Notification
} IOL_PortEventCodes;

/* A.6.4 EventQualifier: Bits 0 to 2: INSTANCE
 * These bits indicate the particular source (instance) of an Event thus refining its evaluation on
 * the receiver side
 * */
typedef enum IOL_EventInstance{
	IOL_EVENTINST_UNKNOWN = 0,
	/*1 to 3 Reserved*/
	IOL_EVENTINST_APPLICATION		= 4,
	/*7 to 7 Reserved*/
}IOL_EventInstance;

/* A.6.4 EventQualifier: Bit 3: SOURCE
 * This bit indicates the source of the Event
 * */
typedef enum IOL_EventSource{
	IOL_EVENTSRC_DEVICE = 0,
	IOL_EVENTSRC_MASTER,
}IOL_EventSource;

/* A.6.4 EventQualifier: Bit 4 to 5: TYPE
 * These bits indicate the Event category.
 * */
typedef enum IOL_EventType{
	/* 0 Reserved*/
	IOL_EVENTTYPE_NOTIFICATION = 1,
	IOL_EVENTTYPE_WARNING,
	IOL_EVENTTYPE_ERROR,
}IOL_EventType;

/* A.6.4 EventQualifier: Bit 6 to 7: MODE
 * These bits indicate the Event mode
 * */
typedef enum IOL_EventMode{
	/* 0 Reserved*/
	IOL_EVENTMODE_SINGLESHOT = 1,
	IOL_EVENTMODE_DISAPPEARS,
	IOL_EVENTMODE_APPEARS,
}IOL_EventMode;


/*Annex: E -----------------------*/
/*REF: E.2 MasterIdent*/
/* service supporting features
 * REF: E.2 MasterIdent: Features_1
 * Bit 0: DeviceParBatch (SMI_ParamWriteBatch)
 * 	0 = not supported, 1 = supported
 * Bit 1: DeviceParBatch (SMI_ParamReadBatch)
 * 	0 = not supported, 1 = supported
 * Bit 2: PortPowerOffOn (SMI_PortPowerOffOn)
 * 	0 = not supported, 1 = supported
 * Bit 3 to 7: Reserved (= 0)
 * */
typedef enum IOL_MasterFeatures1{
	IOL_MSTR_FEATURES_1_SUPPORT_NONE		= 0x00,	// All services are not supported
	IOL_MSTR_FEATURES_1_DEVWRITEPARBATCH	= 0x01, // bit 0, Service SMI_ParamWriteBatch: 0 = not supported, 1 = supported
	IOL_MSTR_FEATURES_1_DEVREADPARBATCH		= 0x02, // bit-1, Service SMI_ParamReadBatch: 0 = not supported, 1 = supported
	IOL_MSTR_FEATURES_1_PORTPWROFFON		= 0x04, // bit-2, Service SMI_PortPowerOffOn: 0 = not supported, 1 = supported
	/*Bit 3 to 7: Reserved (= 0)*/
}IOL_MasterFeatures1;

/* Master port types
 * REF: E.2 MasterIdent: PortTypes
 * Array indicating for all n ports the type of port
 * 	0: Class A
 * 	1: Class A with PortPowerOffOn
 * 	2: Class B; see 5.4.2
 * 	3: FS_Port_A without OSSDe; see [10]
 * 	4: FS_Port_A with OSSDe; see [10]
 * 	5: FS_Port_B; see [10]
 * 	6: W_Master; see [11]
 * 	7 to 255: Reserved
 * */
typedef enum IOL_MasterPortTypes{
	IOL_MSTR_PORTTYP_CLASS_A,				// 0: Class A
	IOL_MSTR_PORTTYP_CLASS_A_PORTPWROFFON,	// 1: Class A with PortPowerOffOn
	IOL_MSTR_PORTTYP_CLASS_B,				// 2: Class B; see 5.4.2
	IOL_MSTR_PORTTYP_FS_PORT_A_NO_OSSDE,	// 3: FS_Port_A without OSSDe; see [10]
	IOL_MSTR_PORTTYP_FSPORT_A_OSSDE,		// 4: FS_Port_A with OSSDe; see [10]
	IOL_MSTR_PORTTYP_FS_PORT_B,				// 5: FS_Port_B; see [10]
	IOL_MSTR_PORTTYP_W_Master,				// 6: W_Master; see [11]
	/*7 to 255: Reserved*/
}IOL_MasterPortTypes;

/*Annex: F -----------------------*/
/*Annex: G -----------------------*/

/*IOLink errors
 * */
typedef enum IOL_Errors{
	IOL_ERROR_NONE = 0U,
	IOL_ERROR_BUSY,
	IOL_ERROR_NULL_POINTER,
	IOL_ERROR_NO_COMM,		  // no communication available
	IOL_ERROR_STATE_CONFLICT, // service unavailable within current state
	IOL_ERROR_VALUE_OUT_OF_RANGE,
	IOL_ERROR_PARM_CONFLICT,
	IOL_ERROR_PDIN_LENGTH,
	IOL_ERROR_PDOUT_LENGTH,
	IOL_ERROR_ODIN_LENGTH,
	IOL_ERROR_ODOUT_LENGTH,
	IOL_ERROR_DIR_PARAM_ADDR,
	IOL_ERROR_ADDRCTRL_INVALID,
	IOL_ERROR_COMM_CH_INVALID,
	IOL_ERROR_ISDU_TIMEOUT,
	IOL_ERROR_ISDU_NOT_SUPPORTED,
	IOL_ERROR_ISDU_ABORT,
	IOL_ERROR_NO_DATA,
	IOL_ERROR_INCORRECT_DATA,
}IOL_Errors;

/*
 * control code 8.2.2.12
 * */
typedef enum IOL_ControlCode{
	IOL_CONTROLCODE_NONE = 0,
	IOL_CONTROLCODE_VALID,
	IOL_CONTROLCODE_INVALID,
	IOL_CONTROLCODE_PDOUTVALID,
	IOL_CONTROLCODE_PDOUTINVALID,
	IOL_CONTROLCODE_DEVICEMODE,
} IOL_ControlCode;

typedef enum IOL_MasterType {
	IOL_MASTER_TYPE_UNSPECIFIC = 0,	   // 0: Unspecific (manufacturer specific)
	IOL_MASTER_TYPE_RESERVED = 1,	   // 1: Reserved
	IOL_MASTER_TYPE_ACC_TO_SPEC = 2, // 2: Master according to this specification or later
	IOL_MASTER_TYPE_FS_MASTER = 3,	   // 3: FS_Master; see [10]
	IOL_MASTER_TYPE_W_MASTER = 4	   // 4: W_Master; see [11]
} IOL_MasterType;

/* Target mode
 * Ref: IO-Link Interface Spec v1.1.3, section 9.2.2.2, Table 81 –Definitions of the Target Modes
 * */
//TODO: duplicate in IOLink_CM.h
typedef enum IOL_TargetMode{
	IOL_TARGET_MODE_INACTIVE = 0,   // Communication disabled, no DI, no DO
	IOL_TARGET_MODE_CFGCOM,		    // Device communicating in CFGCOM mode after successful inspection
	IOL_TARGET_MODE_AUTOCOM,    	// Device communicating in AUTOCOM mode without inspection
	IOL_TARGET_MODE_DI,         	// Port in digital input mode (SIO)
	IOL_TARGET_MODE_DO          	// Port in digital output mode (SIO)
}IOL_TargetMode;

/* Inspection Level
 * Ref: IO-Link Interface Spec v1.1.3, section 9.2.2.2, Table 80 –Definitions of the InspectionLevel
 * */
//TODO: duplicate in IOLink_CM.h
typedef enum IOL_InspecLevel{
	IOL_INSPEC_LEVEL_NO_CHECK = 0,
	IOL_INSPEC_LEVEL_TYPE_COMP,
	IOL_INSPEC_LEVEL_IDENTICAL,
} IOL_InspecLevel;
/*	Baud rate
 * */
typedef enum IOL_BaudRate{
	IOL_BAUDRATE_NONE = 0,
	IOL_BAUDRATE_AUTO,
	IOL_BAUDRATE_COM1,
	IOL_BAUDRATE_COM2,
	IOL_BAUDRATE_COM3,
} IOL_BaudRate;

/*ISDU ------------------------------*/
/* A.5.2 I-Service qualifier
 * */
typedef enum IOL_IServiceQualifier
{
	IOL_ISDU_IS_QUAL_NO_SERVICE_0 = 0, // no service
	IOL_ISDU_IS_QUAL_MSTR_WREQ_1,	   // master write request, 8-bit Index
	IOL_ISDU_IS_QUAL_MSTR_WREQ_2,	   // master write request, 8-bit Index & subindex
	IOL_ISDU_IS_QUAL_MSTR_WREQ_3,	   // master write request, 16-bit Index & subindex
	IOL_ISDU_IS_QUAL_DEV_WRSP_N_4,	   // device negative write response
	IOL_ISDU_IS_QUAL_DEV_WRSP_P_5,	   // device positive write response
	/*6 to 8 reserved*/
	IOL_ISDU_IS_QUAL_MSTR_RREQ_9 = 9, // master write request, 8-bit Index
	IOL_ISDU_IS_QUAL_MSTR_RREQ_10,	  // master write request, 8-bit Index & subindex
	IOL_ISDU_IS_QUAL_MSTR_RREQ_11,	  // master write request, 16-bit Index & subindex
	IOL_ISDU_IS_QUAL_DEV_RRSP_N_12,	  // device negative read response
	IOL_ISDU_IS_QUAL_DEV_RRSP_P_13,	  // device positive read response
									  /*14 to 15 reserved*/
} IOL_IServiceQualifier;



/*Extra-----------------------*/
typedef enum IOL_RWStatus{
	IOL_RW_STATUS_NONE = 0,
	IOL_RW_STATUS_DONE,
	IOL_RW_STATUS_BUSY,
	IOL_RW_STATUS_ALL_DONE,
}IOL_RWStatus;



#endif /* SRC_IOLINK_IOLINK_DEFINE_H_ */
