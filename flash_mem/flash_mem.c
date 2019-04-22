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

void Mem_Init(void)//读取配置信息
{
	

}

void Read_ID()//读取与写入ID、用户名、密码等
{

}



/////************************************
////返回数据区域的存储情况
////1,读取ADDR_DATA_SR后256个数据，并将其存入到数组Data_Sr中
////2,查验256个扇区内是否有数据，若有，在相应标志位置1，否则置0；
////3,打印Data_Sr[]数组，将状态显示出来
////***********************************/
void Read_Data_SR(void)
{
	u16 i=0;
	u8 temp;
	for(i=0;i<256;i++)         //阅读ADDR_DATA_SR的256个状态并写入数组
		 {
			 SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
//			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
		 }
			 
	for(i=0;i<256;i++)         //将第i个地址的状态存入Data_Sr[0]中，并将其状态存入到flash的状态位置保存下来
		{
			temp=SPI_Flash_Read_Byte(ADDR_DATA_1+(i*4096));
//			printf("i=%d,temp=%u\r\n",i,temp);
			if(temp!=0)    //表示该地址内有值存在
			{
				temp=1;
				SPI_Flash_Write_Byte(temp,ADDR_DATA_SR+i);  //设置状态标志位相应的地方为1
//				printf("i=%d,\"不等于\"=%u\r\n",i,temp);
			}
			else        //表示该地址内没有存值，置标志位为0
			{
			    temp=0;
					SPI_Flash_Write_Byte(temp,ADDR_DATA_SR+i);
//				  printf("i=%d,temp=%u\r\n",i,temp);
			}
				
		}
//		printf("目前存储情况：\r\n");
		SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
//		for(i=0;i<256;i++)
//		{
//			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
//	  }

}


/////////************************************
////////返回数据区域的存储情况
////////1,读取ADDR_DATA_SR后256个数据，并将其存入到数组Data_Sr中
////////2,查验256个扇区内是否有数据，若有，在相应标志位置1，否则置0；
////////3,打印Data_Sr[]数组，将状态显示出来
////////***********************************/
////void Read_Data_SR(void)
////{
////	u16 i=0;
////	for(i=0;i<256;i++)         //阅读ADDR_DATA_SR的256个状态并写入数组
////		 {
////			 SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
////			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
////		 }
////			 
////	for(i=0;i<256;i++)         //将第i个地址的状态存入Data_Sr[0]中，并将其状态存入到flash的状态位置保存下来
////		{
////			SPI_Flash_Read(Data_Sr,ADDR_DATA_1+i*4096,1);
////			printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			if(Data_Sr[0]!=255)    //表示该地址内有值存在
////			{
////				Data_Sr[0]=1;
////				SPI_Flash_Write(Data_Sr,ADDR_DATA_SR+i,1);  //设置状态标志位相应的地方为1
////				printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			}
////			else        //表示该地址内没有存值，置标志位为0
////			{
////			    Data_Sr[0]=0;
////					SPI_Flash_Write(Data_Sr,ADDR_DATA_SR+i,1);
////				  printf("i=%d,Data_Sr[0]=%u\r\n",i,Data_Sr[0]);
////			}
////				
////		}
////		printf("目前存储情况：\r\n");
////		SPI_Flash_Read(Data_Sr,ADDR_DATA_SR,256);
////		for(i=0;i<256;i++)
////		{
////			 printf("Data_Sr[%d]=%u\r\n",i,Data_Sr[i]);
////	  }

////}


////////////////////////////////////////////////////删除////////////////////////////////////////////////////////////////
/**************************************
清除数据函数
参数：要清除的第几块扇区

*************************************/
void Flash_Data_Erase(int i)
{
		u32 Dst_Addr = (i+129-1);
		SPI_Flash_Erase_Sector(Dst_Addr);
}

/**************************************
清除FLASH数据区的所有数据


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


////////////////////////////////////////////////////增加////////////////////////////////////////////////////////////////
/*************************************
函数功能：在第i个扇区加数据
参数：i: 						要操作的第i个扇区
			Data_Buffer  	要储存的数组
		  j： 				  0--表示在A曲线  1--表示储存B曲线  2--表示存储整个4K扇区
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




////////////////////////////////////////////////////////查找//////////////////////////////////////////////////////////////////
