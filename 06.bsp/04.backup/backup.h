#ifndef __BACKUP_
#define __BACKUP_

#define Backup_ChargerTimeAddr				(uint16)0x00
#define Backup_ChargerTimeAddr_Len		(uint16)0xC8

void PWR_BackupInit(void);
void BKP_ReadBackupRegDat(uint16_t Addr, uint8_t *Data,uint16_t Len);
void BKP_WriteBackupDat(uint16_t Addr, uint8_t *Data,uint16_t Len);

#endif

