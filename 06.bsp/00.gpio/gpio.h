#ifndef __GPIO_H_
#define __GPIO_H_

#define Debug_Pin0_ON						(GPIOE->BSRRL = GPIO_Pin_10)
#define Debug_Pin0_OFF						(GPIOE->BSRRH = GPIO_Pin_10)

/*
** 升级闪烁灯--修改:20210323--运行闪烁灯
*/
#define Upgrade_FlickerLedON				(GPIOF->BSRRL = GPIO_Pin_10)
#define Upgrade_FlickerLedOFF				(GPIOF->BSRRH = GPIO_Pin_10)

/*
** 固件控制标号 
*/
enum{
	HardwareCtr_UpgradeLed = 0x00,/*升级闪烁灯*/
};

void GPIO_Cfg_Init(void);
/*
** 固件控制
*/
void GPIO_HardwareCtr(uint8 label,bool flag);
#endif

