#ifndef __INTERFACE_H_
#define __INTERFACE_H_

/*
** �л��ӿ�EEPROM��׼�����ӿ�
*/
#define Interface_Sw_BaseAddr						0xA0
#define Interface_Sw_BaseLen							(sizeof(SaveIF_Label) + sizeof(uint16))

#pragma pack(1)
/*
** save Interface Label 
*/
typedef struct{
	uint8 head;
	uint8 label;
	uint16 runLedBaseTime;/*���л�׼ʱ��*/
	uint8 end;
}SaveIF_Label;
#pragma pack()

/*
** get Interface
*/
SaveIF_Label get_Interface(void);

/*
** reset Interface Init 
*/
void reset_InterfaceInit(void);


#endif

