#ifndef _MSGQ_H_
#define _MSGQ_H_

#include "stdint.h"
//#include "Define.h"
#include "MB_Config.h"

#define MBMQ_BUFF_SIZE           MBTCP_BUFF_SIZE
#define MBMQ_QUEUE_SIZE          MBTCPS_MAX_TRANS

/*
 * 1. add to queue 
 * 2. get from queue
 * 3. delete item
 * 4. clear queue
*/
typedef struct  MBMQ_Msg{
	uint32_t id;			// ip address is used as a 1st key
    uint16_t key;		// port is also used as a second key
    uint8_t buff[MBMQ_BUFF_SIZE];
    uint16_t size;
}MBMQ_Msg;

typedef struct MBMQ_Queue{
	uint32_t id;
	uint8_t startIndex;
	uint8_t endIndex;
	uint8_t noOfMsg;
	MBMQ_Msg msg[MBMQ_QUEUE_SIZE];
}MBMQ_Queue;

void MBMQ_init(MBMQ_Queue *que);
uint8_t MBMQ_enque(MBMQ_Queue *que, MBMQ_Msg *msg);
uint8_t MBMQ_deque(MBMQ_Queue *que, MBMQ_Msg *msg);
uint8_t MBMQ_GetQueMsg(MBMQ_Queue *que, MBMQ_Msg *msg);
uint8_t MBMQ_DeleteQueMsg(MBMQ_Queue *que);
//uint8_t MBMQ_getMsg(MBMQ_Queue *que, MBMQ_Msg *msg, uint32_t id);
//uint8_t MBMQ_removeMsg(MBMQ_Queue *que, uint32_t id);
void MBMQ_clear(MBMQ_Queue *que);
uint8_t MBMQ_setMsgParams(MBMQ_Msg *msg, uint32_t id, uint16_t key, uint8_t *buff, uint16_t size);
void MBMQ_clearMsg(MBMQ_Msg *msg);
#endif  /* _MSGQ_H_ */
