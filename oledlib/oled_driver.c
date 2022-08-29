#include "oled_driver.h"
//#include "oled_i2c.h"
#include "codetab.h"
#include "i2c.h"
//#include "sys.h"
//#include "delay.h"
#include "common.h"
u8	oledBuffer[FRAME_BUFFER_SIZE];

#if (TRANSFER_METHOD ==HW_IIC)



extern I2C_HandleTypeDef hi2c1;
//---------------------------------------------------------
//复位I2c所有参数
static void I2Cx_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	if(hi2c->Instance == I2C1)
	{
		/* Configurethe GPIOs ---------------------------------------------------*/
		/* Enable GPIO clock */
		__I2C1_CLK_ENABLE();
		/* ConfigureI2C Tx asalternatefunction */
		GPIO_InitStruct.Pin = GPIO_PIN_6;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
		//GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		/* ConfigureI2C Rx asalternatefunction */
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		/* Configurethe Discovery I2Cx peripheral-------------------------------*/
		/* EnableI2C1 clock */
		__I2C1_FORCE_RESET(); //通过总复位模块对I2C一次复位操作
		__I2C1_RELEASE_RESET();
	}
}
//===================================================
//清除总线忙标志
//Xcp:2021/1/5
void I2C_Busy_C(void)
{
	SET_BIT(hi2c1.Instance->CR1,I2C_CR1_SWRST);
	CLEAR_BIT(hi2c1.Instance->CR1,I2C_CR1_SWRST);
	I2Cx_MspInit(&hi2c1); //自定义函数
	HAL_I2C_Init(&hi2c1);
}



//I2C_Configuration，初始化硬件IIC引脚
void I2C_Configuration(void)
{
//	I2C_InitTypeDef  I2C_InitStructure;
//	GPIO_InitTypeDef  GPIO_InitStructure; 

//	RCC_APB1PeriphClockCmd(IIC_RCC_APB1Periph_I2CX,ENABLE);
//	RCC_APB2PeriphClockCmd(IIC_RCC_APB2Periph_GPIOX,ENABLE);

//	GPIO_InitStructure.GPIO_Pin =  IIC_SCL_Pin_X | IIC_SDA_Pin_X;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C必须开漏输出
//	GPIO_Init(IIC_GPIOX, &GPIO_InitStructure);

//	I2C_DeInit(I2CX);
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
//	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//主机的I2C地址,随便写
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_ClockSpeed = 1320000;

//	I2C_Cmd(I2CX, ENABLE);
//	I2C_Init(I2CX, &I2C_InitStructure);
//	WaitTimeMs(200);
}

 /**
  * @brief  I2C_WriteByte，向OLED寄存器地址写一个byte的数据
  * @param  addr：寄存器地址
	*					data：要写入的数据
  * @retval 无
  */
int I2C_WriteByte(uint8_t addr,uint8_t data)
{
//  while(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY));
//	
//	I2C_GenerateSTART(I2CX, ENABLE);//开启I2C1
//	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

//	I2C_Send7bitAddress(I2CX, OLED_ADDRESS, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
//	while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

//	I2C_SendData(I2CX, addr);//寄存器地址
//	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

//	I2C_SendData(I2CX, data);//发送数据
//	while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//	
//	I2C_GenerateSTOP(I2CX, ENABLE);//关闭I2C1总线
	if(HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100) == HAL_OK)
	{
		return 1;
	}
	return 0;
}

void WriteCmd(unsigned char cmd)//写命令
{
		I2C_WriteByte(0x00, cmd);
}

void WriteDat(unsigned char dat)//写数据
{
		I2C_WriteByte(0x40, dat);
}

void OLED_FILL(unsigned char BMP[])
{
//		unsigned int n;
//		unsigned char *p;
//		p=BMP;
//		WriteCmd(0xb0);		//page0-page1
//		WriteCmd(0x00);		//low column start address
//		WriteCmd(0x10);		//high column start address
//		while(I2C_GetFlagStatus(I2CX, I2C_FLAG_BUSY));	
//		I2C_GenerateSTART(I2C1, ENABLE);//开启I2C1
//		while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/
//		I2C_Send7bitAddress(I2CX, OLED_ADDRESS, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
//		while(!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
//		I2C_SendData(I2CX, 0x40);//寄存器地址
//		while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));	
//		for(n=0;n<128*8;n++)
//		{
//			I2C_SendData(I2CX, *p++);//发送数据
//			while (!I2C_CheckEvent(I2CX, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
//		}	
//		I2C_GenerateSTOP(I2CX, ENABLE);//关闭I2C1总线
	u8 i,j;
	unsigned char *p;
	p=BMP;

  for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}



