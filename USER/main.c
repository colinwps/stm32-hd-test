//=============================================================================
//文件名称：main.h
//功能概要：STM32F103C8
//版权所有：
//版权更新：
//调试方式：
//=============================================================================

//头文件
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "stm32f10x_usart.h"
#include "stdio.h"


//函数声明
void GPIO_Configuration(void);
void Uart1_init(int Buad);//串口初始化
void USART_SendByte(USART_TypeDef* USARTx, char  str); //发送字符
void USART_SendString(USART_TypeDef* USARTx, char * str); //发送字符串
void send_pack(long int * sendbuf); //发送包
void USART1_IRQHandler(void); //接收数据
NVIC_InitTypeDef NVIC_InitStructure; //声明


long int  rxbuf[4];//定义存放十六进制数的接收数据包
long int  sedbuf[4];//定义存放十六进制数的发送数据包

int rxflag = 0;//接收标志位



//=============================================================================
//文件名称：Delay
//功能概要：延时
//参数说明：nCount：延时长短
//函数返回：无
//=============================================================================

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================
int main(void)
{
	  GPIO_Configuration();
	  Uart1_init(9600);


      

    while (1)
		{  //char ptr2 = {"hello"};
		   //USART_SendString(USART1,*ptr2)
		   PCout(13)=1;
		   Delay(0xfffff);
	       PCout(13)=0;
	       Delay(0xfffff);			
        }
}

//=============================================================================
//文件名称：GPIO_Configuration
//功能概要：GPIO初始化
//参数说明：无
//函数返回：无
//=============================================================================
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 						 
//=============================================================================
//LED -> PC13
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void Uart1_init(int Buad)
{
   GPIO_InitTypeDef gpioinitstructure;
	 USART_InitTypeDef USART_InitStructure;
	 
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	 //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	 
	
	//配置PA9  TX
	 
	 gpioinitstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 gpioinitstructure.GPIO_Pin = GPIO_Pin_9;
	 gpioinitstructure.GPIO_Speed = GPIO_Speed_50MHz;
	 
	 GPIO_Init(GPIOA, &gpioinitstructure);
	 
	 //PA10  RX
	 gpioinitstructure.GPIO_Mode = GPIO_Mode_IPU;
	 gpioinitstructure.GPIO_Speed = GPIO_Speed_50MHz;
 
	 gpioinitstructure.GPIO_Pin = GPIO_Pin_10;
	 
	 GPIO_Init(GPIOA, &gpioinitstructure);
 
	 //串口结构体初始化
	USART_InitStructure.USART_BaudRate = Buad;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
 
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	//NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
 
	 
	 USART_Init(USART1, &USART_InitStructure);
	 USART_Cmd(USART1, ENABLE);
}

//发送字符
void USART_SendByte(USART_TypeDef* USARTx, char  str)
{
 
		USART_SendData(USARTx, str);//发送单个字符
 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//判断是否发送完成
 
	
 
}
//发送字符串
void USART_SendString(USART_TypeDef* USARTx, char * str)
{
	while(*str != '\0')
	{
		USART_SendData(USARTx, *str);//发送单个字符
		str++;
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//判断是否发送完成
		//Delay_ms(500);
	}
 
}

void send_pack(long int * sendbuf)//发送包
{
	USART_SendByte(USART1, 0xFF);
	USART_SendByte(USART1,sendbuf[0]);
	USART_SendByte(USART1,sendbuf[1]);
	USART_SendByte(USART1,sendbuf[2]);
	USART_SendByte(USART1,sendbuf[3]);
	USART_SendByte(USART1, 0xFE);
 
	
}

//接收数据
void USART1_IRQHandler(void)
{
	
	/*定义静态变量记录接收状态*/
	static int rxstate;//接收状态  0--等待包头  1--接收数据   2--等待包尾
	static int rxdata;//串口数据暂存
	static int i;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//检测串口状态是否处于接收状态
	{
		rxdata = USART_ReceiveData(USART1);//将接收到的数据暂存到rxdata
		
		
		/*根据接收状态判定进入哪个条件判断*/
		if(rxstate == 0)
		{
			if(rxdata == 0xFF)//接收到包头，下一位是数据，状态置1
			{
				rxstate = 1;
 
			}
		}
		else if(rxstate == 1)//接收到的是数据
		{
			rxbuf[i] = rxdata;//将接收缓冲区数据存放到，接收数组
			i++;
			
			if(i > 4 | i == 4)//固定长度4个字节，接收到4个之后下一位默认是包尾
			{
				i = 0;
				rxstate = 2;
			}
		}
		else if(rxstate == 2)//检测到包尾
		{
			if(rxdata == 0xFE)
			{
				rxflag = 1;//接收标志置1，代表接收完成
				rxstate = 0;//接收状态置0，准备接收下一段数据的包头
			}
		
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//清空中断标志，每接收到一次数据进入一次中断
	
	}
 
 
}



