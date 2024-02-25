/*
 * Scheduler.c
 *
 *  Created on: Oct 7, 2022
 *      Author: wsrra
 */

#include "GlobalVars.h"
#include "Callbacks.h"
#include "Scheduler.h"
#include "main.h"
#include <stdio.h>
#include <math.h>
#include <Task_Handler.h>
#include "Settings.h"
#include "Debug.h"
#include "TimeStamp.h"
#include "Debug.h"
#include "Flash_StorageExt.h"

/*Old------------*/
#include <Callbacks.h>
#include "Scheduler.h"
#include "main.h"
#include <stdio.h>
#include <math.h>
#include <MB_MsgQ.h>
#include "Settings.h"
#include "Debug.h"
#include "Server.h"
#include "socket.h"
#include "Task_Handler.h"
#include "AnalogIO.h"
#include "ADC.h"
#include "MB_Register.h"
#include "Define.h"
#include "TimeStamp.h"
#include "Encoder.h"
#include "MB_UART.h"
#include <MB_Custom.h>
#include "w25Qxx_Interface.h"
#include "Delay_us.h"
/*Public variable*/
extern GlobalVar_t gVar;


/*Old-------------------------*/
//extern DebugStruct dbug;
//extern GlobalVar gVar;
 TimeStamp timStamp;

extern ADC_HandleTypeDef hadc1;

extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim9;
extern TIM_HandleTypeDef htim10;
extern TIM_HandleTypeDef htim24;

extern IWDG_HandleTypeDef hiwdg1;

extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

//TODO: to remove
extern uint32_t rtuRxCount;
extern uint32_t rtuTxCount;
extern uint32_t rtuErrCount;
#define RTU_ERR_MAX 256
extern uint32_t peTime[RTU_ERR_MAX];
extern uint32_t feTime[RTU_ERR_MAX];
extern uint32_t neTime[RTU_ERR_MAX];
extern uint32_t oreTime[RTU_ERR_MAX];

extern uint16_t peTimCount;
extern uint16_t feTimCount;
extern uint16_t neTimCount;
extern uint16_t oreTimCount;




/*Private variable*/



/*
 * Schedules
 * */
typedef enum{
	SCHEDULE_VERYFAST = 0U,
	SCHEDULE_FAST,
	SCHEDULE_MEDIUM,
	SCHEDULE_SLOW,
	SCHEDULE_VERYSLOW,
	SCHEDULE_MAX,
}OD_Schedules;

/*
 * non-blocking Schedule interval time using SysTick
 * */
typedef enum{
	INTERVAL_5MS 		= 5U,
	INTERVAL_20MS 		= 20U,
	INTERVAL_50MS 		= 50U,
	INTERVAL_100MS 		= 100U,
	INTERVAL_200MS 		= 200U,
	INTERVAL_250MS 		= 250U,
	INTERVAL_500MS 		= 500U,
	INTERVAL_1000MS		= 1000U,
	INTERVAL_1500MS		= 1500U,
	INTERVAL_2000MS 	= 2000U,
	INTERVAL_5000MS 	= 5000U,
	INTERVAL_10000MS	= 10000U,
}OD_ScheduleInterval;



uint32_t LastScheduleTime[SCHEDULE_MAX] = {0U};

static uint8_t TimeReached(uint8_t schedule, uint16_t interval);
static void SetLastScheduleTime(uint8_t schedule);
static void SupperLoop();
static void VeryFastLoop();
static void FastLoop();
static void MediumLoop();
static void SlowLoop();
static void VerySlowLoop();



