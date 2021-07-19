#ifndef __USART1_232_H_
#define __USART1_232_H_
#define UART1_BUFLEN_R								sizeof(uint16)*(1024+sizeof(UpgradeProFrame))
#define UART1_BUFLEN_S								64
#define DMA_BUFLEN_1         								64  

/*
** 串口寄存器数据地址
*/
#define USART1_DR_ADDRESS                 				((uint32)USART1 + 0x04)

typedef struct {
  bool*  Busy;    
  Queue*  TxQueue;   
  Queue*  RxQueue;   
}UartInfo1;

/*
** 串口初始化
*/
void Usart1_Init(void);
/*
** DMA 配置
*/
void Usart1_DmaConfig(void);
/*
** 初始化缓冲区
*/
void Usart1_BufferInit(void);
/*
** DMA Write
*/
bool Usart1_DmaWrite(uint8 * txBuf,uint32 txNum);
/*
** 从缓冲区读数据
*/
int Usart1_Rcv(uint8 *rxBuf, int rxNum);

#endif

