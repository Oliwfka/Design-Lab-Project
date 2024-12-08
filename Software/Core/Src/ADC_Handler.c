/*
 * ADC_Handler.c
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#ifndef SRC_ADC_HANDLER_C_
#define SRC_ADC_HANDLER_C_

#include "ADC_Handler.h"
#include <stdint.h>

uint16_t ADC_Values[10][2];
uint32_t GearPostmp;
uint32_t Airtmp;

void ADC_Init(ADC_HandleTypeDef* hadc)
{
	HAL_ADCEx_Calibration_Start(hadc,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(hadc, (uint32_t*)ADC_Values, 20);
}
uint16_t GetGearPosValue(void)
{
	GearPostmp = 0;
	for(uint8_t i = 0; i<=9; i++)
	{
		GearPostmp += ADC_Values[i][0];
	}
	return GearPostmp/10;
}
uint16_t GetAirValue(void)
{
	Airtmp = 0;
	for(uint8_t i = 0; i<=9; i++)
	{
		Airtmp += ADC_Values[i][1];
	}
	return Airtmp/10;
}

#endif /* SRC_ADC_HANDLER_C_ */
