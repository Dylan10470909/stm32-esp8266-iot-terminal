/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>
#include "systick.h" // 需提供 delay_ms() 延時函式
#include "esp8266.h"
#include "usart.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  void ESP8266_Init(void)
**
**  Descriptions: 初始化 USART3 (PA2-TX, PA3-RX)
**
**  Parameters: void
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void ESP8266_Init(void)
{
    USART3_Init(115200); // 呼叫 usart.c 的硬體初始化，設定預設波特率 115200
}

/*_________________________________________________________________________
**  void ESP8266_Rst(void)
**
**  Descriptions: 重啟WF-ESP8266模組
**
**  Parameters: void
**
**  Return: void
**  Note: Call by ESP8266_AT_Test
**_________________________________________________________________________*/
void ESP8266_Rst(void)
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	 ESP8266_RST_LOW_LEVEL();
	 delay_ms ( 500 ); 
	 ESP8266_RST_HIGH_LEVEL();
	 
	#endif

}

/*_________________________________________________________________________
**  void ESP8266_AT_Test ( void )
**
**  Descriptions: WF-ESP8266模組進行AT測試啟動
**
**  Parameters: void
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void ESP8266_AT_Test ( void )
{
	ESP8266_RST_HIGH_LEVEL();
	
	delay_ms ( 1000 ); 
	
	while ( ! ESP8266_Cmd ( "AT", "OK", NULL, 200 ) ) ESP8266_Rst ();  	

}

/*_________________________________________________________________________
**  bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
**
**  Descriptions: 對 WF-ESP8266 模組發送AT指令
**
**  Parameters: reply1，reply2，期待的回應，為NULL表不需回應，兩者為或邏輯關係
**
**  Return: 1 指令發送成功
**          0 指令發送失敗
**  Note:
**_________________________________________________________________________*/
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	ESP8266_Usart ( "%s\r\n", cmd );

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( waittime );                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*_________________________________________________________________________
**  bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
**
**  Descriptions: 選擇 WF-ESP8266 模組的工作模式
**
**  Parameters: enumMode，工作模式
**
**  Return: 1，選擇成功
**          0，選擇失敗
**  Note:
**_________________________________________________________________________*/
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 2500 ); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 2500 ); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 2500 ); 
		
	  default:
		  return false;
  }
	
}

/*_________________________________________________________________________
**  bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
**
**  Descriptions: 選擇WF-ESP8266模組連接外部WiFi
**
**  Parameters: pSSID，WiFi名稱字串
**              pPassWord，WiFi密碼字串
**
**  Return: 1，選擇成功
**          0，選擇失敗
**  Note:
**_________________________________________________________________________*/
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 7000 );
	
}

/*___________________________________________________________________________
**  bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
**
**  Descriptions: WF-ESP8266模組啟動多連接
**
**  Parameters: enumEnUnvarnishTx，設定是否多連接
**
**  Return: 1，設定成功
**          0，設定失敗
**  Note: 
**_________________________________________________________________________*/
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cStr [20];
	
	sprintf ( cStr, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cStr, "OK", 0, 500 );
	
}

/*________________________________________________________________________________________________________________________________________
**  bool ESP8266_Link_Server( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
**
**  Descriptions: WF-ESP8266模組連接外部伺服器
**
**  Parameters: enumE，網絡協議
**              ip，服務器IP字符串
**              ComNum，服務器端口字符串
**              id，模組連接服務器的ID
**  Return: 1，連接成功
**          0，連接失敗
**  Note: 
**_________________________________________________________________________*/
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "UDP", ip, ComNum );
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
    sprintf ( cCmd, "AT+CIPSTART=%d,%s", id, cStr);

  else
	  sprintf ( cCmd, "AT+CIPSTART=%s", cStr );

	return ESP8266_Cmd ( cCmd, "OK", "ALREAY CONNECT", 500 );
	
}
/*________________________________________________________________________________________________________________________________________
**  bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
**
**  Descriptions: WF-ESP8266模組發送字串
**
**  Parameters: enumEnUnvarnishTx，聲明是否已使能了透傳模式
**              pStr，要傳送的字串
**              ulStrLength，要傳送的字串的位元組數
**              ucId，哪個ID發送的字串
**  Return: 1，發送成功
**          0，發送失敗
**  Note: 
**_________________________________________________________________________*/
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
		
	if ( enumEnUnvarnishTx )
		ESP8266_Usart ( "%s", pStr );

	
	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength + 2 );

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength + 2 );
		
		ESP8266_Cmd ( cStr, "> ", 0, 1000 );

		bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 1000 );
  }
	
	return bRet;

}

/*_________________________________________________________________________
**  bool ESP8266_SendThingSpeak(char *api_key, int value)
**
**  Descriptions: 發送數據至 ThingSpeak
**
**  Parameters: char *api_key, int value
**
**  Return: uint8_t
**  Note:
**_________________________________________________________________________*/
bool ESP8266_SendThingSpeak(char *api_key, int value)
{
    char get_buf[128];
    
    // 1. 設定多路連線[cite: 14]
    ESP8266_Enable_MultipleId(ENABLE); //[cite: 14]
    
    // 2. 建立 TCP 連線至 ThingSpeak 80 Port[cite: 14]
    if (!ESP8266_Link_Server(enumTCP, "api.thingspeak.com", "80", Multiple_ID_0)) //[cite: 14]
    {
        return false;
    }
    
    // 3. 組合 HTTP GET 字串[cite: 14]
    sprintf(get_buf, "GET /update?api_key=%s&field1=%d\r\n", api_key, value);
    
    // 4. 發送資料[cite: 14]
    return ESP8266_SendString(DISABLE, get_buf, strlen(get_buf), Multiple_ID_0); //[cite: 14]
}
