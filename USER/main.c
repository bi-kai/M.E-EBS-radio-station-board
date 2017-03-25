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
#include "gray.h"


#define FREQUENCY_MIN 7600
#define FREQUENCY_MAX 10100
#define FREQUENCY_POINT (FREQUENCY_MAX-FREQUENCY_MIN)/10+1
#define FRAME_WAKEUP_BROADCAST 120*2/4
#define FRAME_WAKEUP_UNICAST 144*2/4
#define FRAME_WAKEUP_MULTICAST 168*2/4
#define FRAME_CONTROL 84*2/4
#define FRAME_SECURTY 84/4

//u8 flag_frame_processing=0;//�յ�������֡���ڴ����־λ��1:������;0:����;����״̬��ѯ������FSK�����в��������
u8 index_frame_send=0;//���ڻظ���Ϣ֡�±�
u8 frame_send_buf[100]={0};//���ڻش�������

u16 fm_frame_index_bits=0;//FM�㲥01���б������±�
u8 fm_frame_bits[1100]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,0};//FM�㲥֡���б�����������
u16 fm_frame_index_byte=0;//FM�㲥01�����ֽ����±�
u8 fm_frame_byte[1100/2]={0};//FM�㲥֡�����ֽ���������

void frame_resent(void);
void safe_soc(void);
u8 flag_byte_ready=0;//�ֽ����Ѿ�ȫ�����浽����buffer��־λ

//u8 temp_frame_byte[1100/2]={0};//��������
u16 temp_frame_byte_index=0;//��������

u8 usart2_works=0;//����2����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3����������֡��4����������֡��
u8 usart1_works=0;//����1����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3������Ƶ��ɨ��֡��4������Ƶ��ɨ�跴��֡��
				  //5:���տ���֡;6:���Ϳ��Ʒ���֡;7:��������֡;8:�������ݷ���֡(�����ش�֡);9������֡���ߴ����У�
