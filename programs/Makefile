all:
	nasm -g -f elf ./blank.asm -o ./build/blank.o
	i686-elf-gcc -g -T ./linker.ld -o ./bin/blank.bin -ffreestanding -O0 -nostdlib -fpic -g ./build/blank.o

clean:
	rm -r build
	mkdir build
