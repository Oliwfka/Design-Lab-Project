/*
 * CAN_Handler.h
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#ifndef INC_CAN_HANDLER_H_
#define INC_CAN_HANDLER_H_

#include "CAN_Lib.h"
#include "main.h"
#include "Gearbox.h"

void CAN_Init(FDCAN_HandleTypeDef* hfdcan);
void CANSendGMUFrames(Gearbox_t* gearbox);

#endif /* INC_CAN_HANDLER_H_ */
