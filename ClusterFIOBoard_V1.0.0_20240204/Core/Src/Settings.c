/*
 * Settings.c
 *
 *  Created on: Jan 30, 2024
 *      Author: wsrra
 */
#include "Settings.h"
#include "GlobalVars.h"
#include "Debug.h"
#include "main.h"
#include "stm32h7xx_hal.h"

/*Old--------------*/
#include "Settings.h"
#include "main.h"
#include "Scheduler.h"
#include "wizchip_conf.h"
#include "Config.h"
#include "Debug.h"
#include "Server.h"
#include "MB_Gateway.h"
#include "MB_TCPClient.h"
#include "Storage_Address.h"
#include "Flash_StorageExt.h"
#include "TimeStamp.h"

extern GlobalVar_t gVar;

extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c4;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart5_tx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_uart7_tx;
extern DMA_HandleTypeDef hdma_uart7_rx;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

extern SDRAM_HandleTypeDef hsdram1;


/*old--------------*/
//extern DebugStruct dbug;
//extern GlobalVar gVar;
extern TimeStamp timStamp;

extern ADC_HandleTypeDef hadc1;

SPI_HandleTypeDef hspi1;	// todo: need to change


extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

extern UART_HandleTypeDef huart6;


void SetUpdateSettings(float *pVar, float *setValue, float *defValue, uint8_t condition);


/*Digital Output pins-----------*/
/*GPIO Port list*/
GPIO_TypeDef *DO_PORTS[DO_MAX] = {
		DO_1_PWM_GPIO_Port,
		DO_2_PWM_GPIO_Port,
		DO_3_PWM_GPIO_Port,
		DO_4_PWM_GPIO_Port,
		DO_5_GPIO_Port,
		DO_6_GPIO_Port
};
/*GPIO Pin list*/
const uint16_t DO_PINS[DO_MAX] ={
		DO_1_PWM_Pin,
		DO_2_PWM_Pin,
		DO_3_PWM_Pin,
		DO_4_PWM_Pin,
		DO_5_Pin,
		DO_6_Pin
};

/*Digital Input pins-----------*/
/*GPIO Port list*/
GPIO_TypeDef *DI_PORTS[DI_MAX] = {
		DI_1_ENC_A_GPIO_Port,
		DI_2_ENC_B_GPIO_Port,
		DI_3_ENC_Z_GPIO_Port,
		DI_4_GPIO_Port,
		DI_5_GPIO_Port,
		DI_6_GPIO_Port,
		DI_7_GPIO_Port,
		DI_8_GPIO_Port
};
/*GPIO Pin list*/
const uint16_t DI_PINS[DI_MAX] ={
		DI_1_ENC_A_Pin,
		DI_2_ENC_B_Pin,
		DI_3_ENC_Z_Pin,
		DI_4_Pin,
		DI_5_Pin,
		DI_6_Pin,
		DI_7_Pin,
		DI_8_Pin
};


/* Default Settings
 * */
