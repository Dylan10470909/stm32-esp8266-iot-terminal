/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "systick.h"

/************************************************************************
*                          C O N S T A N T S
*************************************************************************
*/

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

/************************************************************************
*                       P R I V A T E   D A T A
*************************************************************************
*/
static u8 fac_us = 0;
static u16 fac_ms = 0;

/************************************************************************
*                             M A C R O S
*************************************************************************
*/

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/

/*_________________________________________________________________________
**  void SysTick_Init(u8 SYSTICK)
**
**  Descriptions: SysTick_CLKSourceConfig use to configures SYSCLK source.(1,1/8)
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void SysTick_Init(u8 SYSTICK)
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us = SYSTICK / 8;      // #counter for delay 1u sec. 72/8
	fac_ms = (u16)fac_us * 1000;    // 9000
}

/*_________________________________________________________________________
**  void delay_us(u32 number of us) 
**
**  Descriptions: 
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void delay_us(u32 nus) 
{
	u32 temp; // 多數為24位因此用32
	SysTick->LOAD = nus * fac_us;  //time dowmload
	SysTick->VAL = 0x00; //clearing counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // start counting (replace to[|=0x01] it's ok)
	do
	{
		temp = SysTick->CTRL; //用於判斷次數
	}while((temp & 0x01) && !(temp&(1<<16))); //判斷1.是否使能 2.最高位0 or 1(計數結束 因此條件取反) 
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //關閉計數器
	SysTick->VAL = 0x00;  //clearing counter
}
/*_________________________________________________________________________
**  void delay_ms(u32 number_of_ms) 
**
**  Descriptions: 
**
**  Parameters:
**
**  Return: void
**  Note: rig is 24 bits. The rang of Parameters(nms) is less than 1.864sec(nms*9000).
**        Delay 2 sec need call this f() two time.[delay_ms(1000); *2]
**        u32 nms 0xFFFFFF
**_________________________________________________________________________*/
void delay_ms(u16 nms) 
{
	u32 temp; // 多數為24位因此用32
	SysTick->LOAD = (u32)nms * fac_ms;  //time dowmload
	SysTick->VAL = 0x00; //clearing counter
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;  // start counting (replace to[|=0x01] it's ok)
	do
	{
		temp = SysTick->CTRL; //用於判斷次數
	}while((temp & 0x01) && !(temp&(1<<16))); //判斷1.是否使能 2.最高位 0 or 1(計數結束 因此條件取反) 
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  //關閉計數器
	SysTick->VAL = 0x00;  //clearing counter
}
