#ifndef METADATA_META_H_
#define METADATA_META_H_

#include "stdint.h"
#include "bn.h"
#include "version.h"
//#include "personal.h"

#define STRINGIZER(arg)     #arg
#define STR_VALUE(arg)      STRINGIZER(arg)

#define BUILD_DATE    __DATE__ " @ " __TIME__

#if defined (DEBUG)
#define VERSION_TYPE    "D"
#elif defined (RELEASE)
#define VERSION_TYPE    "R"
#elif defined (PRODUCT)
#define VERSION_TYPE    "P"
#else
#define VERSION_TYPE    "T"
#endif

#if defined (VERSION_INFO)
#define VERSION_INFO_DELIMITER "::"
#else
#define VERSION_INFO_DELIMITER ""
#endif

#if VERSION_BUILD >= 1000
#define VERSION_BUILD_STR STR_VALUE(VERSION_BUILD)
#elif VERSION_BUILD >= 100
#define VERSION_BUILD_STR "0" STR_VALUE(VERSION_BUILD)
#elif VERSION_BUILD >= 10
#define VERSION_BUILD_STR "00" STR_VALUE(VERSION_BUILD)
#else
#define VERSION_BUILD_STR "000" STR_VALUE(VERSION_BUILD)
#endif

typedef struct
{
    char version[32];
    char build_date[32];
    char target_cpu[28];
    uint32_t crc;
} meta_data_t;


#define VERSION_FULL_STRING \
        STR_VALUE(VERSION_PROJECT) "~" \
        VERSION_TYPE \
        STR_VALUE(VERSION_MAJOR) "." \
        STR_VALUE(VERSION_MINOR) "." \
        STR_VALUE(VERSION_PATCH) ":" \
        VERSION_BUILD_STR /*"-"*/ \
        /*META_USERNAME*/ \
        VERSION_INFO_DELIMITER \
        VERSION_INFO


//volatile const meta_data_t app_version __at(0x801FFb80) =
//{
//        .version = VERSION_FULL_STRING,
//        .build_date = BUILD_DATE,
//        .target_cpu = STR_VALUE(__CPU__),
//        .crc = UINT32_MAX
//};
#endif /* METADATA_META_H_ */
