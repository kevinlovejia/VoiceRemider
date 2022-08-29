#include "DS3231.h"
#include "i2c_soft.h"
#include "delay.h"
#include "common.h"

Calendar_OBJ calendar;
#define DS3231_WriteAddress        0xD0  
#define DS3231_ReadAddress        0xD1

u8 test1,test2,test3;
u8 alarm1_flag;
u8 alarm_hour,alarm_min,alarm_sec,alarm_week;
extern alarm_s eepAlarms[ALARM_COUNT];

u8 BCD2HEX(u8 val)
{
    u8 i;
    i= val&0x0f;
    val >>= 4;
    val &= 0x0f;
    val *= 10;
    i += val;
    
    return i;
}
u16 B_BCD(u8 val)
{
    u8 i,j,k;
    i=val/10;
    j=val%10;
    k=j+(i<<4);
    return k;
}
void DS3231_WR_Byte(u8 addr,u8 bytedata)
{
    I2C_Start();
    I2C_SendByte(DS3231_WriteAddress);
    I2C_WaitAck();
    I2C_SendByte(addr);
    I2C_WaitAck();
    I2C_SendByte(bytedata);
    I2C_WaitAck();
    I2C_Stop();
}    
u8 DS3231_RD_Byte(u8 addr)
{
    u8 Dat=0;
  
    I2C_Start();
    I2C_SendByte(DS3231_WriteAddress);
    I2C_WaitAck();
    I2C_SendByte(addr);
    I2C_WaitAck();
    I2C_Start();
    I2C_SendByte(DS3231_ReadAddress);
    I2C_WaitAck();
    Dat=I2C_ReceiveByte();
    I2C_Stop();
    
    return Dat;
}

void DS3231_Init(void)
{
    I2C_GPIO_Config();
    DS3231_WR_Byte(0x0e,0x1f);
    delay_ms(2);
    DS3231_WR_Byte(0x0f,0x00);
    delay_ms(2);
	DS3231_WR_Byte(0x0e,0x20);//���ڴ��¶�ת����־
	delay_ms(2);
}

void Set_DS3231_Time(void)
{
    u8 temp=0;
    temp=B_BCD(timeDate.time.hour);
    DS3231_WR_Byte(0x02,temp);
  
    temp=B_BCD(timeDate.time.mins);
    DS3231_WR_Byte(0x01,temp);
  
    temp=B_BCD(timeDate.time.secs);
    DS3231_WR_Byte(0x00,temp);
    
    
}


void Set_DS3231_date(void)
{
	u8 temp=0;
 
    temp=B_BCD(timeDate.date.year);
    DS3231_WR_Byte(0x06,temp);
  
    temp=B_BCD(timeDate.date.month);
    DS3231_WR_Byte(0x05,temp);
  
    temp=B_BCD(timeDate.date.date);
    DS3231_WR_Byte(0x04,temp);
	
	temp=B_BCD(timeDate.date.day);
    DS3231_WR_Byte(0x03,temp);
}

void Date_init(void)
{
	timeDate.date.date=11;
	timeDate.date.day=5;
	timeDate.date.month=2;
	timeDate.date.year=22;
}

float DS3231_Temp;

void Get_DS3231_Time(void)
{
    calendar.w_year=DS3231_RD_Byte(0x06);  
    timeDate.date.year=BCD2HEX(calendar.w_year);
    calendar.w_month=DS3231_RD_Byte(0x05);
    timeDate.date.month=BCD2HEX(calendar.w_month);
    calendar.w_date=DS3231_RD_Byte(0x04);  
    timeDate.date.date=BCD2HEX(calendar.w_date);
    
    calendar.hour=DS3231_RD_Byte(0x02);
  //  calendar.hour&=0x3f;                  
    timeDate.time.hour=BCD2HEX(calendar.hour);
    calendar.min=DS3231_RD_Byte(0x01);
    timeDate.time.mins=BCD2HEX(calendar.min);
    calendar.sec=DS3231_RD_Byte(0x00);
    timeDate.time.secs=BCD2HEX(calendar.sec);
    
    calendar.week=DS3231_RD_Byte(0x03);
    timeDate.date.day=BCD2HEX(calendar.week);
	
	
    /*************��ȡ3231оƬ�¶�***************************************/
    
    calendar.temper_H=DS3231_RD_Byte(0x11);
    calendar.temper_L=(DS3231_RD_Byte(0x12)>>6)*25;
	DS3231_Temp=(float)calendar.temper_H+(float)calendar.temper_L/100;
}
/************************************************************************************************************/
//������   Ԥ�Ƹĳ����Ӹ�λ����
void DS3231Alarm_Config(void)
{
	
 //DS3231_WR_Byte(0x0e,0x1f);//alarm1 enable	
	//DS3231_WR_Byte(0x0f,0x00);//alarm1 enable		
} 
/*********************************************************************************************************************************************/
//�����趨�������ڲ����趨Ϊ����ʱ���֣���ƥ�����ӡ�
//��ѡ���Ƿ�ƥ������
//
//void Set_DS3231_alarm1(void)
//{
//	u8 temp_a=0;

