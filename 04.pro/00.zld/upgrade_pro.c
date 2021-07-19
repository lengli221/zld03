#include "includes.h"

UpgradeSysPara upgradeSysPara;
UpgradeReply upgradeReply;

/*--------------------------------------Ӳ���ӿڱ��----------------------------------*/
uint8 interface_Label = 0x02;/*Ĭ��485���*/
/*------------------------------------------------------------------------------------*/

static void UpgradePro_ParaInit(void){
	upgradeSysPara.processCtr.step = ChkFrameStartFlag;
	upgradeSysPara.processCtr.frameLastId = 0;
	upgradeSysPara.processCtr.binFileOk = false;
	upgradeSysPara.processCtr.sysTim.cnt = 0; 
	upgradeSysPara.processCtr.sysTim.Flag=false;
	memset((uint8*)&upgradeSysPara.processCtr.binFilePara.len,0x00,sizeof(BinFilePara));
	upgradeSysPara.firmware.interFlashAddr = ApplicationBackup_ADDRESS;
	upgradeReply.ack = UpgradeFrame_Ack;
} 

/*
*˵��:��ʱ���������ļ�����
*/
SysTim* Upgrade_GetSysTim(void){
	return(&upgradeSysPara.processCtr.sysTim);
}

/*
*У������֡������Ĵ�С,��ֹ�ڴ����ݸ��Ƶ���Ӳ�ж�
*/
static bool ChkFrameDataItemLenIsOk(uint16 len){
	uint16 baseLen = 0;
	bool ret = false;

	switch(upgradeSysPara.processCtr.step){
		case ChkFrameStartFlag:
			baseLen = sizeof(FileNameItem);			
			break;
		case ChkFrameIngFlag:
			baseLen = FrameDataItem_MaxLen;
			break;
	}

	if(len<=baseLen){
		ret = true;
	}

	return ret;
}

/*У���ַ�Ϸ���*/
static bool ChkFrameAddrIsOk(uint8 addr){
	bool ret=false;

	if(addr == upgradeSysPara.firmware.addr){
		ret = true;
	}

	return ret;
}

/*У��֡ID�Ϸ���*/
static bool ChkFrameIdIsOk(uint16 frameId){
	bool ret=false;

	switch(upgradeSysPara.processCtr.step){
		case ChkFrameStartFlag:
			if(frameId == 0){
				ret = true;
			}
			break;
		case ChkFrameIngFlag:
			if(((uint8)(frameId - upgradeSysPara.processCtr.frameLastId )) == 1){
				ret = true;
			}
			break;
	}

	return ret;
}

static void Upgrade_ReplyFrame(void){
	uint8 tx[sizeof(UpgradeProFrame)+sizeof(UpgradeReply)] = {0};
	uint32 len = 0;
	uint16 crc16=0;
	UpgradeProFrame upgradeProFrame; 
	
	upgradeProFrame.flag = Frame_Head;
	upgradeProFrame.addr = upgradeSysPara.firmware.addr;
	upgradeProFrame.frameId = upgradeSysPara.processCtr.frameId;
	upgradeProFrame.dataFrameLen = sizeof(UpgradeReply);
	memcpy((uint8*)&tx[len],(uint8*)&upgradeProFrame.flag,sizeof(UpgradeProFrame));
	len += (sizeof(UpgradeProFrame)-sizeof(uint16));

	memcpy((uint8*)&tx[len],(uint8*)&upgradeReply.ack,sizeof(UpgradeReply));
	len += sizeof(UpgradeReply);

	crc16 = CRC16((uint8*)&tx[0],(uint32)len);

	memcpy((uint8*)&tx[len],(uint8*)&crc16,sizeof(uint16));
	len += sizeof(uint16);
		
	BSP_SmSend(BSP_ComType_USART, interface_Label, (uint8 *)&tx[0], (uint16 *)&len);
}

/*У������֡����֡���Ϸ��Իظ�*/
static void ChkFrameDataIsIllegal(uint16 frameId){
		upgradeReply.flag = ((uint8)(frameId - upgradeSysPara.processCtr.frameLastId)) == 0?FrameIdRepeat:FrameIdSpan;
		Upgrade_ReplyFrame();
}

/*����֡�Ƿ�ʱ*/
static void ChkFrameIsTimOut(void){
	if(upgradeSysPara.processCtr.sysTim.Flag == true){
		upgradeReply.flag = FrameTimout;
		Upgrade_ReplyFrame();
		UpgradeKey_Clear();
		NVIC_SystemReset();
	}
}