void SchedulerInit(void){


	/*Debug init--------------------------*/
	DBG_INIT(DBG_ENABLED);

	DBG_AttachTxCallback(&CB_DBG_Tx);
//	DBG_SPRINT_NL("Starting...");



	/*Microsecond delay init--------------*/
	Delay_Init();

	/*Initial settings--------------------*/
	InitialSettings();

	/*Time stamp initialize--------*/
	TS_Init(&gVar.ts, TS_TIM_TYP_TIMER);
	TS_AttachTimer(&htim24, HAL_RCC_GetHCLKFreq());
	DBG_ENABLE_TIMESTAMP();

//	DBG_SPRINT_NL("%lu: %lu, %lu", gVar.ts.sec, gVar.ts.us, gVar.ts.us);


	/*Print Device Information-----------*/
	TH_PrintDevInfo();
	DBG_SPRINT_NL("Initializing...");

	/*Digital Output---------------------*/
	DBG_SPRINT("Digital Outputs: ");
	uint8_t count = 0;
	for(uint8_t i = DO_1; i < DO_MAX; i++){
		DO_Error err = DO_Init(&gVar.dio.dout[i], GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,
					   (i <= DO_4 ? GPIO_SPEED_FREQ_VERY_HIGH : GPIO_SPEED_FREQ_LOW));
		if(err != DO_ERR_NONE){
			DBG_SPRINT_NL("\r\nDO_%d: Failed! - Error(%d)",i, err);
			count++;
		}
	}
	if(count == 0){
		DBG_SPRINT_NL("Done!");
	}
	count = 0;

	/*Digital Inputs-----------------------*/
	DBG_SPRINT("Digital Inputs: ");

	DI_AttachTimer(&htim3);
	for(uint8_t i = DI_1; i < DI_MAX; i++){
		DI_Error err = DI_Init(&gVar.dio.di[i]);
		if(err != DI_ERR_NONE){
			DBG_SPRINT_NL("\r\nDI_%d: Failed! - Error(%d)",err);
			count++;
		}
	}
	if(count == 0){
		DBG_SPRINT_NL("Done!");
	}
	count = 0;


	/*PCA9557 IO Extender--------------------------------*/
	DBG_SPRINT("PCA9557 IO Extender: ");
	PCA_AttachI2C(&hi2c1);
	PCA_Error_te error = 0;
	error = PCA_Init(&gVar.ioExt, PCA_IOEX_I2C_DEV_ADDR);
	if(error != PCA_ERR_NONE){
		DBG_SPRINT_NL("Init Failed - Error(%d)", error);
		count++;
	}
	PCA_SET_OUTPUTMODE(&gVar.ioExt, PCA_PIN_ALL);
	PCA_SET_POLARITYNONE(&gVar.ioExt,  PCA_PIN_ALL);
	error = PCA_ConfigPin(&gVar.ioExt);
	if(error != PCA_ERR_NONE){
		DBG_SPRINT_NL("Config Failed - Error(%d)", error);
		count++;
	}
	if(count == 0){
		DBG_SPRINT_NL("Done!");
	}
	count = 0;

	/*Analog output ------------------*/
	DBG_SPRINT("Analog Outputs: ");
	AO_Init(&htim2);

	/*TODO: Comment*/
	AO_Enable(&gVar.ao[0]);
	AO_Enable(&gVar.ao[1]);
	AO_Enable(&gVar.ao[2]);
	AO_Enable(&gVar.ao[3]);
	DBG_SPRINT_NL("Done!");



//
	/*Analog Inputs ------------------*/
	DBG_SPRINT("Analog Inputs: ");
	AI_Init(&hadc1);
	AI_Config( &gVar.ai[AI_1], ADC_CHANNEL_3, AIO_MODE_CURRENT);
	AI_Config( &gVar.ai[AI_2], ADC_CHANNEL_5, AIO_MODE_CURRENT);
#ifdef USE_AI_ADC_DMA
	AI_StartReadingDMA(gVar.aiBuffDMA, AI_MAX);
#endif
	DBG_SPRINT_NL("Done!");



	/*Initialize the external flash memory(W25Q64)--------*/
	DBG_SPRINT("W25Q64 Flash Memory: ");
	if(FSExt_Init(&hspi1, FSEXT_DATATYPE_WORD) == 0){
		DBG_SPRINT_NL("Failed!");
	}else{
		DBG_SPRINT_NL("Done!");
	}





	/*Callback init--------------------*/
	CB_Init();

	/*Reset watchdog-------------------*/
	HAL_IWDG_Refresh(&hiwdg1);

	/*Update setting-------------------*/
	UpdateSettings();

	DBG_ENABLE();//TODO: comment

	/*Modbus RTU Serial-----------------------*/
	DBG_SPRINT("Modbus RTU UART: ");
	gVar.devMode = DEV_MODE_SLAVE;
//	gVar.mbSerial.protocol = MBS_PROTO_RTU_CLUSTER_DMA;

	/*Set timer*/
//	if(gVar.mbSerial.protocol == MBS_PROTO_RTU_STD){
//		TS_SetTimerType(&gVar.ts, TS_TIM_TYP_SYSTICK);
//	}else{
//		TS_SetTimerType(&gVar.ts, TS_TIM_TYP_TIMER);
//		TS_StartTimer(&gVar.ts);
//	}

	/*Decode serial config value (baudrate,data,stop and parity bit)*/
	if(MBS_DecodeConfigValue(&gVar.mbSerial) == 0){
		DBG_SPRINT("Invalid config!");
		count++;
	}
	if(MB_UartInit(&huart2, USART2, gVar.mbSerial.baudRate) == 0){
		DBG_SPRINT("Init failed!");
		count++;
	}
	if(count == 0){
		DBG_SPRINT_NL("Done!");
	}
	count = 0;

	DBG_SPRINT("Modbus RTU Config: ");
	MBS_Init(&gVar.mbSerial, &huart2, &htim4, &htim6, RS485_DIR_GPIO_Port, RS485_DIR_Pin);		// initialize modbus serial//TODO: to uncomment
	if(MBS_Config(&gVar.mbSerial) != MB_OK){								// to configure the modbus serial and 3.5 and 1.5 char timer
		DBG_SPRINT_NL("failed!");
	}else{
		DBG_SPRINT_NL("Done!");
	}
//	HAL_TIM_Base_Start_IT(&htim4);			// timer for 1.5 char interval for modbus serial
//	HAL_TIM_Base_Start_IT(&htim6);			// timer for 3.5 char interval for modbus serial

	MB_CUST_StartTimer(&gVar.mbRTUMaster.custom.reTransTimer);
	gVar.mbSerial.mode = (gVar.devMode == DEV_MODE_MASTER ? MBS_MODE_MASTER : MBS_MODE_SLAVE);
	//TODO:remove later
	MB_SetRTUMode((gVar.devMode == DEV_MODE_SLAVE ? MB_RTU_MODE_SLAVE : MB_RTU_MODE_MASTER));		// set the rtu mode
	gVar.mbHandle = MB_GetHandlerInstance();

	DBG_SPRINT("Modbus RTU DMA: ");
	if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA){		//
		if(MB_UartInitDMA(&hdma_usart2_rx, &hdma_usart2_tx) == 0){
			DBG_SPRINT("Init failed!");
			count++;
		}else{
			if(MB_UartReceiveDMA(gVar.mbSerial.rxBuff, MB_PDU_MAX_SIZE) != HAL_OK){
				DBG_SPRINT("RX idle line config failed!");
				count++;
			}
		}
	}
	if(count == 0){
		DBG_SPRINT_NL("Done!");
	}
	count = 0;

