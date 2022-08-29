#include "pcf8563.h"
#include "i2c.h"
#include "time.h"
#include "common.h"
//#include "oled_i2c.h"

Calendar_OBJ calendar;

extern I2C_HandleTypeDef hi2c1;

unsigned char g8563_Store[6];//={0x22,0x06,0x02,0x21,0x40,0x00}; /*ʱ�佻����,ȫ�ֱ�������*/
unsigned char c8563_Store[6]={0x22,0x06,0x19,0x21,0x56,0x00}; /*д��ʱ���ֵ������һ 07:59:00*/

//������ת��Ϊѹ��BCD��ʽ,���֧��99
static unsigned int DECtoBCD(unsigned int DEC) 
{
	return ((unsigned int)(DEC/10)<<4)+(DEC%10);
}

//��ѹ��BCDתΪDEC,���֧��99
static unsigned int BCDtoDEC(unsigned int BCD)
{
	return (unsigned int)(BCD>>4)*10+(BCD&0x0f);
}

/********************************************
�ڲ�����.��������ֽ�
���:B=����
********************************************/
//void writebyte(unsigned char wdata)
//{
//		unsigned char i;
//   		for(i=0;i<8;i++)
//   		{
//        		if(wdata&0x80) 
//						SDA=1;
//       			else 
//						SDA=0;
//       			wdata<<=1;
//       			SCL=1;
//       			Delay();
//       			SCL=0;
//   		}
//   		WaitACK();     //I2C������ͨѶ���������˳�I2CͨѶ
//}
/********************************************
�ڲ�����.��������
����:B
********************************************/
//unsigned char Readbyte()
//{
//		unsigned char i,bytedata;
//   		SDA=1;
//   		for(i=0;i<8;i++)
//   		{
//      			SCL=1; 
//      			bytedata<<=1;
//      			bytedata|=SDA;
//      			SCL=0;
//      			Delay();
//   		}
//   		return(bytedata);
//}
/********************************************
�������->pcf8563
********************************************/
int writeData(unsigned char address,unsigned char mdata)
{
//	Start();
//	writebyte(0xa2); /*д����*/
//	writebyte(address); /*д��ַ*/
//	writebyte(mdata); /*д����*/
//	Stop();
//	if(READ_BIT(hi2c1.Instance->SR2,I2C_SR2_BUSY) || READ_BIT(hi2c1.Instance->CR1,I2C_CR1_PE) == 0){
//		I2C_Busy_C();
//		return 0;
//	}
//	else
	{
		if(HAL_I2C_Mem_Write(&hi2c1, 0xA2, address, I2C_MEMADD_SIZE_8BIT, &mdata, 1, 0x100) == HAL_OK)
		{
			return 1;
		}
	}
	return 1;
}
/********************************************
��������<-pcf8563
********************************************/
/*unsigned char ReadData(unsigned char address) //���ֽ�
{  
		unsigned char rdata;
   		Start();
	    writebyte(0xa2); //д����
	    writebyte(address); //д��ַ
	    Start();
	    writebyte(0xa3); //������
	    rdata=Readbyte();
	    WriteACK(1);
	    Stop();
	    return(rdata);
}	*/
int ReadData(unsigned char address,unsigned char count,uint8_t * buff) /*���ֽ�*/
{  
//	unsigned char i;
//	Start();
//	writebyte(0xa2); /*д����*/
//	writebyte(address); /*д��ַ*/
//	Start();
//	writebyte(0xa3); /*������*/
//	for(i=0;i<count;i++)
//	{
//		buff[i]=Readbyte();
//		if(i<count-1) 
//			WriteACK(0);
//	}
//	WriteACK(1);
//	Stop();
//	if(READ_BIT(hi2c1.Instance->SR2,I2C_SR2_BUSY) || READ_BIT(hi2c1.Instance->CR1,I2C_CR1_PE) == 0){
//		I2C_Busy_C();
//		return 0;
//	}
//	else
	{
		if(HAL_I2C_Mem_Read(&hi2c1, 0xA3, address, I2C_MEMADD_SIZE_8BIT, buff, count, 0x100) == HAL_OK)
		{
			return 1;
		}
	//	else//HAL_I2C_Mem_Read return HAL_BUSY
	//	{
	//		__set_FAULTMASK(1);
	//		NVIC_SystemReset();
	////		__HAL_RCC_I2C1_FORCE_RESET();
	////		HAL_Delay(2);
	////		__HAL_RCC_I2C1_RELEASE_RESET();
	////		HAL_I2C_MspInit(&hi2c1);
	////		__HAL_RCC_I2C1_CLK_ENABLE();
	//	}
	}
	return 1;
}  
/********************************************
�ڲ�����,����ʱ�䵽�ڲ�������
********************************************/
void pcf8563_Read()
{   
	uint8_t time[7];
	ReadData(0x02,0x07,time);
	
	calendar.sec = time[0]&0x7f; /*�� */
	timeDate.time.secs=BCDtoDEC(calendar.sec);
	calendar.min = time[1]&0x7f; /*�� */
	timeDate.time.mins = BCDtoDEC(calendar.min);
	calendar.hour = time[2]&0x3f; /*Сʱ */
	timeDate.time.hour = BCDtoDEC(calendar.hour);
	calendar.w_date = time[3]&0x3f; /*�� */
	timeDate.date.date = BCDtoDEC(calendar.w_date);
	calendar.w_month = time[5]&0x1f; /*�� */
	timeDate.date.month = (month_t)BCDtoDEC(calendar.w_month);
	calendar.w_year = time[6];			 /*��  */
	timeDate.date.year = BCDtoDEC(calendar.w_year);
	
	calendar.week = time[4]&0x07;;
	timeDate.date.day = (day_t)BCDtoDEC(calendar.week);
	
}
/********************************************
����ʱ�䵽�ڲ�������----�ⲿ���� 
********************************************/
void pcf8563_gettime(void)
{
	pcf8563_Read();
	if(g8563_Store[0]==0)
		pcf8563_Read(); /*���Ϊ��=0��Ϊ��ֹʱ��仯���ٶ�һ��*/
}	
/********************************************
дʱ���޸�ֵ
********************************************/
void pcf8563_settime()
{
	//unsigned char i;
	writeData(8,g8563_Store[0]); //�� 
	writeData(7,g8563_Store[1]); //�� 
	writeData(5,g8563_Store[2]); //�� 
	writeData(4,g8563_Store[3]); //ʱ 
	writeData(3,g8563_Store[4]); //��  
	writeData(2,g8563_Store[5]); //�� 
}
/********************************************
PCF8563�ĳ�ʼ��-----�ⲿ����
********************************************/
void pcf8563_init()
{
	unsigned char i;
	// P8563_settime();
	for(i=0;i<=5;i++) 
		g8563_Store[i]=c8563_Store[i]; /*��ʼ��ʱ��*/
  pcf8563_settime();  
		 // if((ReadData(0x0a)&0x3f)!=0x08) /*����Ƿ��һ�������������ʼ��ʱ��*/
		 // {
			//	    P3_4 = 0;
		 //       for(i=0;i<=3;i++) g8563_Store[i]=c8563_Store[i]; /*��ʼ��ʱ��*/
		 //       P8563_settime();
		 //       writeData(0x0,0x00);
		 //       writeData(0xa,0x8); /*8:00����*/
		 //       writeData(0x1,0x12); /*������Ч*/
			//      writeData(0xd,0xf0);  //������32.768K��Ƶ��
			//  }
}

