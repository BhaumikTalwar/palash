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
	$(CC) $(RELEASE) -c src/renderer.c -o $(BUILD)/renderer.o

	$(AR) rcs \
	$(BUILD)/librenderer.a \
	$(BUILD)/renderer.o

	rm -rf $(BUILD)/renderer.o


release-shared: build
	$(CC) $(RELEASE) $(PIC) \
	-c src/renderer.c \
	-o $(BUILD)/renderer.pic.o

	$(CC) \
	-shared \
	-flto=auto \
	$(BUILD)/renderer.pic.o \
	-o $(BUILD)/librenderer.so \
	$(LIBS)

	rm -rf $(BUILD)/renderer.pic.o

clean:
	rm -rf $(BUILD)

.PHONY: all build build-dir clean release-static release-shared
