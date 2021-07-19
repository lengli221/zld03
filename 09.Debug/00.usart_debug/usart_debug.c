#include "includes.h"
#if USART_485_Optimization_Debug == 1
void Usart_Debug(void){
	uint8 bufl[15] = {0};
	uint8 klkl;

	for(klkl=0;klkl<15;klkl++){
			bufl[klkl]=klkl;
	}
	//Usart2_DmaWrite((uint8*)&bufl[0],sizeof(bufl));
	Usart1_DmaWrite((uint8*)&bufl[0],sizeof(bufl));
	Usart1_DmaWrite((uint8*)&bufl[0],sizeof(bufl));
}
#endif

