/*
 * CAN_Handler.c
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#include "CAN_Handler.h"

can_frame_t gearup,geardown,clutchfr;
can_frame_t gmu1,gmu2;

extern Gearbox_t gearbox;

void CANRXHandler(can_frame_t* frame);

void CAN_Init(FDCAN_HandleTypeDef* hfdcan)
{
	CANInit(hfdcan);
	CANInitFrame(&gearup,hfdcan,0x0);
	CANInitFrame(&geardown,hfdcan,0x1);
	CANInitFrame(&clutchfr, hfdcan, 0x3);
	CANInitFrame(&gmu1, hfdcan, 0x80);
	CANInitFrame(&gmu2, hfdcan, 0x81);
	CANRegisterCallback(CANRXHandler);
}

void CANSendGMUFrames(Gearbox_t* gearbox)
{
	gmu1.data[0] = gearbox->actual_gear;
	gmu1.data[1] = 0;
	gmu1.data[2] = (uint8_t)((gearbox->gearPosADC >> 8) & 0x0F);
	gmu1.data[3] = (uint8_t)(gearbox->gearPosADC & 0xF0);
	gmu1.data[4] = gearbox->_state;
	gmu1.data[5] = (uint8_t)((gearbox->air_press >> 8) & 0x0F);
	gmu1.data[6] = (uint8_t)(gearbox->air_press & 0xF0);
	gmu1.data[7] = gearbox->gear_cut;

	CANSendFrame(&gmu1);
}
void CANRXHandler(can_frame_t* frame)
{
	switch(frame->RxHeader.Identifier)
	{
	case 0x0:
		if(frame->data[0] == 0xFF)
			startUpshift(&gearbox);
		break;
	case 0x1:
		if(frame->data[0] == 0xFF)
			startDownshift(&gearbox);
		break;
	case 0x2:
		if(frame->data[0] == 0xFF)
			clutch(true);
		if(frame->data[0] == 0x0)
			clutch(false);
	default:
		break;
	}
}
