#include "sys.h"
#include "usart2.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"




//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->SR&0X40)==0);//循环发送,直到发送完毕   
    USART2->DR = (u8) ch;      
	return ch;
}
#endif 




#define RX_BUF_SIZE 80     //buffer存储大小

volatile char RX_FLAG_END_LINE = 0;     //接受标志位  0表示能接受

volatile char RXi;     

volatile char RXc;

char RX_BUF[RX_BUF_SIZE] = {'\0'};	     //接受buffer


/*****************************************
清除RX_BUF的函数

*****************************************/
void clear_RXBuffer(void) {

		for (RXi=0; RXi<RX_BUF_SIZE; RXi++)

				RX_BUF[RXi] = '\0';

		RXi = 0;

}


/*****************************
//字符组发送函数
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
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能USART2，GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART1_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串?222

}



void USART2_IRQHandler(void)                	//串口2中断服务程序
	{
	
		
		
		if((USART2->SR&USART_FLAG_RXNE)!=(u16)RESET)
		{
				RXc = USART_ReceiveData(USART2);
				RX_BUF[RXi] = RXc;
				RXi++;
				if(RXc!=0x23)   //判断是否为结束位。不是#的话，继续接受   #的acs码为23   T54
					{
						if(RXi > RX_BUF_SIZE -1)
							clear_RXBuffer();
					}else     //部位结束位，为回车的话 停止接受
					{
							RX_FLAG_END_LINE = 1;     //置接受标志位为1，
					
					}
					//USART_SendData(USART2,RXc);
			
		}
  }    
	      

	
	

////////////////////////////////////////////////////字符串匹配/////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////SUNDAY字符串匹配算法///////////////////////////////////////////////////////


//函数功能：查找字符匹配位置	
int FindIndex(char* target, char temp) {
	int i = 0;
	if (target == NULL) {
		exit(-1); //异常退出，搜索词为空
	}
	for (i = strlen(target) - 1; i >= 0; i--) {
		if (target[i] == temp) {
			return i;
		}
	}
	return -1;  //未找到字符匹配位置

}

	
//sunday算法
//函数参数： source：源字符串
//						target：匹配字符串
//返回值: source中最后一个匹配到的地址
int  Sunday(char* source, char* target)
	{
	int i = 0, j = 0, srclen = strlen(source), tarlen = strlen(target);
	int temp = 0, index = -1;
	if (source == NULL || target == NULL) {
		return -1; //两个字符串可能为空
	}

	while (i < srclen) {  //循环条件
		if (source[i] == target[j]) {
			if (j == tarlen - 1) {
				return i;  //匹配成功，返回的是最后一个字符的位置
			}
			i++; j++;
		}
		else {  //发现位置不相等
			temp = tarlen - j + i;  //字符串后面的第一个字符位置
			index = FindIndex(target, source[temp]);
			if (index == -1) { //没有找到位置，后移
				i = temp + 1;
				j = 0;
			}
			else {  //找到位置
				i = temp - index;
				j = 0;
				
			}
		}
	}
	return -1;
}


//////////////////////
//函数参数： source：源字符串
//						target：匹配字符串
//返回值：匹配字符后面的数值

int  my_atoi(char *source,char *target)
{
	int srclen=strlen(source);
	int  value = 0;
	int n = Sunday(source, target)+1;
//	printf("source[n]=%c\r\n", source[n]);  //测试用
//	printf("srclen=%d\r\n", srclen);
	while(n < srclen)
	{
		if (source[n] < '0' || source[n] > '9')
		{
			return value;
		}
		/*
		计算的value值是乘以10，表示向上	提升一位
		*/
		value = value * 10 + (source[n] - '0');
//		printf("value=%d\r\n", value); //测试用
		n++;
	}
	return value;
	
}
//void main() {

//	char source[] = { "qwertyuioppasdfghjklzxcvbnm" };
//	char target[] = { "fgh" };
//	printf("%d", Sunday(source, target));
//}


