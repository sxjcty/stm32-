
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
 SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI1����Ϊ����ȫ˫��
 SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                    //����SPI2Ϊ��ģʽ
 SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI���ͽ���8λ֡�ṹ
 SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                   //����ʱ���ڲ�����ʱ��ʱ��Ϊ�͵�ƽ
 SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                 //��һ��ʱ���ؿ�ʼ��������
 SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                  //NSS�ź��������ʹ��SSIλ������
 SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ8
 SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;       //���ݴ����MSBλ��ʼ
 SPI_InitStructure.SPI_CRCPolynomial = 7;         //CRCֵ����Ķ���ʽ
 SPI_Init(SPI2, &SPI_InitStructure);
 /* Enable SPI2  */
 SPI_Cmd(SPI2, ENABLE);  
}  

//��ʼ��ADS1256 GPIO
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
//	��    �ܣ�  ģ��SPIͨ��
//	��ڲ���: /	���͵�SPI����
//	���ڲ���: /	���յ�SPI����
//	ȫ�ֱ���: /
//	��    ע: 	���ͽ��պ���
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
//	��    �ܣ�ADS1256 д����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: ��ADS1256�е�ַΪregaddr�ļĴ���д��һ���ֽ�databyte
//-----------------------------------------------------------------//
void ADS1256WREG(unsigned char regaddr,unsigned char databyte)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);;
	while(GPIO_ReadInputDataBit(GPIO_ADS1256DRDY_PORT,GPIO_ADS1256DRDY)){}
	//��ADS1256_DRDYΪ��ʱ����д�Ĵ���
	//��Ĵ���д�����ݵ�ַ
    SPI_WriteByte(ADS1256_CMD_WREG | (regaddr & 0x0F));
    //д�����ݵĸ���n-1
    SPI_WriteByte(0x00);
    //��regaddr��ַָ��ļĴ���д������databyte
    SPI_WriteByte(databyte);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}


//��ʼ��ADS1256
void ADS1256_Init(void)
{
	ADS1256WREG(ADS1256_STATUS,0x06);               // ��λ��ǰ��У׼��ʹ�û���
//	ADS1256WREG(ADS1256_MUX,0x08);                  // ��ʼ���˿�A0Ϊ��+����AINCOMλ��-��
	ADS1256WREG(ADS1256_ADCON,0x00);                // �Ŵ���1
	ADS1256WREG(ADS1256_DRATE,ADS1256_DRATE_10SPS);  // ����5sps
	ADS1256WREG(ADS1256_IO,0x00);               
}

//��ȡADֵ
unsigned int ADS1256ReadData()  
{
    unsigned char i=0;
    unsigned int sum=0;
 	  unsigned int r=0;
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);;

	while(GPIO_ReadInputDataBit(GPIO_ADS1256DRDY_PORT,GPIO_ADS1256DRDY));               //��ADS1256_DRDYΪ��ʱ����д�Ĵ��� 
//	ADS1256WREG(ADS1256_MUX,channel);		//����ͨ��
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
//	��    �ܣ���ȡADS1256��·����
//	��ڲ���: /
//	���ڲ���: /
//	ȫ�ֱ���: /
//	��    ע: /
//-----------------------------------------------------------------//
unsigned int ADS_sum(unsigned char channel)
{
	ADS1256WREG(ADS1256_MUX,channel);		//����ͨ��
	return ADS1256ReadData();//��ȡADֵ������24λ���ݡ�
}


	
		
		
/**********************
	ads1256ת������
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
			ldVolutage = (long double)ulResult*0.59604644775390625;     //׼ȷ�ĵ�ѹֵ
			ld_V = (unsigned long)ldVolutage/100;
      buf[0]=(unsigned char)(ld_V/10000);       //��λ
			buf[0]=buf[0]+0x30;
			buf[1]=(unsigned char)((ld_V%10000)/1000);    //ʮλ
			buf[1]=buf[1]+0x30;		
			buf[2]=(unsigned char)((ld_V%1000)/100);      //��λ
			buf[2]=buf[2]+0x30;
			buf[3]='.';   //����'.'�Ļ�  �ᱻ��һλ����		
		  num=(buf[0]-0x30)*1000+(buf[1]-0x30)*100+(buf[2]-0x30)*10+0;   // ��λ����β��Ϊ0
			
		  //������
//			printf("ulResult��ֵΪ��%lu\r\n",ulResult);
//			printf("ldVolutage��ֵΪ��%LF\r\n",ldVolutage);
//			printf("ld_V��ֵΪ��%lu\r\n",ld_V);			
//			printf("num��ֵΪ��%d\r\n",num);
//			printf("buf��ֵΪ��%s\r\n",buf);
//			
			
}
		


//�˺�����ʱδ�õ�
#define MEDIAN_LEN  5                     //��ֱ�˲����ܳ��ȣ�һ��ѡȡ����   
#define MEDIAN      2                     //��ֵ���˲������е�λ��
unsigned long   AD_buffer[MEDIAN_LEN];    //ad�ɼ����黺��
//unsigned long   MED_buffer[MEDIAN_LEN];   //��ֵ�˲����黺��
unsigned char   medleng = 0;          //���뻺������ݸ���
/**********************************************************/
// ����������ֵ�˲�����
// ����  ����ȡǰ9�βɼ������ݣ�ȥ����3����ȥ����3����Ȼ���м��
// ����  ��3��������ƽ��ֵ�����㷨�ɾ����ܵ��˵��������ݣ�����Ӱ��ɼ��ٶȡ�
// ����  ��9�����ݵ�����
// ���  ���м�3�����ݵ�ƽ��ֵ
/*********************************************************/
unsigned long medina_filter(unsigned long *MED_buffer)  //xad - ADCת��ֵ   
{   
	
	unsigned char i,j;
	unsigned long xd;
	u32 xxd;
	
	for(i = 0; i < MEDIAN_LEN; i ++)     
	{   
		for(j = 0; j < MEDIAN_LEN - i; j ++)
		{
			
			if( MED_buffer[i] > MED_buffer[i + 1]) // ��ѯ���ĵ�ǰԪ��>ADֵ,�򽻻����ǵ�ֵ   
			{ xd = MED_buffer[i]; MED_buffer[i] = MED_buffer[i + 1]; MED_buffer[i + 1] = xd;} 
		}			
	} 
	xxd = MED_buffer[MEDIAN - 1] + MED_buffer[MEDIAN] + MED_buffer[MEDIAN + 1];
	xd = xxd/3;
	return xd; //��ֵ     
}



