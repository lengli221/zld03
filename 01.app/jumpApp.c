#include "includes.h"

pFunction Jump_To_Application;
uint32 JumpAddress;

void Jump_APP_Execute(void){
	if (((*(__IO uint32*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000){
		JumpAddress = *(__IO uint32*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		__set_MSP(*(__IO uint32*) APPLICATION_ADDRESS);
		Jump_To_Application();	
	}
}


