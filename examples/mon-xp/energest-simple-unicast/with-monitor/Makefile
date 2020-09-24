CONTIKI = ../../../..
CONTIKI_PROJECT = fwd-node-monitor
PROJECT_SOURCEFILES += simple-unicast-common.c

CFLAGS += -DUIP_CONF_ND6_SEND_NA=1 -DUIP_CONF_ROUTER=1 -DNETSTACK_CONF_RDC=nullrdc_driver -DPROJECT_CONF_H=\"project-conf.h\"

ifdef RADIO_ISR_OFF
	CFLAGS += -DRADIO_ISR_OFF=1
endif

ifdef PERIOD
	CFLAGS += -DPERIOD=$(PERIOD)
endif

all: $(CONTIKI_PROJECT)

CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL  = 0
include $(CONTIKI)/Makefile.include
