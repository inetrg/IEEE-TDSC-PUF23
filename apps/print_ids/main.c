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
#include <stdlib.h>

// #include "periph_conf.h"
// #include "cpu_conf.h"
#include "board.h"

#include "ecc/repetition.h"
#include "ecc/golay2412.h"
#include "random.h"
#include "timex.h"
#include "puf_sram.h"
#include "periph/rtc.h"
#include "periph/pm.h"

#ifdef MODULE_PM_LAYERED
#include "pm_layered.h"
#endif

#ifdef MODULE_BOARDS_COMMON_IOTLAB

#include "n25q128.h"
static n25q128_dev_t n25q128;

#define HELPER_N25Q128_ADDR         (0x0)     // sector 0
#define HELPER_FLAG_N25Q128_ADDR    (0x20000) // sector 2

#define ID_LOOPS_FLAG_ADDR          (0x30000) // sector 3
#define ID_LOOPS_ITER_ADDR          (0x40000) // sector 4
#define RANDOM_ERASE_MARKER_ADDR    (0x50000) // sector 5

#define RANDOM_ERASE_MARKER         (0xaffecafe)
#define FLAG_SIZE                   (sizeof(uint8_t)) // bytes

#endif /* MODULE_BOARDS_COMMON_IOTLAB */

#ifndef STANDBY_TIME_S
#define STANDBY_TIME_S                (1)             // seconds
#endif


#ifndef MODULE_RISCV_COMMON

/* clamps a specifc value between minimum and maximum */
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

#ifdef MODULE_BOARDS_COMMON_IOTLAB
static inline void _wait_for_wip(void)
{
    while(n25q128_write_in_progress(&n25q128));
}

static inline void _wait_for_controller(void)
{
    while(n25q128_program_erase_status(&n25q128) == 0);
}

static inline void _sector_erase(uint32_t addr)
{
    _wait_for_controller();
    n25q128_sector_erase(&n25q128, addr);
    _wait_for_wip();
}

static inline void _page_program(uint32_t addr, uint8_t * out, size_t len)
{
    _wait_for_controller();
    n25q128_page_program(&n25q128, addr, out, len);
    _wait_for_wip();
}

static inline void _read_bytes(uint32_t addr, uint8_t * in, size_t len)
{
    _wait_for_controller();
    n25q128_read_data_bytes(&n25q128, addr, in, len);
}

static inline void _set_flag(uint32_t flag_addr)
{
    static uint8_t flag[FLAG_SIZE] = {1};
    _page_program(flag_addr, flag, FLAG_SIZE);
}

static inline void _clear_flag(uint32_t flag_addr)
{
    _sector_erase(flag_addr);
}
static inline int _is_flag_set(uint32_t flag_addr)
{
    static uint8_t flag[FLAG_SIZE] = {0};
    _wait_for_controller();
    n25q128_read_data_bytes(&n25q128, flag_addr, flag, FLAG_SIZE);
    return (flag[0] == 1);
}

static inline int _erase_helper(void)
{
    puts("_erase_helper;");
    _sector_erase(HELPER_N25Q128_ADDR);
    _clear_flag(HELPER_FLAG_N25Q128_ADDR);

    return 0;
}

static inline int _setup_config(void)
{
    puts("_setup_config;");

    _sector_erase(RANDOM_ERASE_MARKER_ADDR);

    // program the marker value for power cycles into the flash memory
    uint32_t marker = RANDOM_ERASE_MARKER;
    _page_program(RANDOM_ERASE_MARKER_ADDR, (uint8_t *)&marker, sizeof(marker));

    _wait_for_controller();

    return 0;
}

static inline void _gen_helper_data(void)
{
    static uint8_t helper[PUF_SRAM_HELPER_LEN] = {0};
    static uint8_t golay[PUF_SRAM_GOLAY_LEN] = {0};
    static uint8_t repetition[PUF_SRAM_HELPER_LEN] = {0};
    static uint8_t bytes[PUF_SRAM_CODEOFFSET_LEN] = {0};
    static uint8_t puf_sram_idE[PUF_SRAM_ID_DIGEST_LEN] = {0};

    random_bytes(bytes, PUF_SRAM_CODEOFFSET_LEN);
    golay2412_encode(PUF_SRAM_CODEOFFSET_LEN, bytes, golay);
    repetition_encode(PUF_SRAM_GOLAY_LEN, golay, repetition);

    for(unsigned i = 0; i < PUF_SRAM_HELPER_LEN; i++) {
        helper[i] = repetition[i] ^ debug_meas[i];
    }

    int wrote_bytes = 0;
    int remaining = PUF_SRAM_HELPER_LEN;
    int index=0;

    while (PUF_SRAM_HELPER_LEN > wrote_bytes)
    {
        int snip_len = 0;
        if ((remaining - 256) > 0)
        {
            remaining -= 256;
            snip_len = 256;
        }
        else
        {
            snip_len = remaining;
        }
        wrote_bytes += snip_len;

        _page_program(HELPER_N25Q128_ADDR + index, &helper[index], snip_len);
        index += 256;
    }

    _set_flag(HELPER_FLAG_N25Q128_ADDR);

    printf("idEnroll;");
#ifdef USE_SHA1
    sha1(&puf_sram_idE, ref_mes, PUF_SRAM_HELPER_LEN);
#else
    sha256(debug_meas, PUF_SRAM_HELPER_LEN, &puf_sram_idE);
#endif

    for (unsigned i = 0; i < sizeof(puf_sram_idE); i++) {
        printf("%02x", puf_sram_idE[i]);
    }
    printf("\n");
    printf("codeEnroll;");
    for (unsigned i = 0; i < sizeof(bytes); i++) {
        printf("%02x", bytes[i]);
    }
    printf("\n");
    printf("helperEnroll;");
    for (unsigned i = 0; i < sizeof(helper); i++) {
        printf("%02x", helper[i]);
    }
    printf("\n");
    printf("pufEnroll;");
    for (unsigned i = 0; i < PUF_SRAM_HELPER_LEN; i++) {
        printf("%02x", debug_meas[i]);
    }
    printf("\n");
}

