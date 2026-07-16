CC     := gcc

CPU     := -march=native
WARN    := -Wall -Wextra 
OPT     := -O3 -flto=auto -fomit-frame-pointer -fno-plt
INCLUDE := -I../../src/
LIBS 	:= -lm -lSDL3

RELEASE := $(WARN) $(OPT) $(CPU) $(INCLUDE)

main: main.c 
	$(CC) $(RELEASE) main.c -o main $(LIBS)

debug:
	$(CC) -Wall -Wextra -Werror $(INCLUDE) main.c -o main $(LIBS) -ggdb

clean: 
	rm -f ./main
	
.PHONY: clean debug