///**************************����1����0~7λ�趨********************************************************************************/	
////0X0A�������ڼ���ʱ8λӦ���� x1xx xxxx
//// &0x0f��      //���ݱ�Ϊ��0000 xxxx//����0X0A���������ڼ���ʱֻ�е���λ��Ч����ͨ��&0x0f���θ���λ���趨����ͬʱ�ᵼ�¸���λΪ0����0X0A��6λΪ0ʱ���Ϊ�������Ӽ�����0X0A��7λΪ0���������°취
//// &0x0f|(1<<6);//���ݱ�Ϊ��0100 xxxx//��0X0A��6λ��1���򽫰��������Ӽ���������7λA1M4Ϊ1ʱ��λ��Ӱ�죬��ʱ����ֻ��������Ҳƥ���Ż���
//// &0x0f|(3<<6);//���ݱ�Ϊ��1100 xxxx//0X0A��7λA1M4Ϊ1����6λ������������ʧЧ��ֻҪʱ��ƥ�����Ӿͻ���
// 
//  	//temp_a=B_BCD(eepAlarms[0])&0x0f|(1<<6);
//    //DS3231_WR_Byte(0x0a,temp_a);
//	
///**********************************************************************************************************/	
////&0X3F  //���ݱ�Ϊ��00xx xxxx
////0X09�е�6λΪ0ʱ����24h�ƣ���7λA1M3Ϊ0ʱ,����A1M3
////  
//    temp_a=B_BCD(eepAlarms[0].hour)&0X3F;
//    DS3231_WR_Byte(0x09,temp_a);

////ͨ��&0x7f������ߵ���λ�ı�־λ,����A1M2
////���ݱ�Ϊ��0xxx xxxx
//    temp_a=B_BCD(eepAlarms[0].min)&0x7f;//
//    DS3231_WR_Byte(0x08,temp_a);
///**********************************************************************************************************/	  	
////ͨ��&0x7f������ߵ���λ�ı�־λ,����A1M1
////���ݱ�Ϊ��0xxx xxxx	

//   	temp_a=B_BCD(0)&0x7f;
//    DS3231_WR_Byte(0x07,temp_a);
//	  
//}
//void Get_DS3231_alarm1(void)//��ȡ����1������
//{
//	/**********���ӵ�����*************/ 
//	 test1=DS3231_RD_Byte(0x07);//test����������ʾ�Ĵ����е�ֵ��ת�����ƺ����ں˶�ʹ�ã��ɹ۲����޸ļĴ���������
//	 test2=DS3231_RD_Byte(0x0e);
//	 test3=DS3231_RD_Byte(0x0f);
//	/********************************/ 
//	
//	  calendar.alarm_week=DS3231_RD_Byte(0x0A)&0x0f;//��ȡ0X0A�е��������ݣ�����λΪ�������ݣ�ͨ��&�������θ���λ
//	  calendar.alarm_week=BCD2HEX(calendar.alarm_week);
//	 
//	
//	 
//	  calendar.alarm_hour=DS3231_RD_Byte(0x09);
//	  calendar.alarm_hour&=0x3f;	//����0X09�����λ������A1M3��־λ��
//    calendar.alarm_hour=BCD2HEX(calendar.alarm_hour);
//	
//	
//    calendar.alarm_min=DS3231_RD_Byte(0x08);
//    calendar.alarm_min=BCD2HEX(calendar.alarm_min);
//    calendar.alarm_sec=DS3231_RD_Byte(0x07);
//    calendar.alarm_sec=BCD2HEX(calendar.alarm_sec);
//	
//	
//   
// }
//void get_alarm1_status(void)//���0X0F���ӱ�־λA1F�Ƿ񴥷�����������ʾ1������Ĭ��Ϊ0
// {
//	 u8 a;
//	 a=DS3231_RD_Byte(0x0f);
//	 if(a&0x01)
//		 alarm1_flag=1;
//	 else 
//		 alarm1_flag=0;
//		 
// }
//void enable_alarm1(void)
//{
//	 
//	DS3231_WR_Byte(0x0E,0x1f);
//}
//void reset_alarm1(void)
//{
//	u8 a;
//	a=DS3231_RD_Byte(0x0E);
//	DS3231_WR_Byte(0x0E,a&0xfe);
//	 a=DS3231_RD_Byte(0x0f);
//	DS3231_WR_Byte(0x0f,a&0xfe);
//}
