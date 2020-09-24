CONTIKI = ../../../..
APPS = powertrace collect-view
CONTIKI_PROJECT = udp-mon-sender
PROJECT_SOURCEFILES += collect-common.c

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"


ifdef RADIO_ISR_OFF
	CFLAGS += -DRADIO_ISR_OFF=1
endif

ifdef PERIOD
	CFLAGS += -DPERIOD=$(PERIOD)
endif

all: $(CONTIKI_PROJECT)

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
