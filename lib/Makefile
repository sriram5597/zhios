FILES=./build/entry.o ./build/console.o ./build/stdlib.o ./build/stdlib.asm.o
INCLUDES=-I./stdlib
FLAGS=-g

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./build/lib.elf

./build/entry.o: ./src/entry.asm
	nasm -f elf ./src/entry.asm -o ./build/entry.o

./build/console.o: ./src/console/console.asm
	nasm -f elf ./src/console/console.asm -o ./build/console.o

./build/stdlib.o: ./src/stdlib/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdlib/stdlib.c -o ./build/stdlib.o

./build/stdlib.asm.o: ./src/stdlib/stdlib.asm
	nasm -f elf ./src/stdlib/stdlib.asm -o ./build/stdlib.asm.o

clean:
	rm -rf ${FILES}
	rm ./build/lib.elf
