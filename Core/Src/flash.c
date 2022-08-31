/*
 * Project: Voice Reminder
 * Author: Wei Lee, kevin_jia@foxmail.com
 * Copyright: (C) 2022 by Wei
 * License: GNU GPL v3 (see License.txt)
 */

#include "flash.h"
#include "typedefs.h"
#include <string.h>
//FLASH�Ĵ�С��Ҳ�ɶ�*(uint16_t*)0x1FFFF7E0ֱ�ӻ��;

#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 								//�ֽ�
#else
#define STM_SECTOR_SIZE	2048
#endif

uint16_t STMFLASH_BUF[STM_SECTOR_SIZE/2];		//�����2K�ֽ�

static FLASH_EraseInitTypeDef EraseInitStruct;



//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ
//����ֵ:��Ӧ����.
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr)
{
	return *(volatile uint16_t*)faddr;
}
#if STM32_FLASH_WREN	//���ʹ����д
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);//ÿ��д�����
	    WriteAddr+=2;//��ַ����2.
	}
}

//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	uint32_t secpos;	//������ַ
	uint16_t secoff;	//������ƫ�Ƶ�ַ(16λ�ּ���)
	uint16_t secremain; //������ʣ���ַ(16λ�ּ���)
 	uint16_t i;
	uint32_t offaddr;   //ȥ��0X08000000��ĵ�ַ

	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//�Ƿ���ַ

	HAL_FLASH_Unlock();					    							//����
	offaddr=WriteAddr-STM32_FLASH_BASE;						//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;								//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;						//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;						//����ʣ��ռ��С
	if(NumToWrite<=secremain)
		secremain=NumToWrite;												//�����ڸ�������Χ
	while(1)
	{
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)	//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����
		}
		if(i<secremain)				//��Ҫ����
		{
			FLASH_PageErase(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);	//�����������
			FLASH_WaitForLastOperation(FLASH_WAITETIME);            	//�ȴ��ϴβ������
			CLEAR_BIT(FLASH->CR, FLASH_CR_PER);							//���CR�Ĵ�����PERλ���˲���Ӧ����FLASH_PageErase()����ɣ�
																		//����HAL�����沢û������Ӧ����HAL��bug��
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������
		}
		else
		{
			FLASH_WaitForLastOperation(FLASH_WAITETIME);       	//�ȴ��ϴβ������
			STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
		}
		if(NumToWrite==secremain)
			break;//д�������
		else//д��δ����
		{
			secpos++;				 //������ַ��1
			secoff=0;				 //ƫ��λ��Ϊ0
		   	pBuffer+=secremain;  	 //ָ��ƫ��
			WriteAddr+=secremain*2;	 //д��ַƫ��(16λ���ݵ�ַ,��Ҫ*2)
		   	NumToWrite-=secremain;	 //�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}
	};
	HAL_FLASH_Lock();		//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)
{
	uint16_t i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.
	}
}

//������///
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(uint32_t WriteAddr,uint16_t WriteData)
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ����
}

void writeFlash(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
//	//step 1. ��ȡflash����
//	static driverInfo_s testFlash;
//	static int ret = 0;
//	STMFLASH_Read(WriteAddr, (uint16_t *)&testFlash, NumToWrite);
//	//step 2. �Ƚ϶�ȡ��д�������Ƿ�һ��
//	ret = memcmp(pBuffer, (uint16_t *)&testFlash, NumToWrite);
//	//step 3. д�����ݻ����˳�
//	if(ret)			//ret = 0 �����ڴ��������,������ٴ洢
		STMFLASH_Write(WriteAddr, pBuffer, NumToWrite);
}
void readFlash(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead)
{
	STMFLASH_Read(ReadAddr, pBuffer, NumToRead);
}
uint32_t PAGEError = 0;
void eraseFlash(void)
{
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = START_FLASH_ADDRESS;
	EraseInitStruct.NbPages     = (END_FLASH_ADDRESS - START_FLASH_ADDRESS) / STM_SECTOR_SIZE;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
  	{
    /*
      Error occurred while page erase.
      User can add here some code to deal with this error.
      PAGEError will contain the faulty page and then to know the code error on this page,
      user can call function 'HAL_FLASH_GetError()'
    */
  	}
}
void wFlash(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite)
{
	HAL_FLASH_Unlock();
	eraseFlash();
	uint16_t i;
	for(i=0;i<NumToWrite;i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,WriteAddr,pBuffer[i]);//ÿ��д�����
	    WriteAddr+=2;//��ַ����2.
	}
	HAL_FLASH_Lock();
}