static void Upgrade_StartFlag(void*rx,uint16 dataItemLen){
	FileNameItem fileNameItem;
	uint8 flag = 0;
	uint16 binFileCrc16 = 0;
	uint32 flashAddress = 0;

	if(dataItemLen<=sizeof(FileNameItem)){
		memcpy((uint8*)&fileNameItem.flag,(uint8*)rx,dataItemLen);
	}

	switch(fileNameItem.flag){
		case ChkFrameStartFlag:
			flag = ChkFrameStartFlag;
			if(FLASHInter_Erase(ApplicationBackup_ADDRESS,ApplicationBackup_EndADDRESS)){
				/*����֡Frame Id�쳣��,�������¿�ʼ����֡,���½�������*/
				UpgradePro_ParaInit();
				/*����bin�ļ�����֡*/
				upgradeSysPara.processCtr.step = ChkFrameIngFlag;
				upgradeSysPara.processCtr.binFilePara.crc16 = fileNameItem.fileCrc16;
			}else{
				/*�����������쳣,������--����:���½���Flash��*/
				FlashInterCtr_Init();
				flag = EraseBackupErr;
			}
			break;
		case ChkFrameStopFlag:	
			flag = ChkFrameStopFlag;
			/*�ж��������ò���*/
			binFileCrc16 = CRC16((uint8 *)ApplicationBackup_ADDRESS, upgradeSysPara.processCtr.binFilePara.len);
			if(binFileCrc16 == upgradeSysPara.processCtr.binFilePara.crc16 && binFileCrc16 == fileNameItem.fileCrc16
				&& upgradeSysPara.processCtr.binFilePara.len == fileNameItem.word){
				/*�ļ�����ʽ�쳣(������bin�ļ�)*/
				if(FileNameVerOpt((uint8 *)&fileNameItem.fileName[0], dataItemLen-sizeof(uint8)-sizeof(uint32)-sizeof(uint16))){
					if(FLASHInter_Erase(APPLICATION_ADDRESS,APPLICATION_EndADDRESS)){
						flashAddress =APPLICATION_ADDRESS;
						if(FLASHInter_Write((__IO uint32 *)&flashAddress, (uint32 *)ApplicationBackup_ADDRESS,fileNameItem.word/4) == 0){
							/*1.�ļ���ת����汾�� 2.�洢�汾��,�ļ�������,�ļ�������*/
							
							/*�ļ���ȡ���,У��ɹ�,׼������*/
							upgradeSysPara.processCtr.binFileOk = true;
							UpgradeKey_Clear();
							/*Flash����*/
							FLASH_Lock();
						}else{
							/*дAPP���쳣*/
							flag = WriteAppErr;
						}
					}else{
						/*����APP���쳣*/
						flag = EraseAppErr;
					}
				}else{
					flag = FileNameFormatIilegal;
				}
			}else{
				/*�ļ�����У�����*/
				flag = FileItemChkErr;
			}
			break;
	}

	upgradeReply.flag = flag;
}

static void Upgrade_IngFlag(void*rx,uint16 dataItemLen){
	uint32 source = 0;
	uint8 flag = WriteFlashErr;/*д�������쳣*/

	source = (uint32)rx;

	if(FLASHInter_Write((__IO uint32 *)&upgradeSysPara.firmware.interFlashAddr, (uint32 *)source, (uint16)(dataItemLen/4)) == 0){
		upgradeSysPara.processCtr.binFilePara.len += dataItemLen;
		flag = ChkFrameIngFlag;
		/*��¼���֡Id����*/
		upgradeSysPara.processCtr.frameLastId = upgradeSysPara.processCtr.frameId;
	}

	upgradeReply.flag = flag;
}

static const UpgradePro upgradeProTabel[] = {
	{ChkFrameStartFlag,Upgrade_StartFlag},
	{ChkFrameIngFlag,Upgrade_IngFlag},
};
uint8 upgradeProTabelNum = sizeof(upgradeProTabel)/sizeof(UpgradePro);

static void UpgradeProTabelExecuteFun(uint8*rx,uint16 dataItemLen){
	uint8 i=0;
	for(i=0;i<upgradeProTabelNum;i++){
		if(upgradeSysPara.processCtr.step == upgradeProTabel[i].step){
			upgradeProTabel[i].handle(rx,dataItemLen);
			Upgrade_ReplyFrame();
			break;
		}/*�Ƿ����ӳ�������쳣ҵ���߼�*/
	}	
}

