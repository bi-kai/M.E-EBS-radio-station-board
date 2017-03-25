#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"	 	 
#include "audiosel.h"
#include "rda5820.h"
#include "timer.h"
//#include "usmart.h"

#define FREQUENCY_MIN 7600
#define FREQUENCY_MAX 8800
#define FREQUENCY_POINT (FREQUENCY_MAX-FREQUENCY_MIN)/10+1
#define FRAME_WAKEUP_BROADCAST 120*2/4
#define FRAME_WAKEUP_UNICAST 144*2/4
#define FRAME_WAKEUP_MULTICAST 168*2/4
#define FRAME_CONTROL 84*2/4
#define FRAME_SECURTY 470*2/4

//void RDA5820_Show_Msg(void)
//{
//	u8 rssi;
//	u16 freq;
//	freq=RDA5820_Freq_Get();				//��ȡ���õ���Ƶ��ֵ
//	LCD_ShowNum(100,210,freq/100,3,16);		//��ʾƵ����������
//	LCD_ShowNum(132,210,(freq%100)/10,1,16);//��ʾƵ��С������
//	rssi=RDA5820_Rssi_Get();				//�õ��ź�ǿ��
//	LCD_ShowNum(100,230,rssi,2,16);			//��ʾ�ź�ǿ��
//}

u8 flag_frame_processing=0;//�յ�������֡���ڴ����־λ��1:������;0:����;
u8 index_frame_send=0;//���ڻظ���Ϣ֡�±�
u8 frame_send_buf[100]={0};//���ڻش�������

u16 fm_frame_index_bits=0;//FM�㲥01���б������±�
u8 fm_frame_bits[1100]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,0};//FM�㲥֡���б�����������
u16 fm_frame_index_byte=0;//FM�㲥01�����ֽ����±�
u8 fm_frame_byte[1100/2]={0};//FM�㲥֡�����ֽ���������
unsigned char XOR(unsigned char *BUFF, u16 len);



