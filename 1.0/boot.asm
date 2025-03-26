bits 32 ;32 BITS
section .text
    ;multiboot spec
    align 4
    dd 0x1BADB002 ;magic
    dd 0x00 ;flags
    dd - (0x1BADB002 + 0x00) ;checksum

global start
extern kmain
global inb
global outb
global reboot
global shutdown
; функция для чтения из порта
inb:
    push ebp
    mov ebp, esp
    xor eax, eax
    mov edx, [ebp+8]
    in al, dx
    leave
    ret
;функция для записи в порт
outb:
    push ebp
    mov ebp, esp
    mov edx, [ebp+8]
    mov eax, [ebp+12]
    out dx, al
    leave
    ret
;функция для перезагрузки
reboot:
    ;перезагрузка через переход на вектор сброса биоса
    cli
    mov eax,0xFFFF
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    jmp 0xFFFF:0x0000
;функция для выключения компьютера (НЕ ГОТОВО НИФИГА НЕ РАБОТАЕТ)
shutdown:
    cli
    hlt
    ret

start:
    cli ;clear interrupts
    mov esp, stack_space ;set stack pointer
    call kmain ;call kernel main
    hlt ;halt the cpu

section .bss
    resb 8192 ;reserve 8kb for stack
stack_space:
