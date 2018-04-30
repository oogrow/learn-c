/*******************************************************************************
                                          Oooo
                                    oooO  (   )
                                   (   )   ) /
                                    \ (   (_/
                                     \_)

                           ��һ·̫���ˣ�ףԸ��������ã���
*******************************************************************************/
#include<reg51.h>
#include<intrins.h>

//TYPE
#define uint  unsigned int
#define uchar unsigned char
	
//���ݡ�λѡ��ʾ�˿�
#define Date_Port P0
#define Wei_Port  P2

//�Զ���
sbit CS   = P1^0; 
sbit CLK  = P1^1; 
sbit DATI = P1^2; 
sbit DATO = P1^2;

sbit AIR=P1^4;
sbit FAN=P1^5;
sbit BEEP=P1^6;
sbit LED=P1^7;

sbit KEY_AIR=P2^4;

uchar SEG7_DATA[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};//����������ܶ���
uchar COM_DATA[4] = {0xf7,0xfb,0xfd,0xfe};//�����������λѡ
uchar DISPLAY_DATA[4] = {0x00,0x00,0x00,0x00};

/*== void delay_ms(uint ms) ������ʱ ==*/
void delay_ms(uint ms)
{       
	uint i,j;
	for(i=ms;i>0;i--)
		for(j=110;j>0;j--);
}

/*== uchar ADC0832_Test() ADC0832 ���� ==*/
uchar ADC0832_Test() 
{        
	uchar i=0;
	uchar dat=0;
	uchar test=0;
	uchar adval=0;

//��ʼ��
	CLK = 0;  
	DATI = 1;_nop_();
	CS = 0;_nop_();
	CLK = 1;_nop_();

//ͨ��ѡ��CH0
	CLK = 0;
	DATI = 1; _nop_();
	CLK = 1;_nop_();
	CLK = 0;
	DATI = 0;_nop_();
	CLK = 1;_nop_();
	
	CLK = 0;
	DATI = 1;

//��ȡǰ8λ��ֵ
	for( i = 0;i < 8;i++ ) 
	{
		_nop_();
		adval <<= 1;
		CLK = 1;
		_nop_();
		CLK = 0;
		if (DATO)
		adval |= 0x01;
		else
		adval |= 0x00;
	}

//��ȡ��8λ��ֵ
	for (i = 0; i < 8; i++)
	{
		test >>= 1;
		if (DATO)
		test |= 0x80;
		else
		test |= 0x00;
		_nop_();
		CLK = 1;
		_nop_();
		CLK = 0;
	}
	
//�Ƚ�ǰ8λ���8λ��ֵ��������ͬ��ȥ
	if (adval == test)        
	{
		dat = test;
	}
	
	_nop_();
	CS = 1;    //�ͷ�ADC0832
	DATO = 1;
	CLK = 1;
	
	return dat;
}

/*== SEG7_CC_Display_AD(float DATA,uchar WEI,uchar DP) ��ʾ���� ==*/
void SEG7_CC_Display_AD(float DATA,uchar WEI,uchar DP)
{
	uint AD_DATA1=0;
	uint AD_DATA2=0;

	AD_DATA1=DATA;
	AD_DATA2=1000*(DATA-AD_DATA1);

	DISPLAY_DATA[0]=AD_DATA2%10;
	DISPLAY_DATA[1]=(AD_DATA2/10)%10;
	DISPLAY_DATA[2]=(AD_DATA2/100)%10;
	DISPLAY_DATA[3]=AD_DATA1;
	
	for(WEI;WEI<4;WEI++)
	{		
		Wei_Port=COM_DATA[WEI];
		if(WEI==DP)
		{
			Date_Port=(SEG7_DATA[DISPLAY_DATA[WEI]])|0x80;		
		}
		else
		{
			Date_Port=SEG7_DATA[DISPLAY_DATA[WEI]];
		}
		delay_ms(15);
	}
}

/*== Warning(void) �������� ==*/
//���ⱨ�������ŷ��ȣ��ر�ú����
void Warning(void)
{
		LED=0;
		BEEP=0;
		FAN=0;
		AIR=0;
}

/*== Warning_Up(void) ����������� ==*/
//�ر����ⱨ�����ر��ŷ���
void Warning_Up(void)
{
		LED=1;
		BEEP=1;
		FAN=1;
}

/*== void main(void) ������ ==*/
void main(void)
{
	float AD_DATA=0;
	uint num=0;
	while(1)
	{
		AD_DATA=(float)ADC0832_Test()/255*5.00;    //��ADת�������������
		
		SEG7_CC_Display_AD(AD_DATA,0,3);
		
		if(AD_DATA >= 3.0)
		{
			Warning();    //����
			num=2;
		}
			
		if((AD_DATA < 3.0)&(num==2))	
		{
			Warning_Up();    //������ʼ�������
			num=1;
		}
		
		if((num==1)&(KEY_AIR==0))
		{
			delay_ms(5);
			if(KEY_AIR==0)
			{
				AIR=1;  //��ú����
			}
		}
		
	}
}	

/******************* (C) COPYRIGHT 2018 ***** END OF FILE ****/