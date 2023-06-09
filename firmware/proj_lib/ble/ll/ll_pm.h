/********************************************************************************************************
 * @file	ll_pm.h
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
#if (!WIN32)
#include "proj/mcu/config.h"
#endif
#if(__TL_LIB_8258__ || (MCU_CORE_TYPE == MCU_CORE_8258))
#include "stack/ble/ll/ll_pm.h"
#elif(MCU_CORE_TYPE == MCU_CORE_8278)
#include "stack/ble_8278/ll/ll_pm.h"
#else

#ifndef LL_PM_H_
#define LL_PM_H_



////////////////// Power Management ///////////////////////
#define			SUSPEND_DISABLE			0
#define			SUSPEND_ADV				BIT(0)
#define			SUSPEND_CONN			BIT(1)
#define			MCU_STALL				BIT(6)





typedef struct {
	u8		suspend_mask;
	u8		advMcuStall_en;
	u16		wakeup_src;


	u32 	appWakeup_tick;

	u8		appWakeup_en;
	u8		appWakeup_flg;
	u8		latency_off;
	u8		no_latency;

	u16		sys_latency;
	u16 	user_latency;
	u16		valid_latency;
	u16 	latency_use;

	u8		conn_no_suspend;
	u8 		timer_wakeup;
	u8      ble_busy_pending;

}st_ll_pm_t;



typedef 	void (*ll_module_pm_callback_t)(void);


typedef 	void (*pm_appWakeupLowPower_callback_t)(int);




/******************************* User Interface  ************************************/
void 		blc_ll_initPowerManagement_module(void);


void		bls_pm_setSuspendMask (u8 mask);
u8 			bls_pm_getSuspendMask (void);
void 		bls_pm_setWakeupSource(u16 source);
u32 		bls_pm_getSystemWakeupTick(void);

void 		bls_pm_setManualLatency(u16 latency); //manual set latency to save power
void 		bls_pm_enableAdvMcuStall(u8 en);

void 		bls_pm_setAppWakeupLowPower(u32 wakeup_tick, u8 enable);
void 		bls_pm_registerAppWakeupLowPowerCb(pm_appWakeupLowPower_callback_t cb);

/************************* Stack Interface, user can not use!!! ***************************/




#endif /* LL_PM_H_ */
#endif
