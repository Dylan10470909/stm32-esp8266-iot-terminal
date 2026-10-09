#ifndef _iic_H
#define _iic_H

#include "system.h"

/*  IIC_SCL 時脈端口、引脚定義 */
#define IIC_SCL_PORT 			GPIOB   
#define IIC_SCL_PIN 			(GPIO_Pin_6)
#define IIC_SCL_PORT_RCC		RCC_APB2Periph_GPIOB

/*  IIC_SDA 時脈端口、引脚定義 */
#define IIC_SDA_PORT 			GPIOB  
#define IIC_SDA_PIN 			(GPIO_Pin_7)
#define IIC_SDA_PORT_RCC		RCC_APB2Periph_GPIOB

// IO 操作函数	 
#define IIC_SCL    PBout(6) // SCL
#define IIC_SDA    PBout(7) // SDA	 
#define READ_SDA   PBin(7)  // 輸入SDA

// IIC 所有操作函数
void IIC_Init(void);                // 初始化 IIC 的 IO 口				 
void IIC_Start(void);				// 發送 IIC 開始訊號
void IIC_Stop(void);	  			// 發送 IIC 停止訊號
void IIC_Send_Byte(u8 txd);			// IIC 發送一个字节
u8 IIC_Read_Byte(u8 ack);			// IIC 讀取一个字节
u8 IIC_Wait_Ack(void); 				// IIC 等待 ACK 訊號
void IIC_Ack(void);					// IIC 發送 ACK 訊號
void IIC_NAck(void);				// IIC 不發送 ACK 訊號


#endif
