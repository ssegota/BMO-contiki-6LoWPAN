DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = bmo-ping-pong

all: $(CONTIKI_PROJECT)

APPS=servreg-hack
CONTIKI_WITH_IPV6 = 1

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
