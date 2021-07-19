#include "includes.h"

bool UpgradeKey_IsOk(void){
	bool ret = false;
	uint8 key[6] = {0x7A,0x6C,0x64,0x12,0x04,0x01};//"zld180401"
	UpgradeKey upgradeKey = {0}; 
	uint16 crc16 =0;

	PWR_BackupInit();

	BKP_ReadBackupRegDat(UpgradeKey_Addr, (uint8_t *)&upgradeKey.head, UpgradeKey_Len_Addr);

	if(upgradeKey.head == UpgradeKey_Head){
		crc16 = CRC16((uint8 *)&upgradeKey.head, sizeof(UpgradeKey)-sizeof(uint16));
		if(memcmp((uint8*)&key,(uint8*)&upgradeKey.key[0],6)==0 && upgradeKey.crc16 == crc16){
			ret = true;
		}
	}

	return ret;
}

void UpgradeKey_Clear(void){
	UpgradeKey upgradeKey = {0}; 

	upgradeKey.head = UpgradeKey_Head;
	memset((uint8*)&upgradeKey.key[0],0x00,6);
	upgradeKey.crc16 = CRC16((uint8 *)&upgradeKey.head, sizeof(UpgradeKey)-sizeof(uint16));
	BKP_WriteBackupDat(UpgradeKey_Addr, (uint8_t *)&upgradeKey.head, sizeof(UpgradeKey));
}

