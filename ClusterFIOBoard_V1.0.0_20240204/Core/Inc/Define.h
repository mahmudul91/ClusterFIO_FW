/*
 * Define.h
 *
 *  Created on: Oct 17, 2022
 *      Author: wsrra
 */

#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_

/*
 * high 8 bits for integer value and lower 8 bits for decimal value
 * for example, 0x0110 = 1.1
 * 				0x1234 = 12.34
 * */
#define HW_VIRSION							0x0100
#define FW_VIRSION							0x0100


/*
 *  Define.h file contains only volatile parameters, those can't be changed in runtime
 *
 *  */

#define DEF_U8_MAX_VALUE				255U
#define DEF_U16_MAX_VALUE				65535U
#define DEF_U32_MAX_VALUE				4294967295U

/*Socket configuration*/
//#define DEF_SOCK_MAX						4 // 2 socket for server, 2 socket for client
/**/
#define DEF_MBTCP_MAX_BUFF_SIZE				360
#define DEF_SERVER_MAX_TCP_CONNS		1	// Maximum client connections for the modbus server, this number of client can be connected

#define DEF_CLIENT_MAX_TCP_CONNS		1	// Maximum client connections for the modbus client, this number of client can be connected

/*MOdbus TCP server--------*/
#define DEF_MBTCPS_MAX_TRANS				4U

/*MOdbus TCP server--------*/
#define DEF_MBTCPC_MAX_TRANS				4U

/*------- FIR Filter macro--------*/
//#define FILTER_FIR_ENABLED




/*-------- Digital output---------*/
// High Speed PWM frequency
#define DEF_DO_PWM_FREQ						10000.0f	//in Hz, maximum supported frequency



/*-------- Digital Inputt---------*/
#define DEF_DI_BOUNCE_COUNT_MAX				32U		//in Hz, minimum supported frequency
#define DEF_DI_CNTR_PULSE_DETECT_TIMEOUT	10000		//in Hz,

/*PID feedback scaling parameters for motor speed controlling--------*/
#define DEF_PRD2FB_FRML_A_LOWER_LIMIT		220
#define DEF_PRD2FB_FRML_A_COEFF				1000000000
#define DEF_PRD2FB_FRML_A_POW				-2.554f
#define DEF_PRD2FB_FRML_B_LOWER_LIMIT		140
#define DEF_PRD2FB_FRML_B_UPPER_LIMIT		220
#define DEF_PRD2FB_FRML_B_COEFF				2000000
#define DEF_PRD2FB_FRML_B_POW				-1.4f

/*------------Analog Inputs-------------*/
/*
 * Uncomment to enable ADC DMA
 * */
//#define USE_AI_ADC_DMA
#ifdef USE_AI_ADC_DMA
	#ifndef AI_USE_ADC_DMA
		#define AI_USE_ADC_DMA
	#endif
#endif


/*Extended IOs with PCA9557 IO extender---------*/
#define PCA_IOEX_I2C_DEV_ADDR				2
/*Pins for analog output voltage and
 * current outout mode selection
 * */
#define AO_1_VI_MODE_PIN					PCA_PIN_0
#define AO_2_VI_MODE_PIN					PCA_PIN_1
#define AO_3_VI_MODE_PIN					PCA_PIN_2
#define AO_4_VI_MODE_PIN					PCA_PIN_3

/*Pins for RS485 RX/TX LED */
#define RS485_RX_LED_PIN					PCA_PIN_4
#define RS485_TX_LED_PIN					PCA_PIN_5

/*Pins for MCU general purpose LED 1 & 2*/
#define LED_1_PIN							PCA_PIN_6
#define LED_2_PIN							PCA_PIN_7

/* RS485 LED blinking behavior
 *
 * */
#define RS485_RX_LED_BLINK_ON_RX_COUNT		3u	// after this count over the rs led will blink 1 time
#define RS485_TX_LED_BLINK_ON_TX_COUNT		3u	// after this count over the rs led will blink 1 time


#endif /* INC_DEFINE_H_ */