static inline int is_initial_boot(void)
{
    uint32_t marker=0;
    _read_bytes(RANDOM_ERASE_MARKER_ADDR, (uint8_t *)&marker, sizeof(marker));
    printf("read marker; %"PRIx32"\n", marker);
    if (marker == RANDOM_ERASE_MARKER)
    {
        return 0;
    }
    return 1;
}
#endif /* MODULE_BOARDS_COMMON_IOTLAB */

int main(void)
{
#ifdef MODULE_BOARDS_COMMON_IOTLAB
    // initialize flash memory
    n25q128.conf.bus = EXTFLASH_SPI;
    n25q128.conf.mode = SPI_MODE_0;
    n25q128.conf.clk = SPI_CLK_100KHZ;
    n25q128.conf.cs = EXTFLASH_CS;
    n25q128.conf.write = EXTFLASH_WRITE;
    n25q128.conf.hold = EXTFLASH_HOLD;

    if ((n25q128_init(&n25q128) != 0)) {
        puts("FAILED to initialize n25q128 flash memory");
        return 0;
    }

#ifdef SRAM_PUF_ID_CLEAR
    _erase_helper();
    _sector_erase(RANDOM_ERASE_MARKER_ADDR);
    _clear_flag(HELPER_FLAG_N25Q128_ADDR);
#endif /* SRAM_PUF_ID_CLEAR */

#ifdef SRAM_PUF_ID_ENROLL
    if(is_initial_boot())
    {
        printf("PUF_SRAM_CODEOFFSET_LEN;%i\n", PUF_SRAM_CODEOFFSET_LEN);
        printf("PUF_SRAM_GOLAY_LEN;%i\n", PUF_SRAM_GOLAY_LEN);
        printf("PUF_SRAM_REP_COUNT;%i\n", PUF_SRAM_REP_COUNT);
        printf("PUF_SRAM_HELPER_LEN;%i\n", PUF_SRAM_HELPER_LEN);
        printf("STANDBY_TIME_S;%i\n", STANDBY_TIME_S);

        _erase_helper();
        _setup_config();
        puts("Now trigger standby;");

        _going_into_standby(STANDBY_TIME_S);
    }
    else
    {
        // don't produce new helper data if already in place
        if(!_is_flag_set(HELPER_FLAG_N25Q128_ADDR)){
            puts("calculate and save new helperdata;");
            _gen_helper_data();
        }
        if(_is_flag_set(HELPER_FLAG_N25Q128_ADDR)){
            puts("helper data already exists;");
        }
    }

#endif /* SRAM_PUF_ID_ENROLL */

#endif /* MODULE_BOARDS_COMMON_IOTLAB */

#ifdef SRAM_PUF_ID_RECONSTRUCT

    printf("\n");
    printf("idReconstruct;");
    for (unsigned i = 0; i < sizeof(puf_sram_id); i++) {
        printf("%02x", puf_sram_id[i]);
    }
    printf("\n");

    printf("codeReconstruct;");
    for (unsigned i = 0; i < sizeof(debug_codeoffset); i++) {
        printf("%02x", debug_codeoffset[i]);
    }
    printf("\n");

    printf("pufReconstruct;");
    for (unsigned i = 0; i < sizeof(debug_reconstruct); i++) {
        printf("%02x", debug_reconstruct[i]);
    }
    printf("\n");

    printf("debug_measReconstruct;");
    for (unsigned i = 0; i < sizeof(debug_meas); i++)
    {
        printf("%02x", debug_meas[i]);
    }
    printf("\n");

    // clear memory
    memset(debug_codeoffset, 0, sizeof(debug_codeoffset));
    memset(debug_reconstruct, 0, sizeof(debug_reconstruct));
    memset(debug_meas, 0, sizeof(debug_meas));

    puf_sram_delete_secret();

#ifndef MODULE_RISCV_COMMON
    _going_into_standby(STANDBY_TIME_S);
#endif

#endif /* SRAM_PUF_ID_RECONSTRUCT */

    return 0;
}
