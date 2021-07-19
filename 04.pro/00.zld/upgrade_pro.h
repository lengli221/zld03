#ifndef __UPGRADE_PRO_H_
#define __UPGRADE_PRO_H_

#define FileName_MaxLen						128//�����ļ�����󳤶�
#define FrameDataItem_MaxLen				1024//��������󳤶�

#define Frame_Head								(uint8)0x68

#define UpgradeFrame_Ack						(uint8)0x06				

/*
*1.�ֽڶ��뷽������
*#pragma�Ƿ���������
*2.�ṹ��"�ֶ�"����ʼ��ַ��ռ�ڴ�ռ��С
*/
#pragma pack(1)
typedef struct{
	uint8 flag;
	uint8 addr;
	uint16 frameId;
	uint16 dataFrameLen;
	uint16 crc16;
}UpgradeProFrame;

typedef struct{
	uint8 flag;
	uint32 word;
	uint16 fileCrc16;
	uint8 fileName[FileName_MaxLen];
}FileNameItem;

typedef struct{
	uint8 ack;
	uint8 flag;
}UpgradeReply;

enum{
	ChkFrameStartFlag = 0x02,//����"�������ݴ���"���̿���--0x03
	ChkFrameIngFlag = 0x01,
	ChkFrameStopFlag = 0x03,

	FileNameFormatIilegal = 0xF5,//�ļ�����ʽ���Ϸ�(������bin�ļ�)
	WriteAppErr = 0xF6,//дAPP���쳣
	EraseAppErr = 0xF7,//����APP���쳣
	EraseBackupErr = 0xF8,//�����������쳣
	FrameIdSpan = 0xF9,//����֡��Խ
	FrameIdRepeat = 0xFA,//����֡�ظ�
	WriteFlashErr = 0xFB,//д�������쳣
	FileItemChkErr = 0xFC,//�ļ�����У�����
	FrameTimout = 0xFE,//֡��ʱ,ȡ������ģʽ
};

typedef struct{
	uint8 step;
	void (*handle)(void*,uint16);
}UpgradePro;
#pragma pack()

typedef struct{
	uint32 len;
	uint16 crc16;
}BinFilePara;

typedef struct{
	uint32 cnt;
	bool Flag;
}SysTim;

typedef struct{
	uint8 step;
	uint16 frameId;
	uint16 frameLastId;
	BinFilePara binFilePara;
	uint8 binFileOk;
	SysTim sysTim;
}ProcessCtr;

typedef struct{
	uint8 addr;
	uint32 interFlashAddr;
}Firmware;

typedef struct{
	ProcessCtr processCtr; 
	Firmware firmware;
}UpgradeSysPara;


void UpgradePro_Frame(void);
SysTim* Upgrade_GetSysTim(void);

#endif

