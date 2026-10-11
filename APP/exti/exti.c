/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "exti.h"
#include "key.h"
#include "led.h"
#include "systick.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
volatile uint8_t g_DisplayPage = 0;
static volatile uint32_t last_trigger_tick = 0;
volatile uint32_t g_SysTickCounter;

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  void My_EXTI_Init(void)
**
**  Descriptions: NVIC&My_EXTI_Init
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void My_EXTI_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;      	//EXTI 15 IRQChannel
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// IRQChannel Enable
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	// PreemptionPriority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 		//SubPriority
	NVIC_Init(&NVIC_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15; // IRQ / PIN
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                // IRQ En or DisEn
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;      // EXTI mode
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // EXTI Trigger mode
	EXTI_Init(&EXTI_InitStructure);
}
/*_________________________________________________________________________
**  void EXTI15_10_IRQHandler(void)
**
**  Descriptions: EXTI15_10_IRQHandler
**
**  Parameters:
**
**  Return: void
**  Note: EXTI_GetITStatus used to IRQ. EXTI_GetFlagStatus no.
**_________________________________________________________________________*/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line12) == 1)
	{
		delay_ms(10); // 消抖 
		if(KEY4 == 0) LED2 = 1;
	}
	else if(EXTI_GetITStatus(EXTI_Line13) == 1)
	{
		delay_ms(10);
		if(KEY3 == 0) LED2 = 0;
	}
	else if(EXTI_GetITStatus(EXTI_Line14) == 1)
	{
		delay_ms(10);
		if(KEY2 == 0) LED1 = 1;
	}
	else if(EXTI_GetITStatus(EXTI_Line15) == 1)
	{
		delay_ms(10);
		if(KEY1 == 0) LED1 = 0;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15);
}

/*_________________________________________________________________________
**  void Key_EXTI_Init(void)
**
**  Descriptions: 按鍵外部中斷初始化
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void Key_EXTI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 開啟 GPIOA 與 AFIO (複用功能) 時脈
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    // 2. 配置 PA15 為上拉輸入 (IPU)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 3. 將 EXTI Line15 映射至 GPIOA (PA15)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);

    // 4. 配置 EXTI0 線路：中斷模式、下降沿觸發
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 5. 配置 NVIC 巢狀向量中斷控制器
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; // 搶佔優先級
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        // 次優先級
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*_________________________________________________________________________
**  void EXTI15_IRQHandler(void)
**
**  Descriptions: PA15 外部中斷服務常式
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void EXTI15_IRQHandler(void) 
{
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        // 非阻塞防彈跳：距離前一次有效按下需間隔 > 200ms
        if ((g_SysTickCounter - last_trigger_tick) > 200)
        {
            last_trigger_tick = g_SysTickCounter;

            // 切換頁面 (0 <-> 1)
            g_DisplayPage = !g_DisplayPage;
        }

        // 必須清除 EXTI Pending 旗標，否則會無限重覆進入中斷
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}