void InitialSettings(void){

	L6360_IOLink_hw_t* iolPort[IOL_PI_MAX];

	/*Assign Port number to IOLink chip*/
	for(uint8_t i = IOL_PI_0; i < IOL_PI_MAX; i++ ){
		iolPort[i] 				= &gVar.iolm.port[i];
		iolPort[i]->portNum 	= IOL_PORT_1+i;
		/*I2C*/
		L6360_I2CRegister_t reg = {0};
		iolPort[i]->i2c.hi2c 	= &hi2c4;
		iolPort[i]->i2c.lock	= L6360_UNLOCKED;
		iolPort[i]->i2c.wMode	= L6360_RW_MODE_READ;
		iolPort[i]->i2c.wRetry	= 0U;
		iolPort[i]->i2c.status	= 0;
		iolPort[i]->reg = reg;
		/*UART*/
		iolPort[i]->uart.lock		= L6360_UNLOCKED;
		iolPort[i]->uart.baudrate	= L6360_UART_COM3_BR230400;
	}
	DBG_SPRINT_NL("l6360: %p", &gVar.iolm.port[IOL_PI_0]);
	/*IOLink master chip l6360 1---------------------*/
	/*I2C*/
	iolPort[IOL_PI_0]->i2c.devAddress 	= L6360_I2C_DEV_ADDR_1;

	/*Control pins*/
	iolPort[IOL_PI_0]->ctrlPins.lpEnPort = IOL1_ENL_GPIO_Port;
	iolPort[IOL_PI_0]->ctrlPins.cqEnPort = IOL1_ENCQ_GPIO_Port;
	iolPort[IOL_PI_0]->ctrlPins.rstPort = IOL1_RST_GPIO_Port;
	iolPort[IOL_PI_0]->ctrlPins.lpEnPin = IOL1_ENL_Pin;
	iolPort[IOL_PI_0]->ctrlPins.cqEnPin = IOL1_ENCQ_Pin;
	iolPort[IOL_PI_0]->ctrlPins.rstPin = IOL1_RST_Pin;
	/*UART*/
	iolPort[IOL_PI_0]->uart.huart = &huart5;
	iolPort[IOL_PI_0]->uart.huart->Instance = UART5;
	iolPort[IOL_PI_0]->uart.txPort = IOL1_TX_U5_GPIO_Port;
	iolPort[IOL_PI_0]->uart.txPin = IOL1_TX_U5_Pin;
	iolPort[IOL_PI_0]->uart.rxPort = IOL1_RX_U5_GPIO_Port;
	iolPort[IOL_PI_0]->uart.rxPin = IOL1_RX_U5_Pin;
	iolPort[IOL_PI_0]->uart.irqType = UART5_IRQn;
	iolPort[IOL_PI_0]->uart.priority = 0;
	iolPort[IOL_PI_0]->uart.subPriority = 0;

	iolPort[IOL_PI_0]->uart.dma.hdmaRx = &hdma_uart5_rx;
//	iolPort[IOL_PI_0]->uart.dma.irqTypDmaRx = DMA1_Stream1_IRQn;
	iolPort[IOL_PI_0]->uart.dma.priorityRx = 0;
	iolPort[IOL_PI_0]->uart.dma.subPriorityRx = 0;
	iolPort[IOL_PI_0]->uart.dma.hdmaTx = &hdma_uart5_tx;
//	iolPort[IOL_PI_0]->uart.dma.irqTypDmaTx = DMA1_Stream0_IRQn;
	iolPort[IOL_PI_0]->uart.dma.priorityTx = 0;
	iolPort[IOL_PI_0]->uart.dma.subPriorityTx = 0;

	/*IOLink master chip l6360 2---------------------*/
	/*I2C*/
	iolPort[IOL_PI_1]->i2c.devAddress 	= L6360_I2C_DEV_ADDR_2;

	/*Control pins*/
	iolPort[IOL_PI_1]->ctrlPins.lpEnPort = IOL2_ENL_GPIO_Port;
	iolPort[IOL_PI_1]->ctrlPins.cqEnPort = IOL2_ENCQ_GPIO_Port;
	iolPort[IOL_PI_1]->ctrlPins.rstPort = IOL2_RST_GPIO_Port;
	iolPort[IOL_PI_1]->ctrlPins.lpEnPin = IOL2_ENL_Pin;
	iolPort[IOL_PI_1]->ctrlPins.cqEnPin = IOL2_ENCQ_Pin;
	iolPort[IOL_PI_1]->ctrlPins.rstPin = IOL2_RST_Pin;
	/*UART*/
	iolPort[IOL_PI_1]->uart.huart = &huart7;
	iolPort[IOL_PI_1]->uart.huart->Instance = UART7;
	iolPort[IOL_PI_1]->uart.txPort = IOL2_TX_U7_GPIO_Port;
	iolPort[IOL_PI_1]->uart.txPin = IOL2_TX_U7_Pin;
	iolPort[IOL_PI_1]->uart.rxPort = IOL2_RX_U7_GPIO_Port;
	iolPort[IOL_PI_1]->uart.rxPin = IOL2_RX_U7_Pin;
	iolPort[IOL_PI_1]->uart.irqType = UART7_IRQn;
	iolPort[IOL_PI_1]->uart.priority = 0;
	iolPort[IOL_PI_1]->uart.subPriority = 0;
	/*UART DMA*/
	iolPort[IOL_PI_1]->uart.dma.hdmaRx = &hdma_uart7_rx;
//	iolPort[IOL_PI_1]->uart.dma.irqTypDmaRx = DMA1_Stream1_IRQn;
	iolPort[IOL_PI_1]->uart.dma.priorityRx = 0;
	iolPort[IOL_PI_1]->uart.dma.subPriorityRx = 0;
	iolPort[IOL_PI_1]->uart.dma.hdmaTx = &hdma_uart7_tx;
//	iolPort[IOL_PI_1]->uart.dma.irqTypDmaTx = DMA1_Stream0_IRQn;
	iolPort[IOL_PI_1]->uart.dma.priorityTx = 0;
	iolPort[IOL_PI_1]->uart.dma.subPriorityTx = 0;

	/*IOLink master chip l6360 3 ---------------------*/
	/*I2C*/
	iolPort[IOL_PI_2]->i2c.devAddress 	= L6360_I2C_DEV_ADDR_3;

	/*Control pins*/
	iolPort[IOL_PI_2]->ctrlPins.lpEnPort = IOL3_ENL_GPIO_Port;
	iolPort[IOL_PI_2]->ctrlPins.cqEnPort = IOL3_ENCQ_GPIO_Port;
	iolPort[IOL_PI_2]->ctrlPins.rstPort = IOL3_RST_GPIO_Port;
	iolPort[IOL_PI_2]->ctrlPins.lpEnPin = IOL3_ENL_Pin;
	iolPort[IOL_PI_2]->ctrlPins.cqEnPin = IOL3_ENCQ_Pin;
	iolPort[IOL_PI_2]->ctrlPins.rstPin = IOL3_RST_Pin;
	/*UART*/
	iolPort[IOL_PI_2]->uart.huart = &huart1;
	iolPort[IOL_PI_2]->uart.huart->Instance = USART1;
	iolPort[IOL_PI_2]->uart.txPort = IOL3_TX_U1_GPIO_Port;
	iolPort[IOL_PI_2]->uart.txPin = IOL3_TX_U1_Pin;
	iolPort[IOL_PI_2]->uart.rxPort = IOL3_RX_U1_GPIO_Port;
	iolPort[IOL_PI_2]->uart.rxPin = IOL3_RX_U1_Pin;
	iolPort[IOL_PI_2]->uart.irqType = USART1_IRQn;
	iolPort[IOL_PI_2]->uart.priority = 0;
	iolPort[IOL_PI_2]->uart.subPriority = 0;

	iolPort[IOL_PI_2]->uart.dma.hdmaRx = &hdma_usart1_rx;
//	iolPort[IOL_PI_2]->uart.dma.irqTypDmaRx = DMA1_Stream1_IRQn;
	iolPort[IOL_PI_2]->uart.dma.priorityRx = 0;
	iolPort[IOL_PI_2]->uart.dma.subPriorityRx = 0;
	iolPort[IOL_PI_2]->uart.dma.hdmaTx = &hdma_usart1_tx;
//	iolPort[IOL_PI_2]->uart.dma.irqTypDmaTx = DMA1_Stream0_IRQn;
	iolPort[IOL_PI_2]->uart.dma.priorityTx = 0;
	iolPort[IOL_PI_2]->uart.dma.subPriorityTx = 0;

	/*IOLink master chip l6360 4---------------------*/
	/*I2C*/
	iolPort[IOL_PI_3]->i2c.devAddress 	= L6360_I2C_DEV_ADDR_4;

	/*Control pins*/
//	iolPort[IOL_PI_3]->ctrlPins.lpEnPort = IOL4_ENL_GPIO_Port;
//	iolPort[IOL_PI_3]->ctrlPins.cqEnPort = IOL4_ENCQ_GPIO_Port;
//	iolPort[IOL_PI_3]->ctrlPins.rstPort = IOL4_RST_GPIO_Port;
//	iolPort[IOL_PI_3]->ctrlPins.lpEnPin = IOL4_ENL_Pin;
//	iolPort[IOL_PI_3]->ctrlPins.cqEnPin = IOL4_ENCQ_Pin;
//	iolPort[IOL_PI_3]->ctrlPins.rstPin = IOL4_RST_Pin;
	/*UART*/
	iolPort[IOL_PI_3]->uart.huart = &huart4;
	iolPort[IOL_PI_3]->uart.huart->Instance = UART4;
	iolPort[IOL_PI_3]->uart.txPort = IOL4_TX_U4_GPIO_Port;
	iolPort[IOL_PI_3]->uart.txPin = IOL4_TX_U4_Pin;
	iolPort[IOL_PI_3]->uart.rxPort = IOL4_RX_U4_GPIO_Port;
	iolPort[IOL_PI_3]->uart.rxPin = IOL4_RX_U4_Pin;
	iolPort[IOL_PI_3]->uart.irqType = UART4_IRQn;
	iolPort[IOL_PI_3]->uart.priority = 0;
	iolPort[IOL_PI_3]->uart.subPriority = 0;

	iolPort[IOL_PI_3]->uart.dma.hdmaRx = &hdma_uart4_rx;
//	iolPort[IOL_PI_3]->uart.dma.irqTypDmaRx = DMA1_Stream1_IRQn;
	iolPort[IOL_PI_3]->uart.dma.priorityRx = 0;
	iolPort[IOL_PI_3]->uart.dma.subPriorityRx = 0;
	iolPort[IOL_PI_3]->uart.dma.hdmaTx = &hdma_uart4_tx;
//	iolPort[IOL_PI_3]->uart.dma.irqTypDmaTx = DMA1_Stream0_IRQn;
	iolPort[IOL_PI_3]->uart.dma.priorityTx = 0;
	iolPort[IOL_PI_3]->uart.dma.subPriorityTx = 0;

	/*Old------------*/


	// Utilities-----------------------
	gVar.sysFreq = HAL_RCC_GetSysClockFreq();
	gVar.devMode = CONF_DEF_DEV_MODE;

	MB_SetRTUMode((gVar.devMode== DEV_MODE_SLAVE ? MB_RTU_MODE_SLAVE : MB_RTU_MODE_MASTER));		// set the rtu mode

	gVar.diag.rtuBus.peCount 		= 0;
	gVar.diag.rtuBus.neCount 		= 0;
	gVar.diag.rtuBus.feCount 		= 0;
	gVar.diag.rtuBus.oreCount 		= 0;
	gVar.diag.rtuBus.dteCount 		= 0;
	gVar.diag.rtuBus.dmaTxErrCode	= 0;
	gVar.diag.rtuBus.dmaRxErrCode	= 0;
	gVar.diag.rtuBus.ErrCode	= 0;

	gVar.diag.w5500Spi.modfeCount 	= 0;
	gVar.diag.w5500Spi.crceCount 	= 0;
	gVar.diag.w5500Spi.feCount 		= 0;
	gVar.diag.w5500Spi.oreCount 	= 0;
	gVar.diag.w5500Spi.dteCount 	= 0;
	gVar.diag.w5500Spi.flgeCount 	= 0;
	gVar.diag.w5500Spi.abreCount 	= 0;
	gVar.diag.w5500Spi.dmaTxErrCode	= 0;
	gVar.diag.w5500Spi.dmaRxErrCode	= 0;
	gVar.diag.w5500Spi.ErrCode	= 0;

	gVar.diag.i2c.ErrCode 		= 0;
	gVar.diag.i2c.dmaRxErrCode 		= 0;
	gVar.diag.i2c.dmaTxErrCode 		= 0;




	//W5500 ethernet chip configuration
	// pin and interrupt initialization for w5500 chip spi communication
	// todo: w5500:pass, need to uncomment
//		gVar.w5500Chip.intrptFlag 		= 0U;
//		gVar.w5500Chip.spi.cs_port 		= W5500_CS_GPIO_Port;
//		gVar.w5500Chip.spi.cs_pin 		= W5500_CS_Pin;
//		gVar.w5500Chip.spi.rst_port 	= W5500_RST_GPIO_Port;
//		gVar.w5500Chip.spi.rst_pin 		= W5500_RST_Pin;
//		gVar.w5500Chip.spi.int_port 	= W5500_IRQ_GPIO_Port;
//		gVar.w5500Chip.spi.int_pin		= W5500_IRQ_Pin;
//		gVar.w5500Chip.spi.irqType 		= EXTI15_10_IRQn;
//		gVar.w5500Chip.spi.hspi			= &hspi1;		// spi
//		gVar.w5500Chip.spi.errorCode	= 0U;
//		gVar.w5500Chip.spi.dmaTxErrCode	= 0U;
//		gVar.w5500Chip.spi.dmaRxErrCode	= 0U;

	// initiates default net configuration
	wiz_NetInfo tempNetInfo = {
		.mac 	= CONF_DEF_NETINFO_MAC,
		.ip 	= CONF_DEF_NETINFO_IP,
		.sn 	= CONF_DEF_NETINFO_SN,
		.gw 	= CONF_DEF_NETINFO_GW,
		.dns 	= CONF_DEF_NETINFO_DNS,
		.dhcp 	= CONF_DEF_NETINFO_DHCP
	};
	gVar.w5500Chip.netInfo 	= tempNetInfo;										// sets the server info




	/*Modbus RTU Serial line----------------------*/

	gVar.mbSerial.mode			 				= MBS_MODE_SLAVE;
	gVar.mbSerial.errorCode						= 0U;
	gVar.mbSerial.protocol						= CONF_DEF_MB_SERIAL_PROTOCOL;
	gVar.mbSerial.isFreeze						= 0U;
	gVar.mbSerial.isDataReceived 				= 0U;
	gVar.mbSerial.isBusy 						= 0U;
	gVar.mbSerial.isReceiving 					= 0U;
	gVar.mbSerial.isSending 					= 0U;
	gVar.mbSerial.frameOkFlag					= 0U;
	gVar.mbSerial.respTimer.enable				= 0U;
	gVar.mbSerial.respTimer.isTimeout			= 0U;
	gVar.mbSerial.respTimer.timer				= 0U;
	gVar.mbSerial.respTimer.timeout				= MB_RESP_TIIMEOUT;
	gVar.mbSerial.rxCount						= 0;
	gVar.mbSerial.txCount						= 0;

	gVar.mbSerial.serialConfigValue 			= CONF_DEF_MB_SERIAL_CONFIG;
	MBS_DecodeConfigValue(&gVar.mbSerial);

	/*Modbus RTU Slave----------------------*/
	gVar.mbRTUSlave.slave_address 				= CONF_DEF_MBRTUS_SLAVE_ADDRESS;			// the slave address only for test purpose
	gVar.mbRTUSlave.custom.waitingTimer.timeout = CONF_DEF_MBRTUS_CUST_WAITING_TIMEOUT;
	gVar.mbRTUSlave.custom.waitingTimer.isTimeRst		= 0;
	gVar.mbRTUSlave.custom.eventType			= MB_CUST_EVENT_NONE;
	MBRS_Init(&gVar.mbRTUSlave);

	/*Modbus RTU Master----------------------*/
	MBRM_Init(&gVar.mbRTUMaster);
	gVar.mbRTUMaster.enable = 0;
	gVar.mbRTUMaster.replyTimer.timeout 		= CONF_DEF_MBRTUM_CUST_REPLAY_TIMEOUT;
	gVar.mbRTUMaster.replyTimer.timer 			= 0;
	gVar.mbRTUMaster.replyTimer.enableTimer 	= 0;
	gVar.mbRTUMaster.replyTimer.isTimeout 		= 0;
	gVar.mbRTUMaster.retryMax 					= MBRM_RETRY_MAX;
	gVar.mbRTUMaster.state 						= MBRM_STATE_IDLE;
	gVar.mbRTUMaster.custom.slvStartAddr		= CONF_DEF_MBRTUM_CUST_SLV_START_ADDR;
	gVar.mbRTUMaster.custom.numOfSlv			= CONF_DEF_MBRTUM_CUST_NUM_OF_SLV;
	gVar.mbRTUMaster.custom.enableRetrans		= 0;
	gVar.mbRTUMaster.custom.reTransTimer.enable	= 0;
	gVar.mbRTUMaster.custom.reTransTimer.time	= 0;
	gVar.mbRTUMaster.custom.reTransTimer.timeout= CONF_DEF_MBRTUM_CUST_PREIODIC_REQ_INTERVAL;//MBRM_RETRANS_TIMEOUT_CUST;
	gVar.mbRTUMaster.custom.timeoutCount        = 0;
	memset(gVar.mbRTUMaster.custom.rcvFailedCount, 0, sizeof(gVar.mbRTUMaster.custom.rcvFailedCount));
	memset(gVar.mbRTUMaster.custom.rcvConsecCount, 0, sizeof(gVar.mbRTUMaster.custom.rcvConsecCount));
	MBRM_BindSerial(&gVar.mbSerial);

	/*Modbus TCP Server-------------------*/
	gVar.mbTCPServer.isDataRcvd 				= 0;
	gVar.mbTCPServer.isReloadReq 				= 0;
	gVar.mbTCPServer.clientIndex 				= 0;
	gVar.mbTCPServer.curntTrnsNo 				= 0;
	gVar.mbTCPServer.maxTrnsNo 					= MBTCPS_MAX_TRANS;
	gVar.mbTCPServer.nbOfConns 					= 0;
	for(uint8_t i = 0; i < MBTCPS_MAX_CONNS; i++){
		MBTCPS_ClearClientParams(&gVar.mbTCPServer.clients[i]);
	}
//	MBMQ_clear(&gVar.mbTCPServer.msgQ);
	MBMQ_clearMsg(&gVar.mbTCPServer.msg);


	/*Modbus TCP Client-----------------------*/
	gVar.mbTCPClient.id 						= 0U;
	gVar.mbTCPClient.connect 					= 1;
	gVar.mbTCPClient.curntTrnsNo				= 0U;
	gVar.mbTCPClient.nbOfConns 					= 0U;
	gVar.mbTCPClient.lastTrnsIndex 				= 0U;
	gVar.mbTCPClient.header.transId 			= 0U;
	gVar.mbTCPClient.header.protocolId			= MBTCP_PROTOCOL_ID;
	gVar.mbTCPClient.header.length				= 0U;
	gVar.mbTCPClient.header.unitId 				= MB_BROADCAST_ADDR;//MBTCP_SERVER_UNIT_ID;
	gVar.mbTCPClient.destPort 					= CONF_DEF_MB_TCPC_DEST_PORT;
	gVar.mbTCPClient.srcPort 					= CONF_DEF_MB_TCPC_SRC_PORT;
	gVar.mbTCPClient.txTimIntrvl 				= CONF_DEF_MB_TCPC_TX_TIM_INTRVL;
	gVar.mbTCPClient.dlyAftrConct				= CONF_DEF_MB_TCPC_DLY_AFTER_CONNECT;
	uint8_t ip_0[4] 							= CONF_DEF_MB_TCPC_DEST_IP;
	memcpy(gVar.mbTCPClient.destIP, ip_0, 4);

	MBTCPC_ClearPendingList(gVar.mbTCPClient.pendingList);
	MBMQ_clear(&gVar.mbTCPClient.msgQ);



	/*Modbus gateway---------------*/
	MBG_Init(&gVar.mbGateway);
	gVar.mbGateway.rplyTimer.timeout 						= CONF_DEF_MBGW_TIMEOUT;//;
	gVar.mbGateway.rplyTimer.timer							= 0;
	gVar.mbGateway.rplyTimer.enable							= 0;
	gVar.mbGateway.rplyTimRglrFC.timeout 					= CONF_DEF_MBGW_TIMEOUT;//;
	gVar.mbGateway.rplyTimRglrFC.timer						= 0;
	gVar.mbGateway.rplyTimRglrFC.enable						= 0;
	MB_CUST_Init(&gVar.mbGateway.custom);

	/*Modbus custom -----------------*/
	MB_CUST_Init(&gVar.mbRTUMaster.custom);


	/*Server settings----------------------*/
	gVar.server.sockIndex 		= 0;											// sets the first socket id

	for(uint8_t i = 0; i < SERVER_SOCK_MAX; i++){								// sets the socket id
		Sock_ClearAll(&gVar.server.sock[i]);									// set the protocol as tcp
		gVar.server.sock[i].id 			= i;										//assign a socket id
		gVar.server.sock[i].connectMode	= SOCK_CONN_MODE_AUTO;
		gVar.server.sock[i].isConnected	= 0;
		gVar.server.sock[i].status		= 0;
		gVar.server.sock[i].mode	 	= SOCK_MODE_TCP_SERVER;					// socket mode
		gVar.server.sock[i].protocol 	= Sn_MR_TCP;
		gVar.server.sock[i].srcPort		= CONF_DEF_MB_TCPS_SRC_PORT;					// default modbus tcp server port
		gVar.server.sock[i].flag 		= Sn_MR_ND;								// set the socket no delay socket mode
		gVar.server.sock[i].kpAlvTime 	= CONF_DEF_MB_TCP_KPALVTR;
		gVar.server.sock[i].retryTick 	= 0;
		gVar.server.sock[i].retryDelay 	= CONF_DEF_SERVER_SOCK_RETRY_DELAY;
		gVar.server.sock[i].retryCounter 	= 0;
		gVar.server.sock[i].retryCountMax 	= SOCK_RETRY_COUNT_MAX;
		W5500_SetMaxSegmentSize(gVar.server.sock[i].id, CONF_DEF_MB_TCP_MSS);	// set the maximum segment size for a socket
	}


	/*Client settings----------------------*/
	gVar.client.sockIndex = 0;
	for(uint8_t i = 0; i < CLIENT_SOCK_MAX; i++){								// sets the socket id
		Sock_ClearAll(&gVar.client.sock[i]);
		gVar.client.sock[i].id 			= SERVER_SOCK_MAX + i;					// assign a socket after the server
		gVar.client.sock[i].connectMode	= CONF_DEF_MB_TCPC_CONN_MODE;
		gVar.client.sock[i].isConnected	= 0;
		gVar.client.sock[i].connect		= 0;
		gVar.client.sock[i].status		= 0;
		gVar.client.sock[i].mode	 	= SOCK_MODE_TCP_CLIENT;					// socket mode
		gVar.client.sock[i].protocol 	= Sn_MR_TCP;							// set the protocol as tcp
		gVar.client.sock[i].srcPort		= CONF_DEF_MB_TCPC_SRC_PORT+i;			// client source port
		gVar.client.sock[i].destPort	= CONF_DEF_MB_TCPC_DEST_PORT;			// client source port
		gVar.client.sock[i].flag 		= Sn_MR_ND;								// set the socket no delay socket mode
		gVar.client.sock[i].kpAlvTime 	= CONF_DEF_MB_TCP_KPALVTR;
		gVar.client.sock[i].retryTick 	= 0;
		gVar.client.sock[i].retryDelay 	= CONF_DEF_CLIENT_SOCK_RETRY_DELAY;
		gVar.client.sock[i].retryCounter 	= 0;
		gVar.client.sock[i].retryCountMax 	= SOCK_RETRY_COUNT_MAX;
		uint8_t ip[4] = CONF_DEF_MB_TCPC_DEST_IP;
		memcpy(gVar.client.sock[i].destIP, ip, 4);
		W5500_SetMaxSegmentSize(gVar.client.sock[i].id, CONF_DEF_MB_TCP_MSS);	// set the maximum segment size for a socket
	}


	/*Encoder Inputs------------------*/
		gVar.dio.enc.enable = CONF_DEF_ENC_DISABLE;
		gVar.dio.enc.configs.mode = CONF_DEF_ENC_MODE;
		gVar.dio.enc.isChanged = 1;
		//Quadrature encoder
		gVar.dio.enc.configs.quadrature.timer = &htim3;
		gVar.dio.enc.configs.quadrature.trigMode = CONF_DEF_ENC_QUAD_TRIG_MODE;

		//Incremental encoder  Channel A
		gVar.dio.enc.configs.channelA.port = DI_1_ENC_A_GPIO_Port;
		gVar.dio.enc.configs.channelA.pin = DI_1_ENC_A_Pin;
		gVar.dio.enc.configs.channelA.intrpt = EXTI0_IRQn;
		gVar.dio.enc.configs.channelA.trigMode = CONF_DEF_ENC_CHA_TRIG_MODE;
		gVar.dio.enc.configs.channelA.countMode = ENC_COUNT_MODE_UP;
		//Incremental encoder  Channel B
		gVar.dio.enc.configs.channelB.port = DI_2_ENC_B_GPIO_Port;
		gVar.dio.enc.configs.channelB.pin = DI_2_ENC_B_Pin;
		gVar.dio.enc.configs.channelB.intrpt = EXTI1_IRQn;
		gVar.dio.enc.configs.channelB.trigMode = CONF_DEF_ENC_CHA_TRIG_MODE;
		gVar.dio.enc.configs.channelB.countMode = ENC_COUNT_MODE_UP;
		//Incremental encoder  Channel C
		gVar.dio.enc.configs.channelZ.port = DI_3_ENC_Z_GPIO_Port;
		gVar.dio.enc.configs.channelZ.pin = DI_3_ENC_Z_Pin;
		gVar.dio.enc.configs.channelZ.intrpt = EXTI4_IRQn;
		gVar.dio.enc.configs.channelZ.trigMode = CONF_DEF_ENC_CHC_TRIG_MODE;
		gVar.dio.enc.configs.channelZ.countMode = ENC_COUNT_MODE_UP;
		gVar.dio.enc.interval = ENC_SPEED_TIM_INTRVAL;


	/*Digital Inputs------------------*/
	for (uint8_t i = DI_1; i < DI_MAX; ++i) {
		FLTR_ClearDebounce(&gVar.dio.di[i].dbunce);
		gVar.dio.di[i].index = i;
		gVar.dio.di[i].isChanged = 1;
		gVar.dio.di[i].dbunce.countMax = DEF_DI_BOUNCE_COUNT_MAX;
		gVar.dio.di[i].config.mode = DI_MODE_NORMAL;
		gVar.dio.di[i].config.countMode = DI_COUNT_MODE_UP;
		gVar.dio.di[i].config.counterType = DI_COUNT_TYPE_TIMER;
		gVar.dio.di[i].config.trigMode = DI_TRIG_MODE_RISING;
		gVar.dio.di[i].state = GPIO_PIN_RESET;

		gVar.dio.di[i].pulseInf.countOVF = 0;
		gVar.dio.di[i].pulseInf.edgeTime = 0;
		gVar.dio.di[i].pulseInf.isEdgeDtctd = 0;
		gVar.dio.di[i].pulseInf.lastEdgeTime = 0;
		gVar.dio.di[i].pulseInf.period = 0;
		gVar.dio.di[i].pulseInf.periodMin = 0;
		gVar.dio.di[i].pulseInf.isPulseDtctd = 0;
		gVar.dio.di[i].config.port = DI_PORTS[i];
		gVar.dio.di[i].config.pin  = DI_PINS[i];

	}
	gVar.dio.di[DI_1].config.channel  = TIM_CHANNEL_1;
//	gVar.dio.di[DI_1].config.inturrptId  = EXTI0_IRQn;
	gVar.dio.di[DI_2].config.channel  = TIM_CHANNEL_2;
//	gVar.dio.di[DI_2].config.inturrptId  = EXTI1_IRQn;
	gVar.dio.di[DI_3].config.channel  = TIM_CHANNEL_3;
//	gVar.dio.di[DI_3].config.inturrptId  = EXTI4_IRQn;
//	gVar.dio.di[DI_4].config.inturrptId  = EXTI9_5_IRQn;
//	gVar.dio.di[DI_5].config.inturrptId  = EXTI9_5_IRQn;
//	gVar.dio.di[DI_6].config.inturrptId  = EXTI3_IRQn;
//	gVar.dio.di[DI_7].config.inturrptId  = EXTI0_IRQn;
//	gVar.dio.di[DI_8].config.inturrptId  = EXTI0_IRQn;


	/*Digital Outputs------------------*/
//	gVar.dio.pwmFreq = DEF_DO_PWM_FREQ;
	for (uint8_t i = DO_1; i < DO_MAX; ++i) {
		gVar.dio.dout[i].index = i;
		gVar.dio.dout[i].state = 0;
		gVar.dio.dout[i].lastState = 0;
		gVar.dio.dout[i].index = i;
		gVar.dio.dout[i].source = DO_SRC_MODBUS;
		gVar.dio.dout[i].mode = DO_MODE_FOLLOW;
		gVar.dio.dout[i].isToggle = 0U;
		gVar.dio.dout[i].state = GPIO_PIN_RESET;
		gVar.dio.dout[i].pwm.enabled = 0U;
		gVar.dio.dout[i].pwm.dutyCycle = 0U;
		gVar.dio.dout[i].pwm.duty12Bits = 0U;
//		gVar.dio.dout[i].port = DO_PORTS[i];
//		gVar.dio.dout[i].pin  = DO_PINS[i];
	}





	gVar.dio.dout[DO_1].port = DO_1_PWM_GPIO_Port;
	gVar.dio.dout[DO_1].pin  = DO_1_PWM_Pin;
	gVar.dio.dout[DO_1].pwm.channel  = TIM_CHANNEL_1;
	gVar.dio.dout[DO_2].port = DO_2_PWM_GPIO_Port;
	gVar.dio.dout[DO_2].pin  = DO_2_PWM_Pin;
	gVar.dio.dout[DO_2].pwm.channel  = TIM_CHANNEL_2;
	gVar.dio.dout[DO_3].port = DO_3_PWM_GPIO_Port;
	gVar.dio.dout[DO_3].pin  = DO_3_PWM_Pin;
	gVar.dio.dout[DO_3].pwm.channel  = TIM_CHANNEL_3;
	gVar.dio.dout[DO_4].port = DO_4_PWM_GPIO_Port;
	gVar.dio.dout[DO_4].pin  = DO_4_PWM_Pin;
	gVar.dio.dout[DO_4].pwm.channel  = TIM_CHANNEL_4;
	gVar.dio.dout[DO_5].port = DO_5_GPIO_Port;
	gVar.dio.dout[DO_5].pin  = DO_5_Pin;
	gVar.dio.dout[DO_6].port = DO_6_GPIO_Port;
	gVar.dio.dout[DO_6].pin  = DO_6_Pin;

	/*Analog inputs 1 to 4 -----------------*/
	gVar.aiId = AI_1;
	for ( uint8_t i = AI_1;  i < AI_MAX; i++) {
#ifdef USE_AI_ADC_DMA
		gVar.aiBuffDMA[i] = 0;
#endif
		gVar.ai[i].id = i;			// Assign the id to each analog input
	}

	/*Analog output 1 to 4 -----------------*/
	gVar.aoId = AO_1;
	for ( uint8_t i = AO_1;  i < AO_MAX; i++) {
		gVar.ao[i].id = i;
		gVar.ao[i].enabled = 0;
		gVar.ao[i].mbValue = 0;
		gVar.ao[i].value = 0;
		gVar.ao[i].lastValue = 0;
		gVar.ao[i].source = CONF_DEF_AO1_SRC;
		gVar.ao[i].mode = CONF_DEF_AO1_MODE;
		gVar.ao[i].lastMode = 0;
		gVar.ao[i].pwm.dutyCycle = 0;
		gVar.ao[i].pwm.lastDutyCycle = 0;
		gVar.ao[i].pwm.offDuration = 0;
		gVar.ao[i].pwm.frequency = 0;
		gVar.ao[i].pwm.onDuration = 0;
		gVar.ao[i].pwm.period = 0;

	}
	gVar.ao[AO_1].pwm.channel = TIM_CHANNEL_1;
	gVar.ao[AO_2].pwm.channel = TIM_CHANNEL_2;
	gVar.ao[AO_3].pwm.channel = TIM_CHANNEL_3;
	gVar.ao[AO_4].pwm.channel = TIM_CHANNEL_4;

	/*PID 1 & 2 -----------------*/
	gVar.pid1.enable = 0;
	gVar.pid1.setpoint = 0;
	gVar.pid1.maxSetpoint = 0;
	gVar.pid1.measurement = 0;
	gVar.pid1.fbSource = 0;
	gVar.pid1.hystValue = 0;
	gVar.pid1.kp = 0;
	gVar.pid1.ki = 0;
	gVar.pid1.kd = 0;
	gVar.pid1.error = 0;
	gVar.pid1.last_error = 0;
	gVar.pid1.last_error_time = 0;
	gVar.pid1.outputMode = CONF_DEF_PID1_OUT_MODE;
	gVar.pid1.output_d = 0;
	gVar.pid1.output_i = 0;
	gVar.pid1.output_p = 0;
	gVar.pid1.output_i_max = 0;
	gVar.pid1.output_pid = 0;

	gVar.pid2.enable = 0;
	gVar.pid2.setpoint = 0;
	gVar.pid2.maxSetpoint = 0;
	gVar.pid2.measurement = 0;
	gVar.pid2.fbSource = 0;
	gVar.pid2.hystValue = 0;
	gVar.pid2.kp = 0;
	gVar.pid2.ki = 0;
	gVar.pid2.kd = 0;
	gVar.pid2.error = 0;
	gVar.pid2.last_error = 0;
	gVar.pid2.last_error_time = 0;
	gVar.pid2.outputMode = CONF_DEF_PID2_OUT_MODE;
	gVar.pid2.output_d = 0;
	gVar.pid2.output_i = 0;
	gVar.pid2.output_p = 0;
	gVar.pid2.output_i_max = 0;
	gVar.pid2.output_pid = 0;


	timStamp.currentTime 	= 0;
	timStamp.ovfCount 		= 0;
	timStamp.timerType 		= TS_TIM_TYP_SYSTICK;
	timStamp.sec 			= 0;
	timStamp.us 			= 0;
	timStamp.ms 			= 0;


	/*LEDs-----------------*/
	for(uint8_t i = 0; i < LED_MAX; i++){
		gVar.led[i].id = i;
		gVar.led[i].state = 0;
	}
	gVar.led[LED_RS485_RX].pin = RS485_RX_LED_PIN;
	gVar.led[LED_RS485_TX].pin = RS485_TX_LED_PIN;
	gVar.led[LED_1].pin = LED_1_PIN;
	gVar.led[LED_1].pin = LED_2_PIN;

	/*IOExtender-----------------*/
	gVar.ioExt.SR.value  = 0;
	gVar.ioExt.CR.value  = 0;
	gVar.ioExt.PR.value  = 0;

}


