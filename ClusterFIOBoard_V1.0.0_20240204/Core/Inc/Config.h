/*
 * Config.h
 *
 *  Created on: Oct 13, 2022
 *      Author: wsrra
 */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_
#include "main.h"

#include "wizchip_conf.h"
#include "MB_Serial.h"
#include "AnalogIO.h"
#include "MB_Handler.h"
#include "Encoder.h"
//#include "SocketHandler.h"

/*Device configs------------------------*/
#define CONF_DEF_DEV_MODE				DEV_MODE_MASTER//DEV_MODE_SLAVE//////////////////////////////////////////////////////DEV_MODE_SLAVE//
/*Modbus RTU Slave------------------------*/
#define CONF_DEF_MBRTUS_SLAVE_ADDRESS					0x01
#define CONF_DEF_MB_RTU_MODE			 MB_RTU_MODE_SLAVE //MB_RTU_MODE_MASTER

/*Modbus RTU Serial------------------------*/
#define CONF_DEF_MB_SERIAL_PROTOCOL		MBS_PROTO_RTU_STD//MBS_PROTO_RTU_CLUSTER//
#define CONF_DEF_MB_SERIAL_BR_USER		230400U //user define baud rate
#define CONF_DEF_MB_SERIAL_BR_INDEX		MBS_BR230400_INDEX // 0 position is the user define baud rate
#define CONF_DEF_MB_SERIAL_CONFIG		((CONF_DEF_MB_SERIAL_BR_INDEX<<12) | (MBS_DB_8<<8) | (MBS_PB_EVEN<<4) | MBS_SB_1)//MBS_BR2000000_INDEX

//	MBS_BR9600_INDEX,
//	MBS_BR14400_INDEX,
//	MBS_BR19200_INDEX,
//	MBS_BR28800_INDEX,
//	MBS_BR38400_INDEX,
//	MBS_BR57600_INDEX,
//	MBS_BR115200_INDEX,
//	MBS_BR230400_INDEX,
//	MBS_BR460800_INDEX,
//	MBS_BR576000_INDEX,
//	MBS_BR1000000_INDEX,
//	MBS_BR2000000_INDEX,
//	MBS_BR3000000_INDEX,
//	MBS_BR4000000_INDEX,

/*Modbus RTU Master------------------------*/
#define CONF_DEF_MBRTUM_ENABLE							0
#define CONF_DEF_MBRTUM_CUST_REPLAY_TIMEOUT				6000 //in us
#define CONF_DEF_MBRTUM_CUST_PREIODIC_REQ_INTERVAL		17000 // in us
#define CONF_DEF_MBRTUM_CUST_RETRANS_ENABLE				0
#define CONF_DEF_MBRTUM_CUST_SLV_START_ADDR				1
#define CONF_DEF_MBRTUM_CUST_NUM_OF_SLV					0
#define CONF_DEF_MBRTUM_PREIODIC_REQ_TIMEOUT_MAX		655350 //in us

/*Modbus RTU Slave------------------------*/
//#define CONF_DEF_MBRTUS_SLAVE_ADDRESS					0x01					// default modbas rtu slave address
#define CONF_DEF_MBRTUS_CUST_WAITING_TIMEOUT			5000//in us
#define CONF_DEF_MBRTUS_CUST_RESP_TIMEOUT_MAX			655350U //in us

/*Modbus Gateway------------*/
#define CONF_DEF_MBGW_TIMEOUT					30000U //in us
#define CONF_DEF_MBGW_TIMEOUT_MAX				655350U //in us


/*Modbus w5500 configs  ------------------------*/
#define CONF_DEF_NETINFO_IP						{11, 200, 0, 100}	//{192U, 168U, 0U, 110U}			// default server ip address
#define CONF_DEF_NETINFO_MAC					{0x80, 0x81, 0x82, 0x83, 0x84, 0x85}	// default mac address
#define CONF_DEF_NETINFO_SN						{255U, 255U, 255U, 0U}			// default subnet mask
#define CONF_DEF_NETINFO_GW						{11, 200, 0, 11}			// default gateway
#define CONF_DEF_NETINFO_DNS					{8U, 8U, 8U, 8U}				// default DNS
#define CONF_DEF_NETINFO_DHCP					NETINFO_STATIC 				// or NETINFO_DHCP
#define CONF_DEF_W5500_RETRY_COUNT				6//4 // actual count is W5500_RETRY_COUNT+1
#define CONF_DEF_W5500_RETRY_BASE_TIME			200 // in ms


//winchip socket------------------
#define CONF_DEF_SERVER_SOCK_RETRY_DELAY		3000 //in ms
#define CONF_DEF_CLIENT_SOCK_RETRY_DELAY		3000 //in ms

