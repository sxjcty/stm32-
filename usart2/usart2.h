#ifndef __USART2_H

#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART2_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define RX_BUF_SIZE 80     //buffer�洢��С

extern volatile char RX_FLAG_END_LINE ;     //���ܱ�־λ  0��ʾ�ܽ���

extern volatile char RXi;     //

extern volatile char RXc;

extern  char RX_BUF[RX_BUF_SIZE];	     //����buffer
	



extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_len;         		//����״̬���

u16  SearchNum( u8 *inBuf,u8 *searchBuf, u16  dataLen,u8 seachLen,unsigned long *data,u8 seachNumLen);
void clear_RXBuffer(void);
void uart2_init(u32 bound);
void USART2Send(char *pucBuffer);

int  Sunday(char* source, char* target);  //�ַ�ƥ�亯��
int  my_atoi(char *source,char *target);	//�ַ�ƥ���������ȡ���ֵ�
#endif

