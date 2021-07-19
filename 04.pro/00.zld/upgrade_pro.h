#ifndef __UPGRADE_PRO_H_
#define __UPGRADE_PRO_H_

#define FileName_MaxLen						128//限制文件名最大长度
#define FrameDataItem_MaxLen				1024//数据项最大长度

#define Frame_Head								(uint8)0x68

#define UpgradeFrame_Ack						(uint8)0x06				

/*
*1.字节对齐方案处理
*#pragma是否限制条件
*2.结构体"字段"的起始地址所占内存空间大小
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
	ChkFrameStartFlag = 0x02,//兼容"结束数据传输"流程控制--0x03
	ChkFrameIngFlag = 0x01,
	ChkFrameStopFlag = 0x03,

	FileNameFormatIilegal = 0xF5,//文件名格式不合法(包含非bin文件)
	WriteAppErr = 0xF6,//写APP区异常
	EraseAppErr = 0xF7,//擦除APP区异常
	EraseBackupErr = 0xF8,//擦除备份区异常
	FrameIdSpan = 0xF9,//数据帧跨越
	FrameIdRepeat = 0xFA,//数据帧重复
	WriteFlashErr = 0xFB,//写备份区异常
	FileItemChkErr = 0xFC,//文件内容校验错误
	FrameTimout = 0xFE,//帧超时,取消升级模式
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

