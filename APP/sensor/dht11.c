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

	GPIO_InitStructure.GPIO_Pin=DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
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
**  Note: 
**_________________________________________________________________________*/
void DHT11_Rst()	   
{                 
	DHT11_IO_OUT(); 	//SET OUTPUT
    DHT11_DQ_OUT=0; 	//拉低DQ
    delay_ms(20);    	//拉低至少18ms
    DHT11_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_Check() 	   
{   
	u8 retry=0;
	DHT11_IO_IN();//SET INPUT	 
    while (DHT11_DQ_IN&&retry<100)//DHT11会拉低40~50us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~50us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//从DHT11读取一个位
//返回值：1/0
u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_DQ_IN&&retry<100)//等待变为低电平 12-14us 开始
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_DQ_IN&&retry<100)//等待变高电平	 26-28us表示0,116-118us表示1
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_DQ_IN)return 1;
	else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
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
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//讀取 40 bits 資料
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
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
**  Note: 
**_________________________________________________________________________*/
void DHT11_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽輸出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
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
**  Note: 
**_________________________________________________________________________*/
void DHT11_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	 //上拉輸出模式
	GPIO_Init(GPIO_DHT11,&GPIO_InitStructure);	
}
