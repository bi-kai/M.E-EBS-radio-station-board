#include "timer.h"
#include "led.h"
#include "usart.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
  	 

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM3, DISABLE);  //�ر�TIMx					 
}



//u8 frame_wakeup_broadcast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
//1,1,1,0,0,0,1,0,0,1,0,
//0,1,0,0,
//0,0,1,0,
//0,1,1,0,0,0,1,1,0,1,0,1,1,1,1,1,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
//0,1,0,0,1,1,1,1,0,1,1,0,1,1,1,0,0,1,1,1,1,0,1,1,0,0,1,0,1,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,1,1,0};
//
//u8 frame_wakeup_unicast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
//1,1,1,0,0,0,1,0,0,1,0, 
//0,1,0,0,
//0,0,1,0,
//1,0,1,0,0,1,0,0,0,0,0,0,1,1,1,0,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
//0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
//0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,1,0,1,0,1,1,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,1,1,1,1};
//
//u8 frame_wakeup_multicast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
//1,1,1,0,0,0,1,0,0,1,0,
//0,1,0,0,
//0,0,1,0,
//1,1,1,0,1,1,1,0,1,0,0,1,1,0,0,1,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
//0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
//0,1,1,0,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,1,1,1}; 
//
//u8 frame_control[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
//1,1,1,0,0,0,1,0,0,1,0,
//1,0,0,0,
//0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,0,
//1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
//0,1,0,0,0,1,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,1,1,0,1,0,1,1,1,0,0,1,1,0,1,1,1,1,0,0,1,0,0,0,1,1,0,1,0,1,1,0,0,0,1,0,1,0};


u16 t=0;//���������±������
extern u16 fm_frame_index_bits;//FM�㲥01���б������±�
extern u8 fm_frame_bits[1100];//FM�㲥֡���б�����������
extern u8 usart2_works;//����2����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3����������֡��4����������֡(�����ش�֡)��
extern u8 usart1_works;//����1����״ָ̬ʾ��0�����У�1����������֡��2���������ӷ���֡��3������Ƶ��ɨ��֡��4������Ƶ��ɨ�跴��֡��
				  //5:���տ���֡;6:���Ϳ��Ʒ���֡;7:��������֡;8:�������ݷ���֡(�����ش�֡);9������֡���ߴ����У�
