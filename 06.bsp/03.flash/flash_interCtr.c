#include "includes.h"

/*说明:解锁内部Flash,初始化状态*/
void FlashInterCtr_Init(void){
	/*1.解锁*/
	FLASH_Unlock();

	/*2.清标志位*/
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
					FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
}

static uint32 GetSector(uint32 Address)
{
	uint32 sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0)){
		sector = FLASH_Sector_0;  
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1)){
		sector = FLASH_Sector_1;  
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2)){
		sector = FLASH_Sector_2;  
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3)){
		sector = FLASH_Sector_3;  
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4)){
		sector = FLASH_Sector_4;  
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5)){
		sector = FLASH_Sector_5;  
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6)){
		sector = FLASH_Sector_6;  
	}
	else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7)){
		sector = FLASH_Sector_7;  
	}
	else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8)){
		sector = FLASH_Sector_8;  
	}
	else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9)){
		sector = FLASH_Sector_9;  
	}
	else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10)){
		sector = FLASH_Sector_10;  
	}
	else/*(Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_11))*/{
		sector = FLASH_Sector_11;  
	}
	return sector;
}

/*疑问:.bin文件的拆包存储Flash,程序的存储,执行连续地址空间?*/
bool FLASHInter_GetWriteProtectionStatus(uint32 startAddr){
	bool ret = true;
	uint32 UserStartSector = FLASH_Sector_1;

	/*Get the sector where start the user flash area*/
	UserStartSector = GetSector(startAddr);

	/*Check if there are write protected sectors inside the user flash area*/
	if ((FLASH_OB_GetWRP() >> (UserStartSector/8)) == (0xFFF >> (UserStartSector/8))){/*No write protected sectors inside the user flash area*/
		ret = false;
	}
	else{/*Some sectors inside the user flash area are write protected*/
		ret = true;
	}
	return(ret);
}

/*明确STM32内部擦除限制条件*/
bool FLASHInter_Erase(uint32 startAddr,uint32 endAddr){
	bool ret =true;
	uint32 UserStartSector = FLASH_Sector_1, i = 0;
	uint32 UserEndSector = FLASH_Sector_11;

	/*Get the sector where start/end the user flash area*/
	UserStartSector = GetSector(startAddr);
	UserEndSector = GetSector(endAddr);

	for(i = UserStartSector; i <= UserEndSector; i += 8)
	{
		/*增加看门狗,增强稳定性能*/
		watchdogUpdate();
		__disable_irq();
		/*Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word */ 
		if(FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
		{
			/*Error occurred while page erase*/
			ret = false;
			break;
		}
		__enable_irq();
	}

	return ret;
}

uint32 FLASHInter_Write(__IO uint32* FlashAddress, uint32* Data ,uint16 DataLength){
	uint32 i = 0;

	for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-4)); i++)
	{
		/*增加看门狗,增强稳定性能*/
		watchdogUpdate();	
		__disable_irq();
		/*Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word*/ 
		if (FLASH_ProgramWord(*FlashAddress, *(uint32_t*)(Data+i)) == FLASH_COMPLETE)
		{
			/*Check the written value*/
			if (*(uint32*)*FlashAddress != *(uint32*)(Data+i))
			{
				/*Flash content doesn't match SRAM content*/
				return(2);
			}
			/*Increment FLASH destination address*/
			*FlashAddress += 4;
		}
		else
		{
			/*Error occurred while writing data in Flash memory*/
			return (1);
		}
		__enable_irq();
	}

	return (0);
}

