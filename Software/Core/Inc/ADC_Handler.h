/*
 * ADC_Handler.h
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#ifndef INC_ADC_HANDLER_H_
#define INC_ADC_HANDLER_H_

#include "main.h"

void ADC_Init(ADC_HandleTypeDef* hadc);
uint16_t GetGearPosValue(void);
uint16_t GetAirValue(void);

#endif /* INC_ADC_HANDLER_H_ */