u8 flag_safe_soc=0;//��ȫоƬ�Ƿ���ñ�־λ��0�����ã�1�������ã�
u8 frame_safe[40]={0};//��֤֡���飬������ȫоƬ
u8 index_frame_safe=0;//��֤֡�����±�������ȫоƬ
u8 index_safe_times=0;//��֤֡���ʹ���������
u8 flag_safe_frame=0;//��֡����֤֡
u8 flag_safe_soc_ok=0;//��ȫоƬ��ʱ��Ӧ��1��δӦ��0����Ӧ��
int main(void)
{	 
	u16 freqset=FREQUENCY_MIN;//���ա�����Ƶ��
	u16 send_frequency=FREQUENCY_MIN;//����Ƶ�㣬ֻ���淢���Ƶ��  
	u16 t=0,j=0;
	signed char i=0;//�ֽ�ת������
	u16 len,len1;
	u16 fm_total_bytes=0;//����֡����Ч�ֽ�����

	u16 safe_total_bytes=0;//��ȫ֡���ֽ���
	u8 safe_mingwen[84]={0};//84bits������Ϣ�Ķ�������
	u8 safe_miwen[384]={0};//384bits��ȫоƬ���ص�������Ϣ
	u8 safe_mingwen_index=0;
	u16 safe_miwen_index=0;
	u8 before_gray[12]={0};//���ױ���ǰ��12λ�Ĵ�
	u8 after_gray[24]={0};//���ױ�����24λ�Ĵ�
	
//	u16 times=0;

//	u8 bit_sync[14]={1,0,1,0,1,0,1,0,1,0,1,0,1,0};//λͬ��ͷ
//	u8 frame_sync[11]={1,1,1,0,0,0,1,0,0,1,0};//֡ͬ��ͷ	

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 	//����1��ʼ��Ϊ9600
	uart2_init(4800);	 //����2��ʼ��Ϊ4800
//	KEY_Init();
 	LED_Init();			     //LED�˿ڳ�ʼ��
	TIM5_Int_Init(2999,7199);//��ȫоƬ���ڲ�ѯ��1s�ж�һ�Σ�5���ѯһ��
//	TIM4_Int_Init(9999,7199);//��ȫоƬӦ��ʱ���
	TIM3_Int_Init(9,7199);//1Khz��FSK����
	TIM7_Int_Init(4,1199);//6K���ڷ���
	TIM6_Int_Init(4,719);//10k���ڷ���
	tim3_pin_init(); 
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
//		if(USART2_RX_STA&0x8000)
//		{					   
//			len=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(USART2, USART2_RX_BUF[t]);  //�򴮿�2��������
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}
//			printf("\r\n\r\n");  //���뻻��
//			USART2_RX_STA=0;
//		}
/******************************************************************����2��������************************************************************************************************/
		if(USART2_RX_STA&0x8000)
		{	
			   
			len1=USART2_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���

			if(USART2_RX_BUF[0]=='$'){
				if(USART2_RX_BUF[len1-1]==XOR(USART2_RX_BUF,len1-1)){
		//			safe_frame_byte_index=0;
		//			safe_mingwen_index=0;
					safe_miwen_index=0;
 					/*******************************************��ȫоƬ���ӷ���֡**********************************************************************************/
					if((USART2_RX_BUF[1]=='s')&&(USART2_RX_BUF[2]=='a')&&(USART2_RX_BUF[3]=='f')&&(USART2_RX_BUF[4]=='_')&&(USART2_RX_BUF[5]=='_')){//����֡
						flag_safe_soc=0;//��ǰ�ȫоƬ����
				//		printf("safe chip, timer checkd!\r\n");
						USART2_RX_STA=0;//������ϣ����������һ֡
						LED0=1;//�رվ�ʾ��
						USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
					}
 					/*******************************************��ȫоƬ��֤���ݻش�֡**********************************************************************************/
				 	else if((flag_safe_soc_ok==0)&&(USART2_RX_BUF[1]=='e')&&(USART2_RX_BUF[2]=='c')&&(USART2_RX_BUF[3]=='c')&&(USART2_RX_BUF[4]=='_')&&(USART2_RX_BUF[5]=='_')){//����֡
						flag_safe_soc=0;//��ǰ�ȫоƬ����					

						safe_total_bytes=USART2_RX_BUF[7]*256+USART2_RX_BUF[8];//�õ������ܵĳ���
						for(t=0;t<safe_total_bytes;t++){//�����Ĳ�ɱ�����
							for (i=3;i>=0;i--)
							{
								safe_miwen[safe_miwen_index]=((USART2_RX_BUF[t]-0x30)&(0x01<<i))?1:0; 
								safe_miwen_index++;
							}
						}
						//�������Ŀ�ʼ���ױ��룬��֡������
						fm_frame_index_bits=25;//FM��������������
						for (t=0;t<(safe_mingwen_index/12);t++)//���ױ������
						{
							for (i=0;i<12;i++)
							{
								before_gray[i]=safe_mingwen[t*12+i];
							}
							gray_encode(before_gray,after_gray);
							for (i=0;i<24;i++)
							{
								fm_frame_bits[fm_frame_index_bits]=after_gray[i];
								fm_frame_index_bits++;
							}
							
						}
						for (t=0;t<(safe_miwen_index/12);t++)//���ױ������
						{
							for (i=0;i<12;i++)
							{
								before_gray[i]=safe_miwen[t*12+i];
							}
							gray_encode(before_gray,after_gray);
							for (i=0;i<24;i++)
							{
								fm_frame_bits[fm_frame_index_bits]=after_gray[i];
								fm_frame_index_bits++;
							}
							
						}

						TIM_Cmd(TIM3, ENABLE); //ʹ��TIMx
						TIM_Cmd(TIM6, ENABLE); //ʹ��TIMx
						TIM_Cmd(TIM7, ENABLE); //ʹ��TIMx

						USART2_RX_STA=0;//������ϣ����������һ֡
						usart2_works=0;//������ϣ������־λ
						fm_frame_index_byte=0;//����Ҫ�ش��ˣ��������1�յ����ֽ�
					}
 					/*******************************************��ȫоƬ�����ش�**********************************************************************************/
				 	else if((USART2_RX_BUF[1]=='r')&&(USART2_RX_BUF[2]=='t')&&(USART2_RX_BUF[3]=='s')&&(USART2_RX_BUF[4]=='_')&&(USART2_RX_BUF[5]=='_')){//����֡
					   safe_soc();
					}else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);usart2_works=0;}
				 }else{safe_soc();USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}//end of XOR��XOR���������ش�
				 }else {USART2_RX_STA=0;USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}//end of check '$'


		}else {USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);}




