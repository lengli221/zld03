#ifndef __GPIO_H_
#define __GPIO_H_

#define Debug_Pin0_ON						(GPIOE->BSRRL = GPIO_Pin_10)
#define Debug_Pin0_OFF						(GPIOE->BSRRH = GPIO_Pin_10)

/*
** ������˸��--�޸�:20210323--������˸��
*/
#define Upgrade_FlickerLedON				(GPIOF->BSRRL = GPIO_Pin_10)
#define Upgrade_FlickerLedOFF				(GPIOF->BSRRH = GPIO_Pin_10)

/*
** �̼����Ʊ�� 
*/
enum{
	HardwareCtr_UpgradeLed = 0x00,/*������˸��*/
};

void GPIO_Cfg_Init(void);
/*
** �̼�����
*/
void GPIO_HardwareCtr(uint8 label,bool flag);
#endif

