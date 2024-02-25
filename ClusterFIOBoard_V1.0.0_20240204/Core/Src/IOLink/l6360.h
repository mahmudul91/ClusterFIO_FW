/*
 * l6360.c
 *
 *  Created on: Jan 16, 2022
 *      Author: wsrra
 */

/* Driver for the IO-Link communication master transceiver IC "L6360"
 * Manufactured by: ST-Electronics
 * Datasheet: DS8900 - Rev 9 - September 2023
 * Download: https://www.st.com/resource/en/datasheet/l6360.pdf
 * */
#ifndef L6360_H_
#define L6360_H_



/*Enable the MCU specific function----------*/
#ifndef L6360_MCU_STM32XXX
	#define L6360_MCU_STM32XXX
#endif

#ifdef L6360_MCU_STM32XXX
	#include "main.h"
#endif

/*UART communicating type: poll, Interrupt and DMA*/
#define L6360_UART_COMTYPE_POLL
//#define L6360_UART_COMTYPE_INTERRUPT
//#define L6360_UART_COMTYPE_DMA



#define L6360_ATTRIB_PACKED 		__attribute__((packed))

#define L6360_MAX_WRITE_RETRY       3U		// max number of retry for writing the register
#define L6360_MAX_UART_RXBUF_LEN	256U	// max uart rx buffer length

/*Default Factory configuration-----------------------*/
#define L6360_DEF_CONF_REG			0x80
#define L6360_DEF_CTRL_REG_1		0x21
#define L6360_DEF_CTRL_REG_2		0x21
#define L6360_DEF_LED1MSB_REG		0x00
#define L6360_DEF_LED1LSB_REG		0x00
#define L6360_DEF_LED2MSB_REG		0x00
#define L6360_DEF_LED2LSB_REG		0x00

/*
 * 	***
 * Timing below was setting for blocking UART RX, now are IT, maybe RX handle time now is faster and need be adjust
 * 	***
 *
 * WAKE UP TIMING PROCEDURE (6 tries)
 * TX LOW for 75...85us -> default TIME0 75
 * WAIT MAX 425us and then wait Tdmt COM3 117...160us -> default TIME1 530
 * Send TX at BAUD 230400
 * WAIT for response 703...963us -> default TIME2 650 (because of add also time of RX handle)
 * If response -> break else
 * Send TX at BAUD 38400
 * WAIT for response 5624...7708us -> default TIME3_1 4000 + Rx handle + TIME3_2 640
 * If response -> break else
 * Send TX at BAUD 4800
 * WAIT for response 20000...40000us -> default 20000
 *
 */
#define WAKE_TIME_0 (75)
#define WAKE_TIME_1 (500)
#define WAKE_TIME_2 (570)
#define WAKE_TIME_3_1 (5600)
#define WAKE_TIME_3_2 (0)
#define WAKE_TIME_4 (20000)

#define PARITY(x)      ((uint8_t)(((((uint8_t)(x)&0x80U)>>7U)^(((uint8_t)(x)&0x40U)>>6U)^(((uint8_t)(x)&0x20U)>>5U)^(((uint8_t)(x)&0x10U)>>4U)^(((uint8_t)(x)&0x08U)>>3U)^(((uint8_t)(x)&0x04U)>>2U)^(((uint8_t)(x)&0x02U)>>1U)^((uint8_t)(x)&0x01U))))
#define ODD_PARITY(x)  ((uint8_t)((((uint8_t)(x)&0x80U)>>7U)^(((uint8_t)(x)&0x20U)>>5U)^(((uint8_t)(x)&0x08U)>>3U)^(((uint8_t)(x)&0x02U)>>1U)))
#define EVEN_PARITY(x) ((uint8_t)((((uint8_t)(x)&0x40U)>>6U)^(((uint8_t)(x)&0x10U)>>4U)^(((uint8_t)(x)&0x04U)>>2U)^((uint8_t)(x)&0x01U)))


/*I2C-------------*/
/*l6360 IOLink chip i2c address*/

#define L6360_I2C_DEV_ADDR_1  	0xC2
#define L6360_I2C_DEV_ADDR_2  	0xC4
#define L6360_I2C_DEV_ADDR_3  	0xC6
#define L6360_I2C_DEV_ADDR_4  	0xC8
#define L6360_CHECK_I2C_ADDR(addr)	(addr == L6360_I2C_DEV_ADDR_1 || addr == L6360_I2C_DEV_ADDR_3 || \
									 addr == L6360_I2C_DEV_ADDR_2 || addr == L6360_I2C_DEV_ADDR_4)

