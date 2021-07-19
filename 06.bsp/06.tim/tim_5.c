#include "includes.h"

SysTim *sysTim;
/*
** ��ʱ��ʱ���ṹ�����
*/
SysTickOut sysTickout;

/*
** Note:�ж�ʱ��--100ms
*/
void TIM5_IRQHandler(void){
	if(TIM_GetITStatus((TIM_TypeDef *)TIM5, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit((TIM_TypeDef *)TIM5, TIM_IT_Update);
		sysTim->cnt++;
		/*
		** ��ʱ��ʱ����������
		*/
		sysTickout.cnt++;
		/*˵��:��ʱ(1min)δ�յ�֡����--�޸�ʱ��5S*/
		if(sysTim->cnt >= 50/*600*/){
			sysTim->cnt = 50/*600*/;
			sysTim->Flag = true;
		}
	}
}

static void TIM5_Config(void){
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure0;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_DeInit(TIM5);
	TIM_TimeBaseStructure0.TIM_Period = 9999;
	TIM_TimeBaseStructure0.TIM_Prescaler =599;
	TIM_TimeBaseStructure0.TIM_ClockDivision = TIM_CKD_DIV2;
	TIM_TimeBaseStructure0.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure0);
	TIM_ARRPreloadConfig((TIM_TypeDef *)TIM5, ENABLE);
	TIM_ClearFlag(TIM5,TIM_FLAG_Update);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM5, ENABLE); 
}

void TIM5_Init(void){
	sysTim = Upgrade_GetSysTim();
	sysTim->cnt = 0;
	sysTim->Flag = false;
	/*
	** �峬ʱ��ʱ��������
	*/
	memset((uint8*)&sysTickout.cnt,0x00,sizeof(SysTickOut));
	/*
	** ��ʱ��ʼ��
	*/
	TIM5_Config();
}

/*
** ��ʱ��������
*/
bool TickOut(uint32* timeCnt,uint32 cnt){
	bool ret = 0;

	if(cnt == 0){
		*timeCnt = sysTickout.cnt;
		ret = true;
	}else{
		if((sysTickout.cnt - *timeCnt) >= cnt){
			ret = true;
		}else{
			ret = false;
		}
	}
	return(ret);
}

