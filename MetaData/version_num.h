// source file version_num.h

#ifndef VERSION_NUM_H
#define VERSION_NUM_H

#define STRINGIZER(arg)     #arg
#define STR_VALUE(arg)      STRINGIZER(arg)

typedef union
{
    char full[124];
    struct
    {
        char project[8];
        char number[11];
        char build[4];
        char info[60];
    }parts;
}project_name_t;


// Number version
#define VERSION_MAJOR_CHARS \
    ((VERSION_MAJOR / 100) + '0'), \
    (((VERSION_MAJOR % 100) / 10) + '0'), \
    ((VERSION_MAJOR % 10) + '0')

#define VERSION_MINOR_CHARS \
    ((VERSION_MINOR / 10) + '0'), \
    ((VERSION_MINOR % 10) + '0')

#define VERSION_PATCH_CHARS \
    ((VERSION_PATCH / 10) + '0'), \
    ((VERSION_PATCH % 10) + '0')

// Project name
#define VERSION_PROJECT_STR     STR_VALUE(VERSION_PROJECT)

#define VERSION_PROJECT_CHARS \
    (VERSION_PROJECT_STR[0])


#endif // VERSION_NUM_H