/*UART----------------------*/
/* L6360 supports 3 UART communication baudrates
 * COM1 (4.8 kbaud), COM2 (38.4 kbaud) and COM3 (230.4 kbaud)
 * Ref: L6360 datasheet, Description, page-1
 * */
#define L6360_UART_COM1_BR4800			4800U	// bit/s, SDCI communication mode with transmission rate of 4.8 kbit/s
#define L6360_UART_COM2_BR38400			38400U	// bit/s, SDCI communication mode with transmission rate of 38.4 kbit/s
#define L6360_UART_COM3_BR230400		230400U	// bit/s, SDCI communication mode with transmission rate of 230.4 kbit/s
#define L6360_UART_CHECK_BR(br)			(br == L6360_UART_COM1_BR4800 || \
										 br == L6360_UART_COM2_BR38400 || \
										 br == L6360_UART_COM3_BR230400)


#define L6360_UART_COM1_TBIT			208.33F	// us
#define L6360_UART_COM2_TBIT			26.04F	// us
#define L6360_UART_COM3_TBIT			4.34F	// us

#define L6360_UART_BIT_TX_TIME(br)		(1000000/br)	// us,  11 bit = 1 byte, tx time
#define L6360_UART_BYTE_TX_TIME(br)		(11/br)	// s,  11 bit = 1 byte, tx time
#define L6360_UART_CALC_TX_TIME(br, n)	(L6360_UART_BYTE_TX_TIME(br)*1000*n)	// ms, n byte tx time
#define L6360_UART_TX_TIME_OFFSET		2U	//ms, offset is needed because calculated time out may less than 1

#define L6360_WURQ_TWU_PULSE			80U	// us, 	Duration of Master's wake-up current pulse 75 us to 85 us
#define L6360_WURQ_TWU_OFFSET			1U	// us, this offset may needed, because function call time may added with the Delay function,

//
///*L6360 error status--------*/
//#define L6360_STATUS_OK					0		// no error
//#define L6360_STATUS_ERORR	 			-1		// error
//#define L6360_STATUS_BUSY	 				-2		// error
//#define L6360_STATUS_TIMEOUT	 			-3		// error
//#define L6360_STATUS_I2C_COM_ERROR		-4		// error
//#define L6360_STATUS_PARITY_ERROR			-5		// error
//#define L6360_STATUS_I2C_ADDR_INVALID		-6		// error

/*	L6360 I2C error status
 * */
typedef enum L6360_Status{
	L6360_OK = 0U,
	L6360_ERROR,
	L6360_BUSY,
	L6360_TIMEOUT,
	L6360_UNKNOWN,
	L6360_VALUE_OUT_OF_RANGE,
	L6360_INVALID_DEV_ADDRESS,
	L6360_INVALID_REG_ADDRESS,
	L6360_INVALID_LEN,
	L6360_INVALID_UART_PARAM,		// invalid uart parameters
	L6360_NULL_PTR_PARAM,		// parameter null pointer error
	L6360_NULL_PTR_FUNC,		// function null pointer error
}L6360_Status;

/*Mutex Start ----------------------*/
/* to lock unlock the process
 * */
typedef enum L6360_ProcessLock{
	L6360_UNLOCKED = 0x00U,
	L6360_LOCKED   = 0x01U,
} L6360_ProcessLock;

#define L6360_PROCESS_BUSY			L6360_BUSY
#define L6360_LOCK(__handle__)   	do{ if((__handle__).lock == L6360_LOCKED) return L6360_PROCESS_BUSY;	\
                                    	else (__handle__).lock = L6360_LOCKED;    \
                                	}while (0U)
#define L6360_UNLOCK(__handle__)	(__handle__).lock = L6360_UNLOCKED

/*Mutex End -------------------------*/



/* TargetMode
 * Ref: IO-Link Interface Spec v1.1.3, section 5.2.2.1,
 * */