/*Old------------*/

/*
 * updates the settings to the last stored value
 * */
void UpdateSettings(void){


	if(FSExt_Download() == 0){
		DBG_SPRINT_NL("Error: Flash read failed!");
	}

	for(uint16_t address = 0 ; address < SA_MAX; address++){
		uint32_t value = FSExt_ReadIntNum(address);
		switch (address) {
			case SA_MEM_FIRST_RUN_CHECK:{
					if(value <= 0U){
						if(FSExt_EraseFull()<=0){
							DBG_SPRINT_NL("Error: Flash erase failed!");
						}
					}

				}
				break;

				/*Digital Inputs--------------*/
			case SA_DI1_MODE:
				if(!(value == DI_MODE_NORMAL || value == DI_MODE_PULSE_CAPTURE)){
					gVar.dio.di[DI_1].config.mode = CONF_DEF_DI1_MODE;
				}else{
					gVar.dio.di[DI_1].config.mode = value;
				}
				break;
			case SA_DI1_TRIGGER_MODE:
				if(!(value == DI_TRIG_MODE_RISING || value == DI_TRIG_MODE_FALLING)){
					gVar.dio.di[DI_1].config.trigMode = CONF_DEF_DI1_TRIGGER_MODE;
				}else{
					gVar.dio.di[DI_1].config.trigMode = value;
				}
				break;
			case SA_DI1_SIGNAL_PERIOD_MIN:
				if(!(value >= 1 && value <= DEF_U16_MAX_VALUE)){
					gVar.dio.di[DI_1].pulseInf.periodMin = CONF_DEF_DI1_SIG_PERIOD_MAX;
				}else{
					gVar.dio.di[DI_1].pulseInf.periodMin  = value;
				}
				break;


			case SA_DI2_MODE:
				if(!(value == DI_MODE_NORMAL || value == DI_MODE_PULSE_CAPTURE)){
					gVar.dio.di[DI_2].config.mode = CONF_DEF_DI2_MODE;
				}else{
					gVar.dio.di[DI_2].config.mode = value;
				}
				break;
			case SA_DI2_TRIGGER_MODE:
				if(!(value == DI_TRIG_MODE_RISING || value == DI_TRIG_MODE_FALLING)){
					gVar.dio.di[DI_2].config.trigMode = CONF_DEF_DI2_TRIGGER_MODE;
				}else{
					gVar.dio.di[DI_2].config.trigMode = value;
				}
				break;
			case SA_DI2_SIGNAL_PERIOD_MIN:
				if(!(value >= 1 && value <= DEF_U16_MAX_VALUE)){
					gVar.dio.di[DI_2].pulseInf.periodMin = CONF_DEF_DI2_SIG_PERIOD_MAX;
				}else{
					gVar.dio.di[DI_2].pulseInf.periodMin  = value;
				}
				break;


				/*Analog Inputs-----------*/
			case SA_AI1_MODE:
				if(!AIO_IsValidMode(value)){
					gVar.ai[AI_1].mode = CONF_DEF_AI1_MODE;
				}else{
					gVar.ai[AI_1].mode = value;
				}
				break;
			case SA_AI2_MODE:
				if(!AIO_IsValidMode(value)){
					gVar.ai[AI_2].mode = CONF_DEF_AI2_MODE;
				}else{
					gVar.ai[AI_2].mode = value;
				}
				break;
//			case SA_AI3_MODE:
//				if(!AIO_IsValidMode(value)){
//					gVar.ai[AI_3].mode = CONF_DEF_AI3_MODE;
//				}else{
//					gVar.ai[AI_3].mode = value;
//				}
//				break;
//			case SA_AI4_MODE:
//				if(!AIO_IsValidMode(value)){
//					gVar.ai[AI_4].mode = CONF_DEF_AI4_MODE;
//				}else{
//					gVar.ai[AI_4].mode = value;
//				}
//				break;



				/*Analog outputs 1 to 4----------*/
				//Analog outputs 1
			case SA_AO1_ENABLE:
				if(!(value == 0 || value == 1)){
					gVar.ao[AO_1].enabled = CONF_DEF_AO1_ENABLE;
				}else{
					gVar.ao[AO_1].enabled = value>0;
				}
				break;
			case SA_AO1_MODE:
//				gVar.ao[AO_1].mode = (AIO_Mode)FSExt_ReadIntNum(address);
				if(!AIO_IsValidMode(value)){
					gVar.ao[AO_1].mode = CONF_DEF_AO1_MODE;
				}else{
					gVar.ao[AO_1].mode = value;
				}
				break;
			case SA_AO1_SRC:
//				gVar.ao[AO_1].source = (AO_InpSrc)FSExt_ReadIntNum(address);
				if(!AO_IsValidInpSrc(value)){
					gVar.ao[AO_1].source = CONF_DEF_AO1_SRC;
				}else{
					gVar.ao[AO_1].source = value;
				}
				break;
//			case SA_AO1_DUTY:
////				gVar.ao[AO_1].mode = (AIO_Mode)FSExt_ReadIntNum(address);
//				if(!(value >= 0 && value <= AO_PWM_RESOLUTION)){
//					gVar.ao[AO_1].value = 0;
//				}else{
//					gVar.ao[AO_1].value = value;
//				}
//				break;

				//Analog outputs 2
			case SA_AO2_ENABLE:
				if(!(value == 0 || value == 1)){
					gVar.ao[AO_2].enabled = CONF_DEF_AO2_ENABLE;
				}else{
					gVar.ao[AO_2].enabled = value>0;
				}
				break;
			case SA_AO2_MODE:
//				gVar.ao[AO_2].mode = (AIO_Mode)FSExt_ReadIntNum(address);
				if(!AIO_IsValidMode(value)){
					gVar.ao[AO_2].mode = CONF_DEF_AO2_MODE;
				}else{
					gVar.ao[AO_2].mode = value;
				}
				break;
			case SA_AO2_SRC:
				if(!AO_IsValidInpSrc(value)){
					gVar.ao[AO_2].source = CONF_DEF_AO1_SRC;
				}else{
					gVar.ao[AO_2].source = value;
				}
				break;
//			case SA_AO2_DUTY:
////				gVar.ao[AO_2].mode = (AIO_Mode)FSExt_ReadIntNum(address);
//				if(!(value >= 0 && value <= AO_PWM_RESOLUTION)){
//					gVar.ao[AO_2].value = 0;
//				}else{
//					gVar.ao[AO_2].value = value;
//				}
//				break;

				//Analog outputs 3
			case SA_AO3_ENABLE:
				if(!(value == 0 || value == 1)){
					gVar.ao[AO_3].enabled = CONF_DEF_AO3_ENABLE;
				}else{
					gVar.ao[AO_3].enabled = value>0;
				}
				break;
			case SA_AO3_MODE:
//				gVar.ao[AO_3].mode = (AIO_Mode)FSExt_ReadIntNum(address);
				if(!AIO_IsValidMode(value)){
					gVar.ao[AO_3].mode = CONF_DEF_AO3_MODE;
				}else{
					gVar.ao[AO_3].mode = value;
				}
				break;
//			case SA_AO3_SRC:
//				gVar.ao[AO_3].source = (AO_InpSrc)FSExt_ReadIntNum(address);
//				if(!AO_IsValidInpSrc(gVar.ao[AO_3].source)){
//					gVar.ao[AO_3].source = CONF_DEF_AO3_SRC;
//				}
//				break;
//			case SA_AO3_DUTY:
////				gVar.ao[AO_3].mode = (AIO_Mode)FSExt_ReadIntNum(address);
//				if(!(value >= 0 && value <= AO_PWM_RESOLUTION)){
//					gVar.ao[AO_3].value = 0;
//				}else{
//					gVar.ao[AO_3].value = value;
//				}
//				break;

				//Analog outputs 4
			case SA_AO4_ENABLE:
				if(!(value == 0 || value == 1)){
					gVar.ao[AO_4].enabled = CONF_DEF_AO4_ENABLE;
				}else{
					gVar.ao[AO_4].enabled = value>0;
				}
				break;
			case SA_AO4_MODE:
//				gVar.ao[AO_4].mode = (AIO_Mode)FSExt_ReadIntNum(address);
				if(!AIO_IsValidMode(value)){
					gVar.ao[AO_4].mode = CONF_DEF_AO4_MODE;
				}else{
					gVar.ao[AO_4].mode = value;
				}
				break;
//			case SA_AO4_SRC:
//				gVar.ao[AO_4].source = (AO_InpSrc)FSExt_ReadIntNum(address);
//				if(!AO_IsValidInpSrc(gVar.ao[AO_4].source)){
//					gVar.ao[AO_4].source = CONF_DEF_AO4_SRC;
//				}
//				break;
//			case SA_AO4_DUTY:
////				gVar.ao[AO_4].mode = (AIO_Mode)FSExt_ReadIntNum(address);
//				if(!(value >= 0 && value <= AO_PWM_RESOLUTION)){
//					gVar.ao[AO_4].value = 0;
//				}else{
//					gVar.ao[AO_4].value = value;
//				}
//				break;

				/*Net Information------------*/
				//Device MAC
			case SA_DEV_MAC_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 6;i++){

					 value = FSExt_ReadIntNum(address+i);

					if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
						uint8_t data[6] = CONF_DEF_NETINFO_MAC;
						gVar.w5500Chip.netInfo.mac[i] = data[i];
					}else{
						gVar.w5500Chip.netInfo.mac[i] = (uint8_t)value;// (uint8_t)FSExt_ReadIntNum(address+i);
					}
				}
				address = SA_DEV_MAC_6;
				break;
				//Device IP address
			case SA_DEV_IP_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 4;i++){
					value = FSExt_ReadIntNum(address+i);
					if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
						uint8_t data[4] = CONF_DEF_NETINFO_IP;
						gVar.w5500Chip.netInfo.ip[i] = data[i];
					}else{
						gVar.w5500Chip.netInfo.ip[i] = (uint8_t)value;
					}
				}
				address = SA_DEV_IP_4;
				break;
				//Device subnet mask
			case SA_DEV_SN_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 4;i++){
					value = FSExt_ReadIntNum(address+i);
					if(!(value >= 0 &&value <= DEF_U8_MAX_VALUE)){
						uint8_t data[4] = CONF_DEF_NETINFO_SN;
						gVar.w5500Chip.netInfo.sn[i] = data[i];
					}else{
						gVar.w5500Chip.netInfo.sn[i] = (uint8_t)value;
					}
				}
				address = SA_DEV_SN_4;
				break;
				//Device gateway
			case SA_DEV_GW_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 4;i++){
					value = FSExt_ReadIntNum(address+i);
					if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
						uint8_t data[4] = CONF_DEF_NETINFO_GW;
						gVar.w5500Chip.netInfo.gw[i] = data[i];
					}else{
						gVar.w5500Chip.netInfo.gw[i] = (uint8_t)value;
					}
				}
				address = SA_DEV_GW_4;
				break;