u8 timer_67_stop=0;//��ʱ��6,7��ֹͣ��־λ��0��δ����ֹ��1������ֹ��
extern u16 USART_RX_STA;       //����1״̬��
extern u16 USART2_RX_STA;       //����2״̬��
extern u16 fm_frame_index_byte;//FM�㲥01����תΪ�ֽ������±�
extern u8 flag_is_wakeup_frame;//��ǰ֡�ǻ���֡�ı�־λ
extern u8 wakeup_times;//����֡���͵Ĵ���
u8 wakeup_times_index=0;//����֡���ʹ���������ֵ
u8 delay_index=0;//֡����ӳٵ������±�
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{	
//		if(t<fm_frame_index_bits){
//			PBout(6)=fm_frame_bits[t];//����Ҫ��λͬ����֡ͬ��ͷ
//			t++;
//		}
		if(flag_is_wakeup_frame==0){//�ǻ���֡
			if(t<fm_frame_index_bits)//�㲥25+120*2������25+144*2���鲥25+168*2������֡��25+84*2��
			{  			
				PAout(7)=fm_frame_bits[t];//frame_control[t];PBout(6)
				timer_67_stop=0;
				t++;
			}else//֡������Ϻ������
			{			
				TIM_Cmd(TIM3,DISABLE);
				PAout(7)=0;			
				t=0;
				timer_67_stop=1;
				delay_ms(10);//��FSK�ź�����һ��
				TIM_Cmd(TIM6,DISABLE);
				TIM_Cmd(TIM7,DISABLE);
				TIM_Cmd(TIM5, ENABLE); //ʹ��TIMx
				USART_RX_STA=0;//������ϣ����������һ֡
				usart1_works=0;//������ϣ���־����1����ʹ��
				usart2_works=0;//������ϣ���־����2����ʹ��
				
				fm_frame_index_byte=0;//�ֽ����������
				
				USART2_RX_STA=0;//������ϣ����������һ֡����ֹѭ������
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
			}//����֡ѭ������
		}
////////////////////////////////////////////////////////////////////////////////////////////////////

		if((flag_is_wakeup_frame==1)&&(wakeup_times_index<wakeup_times)){//����֡
			if(t<fm_frame_index_bits)//�㲥25+120*2������25+144*2���鲥25+168*2������֡��25+84*2��
			{  			
				PAout(7)=fm_frame_bits[t];//frame_control[t];
				timer_67_stop=0;
				t++;
			}else if(t>=fm_frame_index_bits){//�˴�Ҫ��elseǶ�ף������źŵ�ƽ����ά��һ����Ԫʱ��
				PAout(7)=0;
				if(delay_index>=FRAME_INTERVAL){//������һ֡����֡��ͣ��һ�£����ն����´���ʱ��
					wakeup_times_index++;//ͣ����ϣ���ʾһ�λ���֡��ɣ�׼��������һ��
					delay_index=0;//��������
					t=0;
				}else{
					delay_index++;
				}
				if(delay_index==FSK_EXTEND)timer_67_stop=1;//FSK�źŶ���10ms������©�������źţ�Ȼ����ر�
				if((delay_index==FRAME_INTERVAL-FSK_EXTEND)&&(wakeup_times_index<(wakeup_times-1)))timer_67_stop=0;//FSK�źŶ���10ms������©�������źţ�������ǰ��  
								
			}
		}else if((flag_is_wakeup_frame==1)&&(wakeup_times_index>=wakeup_times)&&(wakeup_times!=0)){//��η�����ϣ�����
			flag_is_wakeup_frame=0;//��λ���֡�������
			wakeup_times_index=0;
			wakeup_times=0;

			TIM_Cmd(TIM3,DISABLE);
			PAout(7)=0;			
//			t=0;
//			timer_67_stop=1;
//			delay_ms(10);//��FSK�ź�����һ��
			TIM_Cmd(TIM6,DISABLE);
			TIM_Cmd(TIM7,DISABLE);
			TIM_Cmd(TIM5, ENABLE); //ʹ��TIMx
			USART_RX_STA=0;//������ϣ����������һ֡
			usart1_works=0;//������ϣ���־����1����ʹ��
			usart2_works=0;//������ϣ���־����2����ʹ��
				
			fm_frame_index_byte=0;//�ֽ����������
			
			USART2_RX_STA=0;//������ϣ����������һ֡����ֹѭ������
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
//			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
		}

		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־
	}

}


void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM5�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM5, ENABLE);  //��TIMx					 
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



u8 secury_chip_ckeck=0;//��ȫоƬ��ѯλ��������5��5�룬��ѯһ��
u8 frame_send_buf_chip[15]={0};//����2���Ͳ�ѯ֡�Ļ�����
u8 index_chipcheck_times=0;//оƬ��ѯ����
extern u8 flag_safe_soc_ok;//��ȫоƬ��ʱ��Ӧ��1��δӦ��0����Ӧ��
u8 first_powerup=0;//��¼�״��ϵ硣�״η��Ͳ�ѯ֡���ܻ�©����һ���ַ�����֪Ϊ��
void TIM5_IRQHandler(void)   //TIM5�ж�
{
	u16 ttt=0;
	u8 index_frame_send_chip=0;//����2����֡�±�
	u8 xor_sum=0;

	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM5�����жϷ������
	{		
		LED1=!LED1;		
		if((secury_chip_ckeck>=CHIP_CHECK_FREQUENCY)&&(USART_RX_STA==0)&&(USART2_RX_STA==0)&&(usart1_works==0)&&((usart2_works==0)||(usart2_works==1))){//������5���ӣ�������1��2������ʱ����ʼ��ѯ������ȴ���һ���ж�ʱ�����ж�
			flag_safe_soc_ok=1;//��ѯǰ�ȱ�ǲ����ã�����2�յ�Ӧ����ٱ��Ϊ����
			LED0=0;//��ǲ�����
			usart2_works=1;//��������֡
			index_frame_send_chip=0;
			if(first_powerup==0){
			first_powerup=1;
				frame_send_buf_chip[index_frame_send_chip]=0;//ǰ��
				index_frame_send_chip++;
			}
			frame_send_buf_chip[index_frame_send_chip]='$';
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='s';
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='a';
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='f';
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='_';
			index_frame_send_chip++;
			if(index_chipcheck_times>200){index_chipcheck_times=0;}
			else index_chipcheck_times++;
			frame_send_buf_chip[index_frame_send_chip]=index_chipcheck_times;
			index_frame_send_chip++;
			xor_sum=XOR(frame_send_buf_chip,index_frame_send_chip);
			if((xor_sum==0x0D)||(xor_sum=='$'))xor_sum++;
			frame_send_buf_chip[index_frame_send_chip]=xor_sum;
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='\r';
			index_frame_send_chip++;
			frame_send_buf_chip[index_frame_send_chip]='\n';
			index_frame_send_chip++;
			for(ttt=0;ttt<index_frame_send_chip;ttt++)
			{
				USART_SendData(USART2, frame_send_buf_chip[ttt]);  //�򴮿�2��������
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}

			secury_chip_ckeck=0;		
		}else{
  			if(secury_chip_ckeck>50)secury_chip_ckeck=CHIP_CHECK_FREQUENCY;
			else  secury_chip_ckeck++;
			
		}
	}
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־
}


