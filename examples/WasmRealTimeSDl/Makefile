CC     := emcc

WARN    :=  -Wall -Wextra 
OPT     :=  -O3 -flto=auto -fomit-frame-pointer 
INCLUDE :=  -I../../src/
LIBS 	:=  -s USE_SDL=2
FLAGS   :=  -s ALLOW_MEMORY_GROWTH=0 \
			-s INITIAL_MEMORY=268435456 \
			-s ASSERTIONS=1

ASSET_DIR := ../objs/catgirl


RELEASE := $(WARN) $(OPT) $(INCLUDE) $(LIBS) $(FLAGS)

main: main.c 
		mkdir -p build
		emcc $(RELEASE) main.c -o build/index.html \
			--preload-file $(ASSET_DIR)
clean: 
	rm -rf build
	
.PHONY: clean 
