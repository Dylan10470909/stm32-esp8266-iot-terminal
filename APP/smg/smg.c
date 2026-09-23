/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "smg.h"
#include "systick.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/

// CC SMG display 0~F
u8 gsmg_code[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
				0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/

/*_________________________________________________________________________
**  void SMG_Init(void)
**
**  Descriptions: LED_Init
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void SMG_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(LSA_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LSB_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(LSC_PORT_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(SMG_A_DP_PORT_RCC,ENABLE);
	
	
	// PB3&PB4 have JT Fuc.Use GPIO_PinRemapConfig() Disable this Fuc to use GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); // Need open AFIO CLK befor GPIO_Remap_SWJ_JTAGDisable  
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE); //PB3,PB4
	// GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE); //PA13,PA14
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Pin=LSA_PIN;    
	GPIO_Init(LSA_PORT,&GPIO_InitStructure); 	 
	//GPIO_SetBits(LSA_PORT,LSA_PIN);  
	
	GPIO_InitStructure.GPIO_Pin=LSB_PIN;  
	GPIO_Init(LSB_PORT,&GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin=LSC_PIN;  
	GPIO_Init(LSC_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=SMG_A_DP_PIN;  
	GPIO_Init(SMG_A_DP_PORT,&GPIO_InitStructure); 	
}

/*_________________________________________________________________________
**  SMG_write_data(u16 GPIO_pin, u8 data)
**
**  Descriptions: 
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void SMG_write_data(u16 GPIO_pin, u8 data)
{
	 u8 i = 0, j = GPIO_pin;
	
	 for(i = 0; i < 8; i++)
     {
		 if(data & 0x01)  
			GPIO_WriteBit(SMG_A_DP_PORT, j << i, Bit_SET); // light up, data 0
	     else	 
			GPIO_WriteBit(SMG_A_DP_PORT, j << i, Bit_RESET); // light up, data 1
		 data = data >> 1; 
	 }
}

/*_________________________________________________________________________
**  SMG_write_data(u16 GPIO_pin, u8 data)
**
**  Descriptions: 
**
**  Parameters:
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void SMG_Display(u8 data[], u8 post)
{
	 u8 i = 0;
	 u8 pos_temp = post - 1;
	
	 for(i = pos_temp; i < 8; i++)
     {
		 switch(i)
	     {
			 case 0: LSC = 1; LSB = 1; LSA = 1; break;
			 case 1: LSC = 1; LSB = 1; LSA = 0; break;
			 case 2: LSC = 1; LSB = 0; LSA = 1; break;
			 case 3: LSC = 1; LSB = 0; LSA = 0; break;
			 case 4: LSC = 0; LSB = 1; LSA = 1; break;
			 case 5: LSC = 0; LSB = 1; LSA = 0; break;
			 case 6: LSC = 0; LSB = 0; LSA = 1; break;
			 case 7: LSC = 0; LSB = 0; LSA = 0; break;
		 }
		 SMG_write_data(GPIO_Pin_0, data[i - pos_temp]);
		 delay_ms(1);
		 SMG_write_data(GPIO_Pin_0, 0x00); // clear
	 }
}


