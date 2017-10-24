# note: this Makefile builds the Linux version only

CC = gcc
ARMCC = /usr/local/gcc/csibe/arm-none-eabi-toolchain-gcc-7.1.0-csibe-softfloat/bin/arm-none-eabi-gcc
ARMOBJDUMP = /usr/local/gcc/csibe/arm-none-eabi-toolchain-gcc-7.1.0-csibe-softfloat/bin/arm-none-eabi-objdump

WARNINGS = -Wall -W -Wextra -Wno-unused-parameter -Wno-unused-but-set-variable
CFLAGS = $(WARNINGS) -Os
#-march=pentium
#CFLAGS += -ffast-math
#CFLAGS += -finline-functions-called-once
#CFLAGS += -fno-loop-optimize
#CFLAGS += -fexpensive-optimizations
#CFLAGS += -fpeephole2

STRIPFLAGS  = -R .comment
STRIPFLAGS += -R .note
STRIPFLAGS += -R .note.ABI-tag
STRIPFLAGS += -R .gnu.version

BIN = minimp3
FINALBIN = $(BIN)-linux
OBJS = player_alsa.o minimp3.o

all:	$(BIN)

release:	$(BIN)
	strip $(STRIPFLAGS) $(BIN)
	upx --brute $(BIN)

test:	$(BIN)
#	./$(BIN) "./test/bach_8khz_mono.mp3" default
	./$(BIN) "./test/SoundHelix-Song-1.mp3" default

$(BIN):	$(OBJS)
	$(CC) $(OBJS) -o $(BIN) -lm -lasound

%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

arm:
	$(ARMCC) -mcpu=cortex-m4 -c minimp3.c $(WARNINGS) -Os
	$(ARMOBJDUMP) -xds minimp3.o > minimp3.dis

clean:
	rm -f $(BIN) $(OBJS)

dist:	clean release
	mv $(BIN) $(FINALBIN)
	rm -f $(OBJS)