/***************************FSK*****************************/
void TIM6_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM6, DISABLE);  //�ر�TIMx							 
}

void TIM7_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM7, DISABLE);  //�ر�TIMx							 
}

void tim3_pin_init(void){
     GPIO_InitTypeDef  GPIO_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);//ʹ��PB,�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOB, &GPIO_InitStructure);

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //LED0-->PB.5 �˿�����
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOB,GPIO_Pin_8); //����ķ�����
}

//��ʱ��6�жϷ������
unsigned int index_tim6=0;
void TIM6_IRQHandler(void)   //TIM3�ж�
{
		if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  //���TIMx�����жϱ�־ 
//			if(index_tim6<(19+511-10))
//			{
			if(timer_67_stop==0)//�ñ���־λ����FSK�źŵ���
				PBout(8)=~PBout(8);
			else GPIO_ResetBits(GPIOB,GPIO_Pin_8); //����ķ�����
//				index_tim6++;
//				
//			}else//֡������Ϻ������
//			{
//				PBout(8)=0;
//				index_tim6=0;			
//			}//����֡ѭ������
				
		}
}

//��ʱ��7�жϷ������
unsigned int index_tim7=0;
void TIM7_IRQHandler(void)   //TIM3�ж�
{
   	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
		{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  //���TIMx�����жϱ�־ 
//			if(index_tim7<(19+511-10))
//			{
			if(timer_67_stop==0)//�ñ���־λ����FSK�źŵ���
				PBout(9)=~PBout(9);
			else GPIO_ResetBits(GPIOB,GPIO_Pin_9);
//				index_tim7++;
//				
//			}else//֡������Ϻ������
//			{
//				PBout(9)=0;
//				index_tim7=0;			
//			}//����֡ѭ������
				
		}
}

/***********************��ʱ��4***************************/
void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM4, DISABLE);  //�ر�TIMx							 
}

extern u16 fm_frame_index_byte;//FM�㲥01�����ֽ����±�
//��ʱ��4�жϷ������
void TIM4_IRQHandler(void)   //TIM4�ж�
{
   	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
		{
			TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIMx�����жϱ�־ 
	//		PBout(9)=~PBout(9);//�������ԣ�������ʱ��������û��
			if(flag_safe_soc_ok==1){//��ȫоƬӦ��ʱ
				LED0=0;//�򿪾�ʾ��
				TIM_Cmd(TIM3,DISABLE);
			    PAout(7)=0;
				timer_67_stop=1;			  				
				TIM_Cmd(TIM6,DISABLE);
				TIM_Cmd(TIM7,DISABLE);
				TIM_Cmd(TIM5, ENABLE); //ʹ��TIMx
				usart1_works=0;//������ϣ���־����1����ʹ��
				usart2_works=0;//������ϣ���־����2����ʹ��
				USART_RX_STA=0;
				USART2_RX_STA=0;
				fm_frame_index_byte=0;
				USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//���ж�
				USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���ж�
			}
			TIM_Cmd(TIM4,DISABLE);//�رճ�ʱ�ж϶�ʱ��	
		}
}