void Date_init(void)
{
	timeDate.date.date = 27;
	timeDate.date.day = (day_t)3;//week
	timeDate.date.month = (month_t)7;
	timeDate.date.year = 22;
}

void Set_PCF8563_Time(void)
{
	u8 temp=0;
	temp=DECtoBCD(timeDate.time.hour);
	writeData(4,temp); //ʱ
	//DS3231_WR_Byte(0x02,temp);

	temp=DECtoBCD(timeDate.time.mins);
	writeData(3,temp); //�� 
	//DS3231_WR_Byte(0x01,temp);

	temp=DECtoBCD(timeDate.time.secs);
	writeData(2,temp); //�� 
	//DS3231_WR_Byte(0x00,temp);  	
}


void Set_PCF8563_date(void)
{
	u8 temp=0;

	temp=DECtoBCD(timeDate.date.year);
	writeData(8,temp); //��
	//DS3231_WR_Byte(0x06,temp);

	temp=DECtoBCD(timeDate.date.month);
	writeData(7,temp); //�� 
	//DS3231_WR_Byte(0x05,temp);

	temp=DECtoBCD(timeDate.date.date);
	writeData(5,temp); //�� 
	//DS3231_WR_Byte(0x04,temp);

	temp=DECtoBCD(timeDate.date.day);
	//DS3231_WR_Byte(0x03,temp);
	writeData(6,temp); //�ܼ� 
	
}
//static unsigned char GetWeek(unsigned int year,unsigned char month,unsigned char day)
//{   
//    //���������ݱ�
//    static unsigned char const table_week[12] = {0,3,3,6,1,4,6,2,5,0,3,5};  
//    unsigned int temp2;
//    unsigned char yearH,yearL;
//    yearH = year / 100; 
//    yearL = year % 100; 
//    // ���Ϊ21����,�������100  
//    if(yearH > 19) 
//        yearL += 100;
//    // ����������ֻ��1900��֮���  
//    temp2 = yearL + yearL / 4;
//    temp2 = temp2 % 7; 
//    temp2 = temp2 + day + table_week[month-1];
//    if(yearL % 4 == 0 && month < 3)
//        temp2--;
//    temp2 %= 7;
//    if(temp2 == 0)
//        temp2 = 7;
//    return temp2;
//}  

