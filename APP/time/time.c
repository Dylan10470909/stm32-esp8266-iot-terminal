/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "time.h"
#include "led.h"
#include "systick.h"
/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
volatile u8 g_SampleFlag = 0;   // 定義實體，初始為0

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/

/*_________________________________________________________________________
**  void TIM4_Init(u16 per)
**
**  Descriptions: TIM4_Init
**
**  Parameters: u16 per, u16 psc
**
**  Return: 
**  Note: 計數器頻率 f_CNT = f_TIMx_CLK / (psc + 1)
**        定時器時間計算 T_out = per * (psc+1) / Tick(72M)
**_________________________________________________________________________*/
void TIM4_Init(u16 per, u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	
	// TIME_Init 初始化計時器參數，包含自動重裝值，分頻係數，計數方式等
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 外部訊號濾波與死區控制
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = per;     // 自動裝載值 決定定時器計數器（CNT）溢出
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  // 預分頻係數 決定計數器（Counter）跑得多快
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
	
	// TIM 中斷類型
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	// 清除中斷函數 (非必要,增加運行可靠性)
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	
	// NVIC_Init 設置定時器中斷優先級
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;      	//EXTI 15 IRQChannel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// IRQChannel Enable
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	// PreemptionPriority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 		//SubPriority
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4, ENABLE);
	// 開啟定時器
}


/*_________________________________________________________________________
**  void TIM4_IRQHandler(void)
**
**  Descriptions: TIM4_IRQHandler
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update)){
		g_SampleFlag = 1;  // 只設旗標，不做其他事
		LED1 = !LED1;
	}
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  
}


