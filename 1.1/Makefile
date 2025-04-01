CC      = gcc
CFLAGS  = -m32 -fno-stack-protector
LD      = ld
NASM    = nasm
ISO     = netwide11.iso

OBJFILES = \
    boot.o \
    kernel.o \
    screen.o \
    keyboard.o \
    commands.o \
	vfs.o \
	kmem.o \
	kstring.o 

kernel: $(OBJFILES)
	$(LD) -m elf_i386 -T link.ld -o $@ $^ -z execstack

boot.o: boot.asm
	$(NASM) -f elf32 $< -o $@

kernel.o: kernel.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

screen.o: include/screen.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

keyboard.o: include/keyboard.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

commands.o: include/commands.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

vfs.o: vfs/vfs.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

kmem.o: include/kmem.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

kstring.o: include/kstring.c include/kernel.h
	$(CC) $(CFLAGS) -c $< -o $@


iso: kernel
	mkdir -p iso/boot/grub
	cp kernel iso/boot/kernel.bin

	echo "menuentry 'Netwide 1.1' {" >> iso/boot/grub/grub.cfg
	echo "    multiboot /boot/kernel.bin" >> iso/boot/grub/grub.cfg
	echo "}" >> iso/boot/grub/grub.cfg

	grub-mkrescue -o $(ISO) iso

run: iso
	qemu-system-i386 -cdrom $(ISO) -vga std

clean:
	rm -f $(OBJFILES) kernel
	rm -f $(ISO)
	rm -rf iso