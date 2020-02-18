
CC = cc
RM = rm -f
BIN_DIR  = ../bin

## files ##
HEADERS        = font.h
OBJECTS        = bas2img font__msx-din font__msx
TARGET_RELEASE = $(BIN_DIR)/bas2img
TARGET_DEBUG   = $(BIN_DIR)/bas2img_d

## compiler flags ##
CONFIG_RELEASE = -Os -DNDEBUG
CONFIG_DEBUG   = -O0 -D_DEBUG
CFLAGS_ANSI    = -ansi
CFLAGS_ERRORS  = -Wall -pedantic-errors -Wno-unused-function
CFLAGS         = $(CFLAGS_ANSI) $(CFLAGS_ERRORS)


OBJS_RELEASE = $(addsuffix _r.o, $(OBJECTS))
OBJS_DEBUG   = $(addsuffix _d.o, $(OBJECTS))


.PHONY: all debug release clean


all: debug


#----------------------------------------------
# DEBUG
#
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJS_DEBUG) $(HEADERS)
	$(CC) $(CONFIG_DEBUG)  -o $@  $(OBJS_DEBUG)


%_d.o: %.c
	$(CC) -c $(CFLAGS) $(CONFIG_DEBUG)  -o $@  $<



#-----------------------------------------------
# RELEASE
#
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJS_RELEASE) $(HEADERS)
	$(CC) $(CONFIG_RELEASE)  -o $@  $(OBJS_RELEASE)

%_r.o: %.c
	$(CC) -c $(CFLAGS) $(CONFIG_RELEASE)  -o $@  $<



#-----------------------------------------------
# CLEAN
#
clean:
	$(RM) $(TARGET_RELEASE) $(OBJS_RELEASE) $(TARGET_DEBUG) $(OBJS_DEBUG)