typedef enum  L6360_PortMode{
	L6360_PORT_MODE_INACTIVE = 0U,		// (C/Q line in high impedance)
	L6360_PORT_MODE_DI,					// DI (C/Q line in digital input mode),
	L6360_PORT_MODE_DO,					// DO (C/Q line in digital output mode),
	L6360_PORT_MODE_SDCI,    			// SDCI (C/Q line in digital), actually SDCI is COM1 or COM2 or COM3
	L6360_PORT_MODE_COM1,				// COM1 (C/Q line in COM1 mode),
	L6360_PORT_MODE_COM2,				// COM2 (C/Q line in COM2 mode),
	L6360_PORT_MODE_COM3,				// COM3 (C/Q line in COM3 mode)
} L6360_PortMode;



/*i2c----------------4*/
/* Read/write mode*/
typedef enum L6360_RWMode{
	L6360_RW_MODE_WRITE = 0,	// write mode
	L6360_RW_MODE_READ,			// read mode
}L6360_RWMode;


/* L6360 i2c register address*/
typedef enum L6360_I2CRegAddress{
	L6360_I2C_REG_ADDR_STATUS = 0,		// Status Register Address
	L6360_I2C_REG_ADDR_CONFIG,			// Configuration register
	L6360_I2C_REG_ADDR_CONTROL1,		// Control register 1
	L6360_I2C_REG_ADDR_CONTROL2,		// Control register 2
	L6360_I2C_REG_ADDR_LED1_MSB,		// LED1 register MSB
	L6360_I2C_REG_ADDR_LED1_LSB,		// LED1 register LSB
	L6360_I2C_REG_ADDR_LED2_MSB,		// LED2 register MSB
	L6360_I2C_REG_ADDR_LED2_LSB,		// LED2 register LSB
	L6360_I2C_REG_ADDR_PARITY,			// Parity register
	L6360_I2C_REG_ADDR_MAX,
}L6360_I2CRegAddress;




/* Register write mode
 * Ref: 7.2 Operating modes, page: 30
 * */
typedef enum L6360_I2CWriteMode{
	L6360_WRITE_MODE_NONE	= 0,
	L6360_WRITE_MODE_SINGLE,	// Current: single register
	L6360_WRITE_MODE_ALL,		// Sequential: all registers in sequence
}L6360_I2CWriteMode;

typedef enum L6360_PinState{
	L6360_PIN_RESET = 0,
	L6360_PIN_SET,
}L6360_PinState;

/*Configuration Register Parameters---------*/
/*Table 23. C/Q output stage configuration*/
typedef enum L6360_CNFRcq{
	L6360_CNFR_CQ_OFF = 0,
	L6360_CNFR_CQ_LOW_SIDE,
	L6360_CNFR_CQ_HIGH_SIDE,
	L6360_CNFR_CQ_PUSH_PULL_1,		// 1 & 2 are for same functionality, bit-3 is don't care
	L6360_CNFR_CQ_TRI_STATE,
	L6360_CNFR_CQ_LOW_SIDE_ON,
	L6360_CNFR_CQ_HIGH_SIDE_ON,
	L6360_CNFR_CQ_PUSH_PULL_2,		// 1 & 2 are for same functionality, bit-3 is don't care
	L6360_CNFR_CQ_MAX,
}L6360_CNFRcq;

/*Control Register 1 Parameters---------*/
/*Table 12. ENCGQ: C/Q pull-down enable*/
typedef enum L6360_CR1ENcgq{
	L6360_CR1_ENCGQ_DISABLE	= 0,	// Always OFF, default
	L6360_CR1_ENCGQ_ENABLE,			// If ENC/Q = 0 -> ON, If ENC/Q = 1 -> OFF
}L6360_CR1ENcgq;
/* Table 13: C/QO HS and LS cut-off current
 * */
typedef enum L6360_CR1Icoq{
	L6360_CR1_ICOQ_115_MA = 0,		// 115 mA
	L6360_CR1_ICOQ_220_MA,			// 220 mA, default
	L6360_CR1_ICOQ_350_MA,			// 350 mA
	L6360_CR1_ICOQ_580_MA,			// 580 mA
}L6360_CR1Icoq;

/* Table 14: tdcoq: C/QO HS and LS cut-off current delay time
 * */
typedef enum L6360_CR1Tdcoq{
	L6360_CR1_TDCOQ_100_US = 0,		// 100 us, default
	L6360_CR1_TDCOQ_150_US,			// 150 us
	L6360_CR1_TDCOQ_200_US,			// 200 us
	L6360_CR1_TDCOQ_250_US,			// 250 us
}L6360_CR1coq;

