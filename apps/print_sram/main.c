/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @{
 *
 * @file
 * @brief
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "timex.h"
#include "puf_sram.h"
#include "periph/pm.h"
#include "periph/rtc.h"
#include "periph/cpuid.h"
#include "cpu_conf.h"
#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#ifndef STANDBY_TIME_S
#define STANDBY_TIME_S                (5) // seconds
#endif

// defined by linker
extern uint8_t _sram;
extern uint8_t _eram;


/* clamps value between minimum and maximum */
#define CLAMP(x, min, max)    ((x > max) ? max : ((x < min) ? min : x))

void print_memory(int offset)
{
    int sram_size_half = (&_eram - &_sram) >> 1;
    uint32_t *start_second_half = (uint32_t *)(&_eram - sram_size_half - offset);

    printf("memory\n");

    while(start_second_half < (uint32_t *)&_eram)
    {
        printf("%08" PRIx32 "\n", *start_second_half);
        start_second_half++;
    }
    puts("");
}

static inline void _going_into_standby(int seconds)
{
    struct tm time;

    rtc_get_time(&time);

    time.tm_sec += CLAMP(seconds, 0, 60);
    if (time.tm_sec >= 60) {
        time.tm_sec -= 60;
        time.tm_min += 1;
    }

    rtc_set_alarm(&time, 0, 0);

#ifdef MODULE_BOARDS_COMMON_IOTLAB
    // fix bug that prevents entering sleep mode after flashing MCU
    DBGMCU->CR &= ~(DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY | DBGMCU_CR_TRACE_IOEN);
#endif

    pm_set(0);
}

int main(void)
{
    uint8_t id[CPUID_LEN];

    // print the CPUID
    cpuid_get(id);
    printf("CPUID;0x");
    for (unsigned int i = 0; i < CPUID_LEN; i++) {
        printf("%02x", id[i]);
    }
    printf("\n");

#ifdef MODULE_BOARDS_COMMON_IOTLAB
    // print the MCUID
    uint32_t mcudevid;
    memcpy(&mcudevid, (void *)DBGMCU_BASE, 4);
    printf("MCUID;0x%08"PRIx32"\n", mcudevid);
#endif

#ifdef SRAM_PRINT_FIRST
    print_memory(0);
#endif

#ifdef SRAM_PRINT_LAST
    // 4 is an offset also used in puf_sram_print_last()
    print_memory(4);
#endif

    _going_into_standby(STANDBY_TIME_S);

    return 0;
}
