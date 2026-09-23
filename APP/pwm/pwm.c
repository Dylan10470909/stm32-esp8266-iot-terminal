/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "pwm.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  void TIM3_CH2_PWM_Init(u16 per, u16 psc)
**
**  Descriptions: TIM3_CH2_PWM_Init(ARR, f)
**
**  Parameters:  u16 per, u16 psc
**
**  Return: void
**  Note: 
**_________________________________________________________________________*/
void TIM3_CH2_PWM_Init(u16 per, u16 psc)
{
	// Structure
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// CLK_ENABLE
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	// GPIO_Init
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_7;  
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP;	 // Output Push-Pull
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;	  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	 
	
	// TIME_Init 初始化計時器參數，包含自動重裝值，分頻係數，計數方式等
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 外部訊號濾波與死區控制
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = per;     // 自動裝載值 決定定時器計數器（CNT）溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  // 預分頻係數 決定計數器（Counter）跑得多快
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);
	
	// 初始化PWM輸出參數，包含PWM模式、輸出極性，啟用等
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	// 保證更新過程中週期保持同步 若中途不會改變可以不用配置
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 啟用TIMx在CCRx上的預先裝載暫存器
	TIM_ARRPreloadConfig(TIM3, ENABLE); // 啟用TIMx在CCRx上的預載暫存器 

	// 開啟定時器
	TIM_Cmd(TIM3, ENABLE);
}