//bool PCF8563_SetTimer(unsigned int year, unsigned char mon, unsigned char day,
//                    unsigned char hour, unsigned char min, unsigned char sec)
//{
//    int fd = 0;
//    unsigned char data[8]= {0}; 

//    fd= open("/dev/i2c-0", O_RDWR);
//    if(fd < 0)
//    {
//        printf(RED"RTC2 fd open fail.\n"GRAY);plog("RTC fd open fail.\n");
//        return FALSE;
//    }
//    ioctl(fd, I2C_SLAVE, (0xA2 >> 1));//0x0703

//    if(year < 2000) year = 2000;
//    if(year > 2099) year = 2099;
//    
//    data[0] = 0x02;
//    data[1] = DECtoBCD(sec);    //��
//    data[2] = DECtoBCD(min);    //��
//    data[3] = DECtoBCD(hour);   //Сʱ
//    data[4] = DECtoBCD(day);   //��
//    data[5] = GetWeek(year, mon, day)-1; //����
//    year -= 2000;
//    data[6] = DECtoBCD(mon);  //��
//    data[7] = DECtoBCD(year);   //��
//    // printf("%02d-%02d-%02d %02d:%02d:%02d  wk=%d\n", 
//    //     data[7],data[6],data[4],data[3],data[2],data[1],data[0],data[5]);
//    write(fd,&data, 8);
//    close(fd);
//    return TRUE;
//}
//// bool PCF8563_GetTimer(unsigned int year, unsigned int month, unsigned int date,
////                     unsigned int hour, unsigned int min, unsigned int sec)
//bool PCF8563_GetTimer(void)//ȡ��RTCʱ�䣬������Ϊϵͳʱ��
//{
//    int fd, retry = 0, reg_addr = 2;
//    unsigned char buf, i = 0;
//    unsigned char addr = 0x02, rtc_time[7] = {0}, temp = 0;//
//    char time[TIMEARRAYLENTH];

