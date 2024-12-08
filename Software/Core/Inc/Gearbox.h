/*
 * Gearbox.h
 *
 *  Created on: Feb 18, 2024
 *      Author: Admin
 */

#ifndef GEARBOX_H_
#define GEARBOX_H_

#include <stdint.h>
#include <stdbool.h>

#define US_ACTUATOR_DELAY 20
#define US_FINISH_TIMEOUT 200
#define DS_ACTUATOR_DELAY 40
#define DS_FINISH_TIMEOUT 250
#define OL_US_GEARCUT_DEALAY 100
#define OL_DS_CLUTCH_DELAY 100

typedef enum
{
	Idle,
	StartUpshift,
	UpshiftFromNeutral,
	UpshiftGearCut,
	UpshiftFinish,
	StartDownshift,
	DownshiftToNeutral,
	DownshiftRevMatch,
	DownshiftFinish,
	OL_US_GearCut,
	OL_US_Finish,
	OL_DS_Clutch,
	OL_DS_Finish

}GearboxState_t;

typedef enum
{
	AutoshiftDisable,
	AutoshiftEnable

}AutoshiftState_t;

typedef struct
{
	uint16_t gearPosADC;
	uint16_t neutral_adc;
	uint16_t gear1_adc;
	uint16_t gear2_adc;
	uint16_t gear3_adc;
	uint16_t gear4_adc;

	GearboxState_t _state;
	AutoshiftState_t _autoshift_state;

	uint8_t  actual_gear;
	uint32_t shift_start_tick;
	uint32_t timestamp_tick;
	uint8_t start_gear;

	uint8_t gear_cut;

	uint32_t upshiftTime;
	uint32_t downshiftTime;
	uint16_t air_press;

}Gearbox_t;

	void gearboxInit(Gearbox_t* gearbox);

	void updateGearADC(Gearbox_t* gearbox, uint16_t ADCValue);
	void updateGear(Gearbox_t* gearbox);
	uint8_t getGear(Gearbox_t* gerbox);
	uint8_t getGearCut(Gearbox_t* gerbox);

	void processCallback(Gearbox_t* gearbox);
	void startUpshiftCallback(Gearbox_t* gearbox);
	void upshiftFromNeutral(Gearbox_t* gearbox);
	void upshiftGearCut(Gearbox_t* gearbox);
	void upshiftFinish(Gearbox_t* gearbox);

	void startDownshiftCallback(Gearbox_t* gearbox);
	void downshiftToNeutral(Gearbox_t* gearbox);
	void downshiftRevMatch(Gearbox_t* gearbox);
	void downshiftFinish(Gearbox_t* gearbox);

	void openLoop_upshift_gearCut(Gearbox_t* gearbox);
	void openLoop_upshift_finish(Gearbox_t* gearbox);

	void openLoop_downshift_clutch(Gearbox_t* gearbox);
	void openLoop_downshift_finish(Gearbox_t* gearbox);

	void startUpshift(Gearbox_t* gearbox);
	void startDownshift(Gearbox_t* gearbox);

	void gearshiftTime(Gearbox_t* gearbox);

//	void openLoopUpshift(void);
//	void openLoopDownshift(void);
//
//	void closedLoopUpshift(void);
//	void closedLoopDownshift(void);
	void clutch(bool ON);

	GearboxState_t getState(Gearbox_t* gearbox);
	bool isShiftinProgress(Gearbox_t*gearbox);


#endif /* INC_GEARBOX_H_ */
