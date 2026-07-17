CC      := gcc
AR      := gcc-ar

PIC     := -fPIC
LIBS    := -lm
BUILD   := build

CPU     := -march=native
WARN    := -Wall -Wextra 
OPT     := -O3 -flto=auto -fomit-frame-pointer -fno-plt

RELEASE := $(WARN) $(OPT) $(CPU)

all: build-dir release-static release-shared

build-dir:
	mkdir -p $(BUILD)

release-static: build
	$(CC) $(RELEASE) -c src/palash.c -o $(BUILD)/palash.o

	$(AR) rcs \
	$(BUILD)/libpalash.a \
	$(BUILD)/palash.o

	rm -rf $(BUILD)/palash.o


release-shared: build
	$(CC) $(RELEASE) $(PIC) \
	-c src/palash.c \
	-o $(BUILD)/palash.pic.o

	$(CC) \
	-shared \
	-flto=auto \
	$(BUILD)/palash.pic.o \
	-o $(BUILD)/libpalash.so \
	$(LIBS)

	rm -rf $(BUILD)/palash.pic.o

clean:
	rm -rf $(BUILD)

.PHONY: all build build-dir clean release-static release-shared
