FILES=./build/entry.o ./build/stdio.o ./build/stdlib.o ./build/stdlib.asm.o ./build/stdio.asm.o ./build/proc.o ./build/proc.asm.o ./build/argparser.o
INCLUDES=-I./src -I ./includes -I ../lib/includes
FLAGS=-g -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ${FILES}
	i686-elf-ld -m elf_i386 -relocatable ${FILES} -o ./build/syslib.elf

./build/entry.o: ./src/entry.asm
	nasm -g -f elf ./src/entry.asm -o ./build/entry.o

./build/stdio.asm.o: ./src/stdio/stdio.asm
	nasm -f elf ./src/stdio/stdio.asm -o ./build/stdio.asm.o

./build/stdio.o: ./src/stdio/stdio.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdio/stdio.c -o ./build/stdio.o

./build/stdlib.o: ./src/stdlib/stdlib.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/stdlib/stdlib.c -o ./build/stdlib.o

./build/stdlib.asm.o: ./src/stdlib/stdlib.asm
	nasm -f elf ./src/stdlib/stdlib.asm -o ./build/stdlib.asm.o

./build/proc.o: ./src/proc/proc.c
	i686-elf-gcc ${INCLUDES} $(FLAGS) -std=gnu99 -c ./src/proc/proc.c -o ./build/proc.o

./build/proc.asm.o: ./src/proc/proc.asm
	nasm -f elf ./src/proc/proc.asm -o ./build/proc.asm.o

./build/argparser.o: ./src/command/argparser.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/command/argparser.c -o ./build/argparser.o

clean:
	rm -rf ${FILES}
	rm ./build/syslib.elf
