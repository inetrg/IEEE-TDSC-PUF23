APPLICATION = printseeds

BOARD ?= iotlab-m3

RIOTBASE ?= $(CURDIR)/../../RIOT

CFLAGS +=-DPRINT_SEEDS=1

USEMODULE += puf_sram
FEATURES_OPTIONAL += periph_rtc


include $(RIOTBASE)/Makefile.include
