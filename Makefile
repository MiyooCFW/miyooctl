SOURCES ?= main2.c
PROG ?= miyooctl2

#PREFIX ?= arm-linux-
#PREFIX ?= arm-linux-gnueabi-
PREFIX ?= arm-buildroot-linux-musleabi-

EXTRAFLAGS ?= -Wall -static

all:
	$(PREFIX)gcc $(SOURCES) -o $(PROG) -ggdb -Os -flto $(EXTRAFLAGS)
	$(PREFIX)strip $(PROG)
clean:
	rm -rf $(PROG)