#elif  (TRANSFER_METHOD ==HW_SPI)

	#define OLED_RESET_LOW()					GPIO_ResetBits(SPI_RES_GPIOX, SPI_RES_PIN)	//低电平复位
	#define OLED_RESET_HIGH()					GPIO_SetBits(SPI_RES_GPIOX, SPI_RES_PIN)
	 
	#define OLED_CMD_MODE()						GPIO_ResetBits(SPI_DC_GPIOX, SPI_DC_PIN) 		//命令模式
	#define OLED_DATA_MODE()					GPIO_SetBits(SPI_DC_GPIOX, SPI_DC_PIN)			//数据模式

	#define OLED_CS_HIGH()   					GPIO_SetBits(SPI_CS_GPIOX, SPI_CS_Pin_X)
	#define OLED_CS_LOW()  		  			GPIO_ResetBits(SPI_CS_GPIOX, SPI_CS_Pin_X)
	
	
void SPI_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	#if 	(USE_HW_SPI==SPI_2)
		RCC_APB1PeriphClockCmd(SPI_RCC_APB1Periph_SPIX, ENABLE);
	#elif (USE_HW_SPI==SPI_1)
		RCC_APB2PeriphClockCmd(SPI_RCC_APB2Periph_SPIX, ENABLE);
	#endif
	RCC_APB2PeriphClockCmd(SPI_RCC_APB2Periph_GPIOX|RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SPI_CS_Pin_X;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CS_GPIOX, &GPIO_InitStructure);  
	OLED_CS_HIGH();
	
	GPIO_InitStructure.GPIO_Pin = SPI_HW_ALL_PINS;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_HW_ALL_GPIOX, &GPIO_InitStructure);   
	
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI_Direction_1Line_Tx; 
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	 			
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	 	
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		 	
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		  
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  			
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPIX, &SPI_InitStructure);  
	SPI_Cmd(SPIX, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = SPI_RES_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_RES_GPIOX, &GPIO_InitStructure);  
	GPIO_InitStructure.GPIO_Pin = SPI_DC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_DC_GPIOX, &GPIO_InitStructure);  
	OLED_DATA_MODE();
	
	OLED_RESET_HIGH();
	WaitTimeMs(100);
	OLED_RESET_LOW();
	WaitTimeMs(100);
	OLED_RESET_HIGH();
}

void SPI_WriterByte(unsigned char dat)
{

	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_TXE) == RESET ){}; //检查指定的SPI标志位设置与否:发送缓存空标志位	  
		SPI_I2S_SendData(SPIX, dat); //通过外设SPIx发送一个数据
	while (SPI_I2S_GetFlagStatus(SPIX, SPI_I2S_FLAG_RXNE) == RESET){};//检查指定的SPI标志位设置与否:接受缓存非空标志位  						    
	  SPI_I2S_ReceiveData(SPIX); //返回通过SPIx最近接收的数据	

}

void WriteCmd(unsigned char cmd)
{
	OLED_CMD_MODE();
	OLED_CS_LOW();
	SPI_WriterByte(cmd);
	OLED_CS_HIGH();
	OLED_DATA_MODE();
}

void WriteDat(unsigned char dat)
{
	OLED_DATA_MODE();
	OLED_CS_LOW();
	SPI_WriterByte(dat);
	OLED_CS_HIGH();
	OLED_DATA_MODE();
}

void OLED_FILL(unsigned char BMP[])
{
	u8 i,j;
	unsigned char *p;
	p=BMP;

  for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}

#elif  (TRANSFER_METHOD ==SW_SPI)

#define OLED_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_6)  //D0
#define OLED_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_6)

#define OLED_SDIN_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//DIN  D1
#define OLED_SDIN_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

#define OLED_RST_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_0)//RES
#define OLED_RST_Set() GPIO_SetBits(GPIOB,GPIO_Pin_0)

#define OLED_DC_Clr() GPIO_ResetBits(GPIOB,GPIO_Pin_1)//DC
#define OLED_DC_Set() GPIO_SetBits(GPIOB,GPIO_Pin_1)
 		     
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,GPIO_Pin_10)//CS
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,GPIO_Pin_10)

void SW_SPI_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	GPIO_SetBits(GPIOA,GPIO_Pin_6|GPIO_Pin_7);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  
 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10);
	OLED_RST_Set();
	
	delay_ms(100);
	OLED_RST_Clr();//复位
	delay_ms(100);
	OLED_RST_Set();
}

void WriteCmd(unsigned char dat)
{	
	u8 i;			  
	OLED_DC_Clr();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

void WriteDat(unsigned char dat)
{	
	u8 i;			  
	OLED_DC_Set();
	OLED_CS_Clr();
	for(i=0;i<8;i++)
	{
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_CS_Set();
	OLED_DC_Set();   	  
}

void OLED_FILL(unsigned char BMP[])
{
	u8 i,j;
	unsigned char *p;
	p=BMP;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}

#elif  (TRANSFER_METHOD ==SW_IIC)
void SW_IIC_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6);	
	delay_ms(500);
}
/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
    OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	
}

