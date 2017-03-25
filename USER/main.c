#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 	 
#include "audiosel.h"
#include "rda5820.h"
#include "usmart.h"
#define FREQUENCY_POINT 10

void RDA5820_Show_Msg(void)
{
	u8 rssi;
	u16 freq;
	freq=RDA5820_Freq_Get();				//��ȡ���õ���Ƶ��ֵ
	LCD_ShowNum(100,210,freq/100,3,16);		//��ʾƵ����������
	LCD_ShowNum(132,210,(freq%100)/10,1,16);//��ʾƵ��С������
	rssi=RDA5820_Rssi_Get();				//�õ��ź�ǿ��
	LCD_ShowNum(100,230,rssi,2,16);			//��ʾ�ź�ǿ��
}

u8 flag_frame_processing=0;//�յ�������֡���ڴ����־λ��1:������;0:����;
u8 index_frame_send=0;//�ظ���Ϣ֡�±�
u8 frame_send_buf[100]={0};//���ͻ�����
unsigned char XOR(unsigned char *BUFF, int len);



int main(void)
{	 
	u8 key,rssi;
	u16 freqset=8700;//Ĭ��Ϊ87Mhz  
	u8 i=0;
	u8 mode=0;	//����ģʽ

	u16 t=0,j=0;
	u8 len;	

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
	KEY_Init();
 	LED_Init();			     //LED�˿ڳ�ʼ��
	LCD_Init();			 	
   	usmart_dev.init(72);	//��ʼ��USMART	
 	Audiosel_Init();
	Audiosel_Set(AUDIO_RADIO);

	RDA5820_Init();	  

 	POINT_COLOR=RED;//��������Ϊ��ɫ 
	LCD_ShowString(60,50,200,16,16,"WarShip STM32");	
	LCD_ShowString(60,70,200,16,16,"RDA5820 TEST");	
	LCD_ShowString(60,90,200,16,16,"ATOM@ALIENTEK");
	LCD_ShowString(60,110,200,16,16,"2012/9/14");  
	LCD_ShowString(60,130,200,16,16,"KEY0:Freq+ KEY2:Freq-");
	LCD_ShowString(60,150,200,16,16,"KEY1:Auto Search(RX)");
	LCD_ShowString(60,170,200,16,16,"KEY_UP:Mode Set");
	POINT_COLOR=BLUE;
	//��ʾ��ʾ��Ϣ
	POINT_COLOR=BLUE;//��������Ϊ��ɫ	  
	LCD_ShowString(60,190,200,16,16,"Mode:FM RX");	 				    
	LCD_ShowString(60,210,200,16,16,"Freq: 93.6Mhz");	 				    
	LCD_ShowString(60,230,200,16,16,"Rssi:");	 				    
 	
	RDA5820_Band_Set(0);	//����Ƶ��Ϊ87~108Mhz
	RDA5820_Space_Set(0);	//���ò���Ϊ100Khz
	RDA5820_TxPGA_Set(3);	//�ź���������Ϊ3
	RDA5820_TxPAG_Set(63);	//���书��Ϊ���.
	RDA5820_RX_Mode();		//����Ϊ����ģʽ  
	
	freqset=9360;				//Ĭ��Ϊ93.6Mhz
	RDA5820_Freq_Set(freqset);	//����Ƶ��
 	while(1)
	{	
//		key=KEY_Scan(0);//��֧������
//		switch(key)
//		{
//			case 0://���κΰ�������
//				break;
//			case KEY_UP://�л�ģʽ
//				mode=!mode;
//				if(mode)
//				{
//					Audiosel_Set(AUDIO_PWM);	//���õ�PWM ��Ƶͨ��
//					RDA5820_TX_Mode();			//����ģʽ
//					RDA5820_Freq_Set(freqset);	//����Ƶ��
//					LCD_ShowString(100,190,200,16,16,"FM TX");	 				    
//				}else 
//				{
//						Audiosel_Set(AUDIO_RADIO);	//���õ�����������
//						RDA5820_RX_Mode();			//����ģʽ
//					RDA5820_Freq_Set(freqset);	//����Ƶ��
//					LCD_ShowString(100,190,200,16,16,"FM RX");	 				    
//				}
//				break;
//			case KEY_DOWN://�Զ�������һ����̨.
//				if(mode==0)//���ڽ���ģʽ��Ч
//				{
//  					while(1)
//					{
//						if(freqset<10800)freqset+=10;   //Ƶ������100Khz
//					 	else freqset=8700;				//�ص����
//						RDA5820_Freq_Set(freqset);		//����Ƶ��
//						delay_ms(10);					//�ȴ���Ƶ�ź��ȶ�
//						if(RDA5820_RD_Reg(0X0B)&(1<<8))//��һ����Ч��̨. 
//						{
//  							RDA5820_Show_Msg();			//��ʾ��Ϣ
//							break;		 
//						}		    		
// 						RDA5820_Show_Msg();			//��ʾ��Ϣ
//						//����̨�ڼ��а�������,��������̨.
//						key=KEY_Scan(0);//��֧������
//						if(key)break;
//					}
//				}
//				break;
//			case KEY_LEFT://Ƶ�ʼ�
//				if(freqset>8700)freqset-=10;   //Ƶ�ʼ���100Khz
//				else freqset=10800;			//Խ�紦�� 
//				RDA5820_Freq_Set(freqset);	//����Ƶ��
//				RDA5820_Show_Msg();//��ʾ��Ϣ
//				break;
//			case KEY_RIGHT://Ƶ����
//				if(freqset<10800)freqset+=10;  //Ƶ������100Khz
//				else freqset=8700;		 	//Խ�紦�� 
//				RDA5820_Freq_Set(freqset);	//����Ƶ��
//				RDA5820_Show_Msg();//��ʾ��Ϣ
//				break;
//		}	 
//		i++;
//		delay_ms(10);	  
//		if(i==200)//ÿ�������Ҽ��һ���ź�ǿ�ȵ���Ϣ.
//		{
//			i=0;
//			rssi=RDA5820_Rssi_Get();				//�õ��ź�ǿ��
//			LCD_ShowNum(100,230,rssi,2,16);			//��ʾ�ź�ǿ��
//		}

//	   printf("%d ,",USART_RX_STA);
	   if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3ff;//�õ��˴ν��յ������ݳ���

			if(USART_RX_BUF[0]=='$'){
				if(USART_RX_BUF[len-1]==XOR(USART_RX_BUF,len-1)){
					index_frame_send=0;
 /*******************************************�Ӱ������ж�֡**********************************************************************************/
					if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='d')&&(USART_RX_BUF[3]=='y')&&(USART_RX_BUF[4]=='_')){//����֡
						 frame_send_buf[index_frame_send]='$';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='r';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='d';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='y';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 //printf("\r\n�Ӱ�����֡\r\n");
	
					}
 /*******************************************Ƶ��ɨ��֡**********************************************************************************/					
					else if((USART_RX_BUF[1]=='f')&&(USART_RX_BUF[2]=='r')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')){//Ƶ��ɨ��֡
						frame_send_buf[index_frame_send]='$';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='f';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='r';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='e';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=0;
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=0;
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=23;
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;
						 for(j=0;j<FREQUENCY_POINT;j++){
							 for(t=0;t<index_frame_send;t++)
							 {
								USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
								while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							 }
							 delay_ms(50);
					//		 frame_send_buf[7]++;
					//		 frame_send_buf[8]++;
						 }
						 //printf("\r\nƵ��ɨ��֡\r\n");
					}
 /*******************************************����֡**********************************************************************************/
					else if((USART_RX_BUF[1]=='c')&&(USART_RX_BUF[2]=='o')&&(USART_RX_BUF[3]=='n')&&(USART_RX_BUF[4]=='_')){//����֡
						frame_send_buf[index_frame_send]='$';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='c';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='o';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='n';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]='_';
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						 index_frame_send++;
						 frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						 index_frame_send++;
						 switch(USART_RX_BUF[6]){
						 	case 1:
							break;
							case 2:
							break;
							default:
							break;
						 }
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 //printf("\r\n����֡\r\n");
					}
 /*******************************************����֡**********************************************************************************/					
					else if((USART_RX_BUF[1]=='d')&&(USART_RX_BUF[2]=='a')&&(USART_RX_BUF[3]=='t')&&(USART_RX_BUF[4]=='_')){//����֡
					
					//printf("\r\n����֡\r\n");
					}else{printf("֡�쳣��");}
				}else{printf("У��λ����");}
		    }
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(USART1, USART_RX_BUF[t]);//�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}

			USART_RX_STA=0;
		}
	
	}
}





unsigned char XOR(unsigned char *BUFF, int len)
{
	unsigned char result=0;
	int i;
	for(result=BUFF[0],i=1;i<len;i++)
	{
		result ^= BUFF[i];
	}
	return result;
}







