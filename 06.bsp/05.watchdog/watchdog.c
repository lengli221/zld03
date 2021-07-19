#include "includes.h"

void watchdogInit(void)
{
	/*Enable the LSI OSC*/
	RCC_LSICmd(ENABLE);

	/*Wait till LSI is ready*/
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	{}

	/*IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency dispersion)*/
	/*Enable write access to IWDG_PR and IWDG_RLR registers*/
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

	/*IWDG counter clock: LSI/32*/
	IWDG_SetPrescaler(IWDG_Prescaler_32);

	/* Set counter reload value to obtain 250ms IWDG TimeOut.
	 Counter Reload Value = 250ms/IWDG counter clock period
	                      = 250ms / (LSI/32)
	                      = 0.25s / (LsiFreq/32)
	                      = LsiFreq/(32 * 4)
	                      = LsiFreq/128
	*/
	/*
	** 20210709--��ע:40000/32=1250--���Ź���ʱʱ��Ϊ1.25
	** ���ݹٷ������ֲ�--������FLASH������128KBʱ��,type=1,max=2 
	** �ʽ����Ź�ʱ�����Ϊ:2.5S
	*/
	IWDG_SetReload(2500);

	/* Reload IWDG counter */
	IWDG_ReloadCounter();

	/*Enable IWDG (the LSI oscillator will be enabled by hardware)*/
	IWDG_Enable();
}

void watchdogUpdate(void)
{
	IWDG_ReloadCounter();
}

