#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
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
#define CHIP_CHECK_FREQUENCY 6
#define FRAME_INTERVAL 100//����֡��������֡�ļ��
#define FRAME_INTERVAL_CONTROL 100//����֡��������֡�ļ��
#define FSK_EXTEND 20//fsk�źű�ʵ�ʿ����ź�ǰ����ӳ��ĺ�����	

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void tim3_pin_init(void);
unsigned char XOR(unsigned char *BUFF, u16 len);//���У���
  
#endif
