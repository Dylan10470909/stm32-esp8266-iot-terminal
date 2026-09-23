#ifndef _USART_H
#define _USART_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "system.h"

/************************************************************************
*                             M A C R O S
*************************************************************************
*/
#define USART1_REC_LEN 200
#define USART3_REC_LEN 1024  

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
typedef enum{
		STA,
		AP,
		STA_AP  
} ENUM_Net_ModeTypeDef;

typedef enum{
		enumTCP,
		enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;

extern struct  STRUCT_USART3_Fram                                  // 串列資料幀的處理結構體
{
	char  Data_RX_BUF[ USART3_REC_LEN ];
	
  union {
    __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
} strPc_Fram_Record, strEsp8266_Fram_Record;


// USART1 接收緩衝區與計數器
extern u8 USART1_RX_BUF[USART1_REC_LEN];
extern u16 USART1_RX_STA;

/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void USART1_Init(u32 bound);
void USART3_Init(u32 bound);
void USART3_printf( USART_TypeDef* USARTx, char *Data, ... );

#endif