/*Modbus TCP configs for both server & client ------------------------*/
#define CONF_DEF_MB_TCP_MSS						620U//360U							// max segment size for a socket
#define CONF_DEF_MB_TCP_KPALVTR					15U							// (KPALVTRx5 sec keep alive time) >0 auto keep alive process turned on, =0 Manual-keep-alive-process turned on, works only for TCP mode


/*Modbus TCP server------------------------*/
#define CONF_DEF_MB_TCPS_SRC_PORT				502U


/*Modbus TCP Client------------------------*/
#define CONF_DEF_MB_TCPC_DEST_PORT				502U
#define CONF_DEF_MB_TCPC_SRC_PORT				1025U						// Modbus tcp client destination port must not less than 1025
#define CONF_DEF_MB_TCPC_DEST_IP				{11, 200, 0U, 11}//{192U, 168U, 0U, 102U}			// default destination ip address for the client
#define CONF_DEF_MB_TCPC_KPALVTR				15U							// (KPALVTRx5 sec keep alive time) >0 auto keep alive process turned on, =0 Manual-keep-alive-process turned on, works only for TCP mode
#define CONF_DEF_MB_TCPC_CONN_MODE				SOCK_CONN_MODE_AUTO
#define CONF_DEF_MB_TCPC_TX_TIM_INTRVL			5 // in ms
#define CONF_DEF_MB_TCPC_TX_TIM_INTERVAL_MAX	60000U //in ms
#define CONF_DEF_MB_TCPC_DLY_AFTER_CONNECT		2000 // inms

/*UDP Server*/
#define CONF_DEF_SERVER_UDP_PORT				161U 						// 161 is the default UDP server port


/*Digital Inputs 1 to 8--------------*/
#define CONF_DEF_DI1_MODE						DI_MODE_NORMAL
#define CONF_DEF_DI1_TRIGGER_MODE				DI_TRIG_MODE_RISING
#define CONF_DEF_DI1_SIG_PERIOD_MAX				250 //ms
#define CONF_DEF_DI2_MODE						DI_MODE_NORMAL
#define CONF_DEF_DI2_TRIGGER_MODE				DI_TRIG_MODE_RISING
#define CONF_DEF_DI2_SIG_PERIOD_MAX				250 //ms

/*Analog Input 1 to 4------------------------*/
#define CONF_DEF_AI1_MODE						AIO_MODE_VOLT
#define CONF_DEF_AI2_MODE						AIO_MODE_VOLT
#define CONF_DEF_AI3_MODE						AIO_MODE_VOLT
#define CONF_DEF_AI4_MODE						AIO_MODE_VOLT


/*Analog output 1 to 4------------------------*/
#define CONF_DEF_AO1_ENABLE						1
#define CONF_DEF_AO1_MODE						AIO_MODE_VOLT
#define CONF_DEF_AO1_SRC						AO_PWM_SRC_MODBUS
#define CONF_DEF_AO2_ENABLE						1
#define CONF_DEF_AO2_MODE						AIO_MODE_VOLT
#define CONF_DEF_AO2_SRC						AO_PWM_SRC_MODBUS
#define CONF_DEF_AO3_ENABLE						1
#define CONF_DEF_AO3_MODE						AIO_MODE_VOLT
#define CONF_DEF_AO3_SRC						AO_PWM_SRC_MODBUS
#define CONF_DEF_AO4_ENABLE						1
#define CONF_DEF_AO4_MODE						AIO_MODE_VOLT
#define CONF_DEF_AO4_SRC						AO_PWM_SRC_MODBUS

/*Encoder--------------*/
#define CONF_DEF_ENC_DISABLE					0
#define CONF_DEF_ENC_MODE						ENC_MODE_QUAD
#define CONF_DEF_ENC_QUAD_TRIG_MODE				ENC_TRIG_MODE_RISING
#define CONF_DEF_ENC_CHA_TRIG_MODE				ENC_TRIG_MODE_RISING
#define CONF_DEF_ENC_CHB_TRIG_MODE				ENC_TRIG_MODE_RISING
#define CONF_DEF_ENC_CHC_TRIG_MODE				ENC_TRIG_MODE_RISING

/*PID-------------*/
#define CONF_DEF_PID1_OUT_MODE					PID_OM_PERCENT
#define CONF_DEF_PID1_KP						0.5f
#define CONF_DEF_PID1_KI						1.0f
#define CONF_DEF_PID1_KD						1.0f
#define CONF_DEF_PID1_I_OUT_MAX					10.0f

#define CONF_DEF_PID2_OUT_MODE					PID_OM_PERCENT
#define CONF_DEF_PID2_KP						0.5f
#define CONF_DEF_PID2_KI						1.0f
#define CONF_DEF_PID2_KD						1.0f
#define CONF_DEF_PID2_I_OUT_MAX					10.0f

#endif /* INC_CONFIG_H_ */
