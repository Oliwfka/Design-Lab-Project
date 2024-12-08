/*
 * CAN_Lib.c
 *
 *  Created on: Dec 7, 2024
 *      Author: Admin
 */

#include "CAN_Lib.h"

static CANMessageHandler CANMessageCallback = NULL;

void CANInit(FDCAN_HandleTypeDef* hfdcan)
{
	FDCAN_FilterTypeDef sFilterConfig;
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x0;
	sFilterConfig.FilterID2 = 0x0;
	if(HAL_FDCAN_ConfigFilter(hfdcan, &sFilterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_FDCAN_Start(hfdcan) != HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	{
		Error_Handler();
	}
}

void CANInitFrame(can_frame_t* frame, FDCAN_HandleTypeDef* hfdcan, uint16_t id)
{
	frame->hfdcan = hfdcan;
	frame->TxHeader.Identifier = id;
	frame->TxHeader.IdType = FDCAN_STANDARD_ID;
	frame->TxHeader.TxFrameType = FDCAN_CLASSIC_CAN;
	frame->TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	frame->TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	frame->TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	frame->TxHeader.FDFormat = FDCAN_FD_CAN;
	frame->TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	frame->TxHeader.MessageMarker = 0x0;
}

void CANSendFrame(can_frame_t* frame)
{
/*	if (HAL_FDCAN_GetTxFifoFreeLevel(frame->hfdcan) == 0)
	{
	    Error_Handler();
	}*/
	HAL_FDCAN_AddMessageToTxFifoQ(frame->hfdcan, &(frame->TxHeader), frame->data);
/*	{
		Error_Handler();
	}*/
}

void CANRegisterCallback(CANMessageHandler handler)
{
	if(handler != NULL)
	{
		CANMessageCallback = handler;
	}
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
	can_frame_t frame;
	frame.hfdcan = hfdcan;
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &frame.RxHeader, frame.data) == HAL_OK)
    {
    	if(CANMessageCallback != NULL)
    	{
    		CANMessageCallback(&frame);
    	}
    	else
    	{
    	    Error_Handler();
    	}
    }
    else
    {
    	Error_Handler();
    }
    if (HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
    	Error_Handler();
    }
  }
}
