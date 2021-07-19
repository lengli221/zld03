#include "includes.h"

UpgradeSysPara upgradeSysPara;
UpgradeReply upgradeReply;

/*--------------------------------------硬件接口标号----------------------------------*/
uint8 interface_Label = 0x02;/*默认485标号*/
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
*说明:超时参数其他文件调用
*/
SysTim* Upgrade_GetSysTim(void){
	return(&upgradeSysPara.processCtr.sysTim);
}

/*
*校验数据帧数据项的大小,防止内存数据复制导致硬中断
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

/*校验地址合法性*/
static bool ChkFrameAddrIsOk(uint8 addr){
	bool ret=false;

	if(addr == upgradeSysPara.firmware.addr){
		ret = true;
	}

	return ret;
}

/*校验帧ID合法性*/
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

/*校验数据帧连续帧不合法性回复*/
static void ChkFrameDataIsIllegal(uint16 frameId){
		upgradeReply.flag = ((uint8)(frameId - upgradeSysPara.processCtr.frameLastId)) == 0?FrameIdRepeat:FrameIdSpan;
		Upgrade_ReplyFrame();
}

/*检验帧是否超时*/
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
				/*数据帧Frame Id异常后,接收重新开始数据帧,重新接收数据*/
				UpgradePro_ParaInit();
				/*接收bin文件数据帧*/
				upgradeSysPara.processCtr.step = ChkFrameIngFlag;
				upgradeSysPara.processCtr.binFilePara.crc16 = fileNameItem.fileCrc16;
			}else{
				/*擦除备份区异常,请重试--操作:重新解锁Flash区*/
				FlashInterCtr_Init();
				flag = EraseBackupErr;
			}
			break;
		case ChkFrameStopFlag:	
			flag = ChkFrameStopFlag;
			/*判断条件设置参数*/
			binFileCrc16 = CRC16((uint8 *)ApplicationBackup_ADDRESS, upgradeSysPara.processCtr.binFilePara.len);
			if(binFileCrc16 == upgradeSysPara.processCtr.binFilePara.crc16 && binFileCrc16 == fileNameItem.fileCrc16
				&& upgradeSysPara.processCtr.binFilePara.len == fileNameItem.word){
				/*文件名格式异常(包含非bin文件)*/
				if(FileNameVerOpt((uint8 *)&fileNameItem.fileName[0], dataItemLen-sizeof(uint8)-sizeof(uint32)-sizeof(uint16))){
					if(FLASHInter_Erase(APPLICATION_ADDRESS,APPLICATION_EndADDRESS)){
						flashAddress =APPLICATION_ADDRESS;
						if(FLASHInter_Write((__IO uint32 *)&flashAddress, (uint32 *)ApplicationBackup_ADDRESS,fileNameItem.word/4) == 0){
							/*1.文件名转软件版本号 2.存储版本号,文件名长度,文件名内容*/
							
							/*文件获取完后,校验成功,准备升级*/
							upgradeSysPara.processCtr.binFileOk = true;
							UpgradeKey_Clear();
							/*Flash上锁*/
							FLASH_Lock();
						}else{
							/*写APP区异常*/
							flag = WriteAppErr;
						}
					}else{
						/*擦除APP区异常*/
						flag = EraseAppErr;
					}
				}else{
					flag = FileNameFormatIilegal;
				}
			}else{
				/*文件内容校验错误*/
				flag = FileItemChkErr;
			}
			break;
	}

	upgradeReply.flag = flag;
}

static void Upgrade_IngFlag(void*rx,uint16 dataItemLen){
	uint32 source = 0;
	uint8 flag = WriteFlashErr;/*写备份区异常*/

	source = (uint32)rx;

	if(FLASHInter_Write((__IO uint32 *)&upgradeSysPara.firmware.interFlashAddr, (uint32 *)source, (uint16)(dataItemLen/4)) == 0){
		upgradeSysPara.processCtr.binFilePara.len += dataItemLen;
		flag = ChkFrameIngFlag;
		/*记录完成帧Id参数*/
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
		}/*是否增加充电流程异常业务逻辑*/
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
		/*说明:常数参数优化项*/
		memcpy((uint8*)&dataItemLen,(uint8*)&ptrbuf[4],sizeof(uint16));
		if(ptrbuf[0] == Frame_Head && ChkFrameDataItemLenIsOk(dataItemLen)){
			if(updataLen >= dataItemLen+sizeof(UpgradeProFrame)){
				memcpy((uint8*)&crc16,(uint8*)&ptrbuf[sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen],sizeof(uint16));
				if(crc16 == CRC16((uint8 *)&ptrbuf[0], sizeof(UpgradeProFrame)-sizeof(uint16)+dataItemLen)){
					/*接收完整帧清计时器*/
					upgradeSysPara.processCtr.sysTim.cnt = 0;
					
					memcpy((uint8*)&frameId,(uint8*)&ptrbuf[2],sizeof(uint8)+sizeof(uint8));
					/*兼容"结束数据接收帧"*/
					if(frameId == 0){
						upgradeSysPara.processCtr.step = ChkFrameStartFlag;
					}
					/*帧ID*/
					upgradeSysPara.processCtr.frameId = frameId;
					if(ChkFrameAddrIsOk(ptrbuf[sizeof(uint8)]) && ChkFrameIdIsOk(frameId)){
						/*帧执行函数*/
						UpgradeProTabelExecuteFun((uint8*)&ptrbuf[6],dataItemLen);
					}else{
						/*地址合法,帧异常*/
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
** 升级过程中灯闪烁业务逻辑--修改:20210323--运行闪烁灯
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
	*数据帧,粘帧,断帧,异常帧处理的buf裕量
	*/
	uint8 ucRxBuf[sizeof(uint16)*(FrameDataItem_MaxLen+sizeof(UpgradeProFrame))];
	uint8 *rxptr;
	uint16 ucLen = 0;
	uint16 ucRecvLen = 0;	
	/*-硬件接口-*/
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

		/*增加看门狗,增强稳定性能*/
		watchdogUpdate();

		if(upgradeSysPara.processCtr.binFileOk == true){
			/*复位主控逻辑*/
			NVIC_SystemReset();
			break;
		}

		/*帧超时处理*/
		ChkFrameIsTimOut();

		/*
		** 升级过程中灯闪烁业务逻辑
		*/
		Upgrade_CtrLedLogic();
	}
}



