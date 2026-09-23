/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/
struct  STRUCT_USART3_Fram strEsp8266_Fram_Record = { 0 };

u16 USART1_RX_STA = 0;
u8 USART1_RX_BUF[USART1_REC_LEN];

/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  int fputc(int ch,FILE *p) 
**
**  Descriptions: fputc
**
**  Parameters: int ch,FILE *p
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
int fputc(int ch,FILE *p) 
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET); 
	return ch;
}

/*_________________________________________________________________________
**  void USART1_Init(void)
**
**  Descriptions: USART1_Init
**
**  Parameters: u32 bound
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 1. 開啟 USART1 (APB2) 與 GPIOA (APB2) 時鐘
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	// 始能 USART1 CLK
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	// 始能 GPIO CLK
	
	// 2. PA9 -> USART2_TX (推挽復用輸出)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	 		// Alternate Function Push-Pull
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	 				
	
	// 3. PA10 -> USART2_RX (浮空輸入)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// Floating Input. For USART RX.
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  
	GPIO_Init(GPIOA,&GPIO_InitStructure); 	 
	
	// 4. USART1 設定
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   // Tx/Rx
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 開啟相關中斷
	USART_ClearFlag(USART1, USART_FLAG_TC);
	
	// 5. NVIC 設定 (優先級可依需求調整)
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;      		// USART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			// IRQChannel Enable
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 	// PreemptionPriority
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 			//SubPriority
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);  // ENABLE USART 
}

/*_________________________________________________________________________
**  void USART1_IRQHandler(void)
**
**  Descriptions: USART1_IRQ
**
**  Parameters:  
**
**  Return: void
**  Note: 在中斷函數內接收讀取值時會自動清除中斷標記, 可以不必手動清除
**_________________________________________________________________________*/
void USART1_IRQHandler(void)
{
	u8 r;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	// 接收中斷
	{
		r = USART_ReceiveData(USART1);	// 讀取接收到數值 (USART->DR)
		if((USART1_RX_STA & 0x8000) == 0)	// 接收未完成
		{
			if(USART1_RX_STA & 0x4000) // 判斷是否接收到 0x0d(\r)
			{
				if(r == 0x0a) USART1_RX_STA |= 0x8000; // 判斷是否接收到 0x0a(\n), 接收完成
				else USART1_RX_STA =0; // 接收錯誤
			}
			else // 還沒接收到 0x0d
			{
				if(r == 0x0d) USART1_RX_STA |= 0x4000; // 接收到 0x0d
				else // 未接收到 0x0d
				{					
					USART1_RX_BUF[USART1_RX_STA & 0x3fff]  = r; // 存入暫存器
					USART1_RX_STA++;
					if(USART1_RX_STA >= USART1_REC_LEN) USART1_RX_STA =0; // 接收錯誤 重新開始
				}
			}	
		}
		
		USART_SendData(USART1, r);
	}
}

/*_________________________________________________________________________
**  void USART3_Init(void)
**
**  Descriptions: USART3_Init
**
**  Parameters: u32 bound
**
**  Return: void
**  Note:
**_________________________________________________________________________*/
void USART3_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	/* 設定WiFi模組的片選（CH）接腳 重設重啟（RST）接腳*/
	/* 選擇要控制的PA15（CH）接腳和PB2（RST）接腳*/															   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init( GPIOB, &GPIO_InitStructure );
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init( GPIOA, &GPIO_InitStructure );	
	/* 拉低WiFi模組的片選腳位 */
	GPIO_ResetBits( GPIOA, GPIO_Pin_15 );		
	/* 拉高WiFi模組的複位重啟接腳 */
	GPIO_SetBits( GPIOB, GPIO_Pin_2 );
	
	// RX
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; // PB10
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 複用推挽輸出
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
	// TX
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;// PB11
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;// 浮空输入
  	GPIO_Init(GPIOB, &GPIO_InitStructure);// 初始化GPIOB 11  
  
   	// USART3 初始化設定
	USART_InitStructure.USART_BaudRate = bound;// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;// 字長為8位元資料格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;// 無無奇偶校驗位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// 無硬體資料流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	// 收發模式
  	USART_Init(USART3, &USART_InitStructure); // 初始化串列埠3

  	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);// 開啟串列埠接受與匯流排閒置中斷
  	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART3, ENABLE);                    // 啟用串口3
	
	//Usart3 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;// 搶先優先級0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 子優先權0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// IRQ通道啟用
	NVIC_Init(&NVIC_InitStructure);	// 根據指定的參數初始化VIC暫存器
}

/*_________________________________________________________________________
**  void USART3_IRQHandler(void)
**
**  Descriptions: USART1_IRQ
**
**  Parameters:  void
**
**  Return: void
**  Note: 在中斷函數內接收讀取值時會自動清除中斷標記, 可以不必手動清除
**_________________________________________________________________________*/
void USART3_IRQHandler(void)
{	
	char ch;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		ch  = USART_ReceiveData( USART3 );
		
		if( strEsp8266_Fram_Record .InfBit .FramLength < ( USART3_REC_LEN - 1 ) )                       // 預留1個位元組寫結束符
		{
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ch;

		}
	}
	 	 
	if ( USART_GetITStatus( USART3, USART_IT_IDLE ) == SET )                                         // 資料幀接收完畢
	{
		strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ch = USART_ReceiveData( USART3 );                                                             // 由軟體序列清除中斷標誌位元(先讀USART_SR，然後讀取USART_DR)
	
  }	

}

/*_________________________________________________________________________
**  static char *itoa( int value, char *string, int radix )
**
**  Descriptions: 將整數形資料轉換成字串
**
**  Parameters:  -radix =10 表示10進制，其他結果為0
**               -value 要轉換的整形數
** 				 -buf 轉換後的字串
** 				 -radix = 10
**
**  Return: void
**  Note: Call by USART3_printf()
**_________________________________________________________________________*/
static char *itoa( int value, char *string, int radix )
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */

/*_________________________________________________________________________
**  void USART3_printf( USART_TypeDef* USARTx, char *Data, ... )
**
**  Descriptions: 格式化輸出，類似C函式庫中的printf，但這裡沒有用到C函式庫
**
**  Parameters:  -USARTx 串列埠通道，這裡只用到了串列埠2，即USART2
**				 -Data 要傳送到串列埠的內容的指針
** 				 -... 其他參數
**
**  Return: void
**  Note: 典型應用 USART3_printf( USART3, "\r\n this is a demo \r\n" );
**            	   USART3_printf( USART3, "\r\n %d \r\n", i );
**            	   USART3_printf( USART3, "\r\n %s \r\n", j );
**_________________________________________________________________________*/
void USART3_printf( USART_TypeDef* USARTx, char *Data, ... )
{
	const char *s;
	int d;   
	char buf[16];

	va_list ap;
	va_start(ap, Data);

	while ( *Data != 0)     // 判斷是否到達字串結束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          // 回車符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          // 換行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十進制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}
