/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "state.h"
#include "dht11.h"
#include "time.h"
#include <stdio.h>
/************************************************************************
*                             M A C R O S
*************************************************************************
*/ 

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
static ENUM_SystemState g_CurState = STATE_IDLE;

// 目前最新一筆感測器資料，SENDING 狀態會用到
static u8 g_LastTemp = 0;
static u8 g_LastHumi = 0;
static u8 g_HasNewData = 0;   // 有沒有還沒送出去的新資料 0:yes 1:no

u8 temp_threshold = 40;
u8 humi_threshold = 80;
/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/

// 假的WiFi連線函式，先固定回傳成功
static u8 ESP8266_ConnectWiFi_Stub(void)
{
	printf("[STATE] (stub) Connecting WiFi...\r\n");
	return 1;   // 1=成功, 0=失敗
}

// 假的送資料函式，先固定回傳成功
static u8 ESP8266_SendData_Stub(u8 temp, u8 humi)
{
	printf("[STATE] (stub) Sending Temp=%d Humi=%d ...\r\n", temp, humi);
	return 1;
}

// 假的連線狀態檢查，先固定回傳「還連著」
static u8 ESP8266_IsConnected_Stub(void)
{
	return 1;
}

/* ---------- 狀態機主體 ---------- */

void State_Init(void)
{
	g_CurState = STATE_IDLE;
	printf("[STATE] Init -> STATE_IDLE\r\n");
}

void State_Run(void)
{
	switch (g_CurState)
	{
		case STATE_IDLE:
			if (g_HasNewData) // 有沒有還沒送出去的新資料  0:yes 1:no
			{
				printf("[STATE] IDLE -> WIFI_CONNECTING\r\n");
				g_CurState = STATE_WIFI_CONNECTING;
			}
			break;

		case STATE_WIFI_CONNECTING:
			if (ESP8266_ConnectWiFi_Stub())
			{
				printf("[STATE] WIFI_CONNECTING -> WIFI_CONNECTED\r\n");
				g_CurState = STATE_WIFI_CONNECTED;
			}
			else
			{
				printf("[STATE] WIFI_CONNECTING -> ERROR_RECONNECT\r\n");
				g_CurState = STATE_ERROR_RECONNECT;
			}
			break;

		case STATE_WIFI_CONNECTED:
			printf("[STATE] WIFI_CONNECTED -> SENDING\r\n");
			g_CurState = STATE_SENDING;
			break;

		case STATE_SENDING:
			if (ESP8266_SendData_Stub(g_LastTemp, g_LastHumi))
			{
				g_HasNewData = 0;   // 這筆資料送完了
				printf("[STATE] SENDING -> IDLE (success)\r\n");
				g_CurState = STATE_IDLE;
			}
			else
			{
				printf("[STATE] SENDING -> ERROR_RECONNECT (send failed)\r\n");
				g_CurState = STATE_ERROR_RECONNECT;
			}
			break;

		case STATE_ERROR_RECONNECT:
			printf("[STATE] ERROR_RECONNECT -> WIFI_CONNECTING (retry)\r\n");
			g_CurState = STATE_WIFI_CONNECTING;
			break;

		default:
			g_CurState = STATE_IDLE;
			break;
	}
}

// 給main呼叫，把新的感測器資料塞進來
void State_NewSensorData(u8 temp, u8 humi)
{
	g_LastTemp = temp;
	g_LastHumi = humi;
	g_HasNewData = 1;
}

bool waring(u8 temp, u8 humi){
	printf("Temp Waring %%\r\n");
	return (temp > temp_threshold  || humi > humi_threshold);
}
