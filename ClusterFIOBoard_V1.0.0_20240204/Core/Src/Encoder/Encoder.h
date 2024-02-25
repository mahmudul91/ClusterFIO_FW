/*
 * Encoder.h
 *
 *  Created on: Jan 9, 2023
 *      Author: wsrra
 */


#ifndef SRC_ENCODER_ENCODER_H_
#define SRC_ENCODER_ENCODER_H_
#include "main.h"


#define ENC_INCRE_EXTI_PRIORITY 			1
#define ENC_SPEED_TIM_INTRVAL				100U			// in ms, time interval for calculating thr speed
#define ENC_STEADY_TIMEOUT					1000U			// in ms, if encoder is in steady condition, after 1000ms speed related value will be 0


#define ENC_QUAD_MAX_COUNT					0x7FFFFFFF		// int32_bit value
#define ENC_QUAD_MIN_COUNT					(0-ENC_QUAD_MAX_COUNT)
#define ENC_QUAD_MAX_TIM_PERIOD				(ENC_QUAD_MAX_COUNT|0x80000000)		// int32_bit value


/*
 * This library builds for Quadrature & Incremental encoder
 * */


/* Encoder Mode,
 * to select channel A & B or operation is needed, ex. 0x01 | 0x02 = 0x03
 *
 * */
typedef enum ENC_Mode{
	ENC_MODE_QUAD = 0x00,
	ENC_MODE_INCRE_CHA = 0x01,
	ENC_MODE_INCRE_CHB = 0x02,
	ENC_MODE_INCRE_CHAB = 0x03,
	ENC_MODE_MAX,
}ENC_Mode;


/*Encoder counter type (Maybe not to be implemented)
 * */
typedef enum ENC_CountType{
	ENC_COUNT_TYPE_1X = 0U,
	ENC_COUNT_TYPE_2X,
	ENC_COUNT_TYPE_4X,
}ENC_CountType;

typedef enum ENC_CountMode{
	ENC_COUNT_MODE_UP = 0U,
	ENC_COUNT_MODE_DOWN,
}ENC_CountMode;

typedef enum ENC_TrigMode{
	ENC_TRIG_MODE_RISING = 0U,
	ENC_TRIG_MODE_FALLING,
	ENC_TRIG_MODE_RISING_FALLING,
	ENC_TRIG_MAX,
}ENC_TrigMode;


/*Encoder direction for quadrature
 * */
typedef enum ENC_Direction{

	ENC_DIR_CW = 0,
	ENC_DIR_CCW,
}ENC_Direction;


/*Encoder quadrature configuration*/
typedef struct ENC_ConfigsQuad{
	TIM_HandleTypeDef *timer;
	TIM_Encoder_InitTypeDef timerConfig;
	ENC_TrigMode trigMode;
}ENC_ConfigsQuad;


/*Encoder incremental configuration*/
typedef struct ENC_ConfigIncre{
	GPIO_TypeDef *port;
	uint32_t pin;
	IRQn_Type intrpt;
	ENC_TrigMode trigMode;
	ENC_CountMode countMode;					// counter type, it can be 1x, 2x or 4x counter
}ENC_ConfigIncre;

typedef struct ENC_Configs{
	ENC_Mode mode;
	ENC_CountType countType;					// counter type, it can be 1x, 2x or 4x counter
	ENC_ConfigsQuad quadrature;
	ENC_ConfigIncre channelZ;
	// configuration for incremental encoder
	ENC_ConfigIncre channelA;
	ENC_ConfigIncre channelB;

}ENC_Configs;



/*
 * Encoder structure
 * 1. counter: main encoder counter, it have three counting mode, 1x, 2x, 4x
 *
 * */
typedef struct Encoder{
	uint8_t enable;
	ENC_Configs configs;
	uint8_t isChanged;
	int32_t counterQuad;
	uint32_t counterA;								// used as a quadrature counter and channel A for incremental counter
	uint32_t counterB;								// used as a counter for incremental encoder only, no use for quadrature encoderand channel B for incremental counter
	uint32_t counterZ;								// used as a quadrature home point counter
	int32_t countQuadAtRef;							// Quadrature counter value ref or home or chZ is triggerd
	uint32_t ppr;									// pulse per revolution of the encoder
	ENC_Direction direction;						// Rotational direction, CW, CCW
	uint32_t refPointCounter;						// ref or home counter
	uint8_t isRefPointDetected;						// ref or home detection flag
	//speed measuring parameter
	int32_t lastCountQuad;
	uint32_t lastCountA;							// last count value at home position for quadrature encoder
	uint32_t lastCountB;							//
	uint32_t lastCountZ;							// last count value at home position for quadrature encoder
	uint32_t lastTime;								// last count record time
	uint32_t interval;								// in ms, time interval within this interval the counter value to be recorded
	uint32_t count0;								// last count at a time interval, cha
	uint32_t count1;								// current count at time = interval time
	uint16_t retry;									// for very low rpm(frequency) counter value might no change, to over come this retry will be used
	float speed;									// Rotational speed in rps

}Encoder;


/*Common functions------------*/
void ENC_Start(Encoder *enc);
void ENC_Stop(Encoder *enc);
void ENC_Config(Encoder *enc);
void ENC_SetMode(Encoder *enc, uint8_t mode);
void ENC_NotifyOnChanges(Encoder *enc);


/*Quadrature encoder functions-----------*/
void ENC_GetDirQuad(Encoder *enc);
void ENC_ReadQuad(Encoder *enc);
void ENC_CalcSpeedQuad(Encoder *enc);
void ENC_ReadChZ(Encoder *enc);
void ENC_SetTrigModeQuad(ENC_ConfigsQuad quad, ENC_TrigMode mode);

/*Incremental encoder functions-----------*/
void ENC_ReadChA(Encoder *enc);
void ENC_ReadChB(Encoder *enc);
void ENC_SetTrigModeIncre(ENC_ConfigIncre *increCh, ENC_TrigMode mode);
void ENC_SetCountModeIncre(ENC_ConfigIncre *increCh, ENC_CountMode mode);

#endif /* SRC_ENCODER_ENCODER_H_ */
