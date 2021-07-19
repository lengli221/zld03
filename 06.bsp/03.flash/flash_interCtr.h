#ifndef __FLASH_INTERCTR_H_
#define __FLASH_INTERCTR_H_

/*Base address of the Flash sectors*/
#define ADDR_FLASH_SECTOR_0     ((uint32)0x08000000) /* Base @ of Sector 0, 16 Kbyte */
#define ADDR_FLASH_SECTOR_1     ((uint32)0x08004000) /* Base @ of Sector 1, 16 Kbyte */
#define ADDR_FLASH_SECTOR_2     ((uint32)0x08008000) /* Base @ of Sector 2, 16 Kbyte */
#define ADDR_FLASH_SECTOR_3     ((uint32)0x0800C000) /* Base @ of Sector 3, 16 Kbyte */
#define ADDR_FLASH_SECTOR_4     ((uint32)0x08010000) /* Base @ of Sector 4, 64 Kbyte */
#define ADDR_FLASH_SECTOR_5     ((uint32)0x08020000) /* Base @ of Sector 5, 128 Kbyte */
#define ADDR_FLASH_SECTOR_6     ((uint32)0x08040000) /* Base @ of Sector 6, 128 Kbyte */
#define ADDR_FLASH_SECTOR_7     ((uint32)0x08060000) /* Base @ of Sector 7, 128 Kbyte */
#define ADDR_FLASH_SECTOR_8     ((uint32)0x08080000) /* Base @ of Sector 8, 128 Kbyte */
#define ADDR_FLASH_SECTOR_9     ((uint32)0x080A0000) /* Base @ of Sector 9, 128 Kbyte */
#define ADDR_FLASH_SECTOR_10    ((uint32)0x080C0000) /* Base @ of Sector 10, 128 Kbyte */
#define ADDR_FLASH_SECTOR_11    ((uint32)0x080E0000) /* Base @ of Sector 11, 128 Kbyte */

/*End of the Flash address*/
#define USER_FLASH_END_ADDRESS        	0x080FFFFF

/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x0803FFFF is reserved for the IAP code */
#define APPLICATION_ADDRESS   					(uint32)0x08040000
#define APPLICATION_EndADDRESS				(uint32)0x0809FFFF
/*±¸·ÝÇøÓò:3*128K=384K*/
#define ApplicationBackup_ADDRESS				(uint32)0x080A0000
#define ApplicationBackup_EndADDRESS			(uint32)0x080FFFFF

void FlashInterCtr_Init(void);
bool FLASHInter_GetWriteProtectionStatus(uint32 startAddr);
bool FLASHInter_Erase(uint32 startAddr,uint32 endAddr);
uint32 FLASHInter_Write(__IO uint32* FlashAddress, uint32* Data ,uint16 DataLength);

#endif

