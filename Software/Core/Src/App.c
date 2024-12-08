/*
 * App.C
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#include "App.h"
Gearbox_t gearbox;
uint32_t CANSendFrameTick = 0;

void AppInit(ADC_HandleTypeDef* hadc, FDCAN_HandleTypeDef* hfdcan)
{
	ADC_Init(hadc);
	CAN_Init(hfdcan);
	gearboxInit(&gearbox);
}

void AppHandle(void)
{
	while(1)
	{
		gearbox.gearPosADC = GetGearPosValue();
		gearbox.air_press = GetAirValue();
		updateGear(&gearbox);
		AutoshiftCheck();
		processCallback(&gearbox);
		if((HAL_GetTick() - CANSendFrameTick) > 2)
		{
			CANSendGMUFrames(&gearbox);
			CANSendFrameTick = HAL_GetTick();
		}
	}
}

void AutoshiftCheck(void)
{
	if(gearbox._autoshift_state == AutoshiftEnable)
	{

	}
}

