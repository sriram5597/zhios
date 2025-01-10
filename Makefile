FILES = ./build/kernel.asm.o ./build/kernel.o ./build/terminal/terminal.o ./build/memory/memory.o ./build/interrupts/interrupts.asm.o ./build/interrupts/interrupts.o ./build/io/io.asm.o ./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/string/string.o ./build/fs/path.o ./build/disk/stream.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/task/task.asm.o ./build/task/tss.asm.o ./build/task/task.o ./build/task/process.o ./build/isr80h/isr80h.o

INCLUDES = -I ./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

all: ./bin/boot.bin ./bin/kernel.bin user_programs
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin bs=512 conv=sync >> ./bin/os.bin
	dd if=/dev/zero bs=1048576 count=16 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d
	# sudo cp ./test.txt /mnt/d
	sudo mkdir /mnt/d/root
	sudo mkdir /mnt/d/root/files
	sudo mkdir /mnt/d/bin
	sudo cp ./test.txt /mnt/d/root/files/test.txt
	sudo cp ./programs/bin/blank.bin /mnt/d/bin/blank.bin
	sudo umount /mnt/d

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin -g -o bin/boot.bin src/boot/boot.asm

./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T ./src/linker.ld -o ./bin/kernel.bin -ffreestanding -O0 -nostdlib ./build/kernelfull.o

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g -o ./build/kernel.asm.o ./src/kernel.asm

./build/interrupts/interrupts.asm.o: ./src/interrupts/interrupts.asm
	nasm -f elf -g -o ./build/interrupts/interrupts.asm.o ./src/interrupts/interrupts.asm

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g -o ./build/io/io.asm.o ./src/io/io.asm

./build/kernel.o: ./src/kernel.c
	i686-elf-gcc ${INCLUDES} ${FLAGS} -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/terminal/terminal.o: ./src/terminal/terminal.c
	i686-elf-gcc ${INCLUDES} -I./src/terminal ${FLAGS} -std=gnu99 -c ./src/terminal/terminal.c -o ./build/terminal/terminal.o


./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc ${INCLUDES} -I./src/memory ${FLAGS} -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/memory/heap/heap.o: ./src/memory/heap/heap.c
	i686-elf-gcc ${INCLUDES} -I./src/memory/heap ${FLAGS} -std=gnu99 -c ./src/memory/heap/heap.c -o ./build/memory/heap/heap.o

./build/memory/paging/paging.o: ./src/memory/paging/paging.c
	i686-elf-gcc ${INCLUDES} -I./src/memory/heap ${FLAGS} -std=gnu99 -c ./src/memory/paging/paging.c -o ./build/memory/paging/paging.o

./build/memory/paging/paging.asm.o: ./src/memory/paging/paging.asm
	nasm -f elf -g -o ./build/memory/paging/paging.asm.o ./src/memory/paging/paging.asm

./build/memory/heap/kheap.o: ./src/memory/heap/kheap.c
	i686-elf-gcc ${INCLUDES} -I./src/memory/heap ${FLAGS} -std=gnu99 -c ./src/memory/heap/kheap.c -o ./build/memory/heap/kheap.o


./build/interrupts/interrupts.o: ./src/interrupts/interrupts.c
	i686-elf-gcc ${INCLUDES} -I./src/interrupts ${FLAGS} -std=gnu99 -c ./src/interrupts/interrupts.c -o ./build/interrupts/interrupts.o


./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc ${INCLUDES} -I./src/disk ${FLAGS} -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o


./build/disk/stream.o: ./src/disk/stream.c
	i686-elf-gcc ${INCLUDES} -I./src/disk ${FLAGS} -std=gnu99 -c ./src/disk/stream.c -o ./build/disk/stream.o

./build/string/string.o: ./src/string/string.c
	i686-elf-gcc ${INCLUDES} -I./src/string ${FLAGS} -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/fs/path.o: ./src/fs/path.c
	i686-elf-gcc ${INCLUDES} -I./src/fs ${FLAGS} -std=gnu99 -c ./src/fs/path.c -o ./build/fs/path.o

./build/fs/file.o: ./src/fs/file.c
	i686-elf-gcc ${INCLUDES} -I./src/fs ${FLAGS} -std=gnu99 -c ./src/fs/file.c -o ./build/fs/file.o

./build/fs/fat/fat16.o: ./src/fs/fat/fat16.c
	i686-elf-gcc ${INCLUDES} -I./src/fs -I ./src/fs/fat ${FLAGS} -std=gnu99 -c ./src/fs/fat/fat16.c -o ./build/fs/fat/fat16.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g -o ./build/gdt/gdt.asm.o ./src/gdt/gdt.asm

./build/task/tss.asm.o: ./src/task/tss.asm
	nasm -f elf -gdwarf -o ./build/task/tss.asm.o ./src/task/tss.asm

./build/task/task.asm.o: ./src/task/task.asm
	nasm -f elf32 -gdwarf -o ./build/task/task.asm.o ./src/task/task.asm
	
./build/gdt/gdt.o: ./src/gdt/gdt.c
	i686-elf-gcc ${INCLUDES} -I./src/gdt ${FLAGS} -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/task/task.o: ./src/task/task.c
	i686-elf-gcc ${INCLUDES} -I./src/task ${FLAGS} -std=gnu99 -c ./src/task/task.c -o ./build/task/task.o

./build/task/process.o: ./src/task/process.c
	i686-elf-gcc ${INCLUDES} -I./src/task ${FLAGS} -std=gnu99 -c ./src/task/process.c -o ./build/task/process.o

./build/isr80h/isr80h.o: ./src/isr80h/isr80h.c
	i686-elf-gcc ${INCLUDES} -I./src/isr80h ${FLAGS} -std=gnu99 -c ./src/isr80h/isr80h.c -o ./build/isr80h/isr80h.o

user_programs:
	cd ./programs && ${MAKE} all

user_programs_clean:
	cd ./programs && ${MAKE} clean

clean: user_programs_clean
	rm -rf ./bin/*.bin
	rm -rf ./build/*.o
	rm -rf ./build/*/**.o
