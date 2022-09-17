//=============================================================================
//�ļ����ƣ�main.h
//���ܸ�Ҫ��STM32F103C8
//��Ȩ���У�
//��Ȩ���£�
//���Է�ʽ��
//=============================================================================

//ͷ�ļ�
#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "stm32f10x_usart.h"
#include "stdio.h"


//��������
void GPIO_Configuration(void);
void Uart1_init(int Buad);//���ڳ�ʼ��
void USART_SendByte(USART_TypeDef* USARTx, char  str); //�����ַ�
void USART_SendString(USART_TypeDef* USARTx, char * str); //�����ַ���
void send_pack(long int * sendbuf); //���Ͱ�
void USART1_IRQHandler(void); //��������
NVIC_InitTypeDef NVIC_InitStructure; //����


long int  rxbuf[4];//������ʮ���������Ľ������ݰ�
long int  sedbuf[4];//������ʮ���������ķ������ݰ�

int rxflag = 0;//���ձ�־λ



//=============================================================================
//�ļ����ƣ�Delay
//���ܸ�Ҫ����ʱ
//����˵����nCount����ʱ����
//�������أ���
//=============================================================================

void Delay(uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


//=============================================================================
//�ļ����ƣ�main
//���ܸ�Ҫ��������
//����˵������
//�������أ�int
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
//�ļ����ƣ�GPIO_Configuration
//���ܸ�Ҫ��GPIO��ʼ��
//����˵������
//�������أ���
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
	 
	
	//����PA9  TX
	 
	 gpioinitstructure.GPIO_Mode = GPIO_Mode_AF_PP;
	 gpioinitstructure.GPIO_Pin = GPIO_Pin_9;
	 gpioinitstructure.GPIO_Speed = GPIO_Speed_50MHz;
	 
	 GPIO_Init(GPIOA, &gpioinitstructure);
	 
	 //PA10  RX
	 gpioinitstructure.GPIO_Mode = GPIO_Mode_IPU;
	 gpioinitstructure.GPIO_Speed = GPIO_Speed_50MHz;
 
	 gpioinitstructure.GPIO_Pin = GPIO_Pin_10;
	 
	 GPIO_Init(GPIOA, &gpioinitstructure);
 
	 //���ڽṹ���ʼ��
	USART_InitStructure.USART_BaudRate = Buad;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
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

//�����ַ�
void USART_SendByte(USART_TypeDef* USARTx, char  str)
{
 
		USART_SendData(USARTx, str);//���͵����ַ�
 
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ж��Ƿ������
 
	
 
}
//�����ַ���
void USART_SendString(USART_TypeDef* USARTx, char * str)
{
	while(*str != '\0')
	{
		USART_SendData(USARTx, *str);//���͵����ַ�
		str++;
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//�ж��Ƿ������
		//Delay_ms(500);
	}
 
}

void send_pack(long int * sendbuf)//���Ͱ�
{
	USART_SendByte(USART1, 0xFF);
	USART_SendByte(USART1,sendbuf[0]);
	USART_SendByte(USART1,sendbuf[1]);
	USART_SendByte(USART1,sendbuf[2]);
	USART_SendByte(USART1,sendbuf[3]);
	USART_SendByte(USART1, 0xFE);
 
	
}

//��������
void USART1_IRQHandler(void)
{
	
	/*���徲̬������¼����״̬*/
	static int rxstate;//����״̬  0--�ȴ���ͷ  1--��������   2--�ȴ���β
	static int rxdata;//���������ݴ�
	static int i;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//��⴮��״̬�Ƿ��ڽ���״̬
	{
		rxdata = USART_ReceiveData(USART1);//�����յ��������ݴ浽rxdata
		
		
		/*���ݽ���״̬�ж������ĸ������ж�*/
		if(rxstate == 0)
		{
			if(rxdata == 0xFF)//���յ���ͷ����һλ�����ݣ�״̬��1
			{
				rxstate = 1;
 
			}
		}
		else if(rxstate == 1)//���յ���������
		{
			rxbuf[i] = rxdata;//�����ջ��������ݴ�ŵ�����������
			i++;
			
			if(i > 4 | i == 4)//�̶�����4���ֽڣ����յ�4��֮����һλĬ���ǰ�β
			{
				i = 0;
				rxstate = 2;
			}
		}
		else if(rxstate == 2)//��⵽��β
		{
			if(rxdata == 0xFE)
			{
				rxflag = 1;//���ձ�־��1������������
				rxstate = 0;//����״̬��0��׼��������һ�����ݵİ�ͷ
			}
		
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);//����жϱ�־��ÿ���յ�һ�����ݽ���һ���ж�
	
	}
 
 
}



