#include "includes.h"

void GPIO_Cfg_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF, ENABLE);

	/*���GPIO*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/*����*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init((GPIO_TypeDef *)GPIOE, (GPIO_InitTypeDef *)&GPIO_InitStructure);

	/*
	** ������˸��
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init((GPIO_TypeDef *)GPIOF, (GPIO_InitTypeDef *)&GPIO_InitStructure);	

	Debug_Pin0_OFF;

	/*
	** ������˸��
	*/
	Upgrade_FlickerLedOFF;
}

/*
** �̼�����
*/
void GPIO_HardwareCtr(uint8 label,bool flag){
	switch(label){
		case HardwareCtr_UpgradeLed:
			if(flag == true){
				Upgrade_FlickerLedON;
			}else{
				Upgrade_FlickerLedOFF;
			}
			break;
	}
}