static void ParseUpgradeProFrame(uint8*rxbuf,uint16*rxlen){
	uint8* ptrbuf = rxbuf;
	uint16 updataLen = *rxlen;
	uint16 saveDataLen = *rxlen;
	uint16 dataItemLen = 0;
	uint16 crc16 = 0;
	uint16 frameId = 0;

	/*
	if(updataLen == 0){
		return;
	}
	*/
	
	for(;updataLen>=(uint16)(sizeof(UpgradeProFrame));){
		/*˵��:���������Ż���*/
		memcpy((uint8*)&dataItemLen,(uint8*)&ptrbuf[4],sizeof(uint16));
		if(ptrbuf[0] == Frame_Head && ChkFrameDataItemLenIsOk(dataItemLen)){
			if(updataLen >= dataItemLen+sizeof(UpgradeProFrame)){
				memcpy((uint8*)&crc16,(uint8*)&ptrbuf[sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen],sizeof(uint16));
				if(crc16 == CRC16((uint8 *)&ptrbuf[0], sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen)){
					/*��������֡���ʱ��*/
					upgradeSysPara.processCtr.sysTim.cnt = 0;
					
					memcpy((uint8*)&frameId,(uint8*)&ptrbuf[2],sizeof(uint8)+sizeof(uint8));
					/*����"�������ݽ���֡"*/
					if(frameId == 0){
						upgradeSysPara.processCtr.step = ChkFrameStartFlag;
					}
					/*֡ID*/
					upgradeSysPara.processCtr.frameId = frameId;
					if(ChkFrameAddrIsOk(ptrbuf[sizeof(uint8)]) && ChkFrameIdIsOk(frameId)){
						/*ִ֡�к���*/
						UpgradeProTabelExecuteFun((uint8*)&ptrbuf[6],dataItemLen);
					}else{
						/*��ַ�Ϸ�,֡�쳣*/
						if(ChkFrameAddrIsOk(ptrbuf[sizeof(uint8)])){
							ChkFrameDataIsIllegal(frameId);
						}
					}
					ptrbuf += (dataItemLen+sizeof(UpgradeProFrame));
					updataLen -= (dataItemLen+sizeof(UpgradeProFrame));
					dataItemLen = 0;
				}else{
					ptrbuf++;
					updataLen--;
					dataItemLen = 0;
				}
			}else{
				break;
			}
		}else{
			ptrbuf++;
			updataLen--;
			dataItemLen = 0;
		}
	}
	memcpy((uint8*)&rxbuf[0],(uint8*)&ptrbuf[0],updataLen);
	*rxlen = updataLen;
	memset((uint8*)rxbuf+updataLen,0x00,saveDataLen-updataLen);
}

/*
** ���������е���˸ҵ���߼�--�޸�:20210323--������˸��
*/
void Upgrade_CtrLedLogic(void){
	static uint32 cnt = 0;
	static bool ledFlickFlag = false;

	if(TickOut((uint32 *)&cnt, 2) == true){
		TickOut((uint32 *)&cnt, 0);
		ledFlickFlag = ledFlickFlag == false?true:false;
		GPIO_HardwareCtr(HardwareCtr_UpgradeLed, ledFlickFlag);
	}
}

void UpgradePro_Frame(void){
	/*
	*����֡,ճ֡,��֡,�쳣֡�����bufԣ��
	*/
	uint8 ucRxBuf[sizeof(uint16)*(FrameDataItem_MaxLen+sizeof(UpgradeProFrame))];
	uint8 *rxptr;
	uint16 ucLen = 0;
	uint16 ucRecvLen = 0;	
	/*-Ӳ���ӿ�-*/
	interface_Label  = get_Interface().label;

	UpgradePro_ParaInit();

	for(;;){
		ucRecvLen = 0;
		BSP_SmRecv(BSP_ComType_USART,interface_Label,ucRxBuf + ucLen,&ucRecvLen);
		ucLen += ucRecvLen;
		if(ucLen >( sizeof(ucRxBuf))){
			ucLen = 0;
		}
		
		if(ucRecvLen == 0 && ucLen != 0){
			rxptr = ucRxBuf;
			ucRecvLen = ucLen;
			ParseUpgradeProFrame((uint8 *)rxptr, (uint16 *)&ucRecvLen);
			ucLen = ucRecvLen;
		}

		/*���ӿ��Ź�,��ǿ�ȶ�����*/
		watchdogUpdate();

		if(upgradeSysPara.processCtr.binFileOk == true){
			/*��λ�����߼�*/
			NVIC_SystemReset();
			break;
		}

		/*֡��ʱ����*/
		ChkFrameIsTimOut();

		/*
		** ���������е���˸ҵ���߼�
		*/
		Upgrade_CtrLedLogic();
	}
}



