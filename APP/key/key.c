#include "key.h"
#include "SysTick.h"

/*_________________________________________________________________________
**  void KEY_Init(void)
**
**  Descriptions: 按鍵初始化
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; // 定義結構	
	RCC_APB2PeriphClockCmd(KEY1_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(KEY2_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(KEY3_PORT_RCC,ENABLE);
	// RCC_APB2PeriphClockCmd(KEY4_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=KEY1_PIN;// 選擇要設置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;// 上拉輸入  
	GPIO_Init(KEY1_PORT,&GPIO_InitStructure);/* 初始化GPIO */
	
	GPIO_InitStructure.GPIO_Pin=KEY2_PIN;
	GPIO_Init(KEY2_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY3_PIN;
	GPIO_Init(KEY3_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=KEY4_PIN;
	GPIO_Init(KEY4_PORT,&GPIO_InitStructure);
}

/*_________________________________________________________________________
**  u8 KEY_Scan(u8 mode)
**
**  Descriptions: 按鍵掃描檢測
**
**  Parameters: mode=0:單次按下按鍵
**              mode=1：連續按下按鍵
**
**  Return: 
**  Note: 0：未有按鍵按下
**		  KEY1_PRESS：KEY1 鍵按下
**		  KEY2_PRESS：KEY2 鍵按下
**		  KEY3_PRESS：KEY3 鍵按下
**		  KEY4_PRESS：KEY4 鍵按下
**       
**_________________________________________________________________________*/
u8 KEY_Scan(u8 mode)
{
	static u8 key=1;
	
	if(mode==1) // 連續按鍵按下
		key=1;
	if(key==1 && (KEY1==0||KEY2==0||KEY3==0)) // 任意一个按鍵按下 ||KEY4==0
	{
		delay_ms(10);  // 消抖
		key=0;
		if(KEY1 == 0)
			return KEY1_PRESS; 
		else if(KEY2==0)
			return KEY2_PRESS; 
		else if(KEY3==0)
			return KEY3_PRESS; 
		else if(KEY4==0)
			return KEY4_PRESS; 
	}
	else if(KEY1==1 && KEY2==1 && KEY3==1)    // 無按鍵按下 &&KEY4==1
		key=1;
	return 0;
}

