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
#include <inttypes.h>

#include "puf_sram.h"
#include "periph/rtc.h"
#include "periph/pm.h"

#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#ifndef STANDBY_TIME_S
#define STANDBY_TIME_S                (1) // seconds
#endif

#ifndef MODULE_RISCV_COMMON
/* clamps value between minimum and maximum */
#define CLAMP(x, min, max)    ((x > max) ? max : ((x < min) ? min : x))

static void cb_rtc_dummy(void *arg)
{
    (void)arg;
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

    rtc_set_alarm(&time, cb_rtc_dummy, 0);

#ifdef MODULE_BOARDS_COMMON_IOTLAB
    // fix bug that prevents entering sleep mode after flashing MCU
    DBGMCU->CR &= ~(DBGMCU_CR_DBG_SLEEP | DBGMCU_CR_DBG_STOP | DBGMCU_CR_DBG_STANDBY | DBGMCU_CR_TRACE_IOEN);
#endif

    pm_set(0);
}
#endif /* MODULE_RISCV_COMMON */

int main(void)
{
    printf("STANDBY_TIME_S;%i\n", STANDBY_TIME_S);
    printf("PUF_SRAM_SEED_RAM_LEN;%i\n", PUF_SRAM_SEED_RAM_LEN);
    printf("PUF_SRAM_SEED_RAM_LEN_SECURE;%i\n", PUF_SRAM_SEED_RAM_LEN_SECURE);

    printf("seed;");
    printf("%08" PRIX32 "\n", puf_sram_seed);

    printf("seedsecure;");
    for (unsigned i = 0; i < sizeof(puf_sram_seed_secure); i++) {
        printf("%02x", puf_sram_seed_secure[i]);
    }
    printf("\n");

#ifndef MODULE_RISCV_COMMON
    _going_into_standby(STANDBY_TIME_S);
#endif

    return 0;
}
