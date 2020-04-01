
CC = cc
RM = rm -f
BIN_DIR   = ../bin
DECOS_DIR = ./decos
FONTS_DIR = ./fonts

## files ##
HEADERS  = bas2img.h types.h helpers.h error.h rows.h database.h generate.h import.h export.h image.h gif.h bmp.h
DECOS    = d-atari d-msx d-msxasc
FONTS    = f-atari f-msx f-msxdin
SOURCES  = bas2img helpers error rows database generate import export image gif bmp
TARGET_RELEASE = $(BIN_DIR)/bas2img
TARGET_DEBUG   = $(BIN_DIR)/bas2img_d

## compiler flags ##
CONFIG_RELEASE = -Os -DNDEBUG
CONFIG_DEBUG   = -O0 -D_DEBUG
CFLAGS_ANSI    = -ansi
CFLAGS_ERRORS  = -Wall -pedantic-errors -Wno-unused-function
CFLAGS         = $(CFLAGS_ANSI) $(CFLAGS_ERRORS)


EXTRA        = $(addprefix $(DECOS_DIR)/,$(DECOS)) $(addprefix $(FONTS_DIR)/,$(FONTS))
OBJS_RELEASE = $(addsuffix _r.o, $(SOURCES) $(EXTRA))
OBJS_DEBUG   = $(addsuffix _d.o, $(SOURCES) $(EXTRA))


.PHONY: all debug release clean


all: debug


#----------------------------------------------
# DEBUG
#
debug: $(TARGET_DEBUG)

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) $(CONFIG_DEBUG)  -o $@  $^


%_d.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(CONFIG_DEBUG)  -o $@  $<



#-----------------------------------------------
# RELEASE
#
release: $(TARGET_RELEASE)

$(TARGET_RELEASE): $(OBJS_RELEASE)
	$(CC) $(CONFIG_RELEASE)  -o $@  $^

%_r.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $(CONFIG_RELEASE)  -o $@  $<



#-----------------------------------------------
# CLEAN
#
clean:
	$(RM) $(TARGET_RELEASE) $(OBJS_RELEASE) $(TARGET_DEBUG) $(OBJS_DEBUG)