//    // TUint64 begintime = 0, endtime = 0;
//    // struct timeval tv;
//    // gettimeofday(&tv,NULL);
//    // begintime = (tv.tv_sec * 1000 * 1000 + tv.tv_usec);//��ȡʱ�䵽����
//    pthread_mutex_lock(&i2cmut);//i2c�����û������
//    fd= open("/dev/i2c-0", O_RDWR);
//    if(fd < 0)
//    {
//        printf(RED"RTC1 fd open fail.\n"GRAY);
//        //plog("RTC fd open fail.\n");
//        pthread_mutex_unlock(&i2cmut);
//        return FALSE;
//    }
//    ioctl(fd, I2C_SLAVE, (0xA2 >> 1));//I2C_SLAVE = 0x0703
//    for(retry = 0; retry < 1; retry++)
//    {
//        write(fd,&reg_addr, 1); //pcf8563��02��Ĵ���  
//        read(fd,&rtc_time, 7);         
//        if(rtc_time[0] & 0x80)//ʱ��ΪĬ��ֵ����Ϊ��Ч
//        {
//            //printf("%02X\n", rtc_time[0]);
//            printf(RED"RTC time not initialized.\n"GRAY);
//            //plog("RTC time not initialized.\n");
//            //To do SetTimer
//            PCF8563_SetTimer(2019,11,23,16,17,20);
//            close(fd);
//            //sleep(1);
//            pthread_mutex_unlock(&i2cmut);
//            return FALSE;           
//        }
//        else
//        {
//            temp = BCDtoDEC(rtc_time[0]&0x7F);
//            if(temp > 59) continue;             //���ӷ�Χ����
//            ext_timer.sec = temp;
//            temp = BCDtoDEC(rtc_time[1]&0x7F);
//            if(temp > 59) continue;             //���ӷ�Χ����
//            ext_timer.min = temp;
//            temp = BCDtoDEC(rtc_time[2]&0x3F);
//            if(temp > 23) continue;             //Сʱ��Χ����
//            ext_timer.hour = temp;
//            temp = BCDtoDEC(rtc_time[3]&0x3F);
//            if(temp > 31||temp==0) continue;    //���ڷ�Χ����
//            ext_timer.day = temp;
//            temp = BCDtoDEC(rtc_time[4]&0x07);
//            if(temp > 6) continue;              //���ڷ�Χ����
//            ext_timer.week = temp+1;
//            temp = BCDtoDEC(rtc_time[5]&0x1F);
//            if(temp > 12 || temp == 0) continue;    //�·ݷ�Χ����
//            ext_timer.mon = temp;
//            ext_timer.year = BCDtoDEC(rtc_time[6])+2000;
//            printf(GREEN"RTC=%04d-%02d-%02d %02d:%02d:%02d\n"GRAY, ext_timer.year, ext_timer.mon, 
//            ext_timer.day, ext_timer.hour, ext_timer.min, ext_timer.sec);
//            snprintf(time, sizeof(time), "%04d-%02d-%02d %02d:%02d:%02d\n", ext_timer.year, ext_timer.mon, 
//            ext_timer.day, ext_timer.hour, ext_timer.min, ext_timer.sec);
//            // gettimeofday(&tv,NULL);
//            // endtime = (tv.tv_sec * 1000 * 1000 + tv.tv_usec);//��ȡʱ�䵽���� 
//            // printf(RED"dlT:%lld-%lld=%lld\n"GRAY,  endtime, begintime, endtime - begintime);
//            SetLocalTime(time);
//            close(fd);
//            pthread_mutex_unlock(&i2cmut);
//            return TRUE;
//        }
//    }
//    close(fd);
//    sleep(1);
//    pthread_mutex_unlock(&i2cmut);
//    return FALSE;      
//}