//	DBG_SPRINT_NL("%lu: %lu, %lu", gVar.ts.sec, gVar.ts.us, gVar.ts.us);


//	TaskInit();
//
////	DBG_DISABLE();



//	/*Old------------*/
//	//Callbacks-----------
//	CB_InitErrorCallback();
//	CB_Sock_InitCallback();
//	CB_MBTCPS_InitCallback();
//
////	MB_REG_Init();
//
//
//
//
//
////	dbug.enable = 1;
//	MBTCPS_Init(&gVar.mbTCPServer);
//
//	//w5500 initialize and configuration
////	W5500_Init(&gVar.w5500Chip); 	// initialize w5500 chip
//
//
//
//
//
//
////	DBG_SPRINT_NL("Vrsn: %d,  RTR: %d, RCC: %d", getVERSIONR(), getRTR(), getRCR());
////	if(W5500_SetNetInfo(&gVar.w5500Chip.netInfo) != 0){
////		sprintf(dbug.str,"\r\n%s: %s %s %s  %s  %s",STR_ERROR, STR_SERVER, STR_NET, STR_INFO, STR_NOT, STR_CONFIGURED);
////		DBG_SPRINT_NL(dbug.str);
////	}
////	  HAL_IWDG_Refresh(&hiwdg);
////	W5500_SetNetInfo(&gVar.w5500Chip.netInfo);
////	  W5500_PrintNetInfo();
//
//
//
////	ServerInit(&gVar.server);
//
//	// set the tcp server net information
////	MBTCP_SetNetInfo(&gVar.mbTCPServer.netInfo,
////			gVar.w5500Chip.netInfo.ip,
////			gVar.w5500Chip.netInfo.mac,
////			gVar.w5500Chip.netInfo.sn,
////			gVar.w5500Chip.netInfo.gw,
////			gVar.w5500Chip.netInfo.dns,
////			gVar.w5500Chip.netInfo.dhcp);
////	dbug.enable = 1;
////	/*Modbus RTU Slave---------------*/
////	  HAL_IWDG_Refresh(&hiwdg);
//
//
//
////	HAL_GPIO_DeInit(gVar.dio.di[DI_1].config.port, gVar.dio.di[DI_1].config.pin);
////	HAL_GPIO_DeInit(gVar.dio.di[DI_2].config.port, gVar.dio.di[DI_2].config.pin);
////	HAL_TIM_Encoder_DeInit(&htim2);
//////
////	gVar.dio.di[DI_1].config.mode = DI_MODE_NORMAL;
////	gVar.dio.di[DI_1].config.counterType = DI_COUNT_TYPE_TIMER;
////	gVar.dio.di[DI_1].config.trigMode = DI_TRIG_MODE_RISING;
////	gVar.dio.di[DI_1].config.channel = TIM_CHANNEL_1;
//////	gVar.dio.di[DI_1].pulseInf.periodMax = 1000;
//////
//////
////	gVar.dio.di[DI_2].config.mode = DI_MODE_NORMAL;
////	gVar.dio.di[DI_2].config.counterType = DI_COUNT_TYPE_TIMER;
////	gVar.dio.di[DI_2].config.trigMode = DI_TRIG_MODE_RISING;
////	gVar.dio.di[DI_2].config.channel = TIM_CHANNEL_2;
////	gVar.dio.di[DI_2].pulseInf.periodMax = 1000;
//
////
////	DI_Config(&gVar.dio.di[DI_1]);
////	DI_Config(&gVar.dio.di[DI_2]);
//
//
////
////    HAL_TIM_Base_Start_IT(&htim2);
////    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
////    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
//
//
//
////
////	gVar.dio.enc.enable = 1;
////	gVar.dio.enc.configs.mode = ENC_MODE_INCRE_CHAB;
////
////	//	gVar.dio.enc.isChanged = 1;
////	gVar.dio.enc.configs.quadrature.timer = &htim2;
////	gVar.dio.enc.configs.quadrature.trigMode = ENC_TRIG_MODE_RISING;
////
//////
////	gVar.dio.enc.configs.channelA.port = DI_1_ENC_A_GPIO_Port;
////	gVar.dio.enc.configs.channelA.pin = DI_1_ENC_A_Pin;
////	gVar.dio.enc.configs.channelA.intrpt = EXTI0_IRQn;
////	gVar.dio.enc.configs.channelA.trigMode = ENC_TRIG_MODE_RISING;
////	gVar.dio.enc.configs.channelA.countMode = ENC_COUNT_MODE_UP;
////
////	gVar.dio.enc.configs.channelB.port = DI_2_ENC_B_GPIO_Port;
////	gVar.dio.enc.configs.channelB.pin = DI_2_ENC_B_Pin;
////	gVar.dio.enc.configs.channelB.intrpt = EXTI1_IRQn;
////	gVar.dio.enc.configs.channelB.trigMode = ENC_TRIG_MODE_RISING;
////	gVar.dio.enc.configs.channelB.countMode = ENC_COUNT_MODE_UP;
////////
////	gVar.dio.enc.configs.channelZ.port = DI_3_ENC_Z_GPIO_Port;
////	gVar.dio.enc.configs.channelZ.pin = DI_3_ENC_Z_Pin;
////	gVar.dio.enc.configs.channelZ.intrpt = EXTI4_IRQn;
////	gVar.dio.enc.configs.channelZ.trigMode = ENC_TRIG_MODE_RISING;
////	gVar.dio.enc.configs.channelZ.countMode = ENC_COUNT_MODE_UP;
////	gVar.dio.enc.interval = ENC_SPEED_TIM_INTRVAL;
////
////
//////	ENC_Init(&gVar.dio.enc);
//////	ENC_Config(&gVar.dio.enc);
////	dbug.enable = 1;
////	TH_DigitalInConfig();
//
//
//	HAL_Delay(1000);
	DBG_DISABLE_TIMESTAMP();
}


