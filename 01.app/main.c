#include "includes.h"

int main(void){
	if(UpgradeKey_IsOk() == true){
		
		/*1.����״̬����,�����ڲ�Flash*/
		FlashInterCtr_Init();
		/*2.��ʼ����������*/
		Bsp_Init();
		/*3.����bin�ļ�*/
		UpgradePro_Frame();
	}else{
		/*4.��ת��APP����*/
		Jump_APP_Execute();	
	}

	#if USART_485_Optimization_Debug == 1
		Usart_Debug();
	#endif
		
	while(1){
		watchdogUpdate();
	}
}