//
			case SA_DEV_DNS_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 4;i++){
					value = FSExt_ReadIntNum(address+i);
					if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
						uint8_t data[4] = CONF_DEF_NETINFO_DNS;
						gVar.w5500Chip.netInfo.dns[i] = data[i];
					}else{
						gVar.w5500Chip.netInfo.dns[i] = (uint8_t)value;
					}
				}
				address = SA_DEV_DNS_4;
				break;

				/*NModbus TCP Server------------*/
				//socket - 1 & 2
			case SA_MB_TCPS_SOCK0_SRC_PORT:
//				gVar.server.sock[SOCKET_0].kpAlvTime = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value >= 0 && value <= DEF_U16_MAX_VALUE)){
					gVar.server.sock[0].srcPort = CONF_DEF_MB_TCPS_SRC_PORT;
				}else{
					gVar.server.sock[0].srcPort = (uint16_t)value;
				}
				break;
			case SA_MB_TCPS_SOCK0_KPALV_TIM:
//				gVar.server.sock[SOCKET_0].kpAlvTime = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
					gVar.server.sock[0].kpAlvTime = CONF_DEF_MB_TCP_KPALVTR;
				}else{
					gVar.server.sock[0].kpAlvTime = (uint16_t)value;
				}
				break;
//			case SA_MB_TCPS_SOCK1_KPALV_TIM:
////				gVar.server.sock[SOCKET_1].kpAlvTime = (uint8_t)FSExt_ReadIntNum(address);
//				if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
//					gVar.server.sock[SOCKET_1].kpAlvTime = CONF_DEF_MB_TCP_KPALVTR;
//				}else{
//					gVar.server.sock[SOCKET_1].kpAlvTime = (uint16_t)value;
//				}
//				break;

				/*NModbus TCP Client------------*/
			case SA_MB_TCPC_DEST_IP_1:		// read all mac byte from the memory
				for( uint8_t i = 0;i < 4;i++){
					value = FSExt_ReadIntNum(address+i);
					if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
						uint8_t data[4] = CONF_DEF_MB_TCPC_DEST_IP;
						gVar.client.sock[0].destIP[i] = data[i];
					}else{
						gVar.client.sock[0].destIP[i] = (uint8_t)value;
					}
				}
				address = SA_MB_TCPC_DEST_IP_4;
				break;
			case SA_MB_TCPC_DEST_PORT:
