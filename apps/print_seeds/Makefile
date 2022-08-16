APPLICATION = printseeds

BOARD ?= iotlab-m3

# make sure this branxch is on: puf_continue_rene
#RIOTBASE ?= $(CURDIR)/../../RIOT
RIOTBASE=/media/DATA/RIOT
CFLAGS +=-DPRINT_SEEDS=1

USEMODULE += puf_sram
FEATURES_OPTIONAL += periph_rtc


include $(RIOTBASE)/Makefile.include
