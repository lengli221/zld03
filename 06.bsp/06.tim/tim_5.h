#ifndef __TIM_5_H
#define __TIM_5_H

void TIM5_Init(void);

/*
** ��ʱ��ʱ���ṹ������
*/
typedef struct{
	uint32 cnt;
}SysTickOut;

/*
** ��ʱ��������
*/
bool TickOut(uint32* timeCnt,uint32 cnt);

#endif

