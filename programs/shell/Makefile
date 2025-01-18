FILES=./build/shell.o
INCLUDES=-I../../lib/src
FLAGS=-g -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ../bin/shell.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../../lib/build/lib.elf

./build/blank.o: ./src/blank.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -g -c ./src/blank.c -o ./build/blank.o

./build/shell.o: ./shell.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -g -c ./shell.c -o ./build/shell.o

clean:
	rm -rf ${FILES}
