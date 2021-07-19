#include "includes.h"

/*
** Interface Sw typedef
*/
SaveIF_Label  saveIF_Label;

/*
** get Interface
*/
SaveIF_Label get_Interface(void){
	return saveIF_Label;
}

/*
** reset Interface Init 
*/
void reset_InterfaceInit(void){
	uint8 buf[Interface_Sw_BaseLen] = {0};
	uint8 buf_bkp[Interface_Sw_BaseLen] = {0};	
	uint8 len = 0;
	uint16 crc = 0;
	uint16 getCrc = 0;
	uint16 crc_bkp = 0;
	uint16 getCrc_bkp = 0;
	SaveIF_Label saveIF_LabelTemp = {0};
	
	Eeprom_Read(Interface_Sw_BaseAddr, (uint8 *)&buf[len], Interface_Sw_BaseLen);
	BKP_ReadBackupRegDat(Interface_Sw_BaseAddr, (uint8*)&buf_bkp[len], Interface_Sw_BaseLen);	

	/*计算CRC16校验*/
	crc = CRC16((uint8 *)&buf[len], sizeof(SaveIF_Label));
	crc_bkp = CRC16((uint8 *)&buf_bkp[len], sizeof(SaveIF_Label));
	len += sizeof(SaveIF_Label);	

	/*get CRC16校验*/
	memcpy((uint8*)&getCrc,(uint8*)&buf[len],sizeof(uint16));
	memcpy((uint8*)&getCrc_bkp,(uint8*)&buf_bkp[len],sizeof(uint16));	

	if(crc == getCrc){
		memcpy((uint8*)&saveIF_LabelTemp.head,(uint8*)&buf[0],sizeof(SaveIF_Label));
		if(saveIF_LabelTemp.head == 0x23 && saveIF_LabelTemp.end == 0x45){
			memcpy((uint8*)&saveIF_Label.head,(uint8*)&saveIF_LabelTemp.head,sizeof(SaveIF_Label));
			BKP_WriteBackupDat(Interface_Sw_BaseAddr, (uint8*)&buf[0], Interface_Sw_BaseLen);
		}
	}else if(crc_bkp == getCrc_bkp){
		memcpy((uint8*)&saveIF_LabelTemp.head,(uint8*)&buf_bkp[0],sizeof(SaveIF_Label));
		if(saveIF_LabelTemp.head == 0x23 && saveIF_LabelTemp.end == 0x45){
			memcpy((uint8*)&saveIF_Label.head,(uint8*)&saveIF_LabelTemp.head,sizeof(SaveIF_Label));
			Eeprom_Write(Interface_Sw_BaseAddr, (uint8*)&buf_bkp[0], Interface_Sw_BaseLen);
		}
	}else{
		/*6A默认接口485*/
		saveIF_Label.label = 0x02;
		saveIF_Label.runLedBaseTime = 4000;
	}
}