/**
  * @brief Scheduler A is used for the non blocking task
  *
  */
void Scheduler(void){


	SupperLoop();

	if(TimeReached(SCHEDULE_VERYFAST, INTERVAL_5MS)){
		SetLastScheduleTime(SCHEDULE_VERYFAST);
		VeryFastLoop();
	}

	if(TimeReached(SCHEDULE_FAST, INTERVAL_100MS)){
		SetLastScheduleTime(SCHEDULE_FAST);
//		DBG_SPRINT_NL("FastLoop");

		FastLoop();
	}

	if(TimeReached(SCHEDULE_SLOW, INTERVAL_250MS)){
		SetLastScheduleTime(SCHEDULE_SLOW);
		MediumLoop();

	}

	if(TimeReached(SCHEDULE_MEDIUM, INTERVAL_1000MS)){
		SetLastScheduleTime(SCHEDULE_MEDIUM);
//		HAL_IWDG_Refresh(&hiwdg);
		SlowLoop();
	}

	if(TimeReached(SCHEDULE_VERYSLOW, INTERVAL_5000MS)){
		SetLastScheduleTime(SCHEDULE_VERYSLOW);
		VerySlowLoop();
	}

}



/**
  * @brief if timer reached to next schedule time it will allow to execute the fast loop
  * @param schedule: schedule task id
  * @param interval: interval between two schedule task
  * @retval if time reached it will return true otherwise false
  *
  */
static uint8_t TimeReached(uint8_t schedule, uint16_t interval){
	return (((HAL_GetTick()-LastScheduleTime[schedule]) >= interval) || ((int32_t)(HAL_GetTick() - LastScheduleTime[schedule]) < 0));
}


/**
  * @brief Sets the last time of the executed schedule task
  * @param schedule: schedule task id
  * @retval none
  */
static void SetLastScheduleTime(uint8_t schedule){
	LastScheduleTime[schedule] = HAL_GetTick();
}


/*executes in main loop*/
static void SupperLoop(){

	/*Digital Outputs----------------------------*/
	TH_DigitalOut();	//TODO: to uncomment
	TH_DigitalIn();

//	TaskHandler();


	/*Old---------------*/
	/*----------- Processes Modbus received packet -------------*/
	//Modbus TCP Server
//	Server_TCP(&gVar.server);
//	MBTCPS_ReqHandler(&gVar.mbTCPServer, &gVar.mbGateway);


	if(gVar.devMode == DEV_MODE_MASTER ){
		//MB RTU Master function
		MB_SetRTUMode(MB_RTU_MODE_MASTER);
		if(!gVar.mbGateway.isBusy ){		//&& gVar.mbTCPServer.isDataRcvd > 0//&& gVar.mbRTUMaster.custom.enableRetrans>0
			TH_MBRTUMasterTimoutHandler();		// check timeout for master
			TH_MBRTUMaster();

		}

		//MB gateway function
		if(!gVar.mbRTUMaster.isBusy){
			TH_MBGWCheckTimeOut();
			TH_MBGateway();

		}

//		TH_MBRTUMasterTimoutHandler();		// check timeout for master
//		TH_MBGWCheckTimeOut();				// check timeout for gateway
	}else{
		TH_MBRTUSlaveTimoutHandler();		// check timeout for slave
	}

//	TH_MBRTUBusErrDiag();
//	TH_W5500SPIErrDiag();



	/*Analog inputs-----------------*/
	TH_AnalogInput();

	/*Analog outputs-----------------*/
	TH_AnalogOutput();

	/*IOExtender*/
	TH_IOExtender();
}

static void VeryFastLoop(){

}

/*Old---------------*/
/*executes in every 50 ms */
void FastLoop(){

	/* TH_AOMap2DIandPID(), this function is used to map the analog output, pid & digital input
	 * Warning: This function has time dependency. So, be careful if you need to change the loop time
	 * for this function
	 * */
	TH_AOMap2DIandPID(&gVar.ao[AO_1], &gVar.dio.di[DI_1], &gVar.pid1);
	TH_AOMap2DIandPID(&gVar.ao[AO_2], &gVar.dio.di[DI_2], &gVar.pid2);
}

/*executes in every 250 ms */
void MediumLoop(){

	/*Digital Inputs----------------------*/
	/*COnfigs the digital Inputs & encoder inputs*/
	TH_DigitalInConfig();
	/*If there is no pulse detected by the digital inputs it clears the related parameters*/
	TH_DIClearOnNoPulse();


	/*Check the changes and update the non-volatile data*/
	FSExt_CheckAndUpdateChanges();

	/*Factory reset*/
	TH_FactoryResetButton();

	/*General purpose LED*/
//	HAL_GPIO_TogglePin(LED_1_GPIO_Port, LED_1_Pin);		// blink led// TODO: TO uncomment
}

