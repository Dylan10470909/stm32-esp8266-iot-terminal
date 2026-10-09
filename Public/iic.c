#include "iic.h"
#include "SysTick.h"

/*_________________________________________________________________________
**  void IIC_Init(void)
**
**  Descriptions: IIC_Init
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IIC_SCL_PORT_RCC|IIC_SDA_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(IIC_SCL_PORT,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
	
	IIC_SCL=1;
	IIC_SDA=1;	
}

/*_________________________________________________________________________
**  void SDA_OUT(void)
**
**  Descriptions: SDA輸出配置
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}

/*_________________________________________________________________________
**  void SDA_IN(void)
**
**  Descriptions: SDA輸入配置
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin=IIC_SDA_PIN;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(IIC_SDA_PORT,&GPIO_InitStructure);
}

/*_________________________________________________________________________
**  void IIC_Start(void)
**
**  Descriptions: 產生 IIC 初始信號
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void IIC_Start(void)
{
	SDA_OUT();     // sda output
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(5);
 	IIC_SDA=0;   // START:when CLK is high,DATA change form high to low 
	delay_us(6);
	IIC_SCL=0;   // 抓住 I2C 總線，準備收發資料 
}	

/*_________________________________________________________________________
**  void IIC_Stop(void)
**
**  Descriptions: 產生 IIC 停止信號
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void IIC_Stop(void)
{
	SDA_OUT();  // sda output
	IIC_SCL=0;
	IIC_SDA=0;  //STOP:when CLK is high DATA change form low to high
 	IIC_SCL=1; 
	delay_us(6); 
	IIC_SDA=1;   // 發送 I2C 總線結束訊號
	delay_us(6);							   	
}

/*_________________________________________________________________________
**  u8 IIC_Wait_Ack(void)
**
**  Descriptions: 等待應答訊號收到
**
**  Parameters: void
**
**  Return: 1: sucess, 0: fail
**  Note: 
**       
**_________________________________________________________________________*/
u8 IIC_Wait_Ack(void)
{
	u8 tempTime=0;
	
	IIC_SDA=1;
	delay_us(1);
	SDA_IN();      //  SDA 設為輸入  	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;  // CLK輸出0 	   
	return 0;  
} 


/*_________________________________________________________________________
**  void IIC_Ack(void)
**
**  Descriptions: 產生 ACK 應答
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}


/*_________________________________________________________________________
**  void IIC_NAck(void)
**
**  Descriptions: 產生 ACK 非應答
**
**  Parameters: void
**
**  Return: 
**  Note: 
**       
**_________________________________________________________________________*/	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(5);
	IIC_SCL=0;
}	

/*_________________________________________________________________________
**  void IIC_Send_Byte(u8 txd)
**
**  Descriptions:  IIC 發送一個 Byte
**
**  Parameters: u8 
**
**  Return: 
**  Note: txd: 發送一個 Byte
**       
**_________________________________________________________________________*/	 	  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;  //  拉低 CLK 開始傳輸資料
    for(t=0;t<8;t++)
    {              
        if((txd&0x80)>0) //0x80  1000 0000
			IIC_SDA=1;
		else
			IIC_SDA=0;
        txd<<=1; 	  
		delay_us(2);   // 對 TEA5767 這三個延遲都是必須的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 

/*_________________________________________________________________________
**  u8 IIC_Read_Byte(u8 ack)
**
**  Descriptions:  IIC 讀取一個 Byte
**
**  Parameters: u8 
**
**  Return: ACK or nACK
**  Note: ack = 1 發送 ACK， ack = 0 發送 nACK
**       
**_________________________________________________________________________*/	 
u8 IIC_Read_Byte(u8 ack)
{
	u8 i,receive=0;
	SDA_IN();  // SDA 設為輸入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();  // 發送 nACK
    else
        IIC_Ack();  // 發送ACK   
    return receive;
}
