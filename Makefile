CC      ?= cc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2
LDFLAGS ?=

BIN_DIR     := bin
SRC_DIR     := src
CONSOLE_SRC := $(SRC_DIR)/gol_console.c
X11_SRC     := $(SRC_DIR)/gol_x11.c

CONSOLE_BIN := $(BIN_DIR)/gol_console
X11_BIN     := $(BIN_DIR)/gol_x11

.PHONY: all console x11 clean

all: console x11

console: $(CONSOLE_BIN)

x11: $(X11_BIN)

$(BIN_DIR):
	mkdir -p $@

$(CONSOLE_BIN): $(CONSOLE_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

$(X11_BIN): $(X11_SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -lX11 $< -o $@

clean:
	rm -f $(CONSOLE_BIN) $(X11_BIN)
