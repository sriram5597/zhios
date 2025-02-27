FILES = ./build/kernel.asm.o ./build/kernel.o ./build/terminal/terminal.o ./build/interrupts/interrupts.asm.o ./build/interrupts/interrupts.o ./build/io/io.asm.o ./build/gdt/gdt.asm.o ./build/gdt/gdt.o ./build/memory/heap/heap.o ./build/memory/heap/kheap.o ./build/memory/paging/paging.o ./build/memory/paging/paging.asm.o ./build/disk/disk.o ./build/fs/path.o ./build/disk/stream.o ./build/fs/file.o ./build/fs/fat/fat16.o ./build/task/task.asm.o ./build/task/tss.asm.o ./build/task/task.o ./build/task/process.o ./build/isr80h/isr80h.o ./build/isr80h/io.o ./build/isr80h/process.o ./build/isr80h/memory.o ./build/keyboard/keyboard.o ./build/keyboard/classic.o ./build/loaders/formats/elf.o ./build/loaders/formats/elfloader.o ./build/loaders/formats/binary.o ./build/loaders/loader.o ./build/vesa/vesa.o ./build/font/psf.o
LIB_FILES = ./lib/build/string.o ./lib/build/memory.o
INCLUDES = -I ./src -I ./lib/includes
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-functions -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc
FONTS = ./build/fonts/powerline.o

all: ./bin/boot.bin ./bin/kernel.bin ./bin/vesa.bin user_programs
	rm -rf ./bin/os.bin
	dd if=./bin/boot.bin >> ./bin/os.bin
	dd if=./bin/vesa.bin >> ./bin/os.bin
	dd if=./bin/kernel.bin bs=512 conv=sync >> ./bin/os.bin
	dd if=/dev/zero bs=1M count=100 >> ./bin/os.bin
	sudo mount -t vfat ./bin/os.bin /mnt/d
	sudo mkdir /mnt/d/root
	sudo mkdir /mnt/d/root/files
	sudo mkdir /mnt/d/bin
	sudo mkdir -p /mnt/d/etc/fonts
	sudo cp ./test.txt /mnt/d/root/files/test.txt
	sudo cp ./programs/bin/shell.elf /mnt/d/bin/shell.elf
	sudo cp ./programs/bin/blank.elf /mnt/d/bin/hello.elf
	sudo cp ./files/fonts/unifont.sfn /mnt/d/etc/fonts/unifont.sfn
	sudo umount /mnt/d

./bin/boot.bin: ./src/boot/boot.asm
	nasm -f bin -g -o bin/boot.bin src/boot/boot.asm

./build/vesa/vesa.o: ./src/vesa/vesa.c
	i686-elf-gcc ${INCLUDES} ${FLAGS} -std=gnu99 -c ./src/vesa/vesa.c -o ./build/vesa/vesa.o

./bin/vesa.bin: ./src/vesa/vesa.asm
	nasm -f bin -g -o ./bin/vesa.bin src/vesa/vesa.asm

./bin/kernel.bin: build_lib $(FILES) $(FONTS)
	i686-elf-ld -T ./src/linker.ld -o ./bin/kernel.elf -nostdlib $(FILES) $(FONTS) $(LIB_FILES)
	i686-elf-objcopy -O binary bin/kernel.elf bin/kernel.bin

./build/fonts/powerline.o: ./files/fonts/powerline.psf
	i686-elf-ld -r -b binary -o ./build/fonts/powerline.o ./files/fonts/powerline.psf

./build/fonts/solarize.o: ./files/fonts/solarize.psf
	i686-elf-ld -r -b binary -o ./build/fonts/solarize.o ./files/fonts/solarize.psf

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

./build/isr80h/io.o: ./src/isr80h/io.c
	i686-elf-gcc ${INCLUDES} -I./src/isr80h ${FLAGS} -std=gnu99 -c ./src/isr80h/io.c -o ./build/isr80h/io.o

./build/isr80h/memory.o: ./src/isr80h/memory.c
	i686-elf-gcc ${INCLUDES} -I./src/isr80h ${FLAGS} -std=gnu99 -c ./src/isr80h/memory.c -o ./build/isr80h/memory.o

./build/isr80h/process.o: ./src/isr80h/process.c
	i686-elf-gcc ${INCLUDES} -I./src/isr80h ${FLAGS} -std=gnu99 -c ./src/isr80h/process.c -o ./build/isr80h/process.o

./build/keyboard/keyboard.o: ./src/keyboard/keyboard.c
	i686-elf-gcc ${INCLUDES} -I./src/keyboard ${FLAGS} -std=gnu99 -c ./src/keyboard/keyboard.c -o ./build/keyboard/keyboard.o

./build/keyboard/classic.o: ./src/keyboard/classic.c
	i686-elf-gcc ${INCLUDES} -I./src/keyboard ${FLAGS} -std=gnu99 -c ./src/keyboard/classic.c -o ./build/keyboard/classic.o

./build/loaders/formats/elf.o: ./src/loaders/formats/elf.c
	i686-elf-gcc ${INCLUDES} -I./src/loaders/formats ${FLAGS} -std=gnu99 -c ./src/loaders/formats/elf.c -o ./build/loaders/formats/elf.o

./build/loaders/formats/elfloader.o: ./src/loaders/formats/elfloader.c
	i686-elf-gcc ${INCLUDES} -I./src/loaders/formats ${FLAGS} -std=gnu99 -c ./src/loaders/formats/elfloader.c -o ./build/loaders/formats/elfloader.o

./build/loaders/formats/binary.o: ./src/loaders/formats/binary.c
	i686-elf-gcc ${INCLUDES} -I./src/loaders/formats ${FLAGS} -std=gnu99 -c ./src/loaders/formats/binary.c -o ./build/loaders/formats/binary.o

./build/loaders/loader.o: ./src/loaders/loader.c
	i686-elf-gcc ${INCLUDES} -I./src/loaders ${FLAGS} -std=gnu99 -c ./src/loaders/loader.c -o ./build/loaders/loader.o

./build/font/psf.o: ./src/font/psf.c
	i686-elf-gcc ${INCLUDES} -I./src/font ${FLAGS} -std=gnu99 -c ./src/font/psf.c -o ./build/font/psf.o

build_lib:
	cd ./lib && ${MAKE} all

clean_lib:
	cd ./lib && ${MAKE} clean

user_programs:
	cd ./syslib && ${MAKE} all
	cd ./programs && ${MAKE} all

user_programs_clean:
	cd ./syslib && $(MAKE) clean
	cd ./programs && ${MAKE} clean

clean: user_programs_clean clean_lib
	rm -rf ./bin/*.bin
	rm -rf ./build/*.o
	rm -rf ./build/*/**.o
