APPLICATION = printids

BOARD ?= iotlab-m3

RIOTBASE ?= $(CURDIR)/../../RIOT

## Change one of the following flags to configure the fuzzy extractor.
CFLAGS?=-DPUF_SRAM_CODEOFFSET_LEN=18
CFLAGS?=-DECC_REPETITION_COUNT=7

## Enable one of the following flags to compile firmware that: clears
## non-volatile memory, performs the enrollment, or the reconstruction
# CFLAGS+=-DSRAM_PUF_ID_CLEAR=1
# CFLAGS+=-DSRAM_PUF_ID_ENROLL=1
# CFLAGS+=-DSRAM_PUF_ID_RECONSTRUCT=1

CFLAGS+=-DPRINT_IDS=1
USEMODULE += puf_sram
USEMODULE += puf_sram_secret
USEMODULE += prng_xorshift
FEATURES_OPTIONAL += periph_rtc


ifneq (,$(filter iotlab-m3,$(BOARD)))
	FEATURES_REQUIRED += periph_spi
	USEMODULE += n25q128
	CFLAGS += -DUSE_N25Q128=1
endif

include $(RIOTBASE)/Makefile.include
