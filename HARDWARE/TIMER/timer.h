#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
#define CHIP_CHECK_FREQUENCY 6
#define FRAME_INTERVAL 100//唤醒帧：相邻两帧的间隔
#define FRAME_INTERVAL_CONTROL 100//控制帧：相邻两帧的间隔
#define FSK_EXTEND 20//fsk信号比实际控制信号前后各延长的毫秒数	

void TIM3_Int_Init(u16 arr,u16 psc);
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);
void TIM6_Int_Init(u16 arr,u16 psc);
void TIM7_Int_Init(u16 arr,u16 psc);
void tim3_pin_init(void);
unsigned char XOR(unsigned char *BUFF, u16 len);//异或校验和
  
#endif
