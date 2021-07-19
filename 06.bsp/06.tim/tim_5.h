#ifndef __TIM_5_H
#define __TIM_5_H

void TIM5_Init(void);

/*
** 超时定时器结构体声明
*/
typedef struct{
	uint32 cnt;
}SysTickOut;

/*
** 超时函数处理
*/
bool TickOut(uint32* timeCnt,uint32 cnt);

#endif