/******************************************************************����1���յ�����֡�����ʹ���************************************************************************************/
		if(flag_byte_ready==1){//���ݱ��汾�سɹ�
			usart1_works=9;//�������ߴ�����
			if(flag_safe_frame==1){//��֤֡
				safe_soc();//�����ķ��͸���ȫоƬ
				flag_safe_soc_ok=1;
				TIM5->ARR=2999;//����״̬��ʱ����ֵ
				usart2_works=3;//��Ǵ���2�շ���������
				safe_mingwen_index=0;//�����Ĳ�Ϊ������
				for(t=0;t<21;t++){
					for (i=3;i>=0;i--)
					{
						safe_mingwen[safe_mingwen_index]=(fm_frame_byte[t]&(0x01<<i))?1:0; 
						safe_mingwen_index++;
					}
				}
			}else{			
			
				fm_frame_index_bits=25;//FM��������������
				for(t=0;t<fm_frame_index_byte;t++){
					for (i=3;i>=0;i--)
					{
						fm_frame_bits[fm_frame_index_bits]=(fm_frame_byte[t]&(0x01<<i))?1:0; 
						fm_frame_index_bits++;
					}
				}
				TIM_Cmd(TIM3, ENABLE); //ʹ��TIMx
				TIM_Cmd(TIM6, ENABLE); //ʹ��TIMx
				TIM_Cmd(TIM7, ENABLE); //ʹ��TIMx

				
			}
//			flag_frame_processing=1;
//			for (temp_frame_byte_index=0;temp_frame_byte_index<fm_frame_index_bits/4;temp_frame_byte_index++)
//			{
//				temp_frame_byte[temp_frame_byte_index]=fm_frame_bits[temp_frame_byte_index*4]*8+fm_frame_bits[temp_frame_byte_index*4+1]*4+fm_frame_bits[temp_frame_byte_index*4+2]*2+fm_frame_bits[temp_frame_byte_index*4+3]*1+0x30;//ASCII 0���Ӧʮ������0x30
//			}

//			for(t=0;t<fm_frame_index_bits;t++)
//			{
//				USART_SendData(USART2, fm_frame_bits[t]);//�򴮿ڷ�������
//				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}

			flag_byte_ready=0;//������ϣ�����
			if(usart2_works==0)	{
				fm_frame_index_byte=0;//�ֽ����������
				USART_RX_STA=0;//������ϣ����������һ֡
			}
			flag_safe_frame=0;//�����֤֡��־λ
			usart1_works=0;//������ϣ���־����
		}
		
			