/*executes in every 500 ms */
void SlowLoop(){

	/*IOExtender*/
	TH_IOExtender();

	/* refresh watchdog timer-----------*/
	HAL_IWDG_Refresh(&hiwdg1);


	/*Check the Modbus 107 packet retransmission timer is stop or not
	 * If it is stop, start it immediately
	 * */
	if(gVar.devMode == DEV_MODE_MASTER && !MB_CUST_IsTimerRunning(&gVar.mbRTUMaster.custom.reTransTimer)){
		MB_CUST_StartTimer(&gVar.mbRTUMaster.custom.reTransTimer);
	}


	if(gVar.mbSerial.protocol == MBS_PROTO_RTU_CLUSTER_DMA ){
		/*Check if the rtu bus is frozen due to unknown error-------------*/
		// If the serial receive option is locked, the it will unlocked after 500 ms
		if( gVar.mbSerial.isFreeze == 1){
			MB_UartReceiveDMA(gVar.mbSerial.rxBuff, MB_PDU_MAX_SIZE);
//			DBG_SPRINT_NL("MB_UartReceiveDMA.............");
			gVar.mbSerial.isFreeze = 2;
		}
		if(gVar.mbSerial.isFreeze == 0)	{gVar.mbSerial.isFreeze = 1;}
	}
}

