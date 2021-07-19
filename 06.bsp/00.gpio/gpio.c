#include "includes.h"

void GPIO_Cfg_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOG | RCC_AHB1Periph_GPIOF, ENABLE);

	/*Êä³öGPIO*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	/*Òý½Å*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init((GPIO_TypeDef *)GPIOE, (GPIO_InitTypeDef *)&GPIO_InitStructure);

	/*
	** Éý¼¶ÉÁË¸µÆ
	*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init((GPIO_TypeDef *)GPIOF, (GPIO_InitTypeDef *)&GPIO_InitStructure);	

	Debug_Pin0_OFF;

	/*
	** Éý¼¶ÉÁË¸µÆ
	*/
	Upgrade_FlickerLedOFF;
}

/*
** ¹Ì¼þ¿ØÖÆ
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


