#ifndef __JQ8x00_H
#define __JQ8x00_H
#include "stm32f1xx_hal.h"
//#include "delay.h"
#include <string.h>
#include "usart.h"

/**********************�궨����**************************/
#define JQ8x00_Workmode		0		//ģ�鹤����ʽ��0��ʾ���ڿ��ƣ�1���߿���
#define JQ8x00_BusyCheck	1		//æ��⣬0��ʾ����æ��⣬1����æ���

//��ʱ�����ض���
#define JQ8x00_ms(ms)    HAL_Delay(ms)         //JQ8x00��ʱ�������壬�û����и���Ϊ�Լ�����ʱ����
#define JQ8x00_us(us)    delay_us(us)

//���ں����ض���
#define JQ8x00_UART(pointer,len) HAL_UART_Transmit(&huart2,pointer,len,0xffff)        //pointerΪָ�����ݴ��������׵�ַ��lenΪҪ�������ݸ���

//��ϲ����������,��Ҫ���ù��������ջ���
#define ZH_MAX	50		

/*************************IO�˿ں궨�� ����IO�˿����޸�����Ķ���*********************************/
////���ߴ���IO����
//#define JQ8x00_VPP_RCC           __HAL_RCC_GPIOA_CLK_ENABLE        //ʱ��
//#define JQ8x00_VPP_GPIO         GPIOA
//#define JQ8x00_VPP_GPIO_Pin     GPIO_PIN_1                 //IO

////���߿���IO��ƽ���ã���������
//#define JQ8x00_VPP_Clr()	 HAL_GPIO_WritePin(JQ8x00_VPP_GPIO,JQ8x00_VPP_GPIO_Pin,GPIO_PIN_RESET)
//#define JQ8x00_VPP_Set()	 HAL_GPIO_WritePin(JQ8x00_VPP_GPIO,JQ8x00_VPP_GPIO_Pin,GPIO_PIN_SET)

//æ���IO����
#define JQ8x00_BUSY_RCC          __HAL_RCC_GPIOB_CLK_ENABLE       //ʱ��
#define JQ8x00_BUSY_GPIO         GPIOB
#define JQ8x00_BUSY_GPIO_Pin     GPIO_PIN_12                 			//IO

//BUSY��ƽ��ȡ
#define JQ8x00_BUSY_Read  HAL_GPIO_ReadPin(JQ8x00_BUSY_GPIO,JQ8x00_BUSY_GPIO_Pin)		//��ȡ��ƽ

typedef enum {
    OverallCycle                = 0X00,         /*ȫ��ѭ�� ��˳�򲥷�ȫ����Ŀ,�������ѭ������*/
    SingleCycle                 = 0x01,         /*����ѭ�� һֱѭ�����ŵ�ǰ��Ŀ*/
    SingleStop                  = 0x02,         /*����ֹͣ �����굱ǰ��Ŀһ��ֹͣ*/
    OverallRandom               = 0X03,         /*ȫ����� ��������̷�����Ŀ*/
    CatalogCycle                = 0X04,          /*Ŀ¼ѭ�� ��˳�򲥷ŵ�ǰ�ļ�������Ŀ,�������ѭ�����ţ�Ŀ¼��������Ŀ¼*/
    CatalogRandom               = 0x05,          /*Ŀ¼��� �ڵ�ǰĿ¼��������ţ�Ŀ¼��������Ŀ¼*/
    CatalogTurnPlay             = 0x06,         /*Ŀ¼˳�򲥷� ��˳�򲥷ŵ�ǰ�ļ�������Ŀ,�������ֹͣ��Ŀ¼��������Ŀ¼*/
    OverallTurnPlay             = 0x07,         /*ȫ��˳�򲥷� ��˳�򲥷�ȫ����Ŀ,�������ֹͣ*/ 
}LoopModeSelect;      //ѭ��ģʽѡ��

#if JQ8x00_Workmode
/**********************���߿���**************************/
typedef enum {
    Play 			= 0x11,					/*����*/
    Pause			= 0x12,					/*��ͣ*/
    Stop			= 0x13,					/*ֹͣ*/
    LastSong  = 0x14,					/*��һ��*/
    NextSong	= 0x15,					/*��һ��*/
    LastList	= 0x16,					/*��һĿ¼*/
    NextList	= 0x17,					/*��һĿ¼*/
    ChooseSD	= 0x18,					/*ѡ��SD��*/
    ChooseUdisk	= 0x19,					/*ѡ��U��*/
    ChooseFlash	= 0x1a,					/*ѡ��Flash*/
    SysSleep	= 0x1b,					/*ϵͳ˯��*/
} LineByteSelect;

