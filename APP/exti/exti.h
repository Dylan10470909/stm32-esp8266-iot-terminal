#ifndef _EXTI_H
#define _EXTI_H
/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"
 
/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
// 全域頁面標誌，供 main.c 讀取切換顯示
extern volatile uint8_t g_DisplayPage;

// 靜態變數記錄前一次有效觸發時間，用於軟體防彈跳（Debounce）
static volatile uint32_t last_trigger_tick;

// 引用 systick 計數器（若專案已有 GetTick 函式可直接調用）
extern volatile uint32_t g_SysTickCounter;

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void My_EXTI_Init(void);
void EXTI15_10_IRQHandler(void);
void Key_EXTI_Init(void);
void EXTI15_IRQHandler(void);

#endif