//				gVar.server.sock[SOCKET_0].kpAlvTime = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value >= 0 && value <= DEF_U16_MAX_VALUE)){
					gVar.client.sock[0].destPort = CONF_DEF_MB_TCPC_DEST_PORT;
				}else{
					gVar.client.sock[0].destPort = (uint16_t)value;
				}
				break;
			case SA_MB_TCPC_SOCK_KPALV_TIM:
//				gVar.server.sock[SOCKET_0].kpAlvTime = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value >= 0 && value <= DEF_U8_MAX_VALUE)){
					gVar.client.sock[0].kpAlvTime = CONF_DEF_MB_TCP_KPALVTR;
				}else{
					gVar.client.sock[0].kpAlvTime = (uint16_t)value;
				}
				break;
			case SA_MB_TCPC_SOCK_CONN_MODE:
				if(!(value == SOCK_CONN_MODE_AUTO || value == SOCK_CONN_MODE_MANUAL)){
					gVar.client.sock[0].connectMode = CONF_DEF_MB_TCPC_CONN_MODE;

				}else{
					gVar.client.sock[0].connectMode = value;
				}
				break;

			case SA_MB_TCPC_TX_TIM_INTRVAL:
				if(!(value > 0 && value <= CONF_DEF_MB_TCPC_TX_TIM_INTERVAL_MAX)){
					gVar.mbTCPClient.txTimIntrvl = CONF_DEF_MB_TCPC_TX_TIM_INTRVL;

				}else{
					gVar.mbTCPClient.txTimIntrvl = value;
				}
				break;

				/*NModbus RTU Serial------------*/
			case SA_MB_SERIAL_CONFIG:
				gVar.mbSerial.serialConfigValue = value;											// it will decode the encoded serial config value
				// if the any parameter is not valid, it will reset to default serial config value 0x1821 (ref: OD_Config.h)
				if(MBS_CheckSerialConfigValidity(&gVar.mbSerial) == MB_ERROR){
					gVar.mbSerial.serialConfigValue = CONF_DEF_MB_SERIAL_CONFIG;
				}
				break;
			case SA_MB_SERIAl_BAUD_RATE_USER_DEF:
				if(!(value > 0 && value < DEF_U32_MAX_VALUE)){
					MBS_SetBaudRateUserDef(CONF_DEF_MB_SERIAL_BR_USER);
				}else{
					MBS_SetBaudRateUserDef(value);
				}
				break;
			case SA_MB_SERIAl_PROTOCOL:
				if(!MBS_CheckProtocol(value)){
					gVar.mbSerial.protocol = CONF_DEF_MB_SERIAL_PROTOCOL;
				}else{
					gVar.mbSerial.protocol = value;
				}
				gVar.mbSerial.protocolTemp = gVar.mbSerial.protocol;
				break;

				/*Modbus Master---------------*/
			case SA_MB_MSTR_ENABLE:
				if(!(value == 0 || value ==1 )){
					gVar.mbRTUMaster.enable = CONF_DEF_MBRTUM_ENABLE;
				}else{
					gVar.mbRTUMaster.enable = value;
				}
				break;
			case SA_MB_MSTR_SLV_START_ADDR:
