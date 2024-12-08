/*
 * Gearbox.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Admin
 */

#include "Gearbox.h"
#include "main.h"
#include "ADC_Handler.h"
#include "CAN_Handler.h"

static bool getTimeout(uint32_t start_tick, uint32_t timeout_tick);
GPIO_PinState clutch_state;
void gearboxInit(Gearbox_t* gearbox)
{
	gearbox->gearPosADC = 0;
	gearbox->neutral_adc = 800;
	gearbox->gear1_adc = 1300;
	gearbox->gear2_adc = 1700;
	gearbox->gear3_adc = 2150;
	gearbox->gear4_adc = 2550;

	gearbox->_state = Idle;
	gearbox->_autoshift_state = AutoshiftDisable;

	gearbox->actual_gear = 0xff;
	gearbox->shift_start_tick = 0;
	gearbox->timestamp_tick = 0;
	gearbox->start_gear = 0xff;

	gearbox->gear_cut = 127;

}

void updateGearADC(Gearbox_t* gearbox, uint16_t ADCValue)
{
	gearbox->gearPosADC = ADCValue;
}

void updateGear(Gearbox_t* gearbox)
{
	if((gearbox->gearPosADC>=(gearbox->neutral_adc-100))&&(gearbox->gearPosADC<=(gearbox->neutral_adc+100)))
	{
		gearbox->actual_gear = 0;
	}
	else if((gearbox->gearPosADC>=(gearbox->gear1_adc-100))&&(gearbox->gearPosADC<=(gearbox->gear1_adc+100)))
	{
		gearbox->actual_gear = 1;
	}
	else if((gearbox->gearPosADC>=(gearbox->gear2_adc-100))&&(gearbox->gearPosADC<=(gearbox->gear2_adc+100)))
	{
		gearbox->actual_gear = 2;
	}
	else if((gearbox->gearPosADC>=(gearbox->gear3_adc-100))&&(gearbox->gearPosADC<=(gearbox->gear3_adc+100)))
		{
			gearbox->actual_gear = 3;
		}
	else if((gearbox->gearPosADC>=(gearbox->gear4_adc-100))&&(gearbox->gearPosADC<=(gearbox->gear4_adc+100)))
		{
			gearbox->actual_gear = 4;
		}
	else
		{
			gearbox->actual_gear = 0xff;
		}

}

void processCallback(Gearbox_t* gearbox)
{
	switch(gearbox->_state)
	{
		case Idle:
		break;

		case StartUpshift:
			startUpshiftCallback(gearbox);
		break;

		case UpshiftFromNeutral:
			upshiftFromNeutral(gearbox);
		break;

		case UpshiftGearCut:
			upshiftGearCut(gearbox);
		break;

		case UpshiftFinish:
			upshiftFinish(gearbox);
		break;

		case StartDownshift:
			startDownshiftCallback(gearbox);
		break;

		case DownshiftToNeutral:
			downshiftToNeutral(gearbox);
		break;

		case DownshiftRevMatch:
			downshiftRevMatch(gearbox);
		break;

		case DownshiftFinish:
			downshiftFinish(gearbox);
		break;

		case OL_US_GearCut:
			openLoop_upshift_gearCut(gearbox);
		break;

		case OL_US_Finish:
			openLoop_upshift_finish(gearbox);
		break;

		case OL_DS_Clutch:
			openLoop_downshift_clutch(gearbox);
		break;

		case OL_DS_Finish:
			openLoop_downshift_finish(gearbox);
		break;

		default:
			gearbox->_state = Idle;
		break;

	}
}

void startUpshiftCallback(Gearbox_t* gearbox)
{
	gearbox->start_gear = gearbox->actual_gear;
	gearbox->shift_start_tick = HAL_GetTick();

	if(gearbox->start_gear == 4)
	{
		gearbox->_state = Idle;
		return;
	}
	HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_SET);
	gearbox->timestamp_tick = HAL_GetTick();

	if(gearbox->start_gear == 0)
	{
		gearbox->_state = UpshiftFromNeutral;
	}
	else
	{
		gearbox->_state = UpshiftGearCut;
	}

}
void upshiftFromNeutral(Gearbox_t* gearbox)
{
	if((gearbox->actual_gear == 1) || (getTimeout(gearbox->timestamp_tick, US_FINISH_TIMEOUT)))
	{
		HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_RESET);
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->_state = Idle;
	}
}

void upshiftGearCut(Gearbox_t* gearbox)
{
	if(getTimeout(gearbox->timestamp_tick, US_ACTUATOR_DELAY)||(gearbox->actual_gear == 0xff))
	{
		HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_SET);
		gearbox->gear_cut = 255;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->_state = UpshiftFinish;
	}
}

