#ifndef __BSP_H
#define __BSP_H

#define BSP_ComType_USART             				0x0001

#define BSP_ComUpdrageNo_232					0x01
#define BSP_ComUpdrageNo						0x02

#define BSP_COM_NO									0
#define BSP_COM_EVEN								1
#define BSP_COM_ODD								2

#define BSP_COM_STOP1							1
#define BSP_COM_STOP2							2

void UsartCom_Init(void);
void Bsp_Init(void);
void BSP_SmRecv(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen);
void BSP_SmSend(uint8 ucComType,uint8 ucSmNo,void *msgbuf, uint16 *uclen);

#endif

