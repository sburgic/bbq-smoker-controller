/**
 ** Name
 **   main.h
 **
 ** Purpose
 **   Main application definitions
 **
 ** Revision
 **   28-Nov-2021 (SSB) [] Initial
 **/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdint.h>

#define APP_VER_MAJOR 1
#define APP_VER_MINOR 0
#define APP_VER_PATCH 0

#define STRINGIFY_TMP(s) # s
#define STRINGIFY(s)     STRINGIFY_TMP(s)

#define APP_SW_VERSION (uint8_t*)"SW ver."STRINGIFY(APP_VER_MAJOR)"."\
                       STRINGIFY(APP_VER_MINOR)"."STRINGIFY(APP_VER_PATCH)""

#define APP_WELCOME_TEXT_1 (uint8_t*)"DANGER ZONE BL"
#define APP_WELCOME_TEXT_2 (uint8_t*)"Smoker Controller"
#define APP_WELCOME_TEXT_3 (uint8_t*)"by Thrash Burgija"

#endif /* __MAIN_H__ */
