
#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "ADS1256.h"
#include "usart.h"
#include "hmi.h"
#include "delay.h"


u8 buf[10];
long ulResult;
long double ldVolutage;
int num;
unsigned long ld_V;

void SPI2_Init(void)
{
 SPI_InitTypeDef  SPI_InitStructure;
 GPIO_InitTypeDef GPIO_InitStructure;
 /****Initial SPI2******************/
 
 /* Enable SPI2 and GPIOB clocks */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
 
 

 /* Configure SPI2 pins: NSS, SCK, MISO and MOSI */
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
 GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 //SPI2 NSS 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
 
 GPIO_SetBits(GPIOB, GPIO_Pin_12);
 
  /* SPI2 configuration */ 
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1设置为两线全双工
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                    //设置SPI2为主模式
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收8位帧结构
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                   //串行时钟在不操作时，时钟为低电平
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                 //第一个时钟沿开始采样数据
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                  //NSS信号由软件（使用SSI位）管理
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //定义波特率预分频的值:波特率预分频值为8
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //数据传输从MSB位开始
 SPI_InitStructure.SPI_CRCPolynomial = 7;         //CRC值计算的多项式
 SPI_Init(SPI2, &SPI_InitStructure);
 /* Enable SPI2  */
 SPI_Cmd(SPI2, ENABLE);  
}  

//初始化ADS1256 GPIO
void Init_ADS1256_GPIO(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_ADS1256Reset | RCC_ADS1256DRDY, ENABLE); 

  	GPIO_InitStructure.GPIO_Pin = GPIO_RCC_ADS1256Reset; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  	GPIO_Init(GPIO_RCC_ADS1256Reset_PORT, &GPIO_InitStructure);  
  	GPIO_ResetBits(GPIO_RCC_ADS1256Reset_PORT, GPIO_RCC_ADS1256Reset );


	GPIO_InitStructure.GPIO_Pin = GPIO_ADS1256DRDY; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  	GPIO_Init(GPIO_ADS1256DRDY_PORT, &GPIO_InitStructure);  

	SPI2_Init();
}


//-----------------------------------------------------------------//
//	功    能：  模拟SPI通信
//	入口参数: /	发送的SPI数据
//	出口参数: /	接收的SPI数据
//	全局变量: /
//	备    注: 	发送接收函数
//-----------------------------------------------------------------//
unsigned char SPI_WriteByte(unsigned char TxData)
{
  unsigned char RxData=0;

  while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET); //                                                   
  SPI_I2S_SendData(SPI2,TxData);

   while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_RXNE)==RESET);

   RxData=SPI_I2S_ReceiveData(SPI2);

    return RxData;
} 

//-----------------------------------------------------------------//
//	功    能：ADS1256 写数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: 向ADS1256中地址为regaddr的寄存器写入一个字节databyte
//-----------------------------------------------------------------//
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);;
	while(GPIO_ReadInputDataBit(GPIO_ADS1256DRDY_PORT,GPIO_ADS1256DRDY)){}
	//当ADS1256_DRDY为低时才能写寄存器
	//向寄存器写入数据地址
    SPI_WriteByte(ADS1256_CMD_WREG | (regaddr & 0x0F));
    //写入数据的个数n-1
    SPI_WriteByte(0x00);
    //向regaddr地址指向的寄存器写入数据databyte
    SPI_WriteByte(databyte);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}


//初始化ADS1256
void ADS1256_Init(void)
{
	ADS1256WREG(ADS1256_STATUS,0x06);               // 高位在前、校准、使用缓冲
//	ADS1256WREG(ADS1256_MUX,0x08);                  // 初始化端口A0为‘+’，AINCOM位‘-’
	ADS1256WREG(ADS1256_ADCON,0x00);                // 放大倍数1
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_10SPS);  // 数据5sps
	ADS1256WREG(ADS1256_IO,0x00);               
}

//读取AD值
unsigned int ADS1256ReadData()  
{
    unsigned char i=0;
    unsigned int sum=0;
 	  unsigned int r=0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);;

	while(GPIO_ReadInputDataBit(GPIO_ADS1256DRDY_PORT,GPIO_ADS1256DRDY));               //当ADS1256_DRDY为低时才能写寄存器 
