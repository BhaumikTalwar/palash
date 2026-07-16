CC     := gcc
FRAMES := frames

CPU     := -march=native
WARN    := -Wall -Wextra 
OPT     := -O3 -flto=auto -fomit-frame-pointer -fno-plt
INCLUDE := -I../../src/
LIBS 	:= -lm

RELEASE := $(WARN) $(OPT) $(CPU) $(INCLUDE)

main: main.c 
	$(CC) $(RELEASE) main.c -o main $(LIBS)

debug:
	$(CC) -Wall -Wextra -Werror $(INCLUDE) main.c -o main $(LIBS) -ggdb

gen: clean main
	mkdir -p $(FRAMES)
	./main
	ffmpeg -framerate 60 -i ./$(FRAMES)/output-%06d.ppm -c:v libx264 output.mp4

gen-run: gen
	mpv ./output.mp4

clean: 
	rm -rf ./$(FRAMES)
	rm -f output.mp4
	rm -f ./main
	
.PHONY: gen gen-run clean debug
