/************************************************************************************
*  Copyright (c), 2014, HelTec Automatic Technology co.,LTD.
*            All rights reserved.
*
* Http:    www.heltec.cn
* Email:   cn.heltec@gmail.com
* WebShop: heltec.taobao.com
*
* File name: OLED_I2C.c
* Project  : HelTec.uvprij
* Processor: STM32F103C8T6
* Compiler : MDK fo ARM
* 
* Author : С��
* Version: 1.00
* Date   : 2014.4.8
* Email  : hello14blog@gmail.com
* Modification: none
* 
* Description:128*64�����OLED��ʾ�������ļ����������ڻ����Զ���(heltec.taobao.com)��SD1306����IICͨ�ŷ�ʽ��ʾ��
*
* Others: none;
*
* Function List:
*	1. void I2C_Configuration(void) -- ����CPU��Ӳ��I2C
* 2. void I2C_WriteByte(uint8_t addr,uint8_t data) -- ��Ĵ�����ַдһ��byte������
* 3. void WriteCmd(unsigned char I2C_Command) -- д����
* 4. void WriteDat(unsigned char I2C_Data) -- д����
* 5. void OLED_Init(void) -- OLED����ʼ��
* 6. void OLED_SetPos(unsigned char x, unsigned char y) -- ������ʼ������
* 7. void OLED_Fill(unsigned char fill_Data) -- ȫ�����
* 8. void OLED_CLS(void) -- ����
* 9. void OLED_ON(void) -- ����
* 10. void OLED_OFF(void) -- ˯��
* 11. void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize) -- ��ʾ�ַ���(�����С��6*8��8*16����)
* 12. void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N) -- ��ʾ����(������Ҫ��ȡģ��Ȼ��ŵ�codetab.h��)
* 13. void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]) -- BMPͼƬ
*
* History: none;
*
*************************************************************************************/

#include "oled_i2c.h"
#include "codetab.h"
#include "i2c.h"

#include "common.h"

u8	oledBuffer[FRAME_BUFFER_SIZE];


//#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)//;//GPIO_ResetBits(GPIOB,GPIO_Pin_6)//SCL
//#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)

//#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)//GPIO_ResetBits(GPIOB,GPIO_Pin_7)//SDA
//#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)//GPIO_SetBits(GPIOB,GPIO_Pin_7)

///**********************************************
////IIC Start
//**********************************************/
//void IIC_Start()
//{
//	OLED_SCLK_Set() ;
//	OLED_SDIN_Set();
//	OLED_SDIN_Clr();
//	OLED_SCLK_Clr();
//}


///**********************************************
////IIC Stop
//**********************************************/
//void IIC_Stop()
//{
//	OLED_SCLK_Set() ;
////	OLED_SCLK_Clr();
//	OLED_SDIN_Clr();
//	OLED_SDIN_Set();
//}


//void IIC_Wait_Ack()
//{
//	//GPIOB->CRH &= 0XFFF0FFFF;	//����PB12Ϊ��������ģʽ
//	//GPIOB->CRH |= 0x00080000;
////	OLED_SDA = 1;
////	delay_us(1);
//	//OLED_SCL = 1;
//	//delay_us(50000);
///*	while(1)
//	{
//		if(!OLED_SDA)				//�ж��Ƿ���յ�OLED Ӧ���ź�
//		{
//			//GPIOB->CRH &= 0XFFF0FFFF;	//����PB12Ϊͨ���������ģʽ
//			//GPIOB->CRH |= 0x00030000;
//			return;
//		}
//	}
//*/
//	OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
//}


///**********************************************
//// IIC Write byte
//**********************************************/

//void Write_IIC_Byte(unsigned char IIC_Byte)
//{
//	unsigned char i;
//	unsigned char m,da;
//	da=IIC_Byte;
//	OLED_SCLK_Clr();
//	for(i=0;i<8;i++)		
//	{
//		m=da;
//		//	OLED_SCLK_Clr();
//		m=m&0x80;
//		if(m==0x80)
//		{
//			OLED_SDIN_Set();
//		}
//		else OLED_SDIN_Clr();
//			da=da<<1;
//		OLED_SCLK_Set();
//		OLED_SCLK_Clr();
//		}
//}


///**********************************************
//// IIC Write Command
//**********************************************/
//void Write_IIC_Command(unsigned char IIC_Command)
//{
//	IIC_Start();
//	Write_IIC_Byte(0x78);            //Slave address,SA0=0
//	IIC_Wait_Ack();	
//	Write_IIC_Byte(0x00);				//write command
//	IIC_Wait_Ack();	
//	Write_IIC_Byte(IIC_Command); 
//	IIC_Wait_Ack();	
//	IIC_Stop();
//}


