FILES=./build/blank.o
INCLUDES=-I../../syslib/includes -I../../lib/includes
LIB_FILES=../../syslib/build/syslib.elf ../../lib/build/lib.o
FLAGS=-g -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ../bin/blank.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ${LIB_FILES}

./build/blank.o: ./blank.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -g -c ./blank.c -o ./build/blank.o

clean:
	rm -rf ${FILES}
