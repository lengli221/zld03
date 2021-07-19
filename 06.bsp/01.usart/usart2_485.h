#ifndef __USART2_485_H_
#define __USART2_485_H_

#define UART_BUFLEN_R								sizeof(uint16)*(1024+sizeof(UpgradeProFrame))
#define UART_BUFLEN_S								64
#define DMA_BUFLEN         							64

#define USART2_DR_ADDRESS                 			((uint32_t)USART2 + 0x04)

#define USART2_485_RX       							(GPIOG->BSRRH = GPIO_Pin_14)
#define USART2_485_TX       							(GPIOG->BSRRL = GPIO_Pin_14)

typedef struct {
  bool*  Busy;    
  Queue*  TxQueue;   
  Queue*  RxQueue;   
}UartInfo;

void Usart2_Init(void);
void Usart2_DmaConfig(void);
void Usart2_DmaSend(void);
bool Usart2_DmaWrite(uint8 * txBuf,uint32 txNum);
void Usart2_BufferInit(void);
int Usart2_Rcv(uint8 *rxBuf, int rxNum); 
void USART2_IRQHandler(void);
void Usart2_Stop(void);

#endif