void upshiftFinish(Gearbox_t* gearbox)
{
	if((gearbox->actual_gear == gearbox->start_gear+1)||(getTimeout(gearbox->timestamp_tick, US_FINISH_TIMEOUT)))
	{
		HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_RESET);
		gearbox->gear_cut = 127;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->upshiftTime = (gearbox->timestamp_tick - gearbox->shift_start_tick);
		gearbox->_state = Idle;
	}
}

void startDownshiftCallback(Gearbox_t* gearbox)
{
	gearbox->shift_start_tick = HAL_GetTick();
	gearbox->start_gear = gearbox->actual_gear;

	if(gearbox->start_gear > 4)
	{
		HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_SET);
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->_state = OL_DS_Clutch;
		return;
	}
	else if(gearbox->start_gear == 0)
	{
		gearbox->_state = Idle;
		return;
	}

	HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_SET);
	gearbox->timestamp_tick = HAL_GetTick();

	if(gearbox->start_gear == 1)
	{
		gearbox->_state = DownshiftToNeutral;
	}
	else
	{
		gearbox->_state = DownshiftRevMatch;
	}
}

void downshiftToNeutral(Gearbox_t* gearbox)
{
	if(gearbox->actual_gear == 0 || getTimeout(gearbox->timestamp_tick, DS_FINISH_TIMEOUT))
	{
		HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_RESET);
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->downshiftTime = (gearbox->timestamp_tick - gearbox->shift_start_tick);
		gearbox->_state = Idle;
	}
}

void downshiftRevMatch(Gearbox_t* gearbox)
{
	if((getTimeout(gearbox->timestamp_tick, DS_ACTUATOR_DELAY) || (gearbox->actual_gear == 0xff)))
	{
		HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_SET);
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->gear_cut = 0;
		gearbox->_state = DownshiftFinish;
	}
}

void downshiftFinish(Gearbox_t* gearbox)
{
	if((gearbox->actual_gear == gearbox->start_gear -1 ) || (getTimeout(gearbox->timestamp_tick, DS_FINISH_TIMEOUT)))
	{
		HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_RESET);
		gearbox->gear_cut = 127;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->downshiftTime = (gearbox->timestamp_tick - gearbox->shift_start_tick);
		gearbox->_state = Idle;
	}
}

void openLoop_upshift_gearCut(Gearbox_t* gearbox)
{
	if(getTimeout(gearbox->timestamp_tick, OL_US_GEARCUT_DEALAY))
	{
		HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_SET);
		gearbox->gear_cut = 255;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->_state = OL_US_Finish;
	}
}

void openLoop_upshift_finish(Gearbox_t* gearbox)
{
	if(getTimeout(gearbox->timestamp_tick, US_FINISH_TIMEOUT))
	{
		HAL_GPIO_WritePin(GEAR_CUT_GPIO_Port, GEAR_CUT_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GEAR_UP_GPIO_Port, GEAR_UP_Pin, GPIO_PIN_RESET);
		gearbox->gear_cut = 127;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->upshiftTime = (gearbox->timestamp_tick - gearbox->shift_start_tick);
		gearbox->_state = Idle;
	}
}

void openLoop_downshift_clutch(Gearbox_t* gearbox)
{
	if(getTimeout(gearbox->timestamp_tick, DS_ACTUATOR_DELAY))
	{
		HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_SET);
		gearbox->gear_cut = 0;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->_state = OL_DS_Finish;
	}
}

void openLoop_downshift_finish(Gearbox_t* gearbox)
{
	if(getTimeout(gearbox->timestamp_tick, OL_DS_CLUTCH_DELAY))
	{
		HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GEAR_DOWN_GPIO_Port, GEAR_DOWN_Pin, GPIO_PIN_RESET);
		gearbox->gear_cut = 127;
		gearbox->timestamp_tick = HAL_GetTick();
		gearbox->downshiftTime = (gearbox->timestamp_tick - gearbox->shift_start_tick);
		gearbox->_state = Idle;
	}
}

void startUpshift(Gearbox_t* gearbox)
{
	if(gearbox->_state == Idle)
		gearbox->_state = StartUpshift;
}
void startDownshift(Gearbox_t* gearbox)
{
	if(gearbox->_state == Idle)
		gearbox->_state = StartDownshift;
}

void clutch(bool ON)
{
	HAL_GPIO_WritePin(CLUTCH_GPIO_Port, CLUTCH_Pin, ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
	clutch_state = HAL_GPIO_ReadPin(CLUTCH_GPIO_Port, CLUTCH_Pin);
}

bool isShiftinProgress(Gearbox_t*gearbox)
{
	if(gearbox->_state == Idle)
		return true;
	else
		return false;
}

static bool getTimeout(uint32_t start_tick, uint32_t timeout_tick)
{
	if((HAL_GetTick() - start_tick) >= timeout_tick)
	{
		return true;
	}
	else
		return false;
}

