org 0x0
bits 16

%define ENDL 0x0D, 0x0A



; Code goes here


start:
    ;print message
    mov si, msg_hello
    call puts

.halt:
    cli
    hlt


;Prints a string to the screen
;Params:
;    - ds:si points to the string 
puts:
;   save registers we will modify
    push si
    push ax

.loop:
    lodsb           ;loads next character in al
    or al, al       ;verify if next character is null?
    jz .done

    mov ah, 0x0e    ;call bios interrupt
    int 0x10

    jmp .loop

.done:
    pop ax
    pop si
    ret

msg_hello: db 'Hello world from KERNEL!', ENDL, 0
