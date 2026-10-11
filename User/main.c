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
#include "state.h"
#include "oled.h"
#include "beep.h"
#include "dht11.h"
#include "key.h"
#include "exti.h"

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
int main(void)
{
	u8 temp = 0;
	u8 humi = 0;
	u8 result;

	// 1. 系統時脈 / SysTick 初始化） 中斷優先級分組
	SysTick_Init(72);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// 2. USART1 初始化，讓 printf 能透過 USART1 輸出
	USART1_Init(115200);   // 或原本設定的鮑率

	State_Init();
	OLED_Init(); 
	Beep_Init();
	KEY_Init();
	My_EXTI_Init();
	
	OLED_Clear();
	OLED_ShowString(0, 0, (u8*)"System Ready", 16);
	OLED_Refresh_Gram();      // 記得refresh，不然畫面不會更新
	OLED_Clear();
	
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
			{
				if(g_DisplayPage ==0){
				    printf("Temp: %d C, Humi: %d %%\r\n", temp, humi);
	
				    OLED_Fill_rectangle(0, 0, 128, 16, 0);    // 擦掉第 1 行 最後一個參數 0 是「熄滅」非擦除，等於歸零
				    OLED_ShowString(0, 0, (u8*)"Temp:", 16);
                    OLED_ShowNum(60, 0, temp, 2, 16);
				    OLED_ShowString(90, 0, (u8*)"C", 16);
				
				    OLED_Fill_rectangle(0, 20, 128, 16, 0);   // 擦掉第 2 行
				    OLED_ShowString(0, 20, (u8*)"Humi:", 16);
			        OLED_ShowNum(60, 20, humi, 2, 16);
			        OLED_ShowString(90, 20, (u8*)"%", 16);
					
					OLED_Fill_rectangle(0, 40, 128, 16, 0);   // 擦掉第 3 行
			 
				    if(waring(temp, humi)) {
				    	Beep_Alarm(2,100);					    
				    	OLED_ShowString(0, 40, (u8*)"Waring!", 16);
			    	}
				
				    OLED_Refresh_Gram();   // 全部畫完，一次送出
			    	State_NewSensorData(temp, humi);   // 有新資料，交給狀態機
			    }
				else{
					// ===== 頁面 1：警報閾值設定資訊 =====
                    OLED_ShowString(0, 0, (u8*)"[THRESHOLD CFG]", 16);

                    OLED_ShowString(0, 20, (u8*)"Set Temp > ", 16);
                    OLED_ShowNum(70, 20, temp_threshold, 2, 16);
                    OLED_ShowString(100, 20, (u8*)"C", 16);

                    OLED_ShowString(0, 40, (u8*)"Set Humi > ", 16);
                    OLED_ShowNum(70, 40, humi_threshold, 2, 16);
                    OLED_ShowString(100, 40, (u8*)"%", 16);
				}
			}
			else
			{
			    printf("DHT11 Read FAILED\r\n");
				
				OLED_Clear();
				OLED_ShowString(0, 0, (u8*)"Sensor Error!", 16);
				OLED_Refresh_Gram();
			}
		}
		
		State_Run();   // 每個loop都跑一次狀態機
	}
}
 
