#ifndef _SMG_H
#define _SMG_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
extern u8 gsmg_code[17];

/************************************************************************
*                             M A C R O S
*************************************************************************
*/
#define LSA_PORT 			GPIOB  
#define LSA_PIN 			GPIO_Pin_5
#define LSA_PORT_RCC		RCC_APB2Periph_GPIOB

#define LSB_PORT 			GPIOB  
#define LSB_PIN 			GPIO_Pin_4
#define LSB_PORT_RCC		RCC_APB2Periph_GPIOB
 
#define LSC_PORT 			GPIOB  
#define LSC_PIN 			GPIO_Pin_3
#define LSC_PORT_RCC		RCC_APB2Periph_GPIOB

#define SMG_A_DP_PORT 			GPIOA  
#define SMG_A_DP_PIN 			GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
#define SMG_A_DP_PORT_RCC		RCC_APB2Periph_GPIOB

#define LSA 	PBout(5)
#define LSB 	PBout(4)
#define LSC 	PBout(3) 

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void SMG_Init(void); 
void SMG_Display(u8 data[], u8 post);
void SMG_write_data(u16 GPIO_pin, u8 data);

#endif