typedef enum {
    Track 		= 0x0B,					/*��Ŀ*/
    Volume		= 0x0C,					/*����*/
    EQ			= 0x0D,					/*EQ*/
    LoopMode    = 0x0E,					/*ѭ��ģʽ*/
    Channel		= 0x0F,					/*ͨ��*/
    CBTrack		= 0x10,					/*�����Ŀ*/
} LineModeSelect;

void OneLine_ByteControl(LineByteSelect Mode);
void OneLine_ZHControl(uint8_t Nume,LineModeSelect Mode);
void OneLine_SendData(uint8_t DATA);

#else
/**********************���ڿ���**************************/
typedef enum {
    CheckPlayState                  = 0x01,					/*��ѯ����״̬*/
    Play                            = 0x02,					/*����*/
    pause                           = 0x03,					/*��ͣ*/
    Stop                            = 0x04,					/*ֹͣ*/
    LastSong                        = 0x05,					/*��һ��*/
    NextSong                        = 0x06,					/*��һ��*/   
    CheckOnelineDisksign	        = 0x09,				    /*��ѯ��ǰ�����̷�*/
    CheckCurrentDisksign	        = 0X0A,				    /*��ѯ��ǰ�����̷�*/
    CheckTotalTrack                 = 0x0C,                 /*��ѯ����Ŀ*/
    CurrentTrack                    = 0x0D,                 /*��ǰ��Ŀ*/
    LastFloder                      = 0x0E,                 /*��һ���ļ���Ŀ¼*/
    NextFloder                      = 0x0F,                 /*��һ���ļ���Ŀ¼*/          
    EndPlay	                        = 0x10, 				/*��������*/
    CheckFloderFirstTrack           = 0x11,                 /*��ѯ�ļ�Ŀ¼����Ŀ*/
    CheckFloderAllTrack             = 0x12,                 /*��ѯ�ļ�Ŀ¼����Ŀ*/
    AddVolume                       = 0x14,                 /*������*/
    DecVolume                       = 0x15,                 /*������*/
    EndZHPlay                       = 0X1C,                 /*������ϲ���*/ 
    CheckSongShortName	            = 0x1E,					/*��ѯ�������ļ���*/
    EndLoop                         = 0x21,                 /*��������*/
    GetTotalSongTime                = 0x24,                 /*��ȡ��ǰ��Ŀ��ʱ��*/
    OpenPlayTime                    = 0x25,                 /*����ʱ�俪����*/                
    ClosePlayTime                   = 0x26,                 /*�رղ���ʱ�䷢��*/
}UartCommand;     //�����ݵ�ָ��,��ʼ��-ָ������-���ݳ���-У���

typedef enum {
    AppointTrack                    = 0x07,				    /*ָ����Ŀ����*/
    SetCycleCount                   = 0x19,                 /*����ѭ������*/
    SetEQ                           = 0X1A,                 /*EQ����*/
    SelectTrackNoPlay               = 0x19,                 /*ѡ��������*/
    GoToDisksign                    = 0X0B,                 /*�л�ָ���̷�*/
    SetVolume                       = 0x13,                 /*��������*/
    SetLoopMode                     = 0x18,                 /*����ѭ��ģʽ*/
    SetChannel                      = 0x1D,                 /*����ͨ��*/ 
    AppointTimeBack                 = 0x22,                 /*ָ��ʱ�����*/
    AppointTimeFast                 = 0x23,                 /*ָ��ʱ�����*/
}UartCommandData;       //����������ݵ�ָ��,��ʼ��-ָ������-���ݳ���-����1-...-У���

typedef enum {
    JQ8X00_USB                      = 0X00,                 /*UPANND*/
    JQ8X00_SD                       = 0x01,                 /*SD*/
    JQ8X00_FLASH                    = 0x02,                 /*FLASH*/
}JQ8X00_Symbol;       //ϵͳ�̷�


void JQ8x00_Command(UartCommand MODE);
void JQ8x00_Command_Data(UartCommandData MODE,uint8_t DATA);
void JQ8x00_ZuHeBoFang(uint8_t *DATA,uint8_t Len);
void JQ8x00_RandomPathPlay(JQ8X00_Symbol MODE,char *DATA);
#endif


void JQ8x00_Init(void);

#endif
