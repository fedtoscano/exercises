# Tappa 1 — lexer di mango
#
# make          build
# make test     build + test suite
# make clean    rimuove binario, oggetti e file di dipendenza
#
# I sanitizer sono accesi di default. Per spegnerli:  make SAN=

CC      := gcc
SAN     := -fsanitize=address,undefined

CFLAGS  := -std=gnu17 -D_DEFAULT_SOURCE \
           -Wall -Wextra -Wpedantic -Wconversion \
           -g -MMD -MP $(SAN)
LDFLAGS := $(SAN)

TARGET  := mango
SRC     := $(wildcard *.c)
OBJ     := $(SRC:.c=.o)
DEP     := $(SRC:.c=.d)

.PHONY: all test clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TARGET)
	python3 test_lexer.py

clean:
	rm -f $(TARGET) $(OBJ) $(DEP)

-include $(DEP)
