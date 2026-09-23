#ifndef _DHT11_H
#define _DHT11_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"


/************************************************************************
*                             M A C R O S
*************************************************************************
*/ 
#define DHT11 			GPIO_Pin_11
#define GPIO_DHT11 		GPIOA
#define DHT11_PORT_RCC	RCC_APB2Periph_GPIOA

#define DHT11_DQ_IN 	PAin(11)	
#define DHT11_DQ_OUT 	PAout(11)  


/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/

void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
u8 DHT11_Init(void);
void DHT11_Rst(void);
u8 DHT11_Check(void);
u8 DHT11_Read_Bit(void);
u8 DHT11_Read_Byte(void);
u8 DHT11_Read_Data(u8 *temp,u8 *humi);

#endif
