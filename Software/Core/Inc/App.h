/*
 * App.h
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#ifndef APP_H_
#define APP_H_

#include "stm32h5xx_hal.h"
#include "ADC_Handler.h"
#include "Gearbox.h"

void AppInit(ADC_HandleTypeDef* hadc, FDCAN_HandleTypeDef* hfdcan);
void AppHandle(void);

#endif /* APP_H_ */
