#include "timer.h"
#include "led.h"
#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
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



u8 frame_wakeup_broadcast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
1,1,1,0,0,0,1,0,0,1,0,
0,1,0,0,
0,0,1,0,
0,1,1,0,0,0,1,1,0,1,0,1,1,1,1,1,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
0,1,0,0,1,1,1,1,0,1,1,0,1,1,1,0,0,1,1,1,1,0,1,1,0,0,1,0,1,1,0,1,1,0,1,1,1,1,1,0,0,1,0,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,1,0,0,1,1,0};

u8 frame_wakeup_unicast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
1,1,1,0,0,0,1,0,0,1,0, 
0,1,0,0,
0,0,1,0,
1,0,1,0,0,1,0,0,0,0,0,0,1,1,1,0,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1,1,0,0,1,1,1,0,0,1,0,1,0,1,1,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,1,1,1,1};

u8 frame_wakeup_multicast[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
1,1,1,0,0,0,1,0,0,1,0,
0,1,0,0,
0,0,1,0,
1,1,1,0,1,1,1,0,1,0,0,1,1,0,0,1,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0, 
0,1,1,0,1,0,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,0,1,1,0,1,0,1,1,1,0,0,0,0,0,1,0,1,1,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,0,0,0,1,1,1,1,1,1}; 

u8 frame_control[]={1,0,1,0,1,0,1,0,1,0,1,0,1,0,
1,1,1,0,0,0,1,0,0,1,0,
1,0,0,0,
0,0,0,0,0,0,0,1,0,1,0,0,1,0,0,1,1,1,1,0,
1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,1,0,0,1,1,1,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,0,1,0,1,1,0,
0,1,0,0,0,1,0,1,0,1,0,0,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,0,0,1,1,1,0,1,1,0,1,0,1,1,1,0,0,1,1,0,1,1,1,1,0,0,1,0,0,0,1,1,0,1,0,1,1,0,0,0,1,0,1,0};


u16 t=0;//���������±������
extern u16 fm_frame_index_bits;//FM�㲥01���б������±�
extern u8 fm_frame_bits[1100];//FM�㲥֡���б�����������
extern u8 flag_frame_processing;//�յ�������֡���ڴ����־λ��1:������;0:����;
//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{	
//		if(t<fm_frame_index_bits){
//			PBout(6)=fm_frame_bits[t];//����Ҫ��λͬ����֡ͬ��ͷ
//			t++;
//		}
		if(t<fm_frame_index_bits)//�㲥25+120*2������25+144*2���鲥25+168*2������֡��25+84*2��
		{
			PBout(6)=fm_frame_bits[t];//frame_control[t];
			t++;
		}else//֡������Ϻ������
		{
			PBout(6)=0;
			t=0;
			TIM_Cmd(TIM3,DISABLE);
			TIM_Cmd(TIM6,DISABLE);
			TIM_Cmd(TIM7,DISABLE);
			flag_frame_processing=0;
		}//����֡ѭ������
		

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
u8 frame_send_buf_chip[100]={0};//���ڻش�������
u8 index_chipcheck_times=0;//оƬ��ѯ����
extern u8 main_busy;//��ѭ��������ִ��
void TIM5_IRQHandler(void)   //TIM5�ж�
{
	u16 t=0;
	u8 index_frame_send_chip=0;//���ڻظ���Ϣ֡�±�

	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM5�����жϷ������
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		LED1=!LED1;
		if((secury_chip_ckeck>=CHIP_CHECK_FREQUENCY)&&(main_busy!=1)){
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
			frame_send_buf_chip[index_frame_send_chip]=XOR(frame_send_buf_chip,index_frame_send_chip);
			index_frame_send_chip++;
			
			for(t=0;t<index_frame_send_chip;t++)
			{
				USART_SendData(USART2, frame_send_buf_chip[t]);  //�򴮿�2��������
				while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}

			secury_chip_ckeck=0;		
		}else{
			secury_chip_ckeck++;
		}
	}
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
				PBout(8)=~PBout(8);
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
				PBout(9)=~PBout(9);
//				index_tim7++;
//				
//			}else//֡������Ϻ������
//			{
//				PBout(9)=0;
//				index_tim7=0;			
//			}//����֡ѭ������
				
		}
}





