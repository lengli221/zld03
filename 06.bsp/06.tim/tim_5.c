#include "includes.h"

SysTim *sysTim;
/*
** 超时定时器结构体变量
*/
SysTickOut sysTickout;

/*
** Note:中断时间--100ms
*/
void TIM5_IRQHandler(void){
	if(TIM_GetITStatus((TIM_TypeDef *)TIM5, TIM_IT_Update) != RESET){
		TIM_ClearITPendingBit((TIM_TypeDef *)TIM5, TIM_IT_Update);
		sysTim->cnt++;
		/*
		** 超时定时器参数自增
		*/
		sysTickout.cnt++;
		/*说明:超时(1min)未收到帧数据--修改时间5S*/
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
	** 清超时定时变量参数
	*/
	memset((uint8*)&sysTickout.cnt,0x00,sizeof(SysTickOut));
	/*
	** 定时初始化
	*/
	TIM5_Config();
}

/*
** 超时函数处理
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