/******************************************************************����1��������************************************************************************************************/
	   if((USART_RX_STA&0x8000)&&(flag_byte_ready==0))
		{					   
			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���

			if(USART_RX_BUF[0]=='$'){
				if(USART_RX_BUF[len-1]==XOR(USART_RX_BUF,len-1)){
					index_frame_send=0;
 					/*******************************************�Ӱ������ж�֡************************************************/
					if((USART_RX_BUF[1]=='r')&&(USART_RX_BUF[2]=='d')&&(USART_RX_BUF[3]=='y')&&(USART_RX_BUF[4]=='_')){//����֡
						usart1_works=1;	//�յ�����֡
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
						 usart1_works=2;//�������ӷ���֡
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 //printf("\r\n�Ӱ�����֡\r\n");
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
						 usart1_works=0;//�����꣬��־����
					}
 					/*******************************************Ƶ��ɨ��֡*************************************************************/					
					else if((USART_RX_BUF[1]=='f')&&(USART_RX_BUF[2]=='r')&&(USART_RX_BUF[3]=='e')&&(USART_RX_BUF[4]=='_')){//Ƶ��ɨ��֡
						USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//Ƶ��ɨ��֡���͹����������жϣ����ж�
						usart1_works=3;//���յ�Ƶ��ɨ��֡
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
						index_frame_send+=4;

						usart1_works=4;//����Ƶ��ɨ�跴��֡
						for(j=0;j<FREQUENCY_POINT;j++){
							
							RDA5820_Freq_Set(freqset);//����Ƶ��
							delay_ms(10);//�ȴ�10ms��Ƶ�ź��ȶ�
							if(RDA5820_RD_Reg(0X0B)&(1<<8)){//��һ����Ч��̨. 
								frame_send_buf[9]=1;		 
							}else{
								frame_send_buf[9]=0;
							}
							delay_ms(10);
							frame_send_buf[8]=RDA5820_Rssi_Get();//�õ��ź�ǿ��
							frame_send_buf[10]=XOR(frame_send_buf,index_frame_send-1);
							
							for(t=0;t<index_frame_send;t++)
							{
								if(usart1_works==0){
									break;
								}
								USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
								while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
							}
							if(usart1_works==0){

								break;
							}
						//	delay_ms(10);
							frame_send_buf[7]++;

							if(freqset<FREQUENCY_MAX)freqset+=10;  //Ƶ������100Khz
							else freqset=FREQUENCY_MIN;		 	//Խ�紦�� 
						 }

						 RDA5820_TX_Mode();			//Ƶ��ɨ��֮���л��ط���ģʽ
						 RDA5820_Freq_Set(send_frequency);	//����Ƶ�ʣ���Ϊȫ�ֱ���
						 delay_ms(20);					 
						 USART_RX_STA=0;//������ϣ����������һ֡
						 usart1_works=0;//�����꣬��־����
					}
 					/*******************************************����֡*************************************************************/
					else if((USART_RX_BUF[1]=='c')&&(USART_RX_BUF[2]=='o')&&(USART_RX_BUF[3]=='n')&&(USART_RX_BUF[4]=='_')){//����֡
						usart1_works=5;//���յ�����֡
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
						 usart1_works=6;//���Ϳ��Ʒ���֡
						 for(t=0;t<index_frame_send;t++)
						 {
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						 }
						 //printf("\r\n����֡\r\n");
						 USART_RX_STA=0;//������ϣ����������һ֡
						 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
						 usart1_works=0;//������ϣ���־����
					}
 					/*******************************************����֡*************************************************************/					
					else if((USART_RX_BUF[1]=='d')&&(USART_RX_BUF[2]=='a')&&(USART_RX_BUF[3]=='t')&&(USART_RX_BUF[4]=='_')){//����֡
						usart1_works=7;//���յ�����֡
				//		frame_resent();���������ش�

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
						if(fm_total_bytes==FRAME_SECURTY){flag_safe_frame=1;}//�յ�����֤֡
						flag_byte_ready=1;//����֡�ֽ������浽���أ��ɹ�
				   /*******************����֡����1�ֽ�תΪ4bits������*********************************/
				   		usart1_works=8;//�������ݷ���֡		
						for(t=0;t<index_frame_send;t++)
						{
							USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
					}else{
						flag_byte_ready=0;//����֡�ֽ������浽���أ�δ�ɹ�
						fm_frame_index_byte=0;//�ֽ����������
						USART_RX_STA=0;
						usart1_works=0;//�쳣����־����	
//						printf("\r\nData frame error.\r\n");
						}
					}else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//else{printf("Frame anomalous!");}
				}else {//����֡��У��ͳ���ʱ�������ش�
					USART_RX_STA=0;
					USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
					frame_resent();//���ش�
					usart1_works=0;		
					}//else{printf("Verify bits wrong!");}
		    }else {USART_RX_STA=0;USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);usart1_works=0;}//if(USART_RX_BUF[0]=='$')
						
		}else {USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);}//if(USART_RX_STA&0x8000)
			
	}//while(1)
}



