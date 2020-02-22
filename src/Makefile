
CC = cc
RM = rm -f
BIN_DIR  = ../bin

## files ##
HEADERS        = bas2img.h types.h helpers.h error.h import.h export.h font.h bmp.h gif.h
FONTS          = font__atari.h font__msx.h font__msx-din.h
OBJECTS        = bas2img helpers error import export font bmp
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

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) $(CONFIG_DEBUG)  -o $@  $^


%_d.o: %.c $(HEADERS) $(FONTS)
	$(CC) -c $(CFLAGS) $(CONFIG_DEBUG)  -o $@  $<



#-----------------------------------------------
# RELEASE
#
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJS_RELEASE)
	$(CC) $(CONFIG_RELEASE)  -o $@  $^

%_r.o: %.c $(HEADERS) $(FONTS)
	$(CC) -c $(CFLAGS) $(CONFIG_RELEASE)  -o $@  $<



#-----------------------------------------------
# CLEAN
#
clean:
	$(RM) $(TARGET_RELEASE) $(OBJS_RELEASE) $(TARGET_DEBUG) $(OBJS_DEBUG)







