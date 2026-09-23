/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include <string.h>
#include <stdio.h>  
#include "stm32f10x.h"
#include "system.h"
#include "systick.h" //[cite: 4]
#include "usart.h"  // 用於印出偵錯訊息到電腦[PA 9 10]
// #include "esp8266.h"
#include "time.h"
#include "led.h"
#include "dht11_T.h"

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
int main(void)
{
	u8 temp = 0;
	u8 humi = 0;
	u8 result;

	// 1. 系統時脈 / SysTick 初始化（用你 systick.c 裡的邏輯，72MHz舉例）
	SysTick_Init(72);

	// 2. USART1 初始化，讓 printf 能透過 USART1 輸出
	USART1_Init(115200);   // 或你原本設定的鮑率

	TIM4_Init(19999, 7199);
	printf("\r\n=== System Start ===\r\n");

	// 3. DHT11 初始化
	if (DHT11_Init() == 0)
	{
		printf("DHT11 Init OK\r\n");
	}
	else
	{
		printf("DHT11 Init FAILED - check wiring!\r\n");
	}

	while (1)
	{
		if (g_SampleFlag)
		{
			g_SampleFlag = 0;   // 先清掉，避免重複觸發

			result = DHT11_Read_Data(&temp, &humi);
			if (result == 0)
				printf("Temp: %d C, Humi: %d %%\r\n", temp, humi);
			else
				printf("DHT11 Read FAILED\r\n");
		}

	}
}
 