///**********************************************
//// IIC Write Data
//**********************************************/
//void Write_IIC_Data(unsigned char IIC_Data)
//{
//	IIC_Start();
//	Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
//	IIC_Wait_Ack();	
//	Write_IIC_Byte(0x40);			//write data
//	IIC_Wait_Ack();	
//	Write_IIC_Byte(IIC_Data);
//	IIC_Wait_Ack();	
//	IIC_Stop();
//}












extern I2C_HandleTypeDef hi2c1;

//---------------------------------------------------------
//��λI2c���в���
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
		__I2C1_FORCE_RESET(); //ͨ���ܸ�λģ���I2Cһ�θ�λ����
		__I2C1_RELEASE_RESET();
	}
}

//===================================================
//�������æ��־
//Xcp:2021/1/5
void I2C_Busy_C(void)
{
	SET_BIT(hi2c1.Instance->CR1,I2C_CR1_SWRST);
	CLEAR_BIT(hi2c1.Instance->CR1,I2C_CR1_SWRST);
	I2Cx_MspInit(&hi2c1); //�Զ��庯��
	HAL_I2C_Init(&hi2c1);
}


int I2C_WriteByte(uint8_t addr,uint8_t data)
{
//	if(READ_BIT(hi2c1.Instance->SR2,I2C_SR2_BUSY) || READ_BIT(hi2c1.Instance->CR1,I2C_CR1_PE) == 0){
//		I2C_Busy_C();
//		return 0;
//	}
//	else
	{
		if(HAL_I2C_Mem_Write(&hi2c1, OLED_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 0x100) == HAL_OK)
		{
			return 1;
		}
	}	
	return 1;

}

void WriteCmd(unsigned char I2C_Command)//д����
{
	I2C_WriteByte(0x00, I2C_Command);
	//Write_IIC_Command(I2C_Command);
}

void WriteDat(unsigned char I2C_Data)//д����
{
	I2C_WriteByte(0x40, I2C_Data);
	//Write_IIC_Data(I2C_Data);
}

void OLED_Init(void)
{
	//DelayMs(100); //�������ʱ����Ҫ
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(200);
	HAL_Delay(200);
	
	WriteCmd(0xAE); //display off
	WriteCmd(0x20);	//Set Memory Addressing Mode	
	WriteCmd(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(0xc8);	//Set COM Output Scan Direction
	WriteCmd(0x00); //---set low column address
	WriteCmd(0x10); //---set high column address
	WriteCmd(0x40); //--set start line address
	WriteCmd(0x81); //--set contrast control register
	WriteCmd(0xff); //���ȵ��� 0x00~0xff
	WriteCmd(0xa1); //--set segment re-map 0 to 127
	WriteCmd(0xa6); //--set normal display
	WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(0x3F); //
	WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(0xd3); //-set display offset
	WriteCmd(0x00); //-not offset
	WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(0xf0); //--set divide ratio
	WriteCmd(0xd9); //--set pre-charge period
	WriteCmd(0x22); //
	WriteCmd(0xda); //--set com pins hardware configuration
	WriteCmd(0x12);
	WriteCmd(0xdb); //--set vcomh
	WriteCmd(0x20); //0x20,0.77xVcc
	WriteCmd(0x8d); //--set DC-DC enable
	WriteCmd(0x14); //
	WriteCmd(0xaf); //--turn on oled panel
}

void OLED_SetPos(unsigned char x, unsigned char y) //������ʼ������
{ 
	WriteCmd(0xb0+y);
	WriteCmd(((x&0xf0)>>4)|0x10);
	WriteCmd((x&0x0f)|0x01);
}

void OLED_Fill(unsigned char fill_Data)//ȫ�����
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(0xb0+m);		//page0-page1
		WriteCmd(0x00);		//low column start address
		WriteCmd(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat(fill_Data);
			}
	}
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
		WriteCmd(0x02);		//low column start address
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
void OLED_CLS(void)//����
{
	OLED_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED�������л���
//--------------------------------------------------------------
void OLED_ON(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X14);  //������ɱ�
	WriteCmd(0XAF);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : ��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
//--------------------------------------------------------------
void OLED_OFF(void)
{
	WriteCmd(0X8D);  //���õ�ɱ�
	WriteCmd(0X10);  //�رյ�ɱ�
	WriteCmd(0XAE);  //OLED����
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); ch[] -- Ҫ��ʾ���ַ���; TextSize -- �ַ���С(1:6*8 ; 2:8*16)
// Description    : ��ʾcodetab.h�е�ASCII�ַ�,��6*8��8*16��ѡ��
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
// Parameters     : x,y -- ��ʼ������(x:0~127, y:0~7); N:������codetab.h�е�����
// Description    : ��ʾcodetab.h�еĺ���,16*16����
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
// Parameters     : x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
// Description    : ��ʾBMPλͼ
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
