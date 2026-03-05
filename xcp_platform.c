/*
 * xcp_platform.c
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */

#include <stdint.h>
#include <stddef.h>

#include "xcp_platform.h"

#define CRC_CHECK_FW
#ifdef CRC_CHECK_FW
extern CRC_HandleTypeDef hcrc;
#endif

uint32_t flash_bootloader = 0;

extern void deinit_perif(void);

uint8_t check_programm_available(void)
{
//	// check availability firmware
//
	uint8_t available = 1;
	uint32_t crc = UINT32_MAX;

	uint32_t* firmware_first_word_ptr;
	uint32_t* crc_in_flash_ptr;
	firmware_first_word_ptr = (uint32_t*)SECTOR_0_FW;
	crc_in_flash_ptr = (uint32_t*)CRC_ADDRESS;

	if(firmware_first_word_ptr[0] == 0xFFFFFFFF || firmware_first_word_ptr[0] == 0x00000000 ||
			firmware_first_word_ptr[1] == 0xFFFFFFFF || firmware_first_word_ptr[1] == 0x000000000)
	{
		available = 0;
	}
#ifdef CRC_CHECK_FW
	else
	{
		crc = HAL_CRC_Calculate(&hcrc, firmware_first_word_ptr, FW_MAX_SIZE / 4);
		if(crc != *crc_in_flash_ptr
#ifdef DEBUG
			&& *crc_in_flash_ptr != UINT32_MAX
#endif //DEBUG
		)
		{
			available = 0;
		}
	}
#endif //CRC_CHECK_FW
	return available;
}

size_t flash_range_check(uint32_t mta, uint32_t range)
{
	if(flash_bootloader)
	{
		if(mta < SECTOR_0_BOOT || (mta + range) > (SECTOR_0_BOOT + SECTORS_FOR_BOOT * SECTOR_SIZE))
		{
			return 0;
		}
	}
	else // flash application
	{
		if((mta < SECTOR_0_FW || (mta + range) > (SECTOR_0_FW + SECTORS_FOR_FW * SECTOR_SIZE)) && (mta < SECTOR_PARAM || (mta + range) > (SECTOR_PARAM + SECTOR_SIZE)))
		{
			return 0;
		}
	}

	return 1;
}


void go_to_application(void)
{
	uint32_t appJumpAddress;

	if(check_programm_available() == 1)
	{
		deinit_perif();

		void (*GoToApp)(void);
		appJumpAddress = *((volatile uint32_t*)(SECTOR_0_FW + 4));
		GoToApp = (void (*)(void))appJumpAddress;
//		SCB->VTOR = SECTOR_0_FW;
		__set_MSP(*((volatile uint32_t*) SECTOR_0_FW)); //stack pointer (to RAM) for USER app in this address
		GoToApp();
	}
}

void go_to_bootloader(void)
{
	if(flash_bootloader)
	{
		return;
	}
	HAL_PWR_EnableBkUpAccess();

	BKP->DR1 = BOOT_MSG_XCP_REQ;

	HAL_NVIC_SystemReset();
}

void reset_device(void)
{
	HAL_NVIC_SystemReset();
}

uint32_t check_bootloader_request(void)
{
	uint32_t req = 0;
	HAL_PWR_EnableBkUpAccess();

	if(BKP->DR1 == BOOT_MSG_XCP_REQ)
	{
		req = 1;
	}
	BKP->DR1 = BOOT_MGG_CLEAR;
	return req;
}

void *xcp_memcpy(void *dest __attribute__((unused)),
		const void *src __attribute__((unused)),
		uint32_t n __attribute__((unused)))
{
	memcpy(dest, src, n);
}

void do_nothing(void)
{

}

uint8_t xcp_program_clear(uint32_t mta, uint32_t range)
{
	uint8_t res = XCP_RESP_POS;
	FLASH_EraseInitTypeDef flash_eraseInitStruct =
	{
			.TypeErase = FLASH_TYPEERASE_PAGES,
			.PageAddress = SECTOR_0_FW,
			.NbPages = SECTORS_FOR_FW
	};
	uint32_t out;


	if(!flash_range_check(mta, range))
	{
		res = XCP_ERR_OUT_OF_RANGE;
	}
	else if(mta % SECTOR_SIZE != 0 || range % SECTOR_SIZE != 0)
	{
		res = XCP_ERR_PAGE_NOT_VALID;
	}
	else
	{
		flash_eraseInitStruct.PageAddress = mta;
		flash_eraseInitStruct.NbPages = range / SECTOR_SIZE;

		HAL_FLASH_Unlock();
		if(HAL_FLASHEx_Erase(&flash_eraseInitStruct, &out) != HAL_OK)
		{
			res = XCP_ERR_PGM_ACTIVE; //TODO check right code
		}
		HAL_FLASH_Lock();
	}

	return res;
}

uint8_t xcp_program(uint32_t mta, uint8_t *buf, uint8_t len)
{
	uint8_t res = XCP_RESP_POS;
	uint64_t data_buf;

	if(!flash_range_check(mta, len))
	{
		res = XCP_ERR_OUT_OF_RANGE;
	}
	else if(len % 2 != 0)
	{
		res = XCP_ERR_SEGMENT_NOT_VALID;
	}
	else
	{
		HAL_FLASH_Unlock();

		for(int i = 0; i < len; i += 2)
		{
			data_buf = buf[i] + (buf[i + 1] << 8);
			HAL_StatusTypeDef flash_res = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, mta + i, data_buf);
			if(flash_res != HAL_OK)
			{
				res = XCP_ERR_GENERIC;
			}
		}

		HAL_FLASH_Lock();
	}
	return res;
}

void set_flash_bootloader(void)
{
	flash_bootloader = 1;
}
