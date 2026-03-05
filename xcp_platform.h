/*
 * xcp_platform.h
 *
 *  Created on: Dec 3, 2021
 *      Author: arh
 */

#ifndef INC_XCP_PLATFORM_H_
#define INC_XCP_PLATFORM_H_

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"
#include "stm32f1xx_hal_crc.h"
#include "xcp_client.h"
#include "utils.h"

#define XCP_BASE_ID	0x7AE

#define SECTOR_SIZE 1024
//#define SECTOR_0_ADRESS 0x8020000

#define SECTOR_0_BOOT	0x8000000
#define SECTORS_FOR_BOOT	15
#define BOOT_MAX_SIZE (SECTORS_FOR_BOOT * SECTOR_SIZE - 4)
#define CRC_BOOT_ADDRESS (SECTOR_0_BOOT + BOOT_MAX_SIZE + 4)

#define SECTOR_0_FW	0x8004000
#define SECTORS_FOR_FW	48
#define FW_MAX_SIZE (SECTORS_FOR_FW * SECTOR_SIZE - 4)
#define CRC_ADDRESS (SECTOR_0_FW + FW_MAX_SIZE + 4)

#define SECTOR_PARAM	0x08003C00
#define SECTORS_FOR_PARAM	1

#define BOOT_MGG_CLEAR		0U
#define BOOT_MSG_XCP_REQ	0x01U
#define BOOT_MSG_UDS_REQ	0x10U

void reset_device(void);
void go_to_bootloader(void);
void go_to_application(void);
void do_nothing(void);

uint8_t xcp_program_clear(uint32_t mta, uint32_t range);
uint8_t xcp_program(uint32_t mta, uint8_t *buf, uint8_t len);

uint32_t check_bootloader_request(void);
void set_flash_bootloader(void);

#endif /* INC_XCP_PLATFORM_H_ */