int main(void)
{	 
	u16 freqset=FREQUENCY_MIN;//���ա�����Ƶ��
	u16 send_frequency=FREQUENCY_MIN;//����Ƶ�㣬ֻ���淢���Ƶ��  
	u16 t=0,j=0;
	signed char i=0;//�ֽ�ת������
	u16 len;
	u16 fm_total_bytes=0;//����֡����Ч�ֽ�����
	u8 flag_byte_ready=0;//�ֽ����Ѿ�ȫ�����浽����buffer��־λ
//	u8 bit_sync[14]={1,0,1,0,1,0,1,0,1,0,1,0,1,0};//λͬ��ͷ
//	u8 frame_sync[11]={1,1,1,0,0,0,1,0,0,1,0};//֡ͬ��ͷ	

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//���ڳ�ʼ��Ϊ9600
//	KEY_Init();
 	LED_Init();			     //LED�˿ڳ�ʼ��
	TIM3_Int_Init(4999,7199);//10Khz�ļ���Ƶ�ʣ�������5000Ϊ500ms
//	LCD_Init();			 	
// 	usmart_dev.init(72);	//��ʼ��USMART	
// 	Audiosel_Init();
//	Audiosel_Set(AUDIO_RADIO);

	RDA5820_Init(); 	
	RDA5820_Band_Set(1);	//����Ƶ��Ϊ0:87~108Mhz;1:76~91Mhz;
	RDA5820_Space_Set(0);	//���ò���Ϊ100Khz
	RDA5820_TxPGA_Set(3);	//�ź���������Ϊ3
	RDA5820_TxPAG_Set(63);	//���书��Ϊ���.	
	RDA5820_TX_Mode();			//����ģʽ
	RDA5820_Freq_Set(send_frequency);	//����Ƶ��
 	while(1)
	{
		if(flag_byte_ready==1){//���ݱ��汾�سɹ�
			fm_frame_index_bits=25;//FM�������������
			for(t=0;t<fm_frame_index_byte;t++){
				for (i=3;i>=0;i--)
				{
					fm_frame_bits[fm_frame_index_bits]=(fm_frame_byte[t]&(0x01<<i))?1:0; 
					fm_frame_index_bits++;
				}
			}
			for(t=0;t<fm_frame_index_bits;t++)
				{
	//				PBout(6)=fm_frame_bits[t];//����Ҫ��λͬ����֡ͬ��ͷ
					USART_SendData(USART1, fm_frame_bits[t]);//�򴮿ڷ�������
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
				}
			flag_byte_ready=0;//������ϣ�����
			fm_frame_index_byte=0;//�ֽ����������
			USART_RX_STA=0;//������ϣ����������һ֡
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
		}	

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
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
					}
 /*******************************************Ƶ��ɨ��֡**********************************************************************************/					
					else if((USART_RX_BUF[1]=='f')&&(USART_RX_BUF[2]=='r')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')){//Ƶ��ɨ��֡
						RDA5820_RX_Mode();			//����ģʽ
						freqset=FREQUENCY_MIN;
						RDA5820_Freq_Set(freqset);	//����Ƶ��Ϊ��Сֵ					

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
						index_frame_send+=3;

						for(j=0;j<FREQUENCY_POINT;j++){
							
							RDA5820_Freq_Set(freqset);//����Ƶ��
							delay_ms(90);//�ȴ�10ms��Ƶ�ź��ȶ�
							frame_send_buf[8]=RDA5820_Rssi_Get();//�õ��ź�ǿ��
							frame_send_buf[9]=XOR(frame_send_buf,index_frame_send-1);
							for(t=0;t<index_frame_send;t++)
							{
								USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
								while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							}
							delay_ms(10);
							frame_send_buf[7]++;

							if(freqset<FREQUENCY_MAX)freqset+=10;  //Ƶ������100Khz
							else freqset=FREQUENCY_MIN;		 	//Խ�紦�� 
						 }

						 RDA5820_TX_Mode();			//Ƶ��ɨ��֮���л��ط���ģʽ
						 RDA5820_Freq_Set(send_frequency);	//����Ƶ�ʣ���Ϊȫ�ֱ���
						 delay_ms(20);
						 //printf("\r\nƵ��ɨ��֡\r\n");
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
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
								send_frequency=USART_RX_BUF[7]*10;
								RDA5820_Freq_Set(send_frequency);	//����Ƶ�ʣ���Ϊȫ�ֱ���
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
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
					}
 /*******************************************����֡**********************************************************************************/					
					else if((USART_RX_BUF[1]=='d')&&(USART_RX_BUF[2]=='a')&&(USART_RX_BUF[3]=='t')&&(USART_RX_BUF[4]=='_')){//����֡
						frame_send_buf[index_frame_send]='$';
						index_frame_send++;
						frame_send_buf[index_frame_send]='d';
						index_frame_send++;
						frame_send_buf[index_frame_send]='a';
						index_frame_send++;
						frame_send_buf[index_frame_send]='t';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]='_';
						index_frame_send++;
						frame_send_buf[index_frame_send]=USART_RX_BUF[5];
						index_frame_send++;
						frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
						index_frame_send++;
				   /*******************����֡����1�ֽ�תΪ4bits����ʼ*********************************/
				   		fm_total_bytes=USART_RX_BUF[6]*256+USART_RX_BUF[7];
					if((fm_total_bytes==FRAME_WAKEUP_BROADCAST )||(fm_total_bytes==FRAME_WAKEUP_UNICAST  )||(fm_total_bytes==FRAME_WAKEUP_MULTICAST  )||(fm_total_bytes==FRAME_CONTROL  )||(fm_total_bytes==FRAME_SECURTY  )){
						for(t=0;t<fm_total_bytes;t++){
							fm_frame_byte[fm_frame_index_byte]=USART_RX_BUF[t+8]-0x30;
							fm_frame_index_byte++;
						}
						flag_byte_ready=1;//����֡�ֽ������浽���أ��ɹ�
				   /*******************����֡����1�ֽ�תΪ4bits������*********************************/		
						for(t=0;t<index_frame_send;t++)
						{
//							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
//							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
					}else{
						flag_byte_ready=0;//����֡�ֽ������浽���أ�δ�ɹ�
						fm_frame_index_byte=0;//�ֽ����������
//						printf("\r\nData frame error.\r\n");
						}
					}//else{printf("Frame anomalous!");}
				}//else{printf("Verify bits wrong!");}
		    }//if(USART_RX_BUF[0]=='$')
						
		}//if(USART_RX_STA&0x8000)
			
	}//while(1)
}





unsigned char XOR(unsigned char *BUFF, u16 len)
{
	unsigned char result=0;
	u16 i;
	for(result=BUFF[0],i=1;i<len;i++)
	{
		result ^= BUFF[i];
	}
	return result;
}







