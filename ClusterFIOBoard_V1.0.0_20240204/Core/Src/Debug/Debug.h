/*
 * Debug.h
 *
 *  Created on: Mar 10, 2022
 *      Author: RASEL_EEE
 */

#ifndef OD_LIB_DEBUG_INC_DEBUG_H_
#define OD_LIB_DEBUG_INC_DEBUG_H_

#include "main.h"

#define DBG_NEW_LINE 			"\r\n"
#define DBG_STR_SIZE  		256

typedef enum DBG_EnableState{
	DBG_DISBLED = 0U,
	DBG_ENABLED,
}DBG_EnableState;



void DBG_INIT(DBG_EnableState enable);
void DBG_PRINT(const char *pData);
int DBG_SPRINT_APPEND( char *pData, char *format, ...);
int DBG_SPRINT(char *format, ...);
int DBG_SPRINT_NL(char *format, ...);
void DBG_PRINT_INT(const char *str_h, uint32_t number, const char *str_t);
void DBG_PRINT_FLOAT(const char *str_h, float f_number, const char *str_t);
void DBG_ENABLE(void);
void DBG_DISABLE(void);
uint8_t DBG_IS_ENABLED(void);
void DBG_ENABLE_TIMESTAMP(void);
void DBG_DISABLE_TIMESTAMP(void);

void DBG_AttachTxCallback( void (*callback)(uint8_t* data, uint16_t len));
#endif /* OD_LIB_DEBUG_INC_DEBUG_H_ */
