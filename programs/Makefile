FILES=./build/blank.o
INCLUDES=-I../lib/src

all: ${FILES}
	i686-elf-gcc -g -T ./linker.ld -o ./bin/blank.elf -ffreestanding -O0 -nostdlib -fpic -g ${FILES} ../lib/build/lib.elf

./build/blank.o: ./src/blank.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -g -c ./src/blank.c -o ./build/blank.o

clean:
	rm -rf ${FILES}
	rm ./bin/blank.elf