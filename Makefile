CC := gcc
CFLAGS := -Wall -Wextra
LFLAGS := $(shell pkgconf --cflags --libs sdl2 sdl2_ttf)
# LFLAGS := -lsdl2 -lsdl2_ttf -DSDL_MAIN_HANDLED
SRC_DIR := src
BIN_DIR := bin
SRC := $(SRC_DIR)/swelcomer.c
BIN := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%, $(SRC))

all: $(BIN)

$(BIN_DIR)/%: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< $(LFLAGS) -o $@

clean:
	rm -rf $(BIN_DIR)
