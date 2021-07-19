#include "includes.h"

/*
** ����汾�� ����ASCII��
*/
#define SoftVerNum					3

/*
** �����Ƿ���.bin�ļ��Լ���ȡ�±����
** @return:���ذ汾��Ϣ�±�
*/
int16 fileNameVerSuffix(uint8* data,uint16 len){
	int16 indexSuffix = -1;
	uint16 i = 0;

	for(i = 0;i < len - 3;i++){
		if(data[i] == '.' && data[i+1] == 'b'&& data[i+2] == 'i'&& data[i+3] == 'n'){
			/*
			** ���ذ汾��Ϣ�±�
			*/
			if(i >= SoftVerNum){/*Ŀǰ��������汾��:3 ASCII*/
				indexSuffix = (i - SoftVerNum);
			}
			break;
		}
	}
	return indexSuffix;
}

bool FileNameVerOpt(uint8*data,uint16 len){
	bool ret=false;
	int16 indexSuffix = fileNameVerSuffix((uint8 *)&data[0],len);
	uint8 tmpp[sizeof(uint32)+0x03] = {0};
	uint16 crc16=0;
	uint8 i=0;
	uint32 ver = 0;
	uint8 fileNameLen[sizeof(uint16)+0x03] = {0};
	uint8 fileName[FileName_MaxLen+0x03] = {0};/*fileName+head+crc16*/
	uint8 temp[SoftVerNum] = {0};

	if(indexSuffix != -1){/*.bin�ļ�,�Ϸ�*/
		memcpy((uint8*)&temp[0],(uint8*)&data[indexSuffix],SoftVerNum);
		for(i = 0;i < SoftVerNum;i++){/*���ļ�����ȡ����汾��*/
			ver *= 10;
			ver += temp[i] - '0';
		}

		/*1.д����汾��*/
		tmpp[0] = UpgradeKey_Head;
		memcpy((uint8*)&tmpp[1],(uint8*)&ver,sizeof(uint32));
		crc16 = CRC16((uint8 *)&tmpp[0], sizeof(uint32)+sizeof(uint8));
		memcpy((uint8*)&tmpp[sizeof(uint32)+sizeof(uint8)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeVer_Addr, (uint8_t *)&tmpp[0], UpgradeVer_Len_Addr);
		Eeprom_Write(UpgradeVer_Addr, (uint8_t *)&tmpp[0], UpgradeVer_Len_Addr);
		
		/*2.д�ļ�������,�ļ���*/
		fileNameLen[0] = UpgradeKey_Head;
		memcpy((uint8*)&fileNameLen[1],(uint8*)&len,sizeof(uint16));
		crc16 = CRC16((uint8 *)&fileNameLen[0], sizeof(uint8)+sizeof(uint16));
		memcpy((uint8*)&fileNameLen[sizeof(uint8)+sizeof(uint16)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeFileNameLen_Addr, (uint8_t *)&fileNameLen[0], UpgradeFileNameLen_Len_Addr);
		Eeprom_Write(UpgradeFileNameLen_Addr, (uint8_t *)&fileNameLen[0], UpgradeFileNameLen_Len_Addr);

		fileName[0] = UpgradeKey_Head;
		memcpy((uint8*)&fileName[1],(uint8*)&data[0],len);
		crc16 = CRC16((uint8 *)&fileName[0], len+sizeof(uint8));
		memcpy((uint8*)&fileName[len+sizeof(uint8)],(uint8*)&crc16,sizeof(uint16));
		BKP_WriteBackupDat(UpgradeFileName_Addr, (uint8_t *)&fileName[0], len+sizeof(uint8)+sizeof(uint16));
		Eeprom_Write(UpgradeFileName_Addr, (uint8_t *)&fileName[0], len+sizeof(uint8)+sizeof(uint16));

		ret=true;
	}

	return ret;
}

