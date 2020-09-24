CONTIKI = ../../../..
PROJECT_SOURCEFILES += simple-unicast-common.c

ifdef IPI_DIVISION
	CFLAGS += -DIPI_DIVISION=$(IPI_DIVISION)
endif
ifdef PADDING
	CFLAGS += -DPADDING=$(PADDING)
endif
ifdef CST
	CFLAGS += -DCST=$(CST)
endif
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\" -DNETSTACK_CONF_RDC=contikimac_driver -DNETSTACK_CONF_MAC=nullmac_driver
CFLAGS += -DUIP_CONF_ND6_SEND_NA=1

all: src-node dst-node

CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RIME = 0
CONTIKI_WITH_RPL  = 0

include $(CONTIKI)/Makefile.include
