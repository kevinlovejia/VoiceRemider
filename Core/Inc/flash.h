/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "main.h"

//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 			64 	 						//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 			1              	//ʹ��FLASHд��(0����ʹ�ܣ�1��ʹ��)
#define FLASH_WAITETIME  			50000          	//FLASH�ȴ���ʱʱ��
#define START_FLASH_ADDRESS		0x0800F800			//flash�����ַ
#define END_FLASH_ADDRESS     0x0800FC00

extern void    FLASH_PageErase(uint32_t PageAddress);

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 													//STM32 FLASH����ʼ��ַ

uint8_t STMFLASH_GetStatus(void);				  										//���״̬
uint8_t STMFLASH_WaitDone(uint16_t time);				  						//�ȴ���������
uint8_t STMFLASH_ErasePage(uint32_t paddr);			  						//����ҳ
uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint16_t dat);	//д�����
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		 						//��������
//void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
//uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);										//ָ����ַ��ʼ��ȡָ����������
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����
void writeFlash(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);
void readFlash(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);
void wFlash(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);
//����д��
void Test_Write(uint32_t WriteAddr,uint16_t WriteData);

#endif /* FLASH_H_ */

