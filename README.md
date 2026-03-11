# vent-common

# Общие файлы проектов vent-...

## Подключение через символьную ссылку
Запустить консоль от имени администратора в папке проекта и выполнить  
`mklink /D "vent-common" "..\vent-common"`  

## Настройка проекта
Добавить в проект пути для всех типов сборок:  
Include  
- /${ProjName}/vent-common
- /${ProjName}/vent-common/xcp
- /${ProjName}/vent-common/MetaData

Source
- /${ProjName}/vent-common

В коде подключить использование:
```#include "meta.h"  
#include "can_platform.h"  
#include "xcp_client_can.h"  
#include "xcp_platform.h"  
#include "common_param.h"
```

Скопировать в папку проекта (папку с хедерами) файлы и переименовать, задать имя проекта
- bn.h.template
- version.h.template

В Свойствах проекта прописать:  
- pre-build steps: `${ProjDirPath}/vent-common/MetaData/gen_headers.cmd`
- post-build steps: `${ProjDirPath}/vent-common/MetaData/update_bn.cmd ..\Core\Inc\bn.h` или др. путь к bn.h

Добавить общие константы:  
```const meta_data_t __attribute__((section(".boot_vers_sec"))) bootloader_version;
const meta_data_t __attribute__((section(".app_vers_sec"))) application_version =
{
    .version = VERSION_FULL_STRING,
    .build_date = BUILD_DATE,
    //.target_cpu = CPU,
    .crc = UINT32_MAX
};


               __attribute__((section(".calib_ram_sec")))   common_param_t param;
volatile const __attribute__((section(".calib_flash_sec"))) common_param_t param_flash;
const common_param_t param_def =
{
    .size = sizeof(common_param_t)
};
```

## Использование общего линкер-скрипта
Заменить линкер-скрипт в настройках проекта на `${workspace_loc:/${ProjName}/vent-common/linkerScript/STM32F103C8TX_APP.ld}`  
в файле `Src/system_stm32f1xx.c` 
- раскоментировать `#define USER_VECT_TAB_ADDRESS`
- изменить значение `#define VECT_TAB_OFFSET         0x00004000U`

## Включение XCP протокола
Добавить код использования XCP:
```
extern uint32_t _start_calib_ram[];
extern t_xcp_download_cb update_values;
var_t __attribute__((section(".var_ram_sec"))) v;

uint16_t config_number = 0;
uint32_t xcp_base_id = XCP_BASE_ID;

main()
{
    memset(&v, 0 , sizeof(v));
    memset(&_start_calib_ram, 0xFF , SECTOR_SIZE);
    
    platform_can_init();

    load_param();
    try_read_xcp_id();

    xcp_can_bootloader_init(0, 0, xcp_base_id);
    while (1)
    {
        xcp_can_poll();
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx_header;
    uint8_t data[8];

    if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data) != HAL_OK) return;

    platform_can_msg_recieve(0, &rx_header, data);

}

void try_read_xcp_id(void)
{
    if(param.xcp_canid_tx == param.xcp_canid_rx + 1 && param.xcp_canid_rx != 0xFFFFFFFF)
    {
        xcp_base_id = param.xcp_canid_rx;
    }
    else
    {
        xcp_base_id += config_number * 2;
    }
}

void load_param(void)
{
    uint32_t crc = 0;

    // Load defaul params
    memcpy(&param, &param_def, param_def.size);

    //Check saved params and load
    if(param_flash.size <= (SECTORS_FOR_PARAM * SECTOR_SIZE) && (param_flash.size != 0) && (param_flash.size % 4 == 0))
    {
        crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&param_flash.size, param_flash.size / 4 - 1);
        if(param_flash.crc == crc)
        {
            memcpy(&param, (const void *)&param_flash, param_flash.size);
        }
    }

    //Recalc CRC
    crc = HAL_CRC_Calculate(&hcrc, (uint32_t*)&param.size, param.size / 4 - 1);
    param.crc = crc;
}
```
