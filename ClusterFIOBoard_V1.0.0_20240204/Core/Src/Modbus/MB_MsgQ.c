#include <MB_MsgQ.h>
#include "stdio.h"
#include <string.h>

/**
  * @brief  initialize the msg queue
  * @param  que: Pointer to msg queue
  * @retval none
  */
void MBMQ_init(MBMQ_Queue *que){
	MBMQ_clear(que);
}

/**
  * @brief  enque the msg queue
  * @param  que: Pointer to msg queue
  * @retval error status, 1 = no error, 0 = error
  */
uint8_t MBMQ_enque(MBMQ_Queue *que, MBMQ_Msg *msg){
  if (que->noOfMsg < MBMQ_QUEUE_SIZE){
    if(que->endIndex >= MBMQ_QUEUE_SIZE){
      que->endIndex = 0U;
    }
    que->msg[que->endIndex++] = *msg;
    que->noOfMsg++;
    return 1U;
  }else{
    return 0U;
  }
}

/**
  * @brief  deque the msg queue, it will remove the msg
  * @param  que: Pointer to msg queue
  * @retval error status, 1 = no error, 0 = error
  */
uint8_t MBMQ_deque(MBMQ_Queue *que, MBMQ_Msg *msg){
  if (que->noOfMsg > 0U){
    *msg = que->msg[que->startIndex]; // set the msg to ref. variable

    /*Now cleare the queue*/

    memset(que->msg[que->startIndex].buff, 0U, MBMQ_BUFF_SIZE);
    que->msg[que->startIndex].id = 0U;
//    que->msg[que->startIndex].keyIP = 0U;
    que->msg[que->startIndex].key = 0U;
    que->msg[que->startIndex].size = 0U;
    que->startIndex = (que->startIndex+1) % MBMQ_QUEUE_SIZE;
    que->noOfMsg--;
    return 1U;
  }else{
    return 0U;
  }
}


///**
//  * @brief  set the msg to queue
//  * @param  que: Pointer to msg queue
//  * @retval error status, 1 = no error, 0 = error
//  */
//uint8_t MBMQ_SetQueMsg(MBMQ_Queue *que, MBMQ_Msg *msg){
//  if (que->noOfMsg < MBMQ_QUEUE_SIZE){
//    if(que->endIndex >= MBMQ_QUEUE_SIZE){
//      que->endIndex = 0U;
//    }
//    que->msg[que->endIndex++] = *msg;
//    que->noOfMsg++;
//    return 1U;
//  }else{
//    return 0U;
//  }
//}

/**
  * @brief  get the first msg without removing the msg
  * @param  que: Pointer to msg queue
  * @retval error status, 1 = no error, 0 = error
  */
uint8_t MBMQ_GetQueMsg(MBMQ_Queue *que, MBMQ_Msg *msg){
  if (que->noOfMsg > 0U){
    *msg = que->msg[que->startIndex]; // set the msg to ref. variable
    return 1U;
  }else{
    return 0U;
  }
}


/**
  * @brief  delete the first msg without removing the msg
  * @param  que: Pointer to msg queue
  * @retval error status, 1 = no error, 0 = error
  */
uint8_t MBMQ_DeleteQueMsg(MBMQ_Queue *que){
  if (que->noOfMsg > 0U){
	    /*Now cleare the queue*/
	    memset(que->msg[que->startIndex].buff, 0U, MBMQ_BUFF_SIZE);
	    que->msg[que->startIndex].id = 0U;
	//    que->msg[que->startIndex].keyIP = 0U;
	    que->msg[que->startIndex].key = 0U;
	    que->msg[que->startIndex].size = 0U;
	    que->startIndex = (que->startIndex+1) % MBMQ_QUEUE_SIZE;
	    que->noOfMsg--;

    return 1U;
  }else{
    return 0U;
  }
}


//
//
///**
//  * @brief  get msg from the msg queue but will not remove the msg
//  * @param  que: Pointer to msg queue
//  * @retval error status, 1 = no error, 0 = error
//  */
//uint8_t MBMQ_getMsg(MBMQ_Queue *que, MBMQ_Msg *msg, uint32_t id){
//	for(uint8_t i = 0; i < que->noOfMsg; i++){
//		if(que->msg[i].id == id){
//			msg = &que->msg[i];
//			return 1U;
//		}
//	}
//	return 0U;
//}
//
//
///**
//  * @brief remove the msg from the que by id
//  * @param  que: Pointer to msg queue
//  * @retval error status, 1 = no error, 0 = error
//  */
//uint8_t MBMQ_removeMsg(MBMQ_Queue *que, uint32_t id){
//
//	return 0U;
//}




/**
  * @brief  clear the msg queue
  * @param  que: Pointer to msg queue
  * @retval none
  */
void MBMQ_clear(MBMQ_Queue *que){
	que->id = 0U;
	que->startIndex = 0U;
	que->endIndex = 0U;
	que->noOfMsg = 0U;
	for (uint16_t i = 0U; i < MBMQ_QUEUE_SIZE; i++){
		que->msg[i].id = 0U;
//		que->msg[i].keyIP = 0U;
		que->msg[i].key = 0U;
		que->msg[i].size = 0u;
		memset(que->msg[i].buff, 0U, MBMQ_BUFF_SIZE);
	}
}


/**
  * @brief  sets the msg params
  * @param  msg: Pointer to msg
  * @param  key: msg id or key
  * @param  buff: Pointer to the buffer
  * @param  size: number of bytes
  * @retval none
  */
uint8_t MBMQ_setMsgParams(MBMQ_Msg *msg,  uint32_t id,uint16_t key, uint8_t *buff, uint16_t size){
	if(size <= MBMQ_BUFF_SIZE){
		msg->id = id;
//		msg->keyIP = keyIP;
		msg->key = key;
		msg->size = size;
		memcpy(msg->buff, buff, size);
		return 1U;
	}
	return 0U;
}

/**
  * @brief  clear the msg
  */
void MBMQ_clearMsg(MBMQ_Msg *msg){
	msg->id = 0;
//	msg->keyIP = 0;
	msg->key = 0;
	msg->size = 0;
	memset(msg->buff, 0, MBMQ_BUFF_SIZE);
}