//				DBG_SPRINT_NL("SLSA: %d", value);
				if(!(value > 0 && value <= MB_SLAVE_ADDR_MAX )){
					gVar.mbRTUMaster.custom.slvStartAddr = CONF_DEF_MBRTUM_CUST_SLV_START_ADDR;
					FSExt_WriteIntNum(SA_MB_MSTR_SLV_START_ADDR, value);
				}else{
					gVar.mbRTUMaster.custom.slvStartAddr = value;
				}


				break;
			case SA_MB_MSTR_NUM_OF_SLAVE:
//				DBG_SPRINT_NL("NSA: %d", value);
				if(!(value >= 0 && value <= MB_CUST_NUM_OF_SLAVE_MAX )){
					gVar.mbRTUMaster.custom.numOfSlv = CONF_DEF_MBRTUM_CUST_NUM_OF_SLV;
					FSExt_WriteIntNum(SA_MB_MSTR_NUM_OF_SLAVE, value);
				}else{
					gVar.mbRTUMaster.custom.numOfSlv = value;
				}
				break;
			case SA_MB_MSTR_REPLY_TIMEOUT:
//				DBG_SPRINT_NL("RT: %d", value);
				if(!(value > 0  && value <= CONF_DEF_MBGW_TIMEOUT_MAX )){
					gVar.mbRTUMaster.replyTimer.timeout = CONF_DEF_MBRTUM_CUST_REPLAY_TIMEOUT;
					FSExt_WriteIntNum(SA_MB_MSTR_REPLY_TIMEOUT, value);
				}else{
					gVar.mbRTUMaster.replyTimer.timeout = value;
				}
				break;
			case SA_MB_MSTR_PERIODIC_REQ_INTERVAL:
//				DBG_SPRINT_NL("PI: %d", value);
//				gVar.mbRTUSlave.slave_address = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value > 0  && value <= CONF_DEF_MBRTUM_PREIODIC_REQ_TIMEOUT_MAX )){
					gVar.mbRTUMaster.custom.reTransTimer.timeout = CONF_DEF_MBRTUM_CUST_PREIODIC_REQ_INTERVAL;
					FSExt_WriteIntNum(SA_MB_MSTR_PERIODIC_REQ_INTERVAL, value);
				}else{
					gVar.mbRTUMaster.custom.reTransTimer.timeout = value;
				}
				break;
				/*NModbus RTU Slave------------*/
			case SA_SLAVE_ADDRESS:
//				gVar.mbRTUSlave.slave_address = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value > MB_BROADCAST_ADDR && value <= MB_SLAVE_ADDR_MAX)){
					gVar.mbRTUSlave.slave_address = CONF_DEF_MBRTUS_SLAVE_ADDRESS;
				}else{
					gVar.mbRTUSlave.slave_address = value;
				}
				gVar.slvAddrTemp = gVar.mbRTUSlave.slave_address;
				break;
			case SA_MB_SLAVE_CUST_RESP_TIMEOUT:
//				gVar.mbRTUSlave.slave_address = (uint8_t)FSExt_ReadIntNum(address);
				if(!(value > 0 && value <= CONF_DEF_MBRTUS_CUST_RESP_TIMEOUT_MAX)){
					gVar.mbRTUSlave.custom.waitingTimer.timeout = CONF_DEF_MBRTUS_CUST_WAITING_TIMEOUT;
				}else{
					gVar.mbRTUSlave.custom.waitingTimer.timeout = value;
				}

				break;
				/*NModbus Gateway------------*/
			case SA_MB_GW_TIMEOUT:

				if(!(value > 0  && value <= CONF_DEF_MBGW_TIMEOUT_MAX )){
					gVar.mbGateway.rplyTimer.timeout = CONF_DEF_MBGW_TIMEOUT;

					gVar.mbGateway.rplyTimRglrFC.timeout = CONF_DEF_MBGW_TIMEOUT;
				}else{
					gVar.mbGateway.rplyTimer.timeout = value;
					gVar.mbGateway.rplyTimRglrFC.timeout = value;
				}
				break;

				/*Utilities----------------*/
			case SA_DEV_MODE:
				if(!(value == DEV_MODE_SLAVE || value == DEV_MODE_MASTER)){
					gVar.devMode = CONF_DEF_DEV_MODE;
				}else{
					gVar.devMode = value;
				}
				break;

				/*Encoder------------*/
			case SA_ENC_ENABLE:
				if(!(value >=0 && value <= 1)){
					gVar.dio.enc.enable = CONF_DEF_ENC_DISABLE;
				}else{
					gVar.dio.enc.enable = value;
				}
				break;
			case SA_ENC_MODE:
				if(!(value >=0 && value < ENC_MODE_MAX)){
					gVar.dio.enc.configs.mode = CONF_DEF_ENC_MODE;
				}else{
					gVar.dio.enc.configs.mode = value;
				}
				break;
			case SA_ENC_QUAD_TRIG_MODE:
				if(!(value == ENC_TRIG_MODE_RISING || value == ENC_TRIG_MODE_FALLING)){
					gVar.dio.enc.configs.quadrature.trigMode = CONF_DEF_ENC_QUAD_TRIG_MODE;
				}else{
					gVar.dio.enc.configs.quadrature.trigMode  = value;
				}
				break;
			case SA_ENC_CHA_TRIG_MODE:
				if(!(value == ENC_TRIG_MODE_RISING || value == ENC_TRIG_MODE_FALLING || value == ENC_TRIG_MODE_RISING_FALLING)){
					gVar.dio.enc.configs.channelA.trigMode = CONF_DEF_ENC_CHA_TRIG_MODE;
				}else{
					gVar.dio.enc.configs.channelA.trigMode  = value;
				}
				break;
			case SA_ENC_CHB_TRIG_MODE:
				if(!(value == ENC_TRIG_MODE_RISING || value == ENC_TRIG_MODE_FALLING || value == ENC_TRIG_MODE_RISING_FALLING)){
					gVar.dio.enc.configs.channelB.trigMode = CONF_DEF_ENC_CHB_TRIG_MODE;
				}else{
					gVar.dio.enc.configs.channelB.trigMode  = value;
				}
				break;