void frame_resent(void){
	u16 t=0;
	
if((USART_RX_BUF[1]=='d')&&(USART_RX_BUF[2]=='a')&&(USART_RX_BUF[3]=='t')&&(USART_RX_BUF[4]=='_')){//����֡
	index_frame_send=0;
	frame_send_buf[index_frame_send]='$';
	index_frame_send++;
	frame_send_buf[index_frame_send]='r';
	index_frame_send++;
	frame_send_buf[index_frame_send]='s';
	index_frame_send++;
	frame_send_buf[index_frame_send]='t';
	index_frame_send++;
	frame_send_buf[index_frame_send]='_';
	index_frame_send++;
	frame_send_buf[index_frame_send]='_';
	index_frame_send++;
	frame_send_buf[index_frame_send]=0;
	index_frame_send++;
	frame_send_buf[index_frame_send]=0;//�ش�����֡���͡�1���㲥����֡��2����������֡��3���鲥����֡��4������֡��5����֤֡��
	index_frame_send++;
	frame_send_buf[index_frame_send]=XOR(frame_send_buf,index_frame_send);
	index_frame_send++;

//	fm_total_bytes=USART_RX_BUF[6]*256+USART_RX_BUF[7];
//	if((fm_total_bytes==FRAME_WAKEUP_BROADCAST )||(fm_total_bytes==FRAME_WAKEUP_UNICAST  )||(fm_total_bytes==FRAME_WAKEUP_MULTICAST  )||(fm_total_bytes==FRAME_CONTROL  )||(fm_total_bytes==FRAME_SECURTY  )){
//
//	}

	for(t=0;t<index_frame_send;t++)
	{
		USART_SendData(USART1, frame_send_buf[t]);//�򴮿ڷ�������
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	flag_byte_ready=0;//����֡�ֽ������浽���أ�δ�ɹ�
	fm_frame_index_byte=0;//�ֽ����������


	}

}

void safe_soc(void){
	u16 t=0;
	index_frame_safe=0;
	frame_safe[index_frame_safe]='$';
	index_frame_safe++;
	frame_safe[index_frame_safe]='d';
	index_frame_safe++;
	frame_safe[index_frame_safe]='a';
	index_frame_safe++;
	frame_safe[index_frame_safe]='t';
	index_frame_safe++;
	frame_safe[index_frame_safe]='_';
	index_frame_safe++;
	if (index_safe_times<200)
	{
		index_safe_times++;
	} 
	else
	{
		index_safe_times=0;
	}
	frame_safe[index_frame_safe]=index_safe_times;
	index_frame_safe++;
	frame_safe[index_frame_safe]=(84/4)/256;
	index_frame_safe++;
	frame_safe[index_frame_safe]=(84/4)%256;
	index_frame_safe++;
	for(t=0;t<fm_frame_index_byte;t++){
		frame_safe[index_frame_safe]=fm_frame_byte[t]+0x30;
		index_frame_safe++;
	}
	frame_safe[index_frame_safe]=XOR(frame_safe,index_frame_safe);
	index_frame_safe++;
	frame_safe[index_frame_safe]='\r';
	index_frame_safe++;
	frame_safe[index_frame_safe]='\n';
	index_frame_safe++;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��ʱ�򿪣�ֻΪ����
	for(t=0;t<index_frame_safe;t++)//��֤֡ͨ������2���͸���ȫоƬ
	{
		USART_SendData(USART2, frame_safe[t]);//�򴮿ڷ�������
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
	}
	
	

}