/* Table 15. trcoq: C/QO restart delay time
 * */
typedef enum L6360_CR1Trcoq{
	L6360_CR1_TRCOQ_255X_TDCOQ = 0,	// 255xt_dcoq, default(255x100 = 25ms)
	L6360_CR1_TRCOQ_LATCHED,		// latched
}L6360_CR1Trcoq;

/* Table 16.  tdbq: C/QI debounce time*/
typedef enum L6360_CR1Tdbq{
	L6360_CR1_TDBQ_0_US	= 0,		// 0 us
	L6360_CR1_TDBQ_5_US,			// 5 us, default
	L6360_CR1_TDBQ_20_US,			// 20 us
	L6360_CR1_TDBQ_100_US,			// 100 us
}L6360_CR1Tdbq;

/*Control Register 2 Parameters---------*/
/*Table 17. EN_CGI: I/Q pull-down enable*/
typedef enum L6360_CR2ENcgi{
	L6360_CR2_ENCGI_DISABLE = 0,	// Always OFF
	L6360_CR2_ENCGI_ENABLE,			// Always ON
}L6360_CR2ENcgi;

/*Table 18. CQPDG: C/Q pull-down generator switching*/
typedef enum L6360_CR2CQpdg{
	L6360_CR2_CQPDG_2P5_MA = 0,		// I_INI/Qi (input current C/Qi pin) = 2.5 mA
	L6360_CR2_CQPDG_5P5_MA,			// I_INC/Qi (input current C/Qi pin) = 5.5 mA
}L6360_CR2CQpdg;

/*Table 19. L+COD: L+ cut-off disable*/
typedef enum L6360_CR2LplusCod{
	L6360_CR2_LPLUS_COD_ENABLED = 0,	//L+ cut-off disable is enabled
	L6360_CR2_LPLUS_COD_DISABLED,		//L+ cut-off disable is disabled
}L6360_CR2LplusCod;

/*Table 20. tDCOL: L+ HS cut-off current delay time*/
typedef enum L6360_CR2Tdcol{
	L6360_CR2_TDCOL_500_US = 0,		//500 us
	L6360_CR2_TDCOL_0_US,			//500 us
}L6360_CR2Tdcol;

/*Table 21. tRCOL: L+ restart delay time*/
typedef enum L6360_CR2Trcol{
	L6360_CR2_TRCOL_64_MS = 0,		// 64 ms
	L6360_CR2_TRCOL_LATCHED,		// Latched
}L6360_CR2Trcol;

/*Table 22. Bit 1:0 = tdbi [1:0]: I/Q debounce time*/
typedef enum L6360_CR2Tdbi{
	L6360_CR2_TDBI_0_US = 0,		// 0 us
	L6360_CR2_TDBI_5_US,			// 5 us
	L6360_CR2_TDBI_20_US,			// 20 us
	L6360_CR2_TDBI_100_US,			// 100 us
}L6360_CR2Tdbi;



/*L6360 registers-------------------------------*/
/* Status register (address: 0x00)
 * Ref: 6.9 Internal registers, page- 16 to 18
 * Read only
 * Reset value: [00000000]
 * */
typedef union L6360_ATTRIB_PACKED L6360_StatusReg{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t pe 		: 1;	// Bit 0 = PE: parity check error
		uint8_t regLn 	: 1;	// Bit 1 = REG LN: linear regulator undervoltage fault
		uint8_t _0 		: 1;	// Bit 2 = not used: always at zero
		uint8_t lol 	: 1;	// Bit 3 = LOL: L+ overload
		uint8_t cqol 	: 1;	// Bit 4 = CQOL: C/Q overload
		uint8_t ovt 	: 1;	// Bit 5 = OVT: overtemperature fault
		uint8_t _1 		: 1;	// Bit 6 = not used: always at zero
		uint8_t po 		: 1;	// Bit 7 = PO: Power-on (L+ line)
	}bf;
}L6360_StatusReg_t;


/* Configuration register (address: 0x01)
 * Ref: 6.9 Internal registers, page- 21 to 61
 * Read/write
 * Reset value: [100xxxxx]
 * */
typedef union L6360_ATTRIB_PACKED L6360_ConfigReg{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t _0to4 	: 5;	// Bit 4:0 = not used
		uint8_t cq 		: 3;	// Bit 7:5 = C/Q[2:0]: C/Q output stage configuration
	}bf;
}L6360_ConfigReg_t;

