/************************************************************************
*                E X T E R N A L   R E F E R E N C E S
*************************************************************************
*/
#include "dht11.h"
#include <stdio.h>
#include <stdarg.h>
#include "systick.h"

/************************************************************************
*                         D A T A   T Y P E S
*************************************************************************
*/


/************************************************************************
*                          F U N C T I O N S
*************************************************************************
*/
/*_________________________________________________________________________
**  u8 DHT11_Init()
**
**  Descriptions: DHT11_Init
**
**  Parameters: void
**
**  Return: u8
**  Note: return 0: Init sucess, 1: failure
**_________________________________________________________________________*/
u8 DHT11_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(DHT11_PORT_RCC,ENABLE);

	GPIO_InitStructure.GPIO_Pin = DHT11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);
	GPIO_SetBits(GPIO_DHT11,DHT11);	   //拉高

	DHT11_Rst();	  
	return DHT11_Check();	
}
/*_________________________________________________________________________
**  u8 DHT11_Rst()
**
**  Descriptions: DHT11_Rst
**
**  Parameters: void
**
**  Return: u8
**  Note: STM32 與 DHT11 溝通
**_________________________________________________________________________*/
void DHT11_Rst()	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT STM32主動拉線
    DHT11_DQ_OUT = 0; 	//拉低DQ
    delay_ms(20);    	//拉低至少18ms 規格定義換醒訊號
    DHT11_DQ_OUT = 1; 	//DQ=1 
	delay_us(30);     	//主機拉高20~40us 對應「STM32拉高20~30us」
}

/*_________________________________________________________________________
**  u8 DHT11_Check()
**
**  Descriptions: 確認 DHT11 是否回應 
**
**  Parameters: void
**
**  Return: u8(0:SUCESS, 1:FAIL)
**  Note:  等待DHT11的回應
**_________________________________________________________________________*/
u8 DHT11_Check() 	   
{   
	u8 retry=0;
	DHT11_IO_IN();// SET INPUT 關鍵切換：STM32放手，把腳位改成「輸入模式」
	              // 因為接下來輪到DHT11講話，STM32要改聽的
    while (DHT11_DQ_IN && retry < 100)// DHT11會拉低 40~50us 等 DHT11把線拉低（DHT11要開始回應了）
	{
		retry++;
		delay_us(1);
	};	 
	if(retry >= 100) return 1; // 等太久沒等到，判定失敗
	else retry = 0;
    while (!DHT11_DQ_IN && retry < 100)// DHT11拉低后會再次拉高40~50us 等DHT11把線放開(拉高)，代表低電位那段結束了
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100) return 1;	  
	return 0;  // 正常收到「DHT11拉低約80us -> 拉高約80us」這組回應  
}

/*_________________________________________________________________________
**  u8 DHT11_Read_Bit(void) 
**
**  Descriptions: DHT11_Read_Bit
**
**  Parameters: void 從 DHT11讀取一个位
**
**  Return: u8: 1/0
**  Note:  不用去精確測量高電位到底持續了多久，只要卡在中間的一個時間點（40us）去看當下是高還是低就好。
**_________________________________________________________________________*/
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN && retry < 100)// 等待進入低電位（每個bit開頭都先拉低約50us）
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN && retry < 100)//  等待進入高電位（低電位結束，換成高電位） 26-28us表示0,116-118us表示1
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_DQ_IN) return 1;  // 40us後還是高電位 -> 代表這是「長高電位(70us)」-> bit=1
	else return 0;		   // 40us後已經變回低電位了 -> 代表這是「短高電位(26~28us)」-> bit=0
}

/*_________________________________________________________________________
**  void DHT11_Read_Byte()
**
**  Descriptions: DHT11 讀取一次Byte
**
**  Parameters: u8 *temp,u8 *humi
**
**  Return: void
**  Note: 
**_________________________________________________________________________*/
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat <<= 1;   // 每次先騰出最低位（左移）
	    dat |= DHT11_Read_Bit();  // 把讀到的bit塞進最低位
    }						    
    return dat;  // 湊滿8個bit = 1個byte
}

/*_________________________________________________________________________
**  void DHT11_Read_Data()
**
**  Descriptions: DHT11 讀取一次數據
**
**  Parameters: u8 *temp,u8 *humi
**
**  Return: u8; 0: sucess, 1: fail
**  Note: temp:0~50, humi: 20%~90%
**_________________________________________________________________________*/
u8 DHT11_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst();  // 重新走一次「起始+回應」握手
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//讀取 40 bits 資料
		{
			buf[i]=DHT11_Read_Byte();   // 連續讀5個byte
		}
		if((buf[0]+buf[1]+buf[2]+buf[3]) == buf[4])   // 前4個byte加總要等於第5個byte(校驗和)
		{
			*humi=buf[0];   // 濕度整數部分 buf[1]小數部分
			*temp=buf[2];   // 溫度整數部分 buf[3]小數部分
		}
	}else return 1;
	return 0;	    
}

/*_________________________________________________________________________
**  void DHT11_IO_OUT()
**
**  Descriptions: DHT11 輸出模式
**
**  Parameters: void
**
**  Return: void
**  Note: STM32 要主動輸出（發起18ms的低電位起始訊號），需要很把線壓到低電位
**_________________________________________________________________________*/
void DHT11_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	 // 推挽輸出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}
/*_________________________________________________________________________
**  void DHT11_IO_IN()
**
**  Descriptions: DHT11 輸入模式
**
**  Parameters: void
**
**  Return: void
**  Note: DHT11 沒輸出時線是高電位，而不是亂飄
**_________________________________________________________________________*/
void DHT11_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DHT11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉輸出模式
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}