/*executes in every 5000 ms */
void VerySlowLoop(){

	DBG_ENABLE_TIMESTAMP();
//	static uint8_t count = 0;
//
//	static uint32_t lastPeriod = 0;
//	if(gVar.dio.di[DI_1].pulseInf.period == lastPeriod && count == 20){
//		gVar.dio.di[DI_1].pulseInf.period = 0 ;
//		gVar.dio.di[DI_1].pulseInf.edgeTime = 0 ;
//		gVar.dio.di[DI_1].pulseInf.lastEdgeTime = 0 ;
//		lastPeriod = 0;
//		count++;
//	}else{
//		lastPeriod = gVar.dio.di[DI_1].pulseInf.period;
//	}
//if(count < 20)	count++;



//	static uint32_t tick = 0;
//	if((HAL_GetTick()-tick) > 1000){

	/*For the net configs-------*/
//	HAL_TIM_Base_Start_IT(&htim5);			// timer for 3.5 char interval for modbus serial

//	DBG_SPRINT_NL("Tim5P: %d", htim5.Init.Period);

//	DBG_PRINT("\r\nok");
//	DBG_SPRINT_NL("TCPC socCOn: %d", gVar.server.sock[gVar.server.sockIndex].isConnected );


//	TS_GetUS(&timStamp);
//	RCC_TypeDef rcc;
	static uint32_t rxLast = 0;
	if(rxLast == rtuRxCount){
		DBG_SPRINT_NL(" ");

//		__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST);
//		DBG_SPRINT_NL("Rst: %X", __HAL_RCC_GET_FLAG(RCC_CSR_LPWRRSTF | RCC_CSR_WDGRSTF | RCC_CSR_IWDGRSTF | RCC_CSR_SFTRSTF | RCC_CSR_PORRSTF | RCC_CSR_PINRSTF | RCC_CSR_BORRSTF));

//		rcc.CSR &= ~ RCC_CSR_RMVF;
//		__HAL_RCC_CLEAR_RESET_FLAGS();
		DBG_SPRINT_NL("Dev : { MOD: %d, Addr: %d }", gVar.devMode, (gVar.devMode == DEV_MODE_MASTER ? MBTCP_SERVER_UNIT_ID : gVar.mbRTUSlave.slave_address));
		DBG_SPRINT_NL("RTU: {RX:%u,TX:%u, Err:%u}, Reg: {SR:0x%08X, CR1:0x%08X, CR2:0x%08X, CR3: 0x%08X}",
				rtuRxCount, rtuTxCount, rtuErrCount,
//				0,0,0,0);
				huart2.Instance->ISR, huart2.Instance->CR1, huart2.Instance->CR2, huart2.Instance->CR3);
//		uint8_t m = (huart2.Instance->CR1 & USART_CR1_M) > 0;
//		uint8_t over8 =  (huart2.Instance->CR1 & USART_CR1_OVER8) > 0;
//		uint8_t oneBit = (huart2.Instance->CR3&USART_CR3_ONEBIT) > 0;
//		DBG_SPRINT_NL("UART: {CR1:{M:%d, OVER8:%d}, CR2:{OneBit:%d}, DWTCount: %u}",
//				m,over8,oneBit, DWT->CYCCNT);
//		DBG_SPRINT_NL("DIAG_ERR: { RTU: {PE: %u, FE: %u, NE: %u, ORE: %u, DTE: %u,}"
//								  "{ w5500: {SPI: 0x%04X, DMARX: 0x%04X, DMATX: 0x%04X,}"
//								  "{ EEPROM: {I2C: 0x%04X, DMARX: 0x%04X, DMATX: 0x%04X,}",
//				gVar.diag.rtuBus.peCount, gVar.diag.rtuBus.feCount,gVar.diag.rtuBus.neCount,gVar.diag.rtuBus.oreCount,gVar.diag.rtuBus.dteCount,
//				gVar.diag.w5500Spi.ErrCode, gVar.diag.w5500Spi.dmaRxErrCode,gVar.diag.w5500Spi.dmaTxErrCode,
//				gVar.diag.i2c.ErrCode, gVar.diag.i2c.dmaRxErrCode,gVar.diag.i2c.dmaTxErrCode);

//		DBG_SPRINT_NL("w55_Version: %d, %d",W5500_GetSR(),getVERSIONR());

//		DBG_SPRINT("\r\nRTU_PE_TIM: [");
//		for(uint16_t i = 0; i < peTimCount; i++){ DBG_SPRINT("%u,", peTime[i]); }
//		DBG_SPRINT("]");
//
//		DBG_SPRINT("\r\nRTU_NE_TIM: [");
//		for(uint16_t i = 0; i < neTimCount; i++){ DBG_SPRINT("%u,", neTime[i]); }
//		DBG_SPRINT("]");
//
//		DBG_SPRINT("\r\nRTU_FE_TIM: [");
//		for(uint16_t i = 0; i < feTimCount; i++){ DBG_SPRINT("%u,", feTime[i]); }
//		DBG_SPRINT("]");
//
//		DBG_SPRINT("\r\nRTU_ORE_TIM: [");
//		for(uint16_t i = 0; i < oreTimCount; i++){ DBG_SPRINT("%u,", oreTime[i]); }
//		DBG_SPRINT("]");


	//	DBG_SPRINT_NL("MBTCPC: TX_Int, %d", gVar.mbTCPClient.txTimIntrvl);
//
//	DBG_SPRINT_NL("Salve: { saOld: %d, saNew: %d }",
//			gVar.mbRTUSlave.slave_address, gVar.slvAddrTemp);
//

	DBG_SPRINT_NL("Serial: { Proto: %d, Config: 0x%04X, BR_Hal: %u "
			"BR_Set: %u, data: %d, parit: %d, stop: %ld, "
			"BRR: %04X, Timout: %ld  }",
			gVar.mbSerial.protocol, gVar.mbSerial.serialConfigValue, huart2.Init.BaudRate,
			gVar.mbSerial.baudRate, gVar.mbSerial.dataBits, gVar.mbSerial.parityBit,
			gVar.mbSerial.stopBit, huart2.Instance->BRR,gVar.mbSerial.respTimer.timeout);
////
	DBG_SPRINT_NL("MB Serial: { MD: %d, BSY: %d, TEN: %d, TOUT: %ld }",
			gVar.mbSerial.mode,gVar.mbSerial.isBusy, gVar.mbSerial.respTimer.isTimeout,
			gVar.mbSerial.respTimer.timeout);
	DBG_SPRINT_NL("MB Gateway: { MD: %d, BSY: %d, TEN: %d, TOUT: %ld ,rfcTEN: %d, rfcTOUT: %ld }",
			gVar.mbGateway.mode, gVar.mbGateway.isBusy, gVar.mbGateway.rplyTimer.enable, gVar.mbGateway.rplyTimer.timeout,
			gVar.mbGateway.rplyTimRglrFC.enable, gVar.mbGateway.rplyTimRglrFC.timeout);
	DBG_SPRINT_NL("MB Master: { EN: %d, BSY: %d, RTEN: %d, RTOUT: %d, PTEN: %d, RTOUT: %ld }",
			gVar.mbRTUMaster.enable, gVar.mbRTUMaster.isBusy,
			gVar.mbRTUMaster.replyTimer.enableTimer, gVar.mbRTUMaster.replyTimer.timeout,
			gVar.mbRTUMaster.custom.reTransTimer.enable, gVar.mbRTUMaster.custom.reTransTimer.timeout);
//
	DBG_SPRINT_NL("MB Master: { EN: %d, slvSA: %d, nSlv: %d, rplyTmout: %ld, reTxTimout: %ld }",
				gVar.mbRTUMaster.enable, gVar.mbRTUMaster.custom.slvStartAddr,
				gVar.mbRTUMaster.custom.numOfSlv,  gVar.mbRTUMaster.replyTimer.timeout, gVar.mbRTUMaster.custom.reTransTimer.timeout);

//
	DBG_SPRINT_NL("MB Slave: { SA: %d, Timout: %ld }",
			gVar.mbRTUSlave.slave_address, gVar.mbRTUSlave.custom.waitingTimer.timeout);



//
//
//		DBG_SPRINT_NL("SckS: { ID: %d, MOD: %d, isCn: %d }",
//				gVar.server.sock[0].id, gVar.server.sock[0].mode,
//				gVar.server.sock[0].isConnected);
//
//
//
//







//
//	DBG_SPRINT_APPEND(dbug.str,"%s %s: { %s: ",STR_DEV, STR_INFO,STR_MAC);
//	for (uint8_t i = 0; i < 6; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%02X", gVar.w5500Chip.netInfo.mac[i]);
//		if(i < 5)
//		DBG_SPRINT_APPEND(dbug.str,":");
//	}
////	DBG_SPRINT_APPEND(dbug.str,"}," );
//	DBG_SPRINT_APPEND(dbug.str,", %s: ",STR_IP);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.w5500Chip.netInfo.ip[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//
//	DBG_SPRINT_APPEND(dbug.str,":%d %s: ",gVar.server.sock[0].srcPort, STR_SN);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.w5500Chip.netInfo.sn[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//	DBG_SPRINT_APPEND(dbug.str,", %s: ",STR_GW);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.w5500Chip.netInfo.gw[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
////	DBG_SPRINT_NL(dbug.str);
//	DBG_SPRINT_APPEND(dbug.str,", DNS: ");
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.w5500Chip.netInfo.dns[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//	DBG_SPRINT_APPEND(dbug.str," }");
//	DBG_SPRINT_NL(dbug.str);
//
//	DBG_SPRINT_APPEND(dbug.str,"MB_TCPS: { Dest%s: ",STR_IP);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.server.sock[0].destIP[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//	DBG_SPRINT_APPEND(dbug.str,":%d }", gVar.server.sock[0].destPort);
//	DBG_SPRINT_NL(dbug.str);

//
//	DBG_SPRINT_APPEND(dbug.str,"MB_TCPC: { Dest%s: ",STR_IP);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.client.sock[0].destIP[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//	DBG_SPRINT_APPEND(dbug.str,":%d, srcPort: %d }", gVar.client.sock[0].destPort,  gVar.client.sock[0].srcPort);
//	DBG_SPRINT_NL(dbug.str);
//
//
//		DBG_SPRINT_NL("Client: { CONN: %d, COMD:: %d, stat:%d, kpalv: %d }",
//				gVar.client.sock[0].connect, gVar.client.sock[0].connectMode,
//				gVar.client.sock[0].status,  gVar.client.sock[0].kpAlvTime);


//	DBG_SPRINT_APPEND(dbug.str,"%s %sS: { Dest%s: ",STR_MB, STR_TCP,STR_IP);
//	for (uint8_t i = 0; i < 4; ++i) {
//		DBG_SPRINT_APPEND(dbug.str,"%d", gVar.server.sock[0].destIP[i]);
//		if(i < 3)
//		DBG_SPRINT_APPEND(dbug.str,".");
//	}
//	DBG_SPRINT_APPEND(dbug.str,", Dest%s: %d, Src%s: %d, %s: %d, kpAlv: %d }",
//			STR_PORT, gVar.server.sock[0].destPort, STR_PORT, gVar.server.sock[0].srcPort,
//			STR_STATUS, gVar.server.sock[0].status, gVar.server.sock[0].kpAlvTime);
//
//	DBG_SPRINT_NL(dbug.str);
//

//
	char str[256] = {0};

//	DBG_SPRINT_NL("PID_1: { MOD: %d, SP: %d, FB: %0.2f, Err: %0.2f, Kp: %0.2f, ki: %0.2f, kd: %0.2f, "
//			"p-out: %0.2f, i-out: %0.2f, d-out: %0.2f, out_i_Max: %0.2f, PID: %d }",
//	gVar.pid1.outputMode, gVar.pid1.setpoint, gVar.pid1.measurement, gVar.pid1.error,
//	gVar.pid1.kp,  gVar.pid1.ki, gVar.pid1.kd,
//	gVar.pid1.output_p, gVar.pid1.output_i, gVar.pid1.output_d,
//	gVar.pid1.output_i_max, gVar.pid1.output_pid);
//	DBG_SPRINT_NL("PID_2: { MOD: %d, SP: %d, FB: %0.2f, Err: %0.2f, Kp: %0.2f, ki: %0.2f, kd: %0.2f, "
//			"p-out: %0.2f, i-out: %0.2f, d-out: %0.2f, out_i_Max: %0.2f, PID: %d }",
//	gVar.pid2.outputMode, gVar.pid2.setpoint, gVar.pid2.measurement, gVar.pid2.error,
//	gVar.pid2.kp,  gVar.pid2.ki, gVar.pid2.kd,
//	gVar.pid2.output_p, gVar.pid2.output_i, gVar.pid2.output_d,
//	gVar.pid2.output_i_max, gVar.pid2.output_pid);
//////	DBG_SPRINT_NL(" period: %u", gVar.dio.di[DI_1].pulseInf.period);

	ENC_GetDirQuad(&gVar.dio.enc);
	DBG_SPRINT_APPEND(str,"ENC_1: { " );
	DBG_SPRINT_APPEND(str,"EN: %d, MOD: %d, QTrig: %d, ATrig: %d, BTrig: %d"
			,gVar.dio.enc.enable,gVar.dio.enc.configs.mode, gVar.dio.enc.configs.quadrature.trigMode,
			gVar.dio.enc.configs.channelA.trigMode,gVar.dio.enc.configs.channelB.trigMode);
	DBG_SPRINT_APPEND(str," }\r\n" );
	DBG_PRINT(str);

	ENC_GetDirQuad(&gVar.dio.enc);
	DBG_SPRINT_APPEND(str,"ENC_2: { " );
	DBG_SPRINT_APPEND(str,"CuntQ: %d, DIR: %d, CuntA: %d, CuntB: %d, CuntZ: %d",
			 gVar.dio.enc.counterQuad, gVar.dio.enc.direction,
			 gVar.dio.enc.counterA, gVar.dio.enc.counterB, gVar.dio.enc.counterZ);
	DBG_SPRINT_APPEND(str," }\r\n" );
	DBG_PRINT(str);

	DBG_SPRINT_APPEND(str,"DI_A: { " );
	for (uint8_t i = DI_1; i <= DI_2; ++i) {
		DBG_SPRINT_APPEND(str,"Ch-%d: {MOD: %d, STAT: %d, CTYP: %d, TMD: %d, pPRD: %ld, PRDMIN: %ld}, "
				,i,gVar.dio.di[i].config.mode, gVar.dio.di[i].state, gVar.dio.di[i].config.counterType, gVar.dio.di[i].config.trigMode,
				gVar.dio.di[i].pulseInf.period, gVar.dio.di[i].pulseInf.periodMin);
	}
	DBG_SPRINT_APPEND(str,"}\r\n" );
	DBG_PRINT(str);


	DBG_SPRINT_APPEND(str,"DI_B: { " );
	for (uint8_t i = DI_3; i <= DI_4; ++i) {
		DBG_SPRINT_APPEND(str,"Ch-%d: {MOD: %d, STAT: %d}, "
				,i,gVar.dio.di[i].config.mode, gVar.dio.di[i].state);
	}
//	DBG_SPRINT_APPEND(str,"}" );
//	DBG_PRINT(str);

//	DBG_SPRINT_APPEND(str,"\r\nDI_B: { " );
	for (uint8_t i = DI_5; i < DI_MAX; ++i) {
		DBG_SPRINT_APPEND(str,"Ch-%d: {MOD: %d, STAT: %d}, ",i,gVar.dio.di[i].config.mode, gVar.dio.di[i].state);
	}
	DBG_SPRINT_APPEND(str,"}\r\n" );
	DBG_PRINT(str);
//
//
//	DBG_SPRINT_APPEND(str,"\r\nDO_A: { " );
//	for (uint8_t i = DO_1; i <= DO_4; ++i) {
//		DBG_SPRINT_APPEND(str,"Ch-%d: {MOD: %d, SRC: %d, STAT: %d}, ",i,gVar.dio.dout[i].mode, gVar.dio.dout[i].source, gVar.dio.dout[i].state);
//	}
//	DBG_SPRINT_APPEND(str,"}" );
//	DBG_PRINT(str);
//	DBG_SPRINT_APPEND(str,"\r\nDO_B: { " );
//	for (uint8_t i = DO_5; i < DO_MAX; ++i) {
//		DBG_SPRINT_APPEND(str,"Ch-%d: {MOD: %d, SRC: %d, STAT: %d}, ",i,gVar.dio.dout[i].mode, gVar.dio.dout[i].source, gVar.dio.dout[i].state);
//	}
//	DBG_SPRINT_APPEND(str,"}" );
//	DBG_PRINT(str);

	for(uint8_t i = 0; i < AI_MAX; i++){
	DBG_SPRINT_NL("AI_%d: { MOD: %d, ADC: %d, V: %0.2fmV, I: %0.2fmA }",i+1,
			gVar.ai[i].mode, gVar.ai[i].rawValue, gVar.ai[i].voltage,  gVar.ai[i].current);
	}
//	DBG_SPRINT_NL("AI_2: { MOD: %d, ADC: %d, V: %0.2fmV, I: %0.2fmA }",
//			gVar.ai2.mode, gVar.ai2.rawValue, gVar.ai2.voltage,  gVar.ai2.current);
//	DBG_SPRINT_NL("AI_3: { MOD: %d, ADC: %d, V: %0.2fmV, I: %0.2fmA }",
//			gVar.ai3.mode, gVar.ai3.rawValue, gVar.ai3.voltage,  gVar.ai3.current);
//	DBG_SPRINT_NL("AI_4: { MOD: %d, ADC: %d, V: %0.2fmV, I: %0.2fmA }",
//			gVar.ai4.mode, gVar.ai4.rawValue, gVar.ai4.voltage,  gVar.ai4.current);

	DBG_SPRINT_NL("AO_1: { EN: %d, MOD: %d, SRC: %d, DUTY(12-bit): %d, DUTY: %d%% }",
			gVar.ao[AO_1].enabled, gVar.ao[AO_1].mode, gVar.ao[AO_1].source, gVar.ao[AO_1].value,  gVar.ao[AO_1].pwm.dutyCycle);
	DBG_SPRINT_NL("AO_2: { EN: %d, MOD: %d, SRC: %d, DUTY(12-bit): %d, DUTY: %d%% }",
			gVar.ao[AO_2].enabled, gVar.ao[AO_2].mode, gVar.ao[AO_2].source, gVar.ao[AO_2].value,  gVar.ao[AO_2].pwm.dutyCycle);
	DBG_SPRINT_NL("AO_3: { EN: %d, MOD: %d, SRC: %d, DUTY(12-bit): %d, DUTY: %d%% }",
			gVar.ao[AO_3].enabled, gVar.ao[AO_3].mode, gVar.ao[AO_3].source, gVar.ao[AO_3].value,  gVar.ao[AO_3].pwm.dutyCycle);
	DBG_SPRINT_NL("AO_4: { EN: %d, MOD: %d, SRC: %d, DUTY(12-bit): %d, DUTY: %d%% }",
			gVar.ao[AO_4].enabled, gVar.ao[AO_4].mode, gVar.ao[AO_4].source, gVar.ao[AO_4].value,  gVar.ao[AO_4].pwm.dutyCycle);




//	DBG_SPRINT_NL(" ");
	//	}

//	ADC_Init(&hadc1);
//	uint16_t adc[4];

//	DBG_SPRINT("\r\nADC { CH1: %d, V: %0.2f mV, I: %0.2f mA}",gVar.ai1.rawValue,gVar.ai1.voltage,gVar.ai1.current);
//	DBG_SPRINT("\r\nADC { CH2: %d, V: %0.2f mV, I: %0.2f mA}",gVar.ai2.rawValue,gVar.ai2.voltage,gVar.ai2.current);
//	DBG_SPRINT("\r\nADC { CH3: %d, V: %0.2f mV, I: %0.2f mA}",gVar.ai3.rawValue,gVar.ai3.voltage,gVar.ai3.current);
//	DBG_SPRINT("\r\nADC { CH4: %d, V: %0.2f mV, I: %0.2f mA}",gVar.ai4.rawValue,gVar.ai4.voltage,gVar.ai4.current);

//	for (uint8_t i = DI_1-1; i < DI_MAX; ++i) {
//		DBG_SPRINT("\r\nDI-%d: {MOD: %d, STAT: %d}",i+1,gVar.dio.di[i].mode,gVar.dio.di[i].state);
//	}
//
//
//	for (uint8_t i = DO_1; i < DO_MAX; ++i) {

//		DBG_SPRINT("\r\nDO-%d: { SRC: %d, MOD: %d, STAT: %d}",i+1,gVar.dio.do_[i].source,gVar.dio.do_[i].mode,gVar.dio.do_[i].state);
//	}

//	dbug.enable = 0;
	}
	rxLast = rtuRxCount;
	DBG_DISABLE_TIMESTAMP();
}