/* Control register 1 (address: 0x02)
 * Ref: 6.9 Internal registers, page- 21 to 61
 * Read/write
 * Reset value: [00100001]
 * */
typedef union L6360_ATTRIB_PACKED L6360_ControlReg_1{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t  tDbq	: 2;	// Bit 1:0 = tdbq [1:0]: C/QI debounce time
		uint8_t  tRcoq	: 1;	// Bit 2 = trcoq: C/QO restart delay time
		uint8_t  tDcoq	: 2;	// Bit 4:3 = tdcoq [1:0]: C/QO HS and LS cut-off current delay time
		uint8_t  iCoq 	: 2;	// Bit 6:5 = ICOQ [1:0]: C/QO HS and LS cut-off current
		uint8_t  enCgq 	: 1;	// Bit 7 = ENCGQ: C/QI pull-down enable
	}bf;
}L6360_ControlReg_1_t;

/* Control register 2 (address: 0x03)
 * Ref: 6.9 Internal registers, page- 21 to 61
 * Read/write
 * Reset value: [0x100001]
 * */
typedef union L6360_ATTRIB_PACKED L6360_ControlReg_2{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t tDbi 		: 2;	// Bit 1:0 = tdbi [1:0]: I/Q debounce time
		uint8_t tRcol 		: 1;	// Bit 2 = tRCOL: L+ restart delay
		uint8_t tDcol 		: 1;	// Bit 3 = tDCOL: L+ cut-off current delay time
		uint8_t lPulsCod 	: 1;	// Bit 4 = L+COD: L+ cut-off disable
		uint8_t cqPdg		: 1;	// Bit 5 = CQPDG: C/Q pull-down generator switching
		uint8_t _0			: 1;	// Bit 6 = not used
		uint8_t enCgi 		: 1;	// Bit 7 = ENCGI: I/Q pull-down enable
	}bf;
}L6360_ControlReg_2_t;

/* LED 1 MSB & LSB register (address: 0x04 & 0x05)
 * Ref: 6.9 Internal registers, page- 23
 * Read/write
 * Reset value:  [00000000]
 * */
typedef union L6360_ATTRIB_PACKED L6360_LED1Reg{
	uint16_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t  lsb : 8;	// Bit 7:0 = LED1 LSB
		uint8_t  msb : 8;	// Bit 15:8 = LED1 LSB
	}bf;
}L6360_LED1Reg_t;

/* LED 2 MSB & LSB register (address: 0x06 & 0x07)
 * Ref: 6.9 Internal registers, page- 23
 * Read/write
 * Reset value:  [00000000]
 * */
typedef union L6360_ATTRIB_PACKED L6360_LED2Reg{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t  lsb : 8;	// Bit 7:0 = LED2 LSB
		uint8_t  msb : 8;	// Bit 15:8 = LED2 LSB
	}bf;
}L6360_LED2Reg_t;

/* Parity register (address: 0x08)
 * Ref: 6.9 Internal registers, page- 24
 * Read/write
 * Reset value:  [00000000]
 * */
typedef union L6360_ATTRIB_PACKED L6360_ParityReg{
	uint8_t value;		// register value in byte
	/*register value in bit-field*/
	struct{
		uint8_t l2l : 1;	// Bit 0 = L2L: LED2 low register parity
		uint8_t l2h : 1;	// Bit 1 = L2H: LED2 high register parity
		uint8_t l1l : 1;	// Bit 2 = L1L: LED1 low register parity
		uint8_t l1h : 1;	// Bit 3 = L1H: LED1 high register parity
		uint8_t ct2 : 1;	// Bit 4 = CT2: control register 2 parity
		uint8_t ct1 : 1;	// Bit 5 = CT1: control register 1 parity
		uint8_t cr  : 1;	// Bit 6 = CR: configuration register parity
		uint8_t sr  : 1;	// Bit 7 = SR: status register parity
	}bf;
}L6360_ParityReg_t;

/*Main L6360 structure-------------------------*/
typedef struct L6360_IOLink_hw L6360_IOLink_hw_t;

/* All i2c registers
 * */
