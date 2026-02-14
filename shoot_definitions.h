#ifndef SHOOT_DEFINITIONS_H
#define SHOOT_DEFINITIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long int64;

/** NOTE: Despite the name, bool32 must always be equal to either zero or one.
 * Please consider a safer standard in the future - chief **/
typedef unsigned int bool32;
// typedef double real;
typedef float real;
typedef uint8 byte;

#define KILOBYTES(val) ((val) << 10)
#define MEGABYTES(val) (KILOBYTES(val) << 10)
#define GIGABYTES(val) (MEGABYTES(val) << 10)

#define SUCCESS 0
#define FAILURE 1

#define TRUE 1
#define FALSE 0

#define DONT_CARE -1

#define CRASH *(char *)0 = 0

#ifdef DEBUG
#define verify(check, msg) verify_debug(check, msg, __FILE__, __LINE__)
static void verify_debug(bool32 check, const char *msg, const char *file, uint64 line)
{
    if (check) { return; }

    printf("Program failed with message: [%s] from file %s line %ld\n", msg, file, line);
    CRASH;
}
#define debug_log() debug_log_debug(__FILE__, __LINE__)
static void debug_log_debug(const char *file, uint64 line)
{
    printf("------> debug logger called %s %ld (search for 'debug_log();' in IDE)\n", file, line);
}
#else
#define verify(check, msg)
#define debug_log()
#endif

static void shoot_check_compatibility()
{
    if (sizeof(long) != 8)
    {
        printf("WARNING: sizeof(long) is not 8 bytes, this game uses 8 byte longs in a few places.\n"
                "Game will still open but might not work as expected. (Your long size is %lu)\n", sizeof(long));

        printf("If you are playing on browser or windows, this is a normal warning message.\n"
            "Game should work alright, please message Chiefkat05 if something seems off.\n");
    }
    if (sizeof(int) != 4)
    {
        printf("CRITICAL WARNING: sizeof(int) is not 4 bytes, this game relies on 4 byte integers to work properly.\n"
                "Game will still open but will almost certainly not work. (Your int size is %lu)\n", sizeof(int));
    }
    if (sizeof(short) != 2)
    {
        printf("CRITICAL WARNING: sizeof(short) is not 2 bytes, this game relies on 2 byte shorts to work properly.\n"
                "Game will still open but will almost certainly not work. (Your short size is %lu)\n", sizeof(short));
    }
    if (sizeof(char) != 1)
    {
        printf("CRITICAL WARNING: sizeof(char) is not 1 byte, this game relies on 1 byte chars to work properly.\n"
                "Game will still open but will almost certainly not work. (Your char size is %lu)\n", sizeof(char));
    }
}

#endif