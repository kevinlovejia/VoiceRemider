#include "common.h"


/************************************************************************
功能描述：模块IO初始化函数
入口参数： 无
返 回 值： none
其他说明： 如果采用单线控制JQ8x00_Workmode的值改为1
**************************************************************************/
void JQ8x00_Init()
{
	/* 定义IO硬件初始化结构体变量 */
	GPIO_InitTypeDef GPIO_InitStruct;		//这里如果忙检测和单线功能都没有开启，编译器提示一个声明未使用的警告，可以将这行代码屏蔽或者不用管

	#if JQ8x00_BusyCheck
	JQ8x00_BUSY_RCC();

	GPIO_InitStruct.Pin  = JQ8x00_BUSY_GPIO_Pin;			//JQ8X00忙检测
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 					//设置为输入模式
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;	 						//下拉输入 
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;		 	//IO口速度为低速
	HAL_GPIO_Init(JQ8x00_BUSY_GPIO, &GPIO_InitStruct);
	#endif
	
	#if JQ8x00_Workmode
	JQ8x00_VPP_RCC();																	//使能时钟

	GPIO_InitStruct.Pin  = JQ8x00_VPP_GPIO_Pin;				//JQ8x00单线控制IO配置
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; 		 	//推挽输出
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(JQ8x00_VPP_GPIO, &GPIO_InitStruct);	//根据设定参数初始化GPIOA.1
	HAL_GPIO_WritePin(JQ8x00_VPP_GPIO,JQ8x00_VPP_GPIO_Pin,GPIO_PIN_SET);						 	//VPP口输出 输出高
	#endif
}


/************************条件编译**************************************/
#if JQ8x00_Workmode						//控制方式选择

/******************************单线控制********************************/

/************************************************************************
功能描述：	单线控制单字节发送函数
入口参数： 	DATA：发送数据
返 回 值： none
其他说明： 将需要发送的数据作为形参传入
**************************************************************************/
void OneLine_SendData(uint8_t DATA)
{
	uint8_t i;
	
	JQ8x00_VPP_Set();		//开始信号
	JQ8x00_us(100);
	JQ8x00_VPP_Clr();
	JQ8x00_ms(3);
	
	for(i=0;i<8;i++)
	{
		JQ8x00_VPP_Set();
		if(DATA & 0x01)
		{
			JQ8x00_us(1500);
			JQ8x00_VPP_Clr();
			JQ8x00_us(500);
		}
		else
		{
			JQ8x00_us(500);
			JQ8x00_VPP_Clr();
			JQ8x00_us(1500);
		}
		DATA >>= 1;
	} 
	
	JQ8x00_VPP_Set();
}

/************************************************************************
功能描述：	单线字节控制
入口参数： 	Mode：功能
返 回 值： none
其他说明： 将需要的功能作为形参传入
**************************************************************************/
void OneLine_ByteControl(LineByteSelect Mode)
{
	#if JQ8x00_BusyCheck
	while(JQ8x00_BUSY_Read);				//忙检测
	#endif
	OneLine_SendData(Mode);
}

/************************************************************************
功能描述：	单线控制组合发送函数
入口参数： 	Nume：数字，Mode：功能
返 回 值： none
其他说明： 将需要发送的数据和需要的功能作为形参传入
**************************************************************************/
void OneLine_ZHControl(uint8_t Nume,LineModeSelect Mode)
{
	#if JQ8x00_BusyCheck
	while(JQ8x00_BUSY_Read);				//忙检测
	#endif
	OneLine_SendData(0x0a);
	if(Nume < 10)
	{
		OneLine_SendData(Nume);
	}
	else
	{
		OneLine_SendData(Nume/10);
		OneLine_SendData(Nume%10);
	}
	OneLine_SendData(Mode);
}

#else

/**************************串口控制************************************/