typedef struct L6360_ATTRIB_PACKED L6360_I2CRegister{
	L6360_StatusReg_t sr;
	L6360_ConfigReg_t cnfr;
	L6360_ControlReg_1_t cr1;
	L6360_ControlReg_2_t cr2;
	L6360_LED1Reg_t led1;
	L6360_LED2Reg_t led2;
	L6360_ParityReg_t pr;
}L6360_I2CRegister_t;


/*UART port----*/
typedef struct L6360_ATTRIB_PACKED L6360_UARTConfig{
	L6360_ProcessLock lock;
	uint32_t baudrate;
	uint32_t txTimeout;		//TX timeout for non-dma uart tx service
	uint8_t rxBuf[L6360_MAX_UART_RXBUF_LEN];
	uint16_t rxLen;
	L6360_Status (*config_cb)(L6360_IOLink_hw_t* l6360, L6360_RWMode rwMode, uint32_t* baudrate);
	L6360_Status (*tx_cb) (L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout);
	L6360_Status (*rx_cb) (L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout);	// this services is used for polling implementation

#ifdef L6360_MCU_STM32XXX
	UART_HandleTypeDef* huart;
	GPIO_TypeDef* txPort;
	GPIO_TypeDef* rxPort;
	uint16_t txPin;
	uint16_t rxPin;
	IRQn_Type irqType;
	uint8_t priority;
	uint8_t subPriority;

	struct{
		DMA_HandleTypeDef* hdmaTx;
		DMA_HandleTypeDef* hdmaRx;
//		IRQn_Type irqTypDmaTx;
//		IRQn_Type irqTypDmaRx;
		uint8_t priorityTx;
		uint8_t subPriorityTx;
		uint8_t priorityRx;
		uint8_t subPriorityRx;
	}dma;

#endif
}L6360_UARTConfig_t;


/*I2C port----*/
typedef struct L6360_ATTRIB_PACKED L6360_I2CConfig{
	L6360_ProcessLock lock;
	uint8_t devAddress;
	L6360_Status status;		//
	uint8_t wRetry;				// register write retry
	L6360_I2CWriteMode wMode;	// write mode
	L6360_Status (*write_cb) (uint8_t devAddress, uint8_t* data, uint16_t len);
	L6360_Status (*read_cb) (uint8_t devAddress, uint8_t* data, uint16_t len);

#ifdef L6360_MCU_STM32XXX
	I2C_HandleTypeDef* hi2c;
#endif
}L6360_I2CConfig_t;


struct L6360_ATTRIB_PACKED L6360_IOLink_hw{
	uint8_t portNum;
	 uint8_t portMode;
	L6360_I2CRegister_t reg;	// all registers

	/*UART port----*/
	L6360_UARTConfig_t uart;

	/*I2C port----*/
	L6360_I2CConfig_t i2c;

	/*Config SIO mode----*/
	void (*sio_config_cb)	(GPIO_TypeDef* rx_port, uint16_t rx_pin);

	/*Control pins----*/

	struct{
#ifdef L6360_MCU_STM32XXX
		GPIO_TypeDef* lpEnPort;
		GPIO_TypeDef* cqEnPort;
		GPIO_TypeDef* rstPort;
		uint16_t lpEnPin;
		uint16_t cqEnPin;
		uint16_t rstPin;
#endif

		void (*enLplus)(L6360_IOLink_hw_t* l6360, L6360_PinState state);
		void (*encq)(L6360_IOLink_hw_t* l6360, L6360_PinState state);
		void (*rst)(L6360_IOLink_hw_t* l6360, L6360_PinState state);
	}ctrlPins;
};


/*Services for l6360 register-------------*/
int16_t L6360_GetSR(L6360_IOLink_hw_t* l6360);		/* get status resister*/
int16_t L6360_GetCNFR(L6360_IOLink_hw_t* l6360);	/* get Configuration resister*/
int16_t L6360_GetCR1(L6360_IOLink_hw_t* l6360);		/* get Control resister 1*/
int16_t L6360_GetCR2(L6360_IOLink_hw_t* l6360);		/* get Control resister 2*/
int32_t L6360_GetLED1(L6360_IOLink_hw_t* l6360);	/* get LED 1 resister (2-byte)*/
int32_t L6360_GetLED2(L6360_IOLink_hw_t* l6360);	/* get LED 2 resister (2-byte)*/
int16_t L6360_GetPR(L6360_IOLink_hw_t* l6360);		/* get parity resister*/

