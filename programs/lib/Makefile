FILES=./build/start.o ./build/utils.o
INCLUDES=

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./build/stdlib.elf

./build/start.o: ./stdlib/start.asm
	nasm -f elf ./stdlib/start.asm -o ./build/start.o

./build/utils.o: ./stdlib/utils.asm
	nasm -f elf ./stdlib/utils.asm -o ./build/utils.o

clean:
	rm -rf ${FILES}
	rm ./build/stdlib.elf
