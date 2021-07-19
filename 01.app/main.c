#include "includes.h"

int main(void){
	if(UpgradeKey_IsOk() == true){
		
		/*1.根据状态条件,解锁内部Flash*/
		FlashInterCtr_Init();
		/*2.初始化升级串口*/
		Bsp_Init();
		/*3.接收bin文件*/
		UpgradePro_Frame();
	}else{
		/*4.跳转至APP区域*/
		Jump_APP_Execute();	
	}

	#if USART_485_Optimization_Debug == 1
		Usart_Debug();
	#endif
		
	while(1){
		watchdogUpdate();
	}
}

