#ifndef _WIFI_CONFIG_H
#define _WIFI_CONFIG_H

/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include <stdbool.h>
#include "system.h"
#include "usart.h"


/************************************************************************
*                             M A C R O S
*************************************************************************
*/
#define     ESP8266_Usart( fmt, ... )           USART3_printf (USART3, fmt, ##__VA_ARGS__ ) 
#define     PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ )

#define     ESP8266_CH_HIGH_LEVEL()             GPIO_SetBits( GPIOA, GPIO_Pin_15 )
#define     ESP8266_CH_LOW_LEVEL()              GPIO_ResetBits( GPIOA, GPIO_Pin_15 )

#define     ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits( GPIOB, GPIO_Pin_2 )
#define     ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits( GPIOB, GPIO_Pin_2 )


/************************************************************************
*              F U N C T I O N   D E C L A R A T I O N S
*************************************************************************
*/
void ESP8266_Init(void);
void ESP8266_Rst ( void );
void ESP8266_AT_Test (void);
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime );
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode );
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord );
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx );
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
bool ESP8266_SendThingSpeak(char *api_key, int value);

#endif
