/*
 * Debug.c
 *
 *  Created on: Mar 10, 2022
 *      Author: RASEL_EEE
 */

#include "Debug.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include <stdarg.h>
#include "Instances.h"

typedef struct DebugStruct_t{
	DBG_EnableState enable;
	DBG_EnableState enTimeStamp;
	uint16_t len;
	char str[DBG_STR_SIZE];
	void (*txCallback)(uint8_t* data, uint16_t len);
}DebugStruct_t;


void DBG_PRINT_TIMESTAMP(void);

extern UART_HandleTypeDef huart3;
UART_HandleTypeDef *huart;
uint16_t DBG_TransTimeout(int16_t len);
TimeStamp* ts;
DebugStruct_t dbg;

/**
  * @brief  Initializes Debug port
   * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *	               the configuration information for the specified UART module.
  * @retval HAL status
  */
void DBG_INIT(DBG_EnableState enable){
	dbg.enable = enable;
	dbg.enTimeStamp = DBG_DISBLED;
	dbg.txCallback = NULL;
	dbg.len = 0;
	memset(dbg.str, 0, sizeof(dbg.str));
	ts = INST_GetTimeStamp();
}


/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
void DBG_PRINT(const char *pData){
	if(dbg.enable == DBG_ENABLED){
		int len = strlen(pData);
		if(len > 0U){
			if(dbg.enTimeStamp){
//				DBG_PRINT_TIMESTAMP();
			}

			/*TX external print callback*/
			dbg.txCallback((uint8_t *)pData, len);
			/*TX internal print*/
//			HAL_UART_Transmit(&huart3, (uint8_t *)pData, len, DBG_TransTimeout(len));
			memset((char *)pData,0,len);
		}
	}
}

/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DBG_SPRINT(char *format, ...){
	if(dbg.enable != DBG_ENABLED) return 0;
	va_list aptr;
	int32_t ret;

	va_start(aptr, format);
	ret = vsprintf(dbg.str, format, aptr);
	va_end(aptr);
	if(ret>0){
		dbg.len = ret;
		DBG_PRINT(dbg.str);
	}

	return ret;
}

/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DBG_SPRINT_NL(char *format, ...){
	if(dbg.enable != DBG_ENABLED) return 0;
	va_list aptr;
	int len = 0;

	va_start(aptr, format);
	len = vsprintf(dbg.str, format, aptr);
	va_end(aptr);
	if(len>0){
//		TS_GetTime(&timStamp);
//		char temp[50];
//		sprintf(temp, "%lu.%03lu.%03lu : ", timStamp.sec, timStamp.ms, timStamp.us );

//		HAL_UART_Transmit(huart, (uint8_t *)temp, size, DBUG_TransTimeout(size));

//		dbg_PRINT(temp);
		dbg.str[len++] = '\r';
		dbg.str[len++] = '\n';
		DBG_PRINT(dbg.str);
		memset(format,0,len);
	}
	return len;
}



/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
int DBG_SPRINT_APPEND(char *pData, char *format, ...){
	if(dbg.enable != DBG_ENABLED) return 0;
	va_list aptr;
	int32_t ret;
	char buff[DBG_STR_SIZE];

	va_start(aptr, format);
	ret = vsprintf(buff, format, aptr);
	va_end(aptr);

	dbg.len = ret+strlen(pData);
	if(ret>0 &&  (dbg.len<= DBG_STR_SIZE)){
		strcat((char *)pData,buff );
	}
	return (ret>0 ? dbg.len : ret);
	//	dbg_PRINT();
}


/**
  * @brief  Prints string
  * @param  pData Pointer to data buffer (uint8_t or uint16_t data elements).
  * @retval HAL status
  */
void DBG_PRINT_INT(const char *str_h, uint32_t number, const char *str_t){
	if(dbg.enable != DBG_ENABLED) return;
	DBG_PRINT(str_h);
	char str[4];
	sprintf(str,"%ld",number);
	DBG_PRINT(str);
	DBG_PRINT(str_t);
}

/**
  * @brief  Prints floating point number
  * @param  f_number floting point number.
  * @param  precision number of precisions after point
  * @retval HAL status
  */
void DBG_PRINT_FLOAT(const char *str_h, float f_number,  const char *str_t){
	if(dbg.enable != DBG_ENABLED) return;
	DBG_PRINT(str_h);
	char str[6];
	sprintf(str,"%0.3f",f_number);
	DBG_PRINT(str);
	DBG_PRINT(str_t);
}

/**
  * @brief  read form the debug port
  * @param  f_number floting point number.
  * @param  precision number of precisions after point
  * @retval HAL status
  */
void DBG_READ(uint8_t *buff){
	//TODO: Debug read - to implement later
}

/**
  * @brief enable debug print in serial terminal
  * @param none
  * @retval None
  */
void DBG_ENABLE(void){
	dbg.enable = DBG_ENABLED;
}

/**
  * @brief disable debug print in serial terminal
  * @param none
  * @retval None
  */
void DBG_DISABLE(void){
	dbg.enable = DBG_DISBLED;
}

/**
  * @brief enable timestamp print in serial terminal
  * @param none
  * @retval None
  */


void DBG_PRINT_TIMESTAMP(void){
	TS_GetTime(ts);
	char temp[32];
	uint8_t size = sprintf(temp, "%lu.%03lu.%03lu : ", ts->sec, ts->ms, ts->us );
//				HAL_UART_Transmit(&huart3, (uint8_t *)pData, len,len);
	dbg.txCallback((uint8_t *)temp, size);
}
void DBG_ENABLE_TIMESTAMP(void){
	dbg.enTimeStamp = DBG_ENABLED;
}

/**
  * @brief disable timestamp print in serial terminal
  * @param none
  * @retval None
  */
void DBG_DISABLE_TIMESTAMP(void){
	dbg.enTimeStamp = DBG_DISBLED;
}

/**
  * @brief check debug is enabled or not
  * @param none
  * @retval None
  */
uint8_t DBG_IS_ENABLED(void){
	return dbg.enable;
}

/**
  * @brief attach tx callback function
  * @param none
  * @retval None
  */
void DBG_AttachTxCallback( void (*callback)(uint8_t* data, uint16_t len)){
	dbg.txCallback = callback;
}

/**
  * @brief to get the transmission rate according to the number of byte to be sent
  * @param none
  * @retval return timeout in ms
  */
uint16_t DBG_TransTimeout(int16_t len){
	return ceil(len*0.086806f);		// for 115200 baud rate byte transmission rate is 1.05 ms (aprox.)
}
