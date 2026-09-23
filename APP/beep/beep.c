/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "beep.h"
#include "systick.h"

/************************************************************************
*                             M A C R O S
*************************************************************************
*/

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  Beep_Init(void)
**
**  Descriptions: Beep_Init
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BEEP_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=BEEP_PIN ;  
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  
	GPIO_Init(BEEP_PORT,&GPIO_InitStructure); 	 
	GPIO_ResetBits(BEEP_PORT,BEEP_PIN);  
}

/*_________________________________________________________________________
**  Beep_Alarm(u8 times)
**
**  Descriptions: Beep_Alarm
**
**  Parameters: (times, delay time)
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void Beep_Alarm(u8 times, u8 us)
{
	while(times--)
	{
	    BEEP = !BEEP;
		delay_us(us);
	}
}