L6360_Status L6360_SetCNFR(L6360_IOLink_hw_t* l6360, L6360_CNFRcq value);	/* set Configuration resister*/
L6360_Status L6360_SetCR1(L6360_IOLink_hw_t* l6360, uint8_t value);		/* set Control resister 1*/
L6360_Status L6360_SetCR2(L6360_IOLink_hw_t* l6360, uint8_t value);		/* set Control resister 2*/
L6360_Status L6360_SetLED1(L6360_IOLink_hw_t* l6360, uint16_t value);	/* set LED 1 resister (2-byte)*/
L6360_Status L6360_SetLED2(L6360_IOLink_hw_t* l6360, uint16_t value);	/* set LED 2 resister (2-byte)*/

/*Services for I2C port----------------------*/
L6360_Status L6360_I2CWriteSingle(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t regAddr, uint8_t data);
L6360_Status L6360_I2CWriteAll(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t* data);
L6360_Status L6360_I2CRead(L6360_IOLink_hw_t* l6360, uint8_t devAddr, uint8_t regAddr, uint8_t* data, uint16_t len);
L6360_Status L6360_I2CReadStatusParity(L6360_IOLink_hw_t* l6360, uint8_t *status, uint8_t* parity);
void L6360_I2CIRQHandler(L6360_IOLink_hw_t* l6360);
void L6360_I2CAttachWriteCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (uint8_t devAddress, uint8_t* data, uint16_t len));
void L6360_I2CAttachReadCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (uint8_t devAddress, uint8_t* data, uint16_t len));

/*Services for Uart port----------------------*/
#ifdef L6360_MCU_STM32XXX
L6360_Status L6360_UartInit( L6360_IOLink_hw_t* l6360, uint32_t baudrate );
L6360_Status L6360_UartSetBaudrate(L6360_IOLink_hw_t* l6360, uint32_t baudrate);
#endif

L6360_Status L6360_UartConfig( L6360_IOLink_hw_t* l6360);

L6360_Status L6360_UartSend(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len);
L6360_Status L6360_UartRecv(L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len);	// this service is used only for the polling implementation
void L6360_UartTxcIRQ(L6360_IOLink_hw_t* l6360);
void L6360_UartRxcIRQ(L6360_IOLink_hw_t* l6360, uint8_t *data, uint16_t len);
void L6360_UartAttachConfigCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback)(L6360_IOLink_hw_t* l6360, L6360_RWMode rwMode, uint32_t* baudrate));
void L6360_UartAttachTXCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout));
void L6360_UartAttachRXCallback(L6360_IOLink_hw_t* l6360, L6360_Status (*callback) (L6360_IOLink_hw_t* l6360, uint8_t* data, uint16_t len, uint32_t timeout));

/*Services for L6360 Control pins-------------*/
void L6360_LplusEnable(L6360_IOLink_hw_t* l6360); /* Enable the ENL+  gpio pin*/
void L6360_LplusDisable(L6360_IOLink_hw_t* l6360); /* Disable the ENL+ gpio pin*/
void L6360_CqEnable(L6360_IOLink_hw_t* l6360); /* Enable the ENC/Q gpio pin*/
void L6360_CqDisable(L6360_IOLink_hw_t* l6360); /* Disable the ENC/Q  gpio pin*/
void L6360_Reset(L6360_IOLink_hw_t* l6360); /* Enable the reset gpio pin*/
void L6360_AttachLplusPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state));
void L6360_AttachCQPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state));
void L6360_AttachResetPinCallback(L6360_IOLink_hw_t* l6360, void (*callback)(L6360_IOLink_hw_t* l6360, L6360_PinState state));

/*Timer related services-------------------------------*/
void L6360_AttachTimerCallback(uint64_t (*callback)(void));

/*LOLink Services-----------*/
L6360_Status L6360_GenWURQ(L6360_IOLink_hw_t* l6360);
L6360_Status L6360_SetPortMode(L6360_IOLink_hw_t* l6360, uint8_t mode);
L6360_Status L6360_ConfigSDCIMode(L6360_IOLink_hw_t* l6360,  uint8_t mode);
L6360_Status L6360_ConfigSIOMode(L6360_IOLink_hw_t* l6360, L6360_PortMode mode);
//void L6360_sio_config( GPIO_TypeDef* rx_port, uint16_t rx_pin );



#endif /* L6360_H_ */

