CC = gcc
FRAMES := frames

main: main.c 
	$(CC) -Wall -Wextra -I../../src main.c -o main -lm -flto=auto -O3 

debug:
	$(CC) -Wall -Wextra -Werror -I../../src main.c -o main -lm -ggdb

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
