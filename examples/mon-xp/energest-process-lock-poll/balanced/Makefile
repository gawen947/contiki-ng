all: balanced
CONTIKI = ../../../..

CONTIKI_WITH_RIME=1

ifndef SEND_COUNT
	${error SEND_COUNT not defined! You must specify the number of messages to send}
endif

ifndef CYCLES
	${error CYCLES not defined! You must specify a working time duration}
endif

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\" -DCYCLES=$(CYCLES) -DSEND_COUNT=$(SEND_COUNT)
SMALL = 1

ifdef RADIO_ISR_OFF
	CFLAGS += -DRADIO_ISR_OFF=1
endif

include $(CONTIKI)/Makefile.include
