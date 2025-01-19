FILES= ./build/string.o ./build/memory.o
FLAGS=-g -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
INCLUDES=-I ./src -I ./includes

all: $(FILES)
	i686-elf-ld -g --relocatable ${FILES} -o ./build/lib.o

./build/string.o: ./src/string.c
	i686-elf-gcc ${INCLUDES} ${FLAGS} -std=gnu99 -c ./src/string.c -o ./build/string.o

./build/memory.o: ./src/memory.c
	i686-elf-gcc ${INCLUDES} ${FLAGS} -std=gnu99 -c ./src/memory.c -o ./build/memory.o

clean:
	rm ${FILES}
	rm ./build/lib.o
