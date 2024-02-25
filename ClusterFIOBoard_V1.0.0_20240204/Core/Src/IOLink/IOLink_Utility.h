/*
 * IOL_Utility.h
 *
 *  Created on: Oct 26, 2023
 *      Author: wsrra
 */

#ifndef SRC_IOLINK_IOLINK_UTILITY_H_
#define SRC_IOLINK_IOLINK_UTILITY_H_
#include "IOLink_Define.h"
#include "stdint.h"


#ifndef IOL_ATTRIB_PACKED
#define IOL_ATTRIB_PACKED  			__attribute__((__packed__))
#endif
/* Getter and setter method to get & set 2 & 4 byte data to an array
 * */
#define IOL_GET2BYTES(arr, firstIndex)			((arr[firstIndex]<<8)|arr[firstIndex+1])
#define IOL_GET4BYTES(arr, firstIndex)			((arr[firstIndex]<<24)|(arr[firstIndex+1]<<16)|(arr[firstIndex+2]<<8)|arr[firstIndex+3])
#define IOL_SET2BYTES(arr, firstIndex, data)	do{ arr[firstIndex] = data>>8; 			\
													arr[firstIndex+1] = (uint8_t)data; 	\
												}while(0)
#define IOL_SET4BYTES(arr, firstIndex, data)	do{ arr[firstIndex] = data>>24;			\
													arr[firstIndex+1] = data>>16;		\
													arr[firstIndex+2] = data>>8;		\
													arr[firstIndex+3] = (uint8_t)data;	\
												}while(0)

/* to lock unlock the process
 * */
typedef enum IOL_ProcessLock{
  IOL_UNLOCKED = 0x00U,
  IOL_LOCKED   = 0x01U,
} IOL_ProcessLock;

#define IOL_BUSY		IOL_ERROR_BUSY
#define IOL_LOCK(__handle__)    do{ if((__handle__)->lock == IOL_LOCKED) return IOL_BUSY;	\
                                    else (__handle__)->lock = IOL_LOCKED;    \
                                  }while (0U)
#define IOL_UNLOCK(__handle__)    do{(__handle__)->lock = IOL_UNLOCKED;}while (0U)



/*Access bit field*/
typedef union IOL_BitAccess_t{
	struct {
		uint8_t b0:1, b1:1,	b2:1, b3:1,	b4:1, b5:1, b6:1, b7:1;
	}bits;
	uint8_t octet;
}IOL_BitAccess_t;

#endif /* SRC_IOLINK_IOLINK_UTILITY_H_ */