//	ADS1256WREG(ADS1256_MUX,channel);		//设置通道
	SPI_WriteByte(ADS1256_CMD_SYNC);
	SPI_WriteByte(ADS1256_CMD_WAKEUP);	               
	SPI_WriteByte(ADS1256_CMD_RDATA);

   	sum |= (SPI_WriteByte(0xff) << 16);
	sum |= (SPI_WriteByte(0xff) << 8);
	sum |= SPI_WriteByte(0xff);

	GPIO_SetBits(GPIOB, GPIO_Pin_12); 
    return sum;
}


//-----------------------------------------------------------------//
//	功    能：读取ADS1256单路数据
//	入口参数: /
//	出口参数: /
//	全局变量: /
//	备    注: /
//-----------------------------------------------------------------//
unsigned int ADS_sum(unsigned char channel)
{
	ADS1256WREG(ADS1256_MUX,channel);		//设置通道
	return ADS1256ReadData();//读取AD值，返回24位数据。
}


	
		
		
/**********************
	ads1256转换函数
**********************/
void ads1256(void)
{
			ulResult = ADS_sum( ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);	
			if( ulResult & 0x800000 )
			{
			 	ulResult = ~(unsigned long)ulResult;
				ulResult &= 0x7fffff;
				ulResult += 1;
				ulResult = -ulResult;
			}
			ldVolutage = (long double)ulResult*0.59604644775390625;     //准确的电压值
			ld_V = (unsigned long)ldVolutage/100;
      buf[0]=(unsigned char)(ld_V/10000);       //百位
			buf[0]=buf[0]+0x30;
			buf[1]=(unsigned char)((ld_V%10000)/1000);    //十位
			buf[1]=buf[1]+0x30;		
			buf[2]=(unsigned char)((ld_V%1000)/100);      //个位
			buf[2]=buf[2]+0x30;
			buf[3]='.';   //不加'.'的话  会被吞一位数据		
		  num=(buf[0]-0x30)*1000+(buf[1]-0x30)*100+(buf[2]-0x30)*10+0;   // 四位数，尾数为0
			
		  //测试用
//			printf("ulResult的值为：%lu\r\n",ulResult);
//			printf("ldVolutage的值为：%LF\r\n",ldVolutage);
//			printf("ld_V的值为：%lu\r\n",ld_V);			
//			printf("num的值为：%d\r\n",num);
//			printf("buf的值为：%s\r\n",buf);
//			
			
}
		


//此函数暂时未用到
#define MEDIAN_LEN  5                     //中直滤波的总长度，一般选取奇数   
#define MEDIAN      2                     //中值在滤波数组中的位置
unsigned long   AD_buffer[MEDIAN_LEN];    //ad采集数组缓存
//unsigned long   MED_buffer[MEDIAN_LEN];   //中值滤波数组缓存
unsigned char   medleng = 0;          //存入缓存的数据个数
/**********************************************************/
// 函数名：中值滤波函数
// 描述  ：提取前9次采集的数据，去掉高3个，去掉低3个，然后中间的
// 描述  ：3个数据求平均值，该算法可尽可能的滤掉干扰数据，并不影响采集速度。
// 输入  ：9个数据的数组
// 输出  ：中间3个数据的平均值
/*********************************************************/
unsigned long medina_filter(unsigned long *MED_buffer)  //xad - ADC转换值   
{   
	
	unsigned char i,j;
	unsigned long xd;
	u32 xxd;
	
	for(i = 0; i < MEDIAN_LEN; i ++)     
	{   
		for(j = 0; j < MEDIAN_LEN - i; j ++)
		{
			
			if( MED_buffer[i] > MED_buffer[i + 1]) // 轮询到的当前元素>AD值,则交换它们的值   
			{ xd = MED_buffer[i]; MED_buffer[i] = MED_buffer[i + 1]; MED_buffer[i + 1] = xd;} 
		}			
	} 
	xxd = MED_buffer[MEDIAN - 1] + MED_buffer[MEDIAN] + MED_buffer[MEDIAN + 1];
	xd = xxd/3;
	return xd; //中值     
}



