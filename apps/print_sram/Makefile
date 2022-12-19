APPLICATION = printsram

BOARD ?= iotlab-m3

ifneq (iotlab-m3, $(BOARD))
  $(error This app is currently only supported by 	iotlab-m3.)
endif

RIOTBASE ?= $(CURDIR)/../../RIOT

## Enable one of the following flags to output of first or second half of SRAM.
## It can also be set guring the make call
CFLAGS=-DSRAM_PRINT_FIRST=1
# CFLAGS=-DSRAM_PRINT_LAST=1

FEATURES_REQUIRED = periph_cpuid
FEATURES_OPTIONAL += periph_rtc

include $(RIOTBASE)/Makefile.include
