/*
 * CAN_Lib.h
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#ifndef INC_CAN_LIB_H_
#define INC_CAN_LIB_H_

#include "main.h"

//typedef void (*CANMessageRXHandler)(void);

typedef struct
{
	uint8_t data[8];
	FDCAN_TxHeaderTypeDef   TxHeader;
	FDCAN_RxHeaderTypeDef   RxHeader;
	FDCAN_HandleTypeDef* hfdcan;
	//CANMessageHandler RXcallback;
}can_frame_t;

typedef void (*CANMessageHandler)(can_frame_t* frame);

void CANInit(FDCAN_HandleTypeDef* hfdcan);
void CANInitFrame(can_frame_t* frame, FDCAN_HandleTypeDef* hfdcan, uint16_t id);
void CANSendFrame(can_frame_t* frame);
void CANRegisterCallback(CANMessageHandler handler);

#endif /* INC_CAN_LIB_H_ */
