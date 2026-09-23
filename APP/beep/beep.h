#ifndef _BEEP_H
#define _BEEP_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"

/************************************************************************
*                             M A C R O S
*************************************************************************
*/
#define BEEP_PORT 			GPIOB
#define BEEP_PIN            GPIO_Pin_0
#define BEEP_PORT_RCC		RCC_APB2Periph_GPIOB

#define BEEP 	PBout(0) 

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void Beep_Init(void);
void Beep_Alarm(u8 times, u8 us);

#endif
