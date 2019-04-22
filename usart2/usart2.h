#ifndef __USART2_H

#define __USART2_H
#include "stdio.h"	
#include "sys.h" 


#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口1接收

#define RX_BUF_SIZE 80     //buffer存储大小

extern volatile char RX_FLAG_END_LINE ;     //接受标志位  0表示能接受

extern volatile char RXi;     //

extern volatile char RXc;

extern  char RX_BUF[RX_BUF_SIZE];	     //接受buffer
	



extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_len;         		//接收状态标记

u16  SearchNum( u8 *inBuf,u8 *searchBuf, u16  dataLen,u8 seachLen,unsigned long *data,u8 seachNumLen);
void clear_RXBuffer(void);
void uart2_init(u32 bound);
void USART2Send(char *pucBuffer);

int  Sunday(char* source, char* target);  //字符匹配函数
int  my_atoi(char *source,char *target);	//字符匹配后用来提取数字的
#endif

