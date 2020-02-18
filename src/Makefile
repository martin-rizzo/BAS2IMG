
CC = cc
RM = rm -f
BIN_DIR  = ../bin
DBG_DIR  = debug
REL_DIR  = release

## files ##
HEADERS       = font.h
OBJS          = bas2img.o font__msx-din.o font__msx.o
TARGET        = $(BIN_DIR)/bas2img
TARGET_DBG    = $(BIN_DIR)/bas2img_d

## compiler flags ##
DEBUG_FLAGS   = -O0 -D_DEBUG
RELEASE_FLAGS = -Os -DNDEBUG
CFLAGS_ANSI   = -ansi
CFLAGS_ERRORS = -Wall -pedantic-errors -Wno-unused-function
CFLAGS        = $(CFLAGS_ANSI) $(CFLAGS_ERRORS)



OBJS_DBG = $(addprefix $(DBG_DIR)/, $(OBJS))
OBJS_REL = $(addprefix $(REL_DIR)/, $(OBJS))


.PHONY: all debug release clean


all: debug


#----------------------------------------------
# DEBUG
#
debug: $(TARGET_DBG)

$(TARGET_DBG): $(OBJS_DBG) $(HEADERS)
	$(CC) $(DEBUG_FLAGS)  -o $@  $(OBJS_DBG)


$(DBG_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DEBUG_FLAGS)  -o $@  $<



#-----------------------------------------------
# RELEASE
#
release: $(TARGET)

$(TARGET): $(OBJS_REL) $(HEADERS)
	$(CC) $(RELEASE_FLAGS)  -o $@  $(OBJS_REL)

$(REL_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(RELEASE_FLAGS)  -o $@  $<



#-----------------------------------------------
# CLEAN
#
clean:
	$(RM) $(TARGET) $(OBJS_REL) $(TARGET_DBG) $(OBJS_DBG)