/************************************************************************
功能描述：组合播报函数
入口参数： 	*DAT：字符串指针,Len字符串长度
返 回 值： none
其他说明： 将需要播报的文件名放入数组中作为形参即可
**************************************************************************/
void  JQ8x00_ZuHeBoFang(uint8_t *DATA,uint8_t Len)
{
	uint8_t CRC_data=0,i = 3;
	uint8_t Buffer[ZH_MAX] ={0xaa,0x1b};
	Buffer[2] = Len*2;			//计算长度
	CRC_data = CRC_data + 0xaa + 0x1b + Buffer[2];
	while(Len--)
	{
		Buffer[i] = *DATA/10+0x30;			//取出十位
		CRC_data = CRC_data + Buffer[i];
		i++;
		Buffer[i] = *DATA%10+0x30;			//取出个位
		CRC_data = CRC_data + Buffer[i];
		i++;
		DATA++;
	}
	Buffer[i] = CRC_data;
	i++;

	JQ8x00_UART(Buffer,i);
}

/************************************************************************
功能描述：起始码-指令类型-数据长度-校验和
入口参数： 	MODE：模式
返 回 值： none
其他说明： 无数数据传入
**************************************************************************/
void  JQ8x00_Command(UartCommand Command)
{
	uint8_t Buffer[4] ={0xaa};
	int cnt = 0;
    Buffer[1] = Command;            //指令类型			
    Buffer[2] = 0x00;           //数据长度
    Buffer[3] = Buffer[0] +  Buffer[1] +  Buffer[2];            //校验和

	#if JQ8x00_BusyCheck
	while(JQ8x00_BUSY_Read)				//忙检测
	{
		HAL_Delay(1);
		if(cnt++ > 1500)				//等待超过1.5s自动退出不再等待
			break;
	}
	#endif
	JQ8x00_UART(Buffer,4);
}

/************************************************************************
功能描述：起始码-指令类型-数据长度-数据-校验和
入口参数： 	*DAT：字符串指针,Len字符串长度
返 回 值： none
其他说明： 
**************************************************************************/
void  JQ8x00_Command_Data(UartCommandData Command,uint8_t DATA)
{
	uint8_t Buffer[6] ={0xaa};
	uint8_t DataLen = 0;
	Buffer[1] = Command;       //指令类型
	if((Command != AppointTrack) && (Command != SetCycleCount) && (Command != SelectTrackNoPlay) && (Command != AppointTimeBack) && (Command != AppointTimeFast))        //只含一个数据指令    
	{
			Buffer[2] = 1;			//数据长度
			Buffer[3] = DATA;       //数据
			Buffer[4] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3];            //校验和
			DataLen = 5;
	}
	else                                                                                        //含两个数据指令 
	{
			Buffer[2] = 2;			//数据长度
			Buffer[3] = DATA/256;       //数据
			Buffer[4] = DATA%256;       //数据
			Buffer[5] = Buffer[0] +  Buffer[1] +  Buffer[2] + Buffer[3] + Buffer[4];  
			DataLen = 6;
	}
    
//	#if JQ8x00_BusyCheck
//	while(JQ8x00_BUSY_Read);				//忙检测
//	#endif
	JQ8x00_UART(Buffer,DataLen);
}

/************************************************************************
功能描述：播放任意路径下的的音频文件
入口参数：JQ8X00_Symbol:系统盘符，*DATA:需要播放的音频文件路径
返 回 值： none
其他说明：播放说明例 如 ：/广告/小米手机.mp3,可以按如下格式
        /广告* /小米*???,“广告*”代表前两个字为“广告的文件夹”，*为通配符
        注意格式，下一个目录符号前要加*，如/测试1* /测试2* /00002*???
        JQ_8x00_RandomPathPlay(JQ8X00_FLASH,"广告* /小米")
        播放FLASH根目录下文件名为00001.mp3的音频，则传JQ_8x00_RandomPathPlay(JQ8X00_FLASH,"/00001");
**************************************************************************/
void JQ8x00_RandomPathPlay(JQ8X00_Symbol symbol,char *DATA)
{
    uint8_t Buffer[52] ={0xaa,0x08};
    uint8_t i,j;
    Buffer[2] = 1 + strlen(DATA) + 4;       	//长度,1为盘符，4为*？？？
    Buffer[3] = symbol;        								//盘符
    i = 4;
    while(*DATA)
    {
    	Buffer[i++] =  *DATA;
    	DATA++;
    }
    Buffer[i++] = '*';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    Buffer[i++] = '?';
    for(j=0;j<i;j++)
    {
        Buffer[i] = Buffer[i] + Buffer[j];      //计算校验合
    }
    i++;
    JQ8x00_UART(Buffer,i);
}
#endif
