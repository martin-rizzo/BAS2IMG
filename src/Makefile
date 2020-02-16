
CC = cc
RM = rm -f
BIN_DIR  = ../bin
APP_NAME = bas2img

TARGET        = $(BIN_DIR)/$(APP_NAME)
TARGET_DBG    = $(BIN_DIR)/$(APP_NAME)_d
SRC           = $(APP_NAME).c
DEBUG_FLAGS   = -O0 -D_DEBUG
RELEASE_FLAGS = -Os -DNDEBUG
CFLAGS_ANSI   = -ansi
CFLAGS_ERRORS = -Wall -pedantic-errors -Wno-unused-function
CFLAGS        = $(CFLAGS_ANSI) $(CFLAGS_ERRORS)


.PHONY: all debug release clean


all: debug

debug: $(TARGET_DBG)

release: $(TARGET)

clean:
	$(RM) $(TARGET) $(TARGET_DBG)


$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $^ -o $@

$(TARGET_DBG): $(SRC)
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) $^ -o $@



