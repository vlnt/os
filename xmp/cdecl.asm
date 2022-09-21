; save content of eax, ecx, edx if important
    push y
    push x
    call _lengthSq
    add 4, sp

_lengthSq:
    push bp
    mov bp, sp

    sub sp, 2
    mov ax, [bp + 4]
    mul ax
    mov [bp - 2], ax

    mov ax, [bp + 6]
    mul ax
    add [bp - 2], ax

    mov ax, [bp - 2]

    mov sp, bp
    pop bp
    ret 

