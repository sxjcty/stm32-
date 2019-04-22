#include "sys.h"
#include "usart2.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"




//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 




#define RX_BUF_SIZE 80     //buffer�洢��С

volatile char RX_FLAG_END_LINE = 0;     //���ܱ�־λ  0��ʾ�ܽ���

volatile char RXi;     

volatile char RXc;

char RX_BUF[RX_BUF_SIZE] = {'\0'};	     //����buffer


/*****************************************
���RX_BUF�ĺ���

*****************************************/
void clear_RXBuffer(void) {

		for (RXi=0; RXi<RX_BUF_SIZE; RXi++)

				RX_BUF[RXi] = '\0';

		RXi = 0;

}


/*****************************
//�ַ��鷢�ͺ���
*****************************/


void USART2Send(char *pucBuffer)
{
		while(*pucBuffer)
		{
				USART_SendData(USART2,*pucBuffer++);
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET)
				{
				}
		}
}



void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART2��GPIOAʱ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART1_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ�?222

}



void USART2_IRQHandler(void)                	//����2�жϷ������
	{
	
		
		
		if((USART2->SR&USART_FLAG_RXNE)!=(u16)RESET)
		{
				RXc = USART_ReceiveData(USART2);
				RX_BUF[RXi] = RXc;
				RXi++;
				if(RXc!=0x23)   //�ж��Ƿ�Ϊ����λ������#�Ļ�����������   #��acs��Ϊ23   T54
					{
						if(RXi > RX_BUF_SIZE -1)
							clear_RXBuffer();
					}else     //��λ����λ��Ϊ�س��Ļ� ֹͣ����
					{
							RX_FLAG_END_LINE = 1;     //�ý��ܱ�־λΪ1��
					
					}
					//USART_SendData(USART2,RXc);
			
		}
  }    
	      

	
	

////////////////////////////////////////////////////�ַ���ƥ��/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////SUNDAY�ַ���ƥ���㷨///////////////////////////////////////////////////////


//�������ܣ������ַ�ƥ��λ��	
int FindIndex(char* target, char temp) {
	int i = 0;
	if (target == NULL) {
		exit(-1); //�쳣�˳���������Ϊ��
	}
	for (i = strlen(target) - 1; i >= 0; i--) {
		if (target[i] == temp) {
			return i;
		}
	}
	return -1;  //δ�ҵ��ַ�ƥ��λ��

}

	
//sunday�㷨
//���������� source��Դ�ַ���
//						target��ƥ���ַ���
//����ֵ: source�����һ��ƥ�䵽�ĵ�ַ
int  Sunday(char* source, char* target)
	{
	int i = 0, j = 0, srclen = strlen(source), tarlen = strlen(target);
	int temp = 0, index = -1;
	if (source == NULL || target == NULL) {
		return -1; //�����ַ�������Ϊ��
	}

	while (i < srclen) {  //ѭ������
		if (source[i] == target[j]) {
			if (j == tarlen - 1) {
				return i;  //ƥ��ɹ������ص������һ���ַ���λ��
			}
			i++; j++;
		}
		else {  //����λ�ò����
			temp = tarlen - j + i;  //�ַ�������ĵ�һ���ַ�λ��
			index = FindIndex(target, source[temp]);
			if (index == -1) { //û���ҵ�λ�ã�����
				i = temp + 1;
				j = 0;
			}
			else {  //�ҵ�λ��
				i = temp - index;
				j = 0;
				
			}
		}
	}
	return -1;
}


//////////////////////
//���������� source��Դ�ַ���
//						target��ƥ���ַ���
//����ֵ��ƥ���ַ��������ֵ

int  my_atoi(char *source,char *target)
{
	int srclen=strlen(source);
	int  value = 0;
	int n = Sunday(source, target)+1;
//	printf("source[n]=%c\r\n", source[n]);  //������
//	printf("srclen=%d\r\n", srclen);
	while(n < srclen)
	{
		if (source[n] < '0' || source[n] > '9')
		{
			return value;
		}
		/*
		�����valueֵ�ǳ���10����ʾ����	����һλ
		*/
		value = value * 10 + (source[n] - '0');
//		printf("value=%d\r\n", value); //������
		n++;
	}
	return value;
	
}
//void main() {

//	char source[] = { "qwertyuioppasdfghjklzxcvbnm" };
//	char target[] = { "fgh" };
//	printf("%d", Sunday(source, target));
//}