//			case SA_ENC_CHZ_TRIG_MODE:
//				break;
				/*PID-------------*/
			case SA_PID1_KP:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid1.kp = CONF_DEF_PID1_KP;
				}else{
					gVar.pid1.kp = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID1_KI:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid1.ki = CONF_DEF_PID1_KI;
				}else{
					gVar.pid1.ki = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID1_KD:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid1.kd = CONF_DEF_PID1_KD;
				}else{
					gVar.pid1.kd = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID1_I_OUT_LIMIT:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid1.output_i_max = CONF_DEF_PID1_I_OUT_MAX;
				}else{
					gVar.pid1.output_i_max = value;
				}
				break;

			case SA_PID2_KP:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid2.kp = CONF_DEF_PID2_KP;
				}else{
					gVar.pid2.kp = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID2_KI:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid2.ki = CONF_DEF_PID2_KI;
				}else{
					gVar.pid2.ki = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID2_KD:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid2.kd = CONF_DEF_PID2_KD;
				}else{
					gVar.pid2.kd = (float)(value/MB_VALUE_MULTIPLIER_100);
				}
				break;
			case SA_PID2_I_OUT_LIMIT:
				if(!(value >=0  && value <= DEF_U16_MAX_VALUE)){
					gVar.pid2.output_i_max = CONF_DEF_PID2_I_OUT_MAX;
				}else{
					gVar.pid2.output_i_max = value;
				}
				break;
			case SA_DBUG_ENABLE:
				if(!(value ==0  || value == 1)){
					DBG_DISABLE();
				}else{
					value==1 ? DBG_ENABLE() : DBG_DISABLE();
				}

				break;

			default:
				break;
		}
	}
}



void SetUpdateSettings(float *pVar, float *setValue, float *defValue, uint8_t condition){
	*pVar = condition ? *setValue :*defValue;
}



/*
 * Factory settings for required variables
 * */

void FactorySettings(void){

		/*Analog outputs 1 to 4----------*/
		FSExt_EraseFull();
}