void IIC_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)		
	{
			m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();}
		else OLED_SDIN_Clr();
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		}


}
/**********************************************
// IIC Write Command
**********************************************/
void WriteCmd(unsigned char dat)
{
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();	
   Write_IIC_Byte(dat); 
	IIC_Wait_Ack();	
   IIC_Stop();
}
void WriteDat(unsigned char dat)
{	
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();	
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();	
   Write_IIC_Byte(dat);
	IIC_Wait_Ack();	
   IIC_Stop();  	  
}

void OLED_FILL(unsigned char BMP[])
{
	u8 i,j;
	unsigned char *p;
	p=BMP;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			WriteDat(*p++);
		}
	}
}

#endif 



void OLED_Init(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_Delay(200);
	
//	WriteCmd(0xAE); //display off
//	WriteCmd(0x20);	//Set Memory Addressing Mode	
//	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
//	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
//	WriteCmd(0xc8);	//Set COM Output Scan Direction
//	WriteCmd(0x00); //---set low column address
//	WriteCmd(0x10); //---set high column address
//	WriteCmd(0x40); //--set start line address
//	WriteCmd(0x81); //--set contrast control register
//	WriteCmd(0xff); //亮度调节 0x00~0xff
//	WriteCmd(0xa1); //--set segment re-map 0 to 127
//	WriteCmd(0xa6); //--set normal display
//	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
//	WriteCmd(0x3F); //
//	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
//	WriteCmd(0xd3); //-set display offset
//	WriteCmd(0x00); //-not offset
//	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
//	WriteCmd(0xf0); //--set divide ratio
//	WriteCmd(0xd9); //--set pre-charge period
//	WriteCmd(0x22); //
//	WriteCmd(0xda); //--set com pins hardware configuration
//	WriteCmd(0x12);
//	WriteCmd(0xdb); //--set vcomh
//	WriteCmd(0x20); //0x20,0.77xVcc
//	WriteCmd(0x8d); //--set DC-DC enable
//	WriteCmd(0x14); //
//	WriteCmd(0xaf); //--turn on oled panel
	
	WriteCmd(0xAE);//--display off
	WriteCmd(0x00);//---set low column address  wei 2022.8.21 0x02->0x00
	WriteCmd(0x10);//---set high column address
	WriteCmd(0x40);//--set start line address  
	WriteCmd(0xB0);//--set page address
	WriteCmd(0x81); // contract control
	WriteCmd(0xFF);//--128   
	WriteCmd(0xA1);//set segment remap 
	WriteCmd(0xA6);//--normal / reverse
	WriteCmd(0xA8);//--set multiplex ratio(1 to 64)
	WriteCmd(0x3F);//--1/32 duty
	WriteCmd(0xC8);//Com scan direction
	WriteCmd(0xD3);//-set display offset
	WriteCmd(0x00);//
	
	WriteCmd(0xD5);//set osc division
	WriteCmd(0x80);//
	
	WriteCmd(0xD8);//set area color mode off
	WriteCmd(0x05);//
	
	WriteCmd(0xD9);//Set Pre-Charge Period
	WriteCmd(0xF1);//
	
	WriteCmd(0xDA);//set com pin configuartion
	WriteCmd(0x12);//
	
	WriteCmd(0xDB);//set Vcomh
	WriteCmd(0x30);//
	
	WriteCmd(0x8D);//set charge pump enable
	WriteCmd(0x14);//
	
	WriteCmd(0xAF);//--turn on oled panel
	OLED_CLS();
	
	if(!appConfig.display180)
	{
		WriteCmd(0xA1); 
		WriteCmd(0XC8); 
	}
	else
	{
		WriteCmd(0xA0); 
		WriteCmd(0xC0); 
	}
	
}


void OLED_CLS(void)//清屏
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
		{
				WriteDat(0x00);
		}
	}
}

void OLED_ON(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X14);  //开启电荷泵
	WriteCmd(0XAF);  //OLED唤醒
}



void OLED_OFF(void)
{
	WriteCmd(0X8D);  //设置电荷泵
	WriteCmd(0X10);  //关闭电荷泵
	WriteCmd(0XAE);  //OLED休眠
}

void oled_flush(void)
{
	
	//OLED_FILL(oledBuffer);
	u8 i,j;
	unsigned char *p;
	p=oledBuffer;

	for(i=0;i<8;i++)
	{
		WriteCmd(0xb0+i);		//page0-page1
		WriteCmd(0x00);		//low column start address  wei 2022.8.21 0x02->0x00
		WriteCmd(0x10);	

		for(j=0;j<128;j++)
		{
			if(appConfig.invert)
			{
				WriteDat(~(*p++));
			}
			else
			{
				WriteDat(*p++);
			}
		}
	}
}
void OLED_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}
//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat(F6x8[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i]);
				OLED_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat(F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat(BMP[j++]);
		}
	}
}
