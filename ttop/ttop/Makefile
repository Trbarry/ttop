CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
SRC_DIR = src
OBJ_DIR = obj
TARGET = ttop

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OS_DIR = linux
endif
ifeq ($(UNAME_S),FreeBSD)
    OS_DIR = freebsd
endif

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/daemon.c $(SRC_DIR)/reporter.c $(SRC_DIR)/webhook.c \
       $(SRC_DIR)/common/filesystem.c \
       $(wildcard $(SRC_DIR)/$(OS_DIR)/*.c)

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/
	if [ -f ttop.service ]; then cp ttop.service /etc/systemd/system/ 2>/dev/null || sudo cp ttop.service /etc/systemd/system/; fi
	if [ -f ttop.conf ]; then install -m 600 ttop.conf /etc/ttop.conf 2>/dev/null || sudo install -m 600 ttop.conf /etc/ttop.conf; fi

uninstall:
	rm -f /usr/local/bin/$(TARGET)
	rm -f /etc/systemd/system/ttop.service
	rm -f /etc/ttop.conf

.PHONY: all clean install uninstall
