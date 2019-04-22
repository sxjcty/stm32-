#include "flash_mem.h"
#include "bsp.h"
#include "sys.h"
#include <string.h>
#include "flash.h"
#include "usart2.h"
#include "hmi.h"


char DEVID[32] = "";
u8 Data_Sr[256];
u8 temp;

void Mem_Init(void)//��ȡ������Ϣ
{
	

}

void Read_ID()//��ȡ��д��ID���û����������
{

}



/////************************************
////������������Ĵ洢���
////1,��ȡADDR_DATA_SR��256�����ݣ���������뵽����Data_Sr��
////2,����256���������Ƿ������ݣ����У�����Ӧ��־λ��1��������0��
////3,��ӡData_Sr[]���飬��״̬��ʾ����
////***********************************/
void Read_Data_SR(void)
{
	u16 i=0;
	u8 temp;
	for(i=0;i<256;i++)         //�Ķ�ADDR_DATA_SR��256��״̬��д������
		 {
			 SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
//			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
		 }
			 
	for(i=0;i<256;i++)         //����i����ַ��״̬����Data_Sr[0]�У�������״̬���뵽flash��״̬λ�ñ�������
		{
			temp=SPI_Flash_Read_Byte(ADDR_DATA_1+(i*4096));
//			printf("i=%d,temp=%u\r\n",i,temp);
			if(temp!=0)    //��ʾ�õ�ַ����ֵ����
			{
				temp=1;
				SPI_Flash_Write_Byte(temp,ADDR_DATA_SR+i);  //����״̬��־λ��Ӧ�ĵط�Ϊ1
//				printf("i=%d,\"������\"=%u\r\n",i,temp);
			}
			else        //��ʾ�õ�ַ��û�д�ֵ���ñ�־λΪ0
			{
			    temp=0;
					SPI_Flash_Write_Byte(temp,ADDR_DATA_SR+i);
//				  printf("i=%d,temp=%u\r\n",i,temp);
			}
				
		}
//		printf("Ŀǰ�洢�����\r\n");
		SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
//		for(i=0;i<256;i++)
//		{
//			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
//	  }

}


/////////************************************
////////������������Ĵ洢���
////////1,��ȡADDR_DATA_SR��256�����ݣ���������뵽����Data_Sr��
////////2,����256���������Ƿ������ݣ����У�����Ӧ��־λ��1��������0��
////////3,��ӡData_Sr[]���飬��״̬��ʾ����
////////***********************************/
////void Read_Data_SR(void)
////{
////	u16 i=0;
////	for(i=0;i<256;i++)         //�Ķ�ADDR_DATA_SR��256��״̬��д������
////		 {
////			 SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
////			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
////		 }
////			 
////	for(i=0;i<256;i++)         //����i����ַ��״̬����Data_Sr[0]�У�������״̬���뵽flash��״̬λ�ñ�������
////		{
////			SPI_Flash_Read(Data_Sr,ADDR_DATA_1+i*4096,1);
////			printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			if(Data_Sr[0]!=255)    //��ʾ�õ�ַ����ֵ����
////			{
////				Data_Sr[0]=1;
////				SPI_Flash_Write(Data_Sr,ADDR_DATA_SR+i,1);  //����״̬��־λ��Ӧ�ĵط�Ϊ1
////				printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			}
////			else        //��ʾ�õ�ַ��û�д�ֵ���ñ�־λΪ0
////			{
////			    Data_Sr[0]=0;
////					SPI_Flash_Write(Data_Sr,ADDR_DATA_SR+i,1);
////				  printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			}
////				
////		}
////		printf("Ŀǰ�洢�����\r\n");
////		SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
////		for(i=0;i<256;i++)
////		{
////			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
////	  }

////}


////////////////////////////////////////////////////ɾ��////////////////////////////////////////////////////////////////
/**************************************
������ݺ���
������Ҫ����ĵڼ�������

*************************************/
void Flash_Data_Erase(int i)
{
		u32 Dst_Addr = (i+129-1);
		SPI_Flash_Erase_Sector(Dst_Addr);
}

/**************************************
���FLASH����������������


*************************************/
void Flash_ALLData_Erase(void)
{
		int i;
	 for(i=1;i<256;i++)
	{
		u32 Dst_Addr = (i+129-1);
		SPI_Flash_Erase_Sector(Dst_Addr);
	}
}


////////////////////////////////////////////////////����////////////////////////////////////////////////////////////////
/*************************************
�������ܣ��ڵ�i������������
������i: 						Ҫ�����ĵ�i������
			Data_Buffer  	Ҫ���������
		  j�� 				  0--��ʾ��A����  1--��ʾ����B����  2--��ʾ�洢����4K����
*************************************/
void Flash_InsertData(int i,u8* Data_Buffer,int j)
{
			u32 Dst_Addr;
			if(j==0)
				{
						Dst_Addr = (i+129-1)*4096;
						SPI_Flash_Write((u8*)Data_Buffer,Dst_Addr,1024);
				}
				else if(j==1)
				{
						Dst_Addr = (i+129-1)*4096+2048;
						SPI_Flash_Write((u8*)Data_Buffer,Dst_Addr,1024);				
				}
				else
				{
						Dst_Addr = (i+129-1)*4096;
						SPI_Flash_Write((u8*)Data_Buffer,Dst_Addr,4096);
				}
}




////////////////////////////////////////////////////////����//////////////////////////////////////////////////////////////////
