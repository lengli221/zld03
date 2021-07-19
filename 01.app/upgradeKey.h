#ifndef __UPGRADEKEY_H_
#define __UPGRADEKEY_H_

#define UpgradeKey_Head							0xA5

#define ByteReserveOffset							(0x04)

#define UpgradeKey_Addr							(uint16)0x0000
#define UpgradeKey_Len_Addr						sizeof(UpgradeKey)

#define UpgradeVer_Addr							ByteReserveOffset + UpgradeKey_Addr + UpgradeKey_Len_Addr
#define UpgradeVer_Len_Addr						sizeof(uint32) + 0x03/*head+crc16*/

#define UpgradeFileNameLen_Addr				ByteReserveOffset+UpgradeVer_Addr+UpgradeVer_Len_Addr
#define UpgradeFileNameLen_Len_Addr			sizeof(uint16)+0x03

#define UpgradeFileName_Addr						ByteReserveOffset + UpgradeFileNameLen_Addr + UpgradeFileNameLen_Len_Addr
#define UpgradeFileName_Len_Addr				FileName_MaxLen + 0x03

#pragma pack(1)
typedef struct{
	uint8 head;
	uint8 key[6];
	uint16 crc16;
}UpgradeKey;
#pragma pack()

bool UpgradeKey_IsOk(void);
void UpgradeKey_Clear(void);

#endif

