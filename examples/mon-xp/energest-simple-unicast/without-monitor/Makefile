CONTIKI = ../../../..
CONTIKI_PROJECT = src-node fwd-node dst-node
PROJECT_SOURCEFILES += simple-unicast-common.c

CFLAGS += -DUIP_CONF_ND6_SEND_NA=1 -DUIP_CONF_ROUTER=1

ifdef PERIOD
CFLAGS=-DPERIOD=$(PERIOD)
endif

all: $(CONTIKI_PROJECT)

CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL  = 0
include $(CONTIKI)/Makefile.include
