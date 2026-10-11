#ifndef _STATE_H
#define _STATE_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"
#include <stdbool.h>
/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
typedef enum {
	STATE_IDLE,             // 剛開機/待命，等待有新資料要送
	STATE_WIFI_CONNECTING,  // 正在嘗試連WiFi
	STATE_WIFI_CONNECTED,   // WiFi已連上，等待送資料
	STATE_SENDING,          // 正在傳送資料到伺服器
	STATE_ERROR_RECONNECT,  // 發生錯誤，準備重新連線
} ENUM_SystemState;
 

extern u8 temp_threshold;
extern u8 humi_threshold;
/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void State_Init(void);
void State_Run(void);   // 放在 main loop 裡每次呼叫一次
bool waring(u8 temp, u8 humi);  // 溫溼度異常回報

#endif
