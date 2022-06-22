BITS    16
ORG     0x7c00

jmp start

start:
        mov ax,cs
        mov ds,ax
        mov si,msg

        call print

print:
        push ax
        cld
next:
        mov al,[si]
        cmp al,0
        je waitEnter
        call printchar
        inc si
        jmp next

printchar:
        mov ah,0x0e
        int 0x10
        ret

waitEnter:
    mov ah, 00h
    int 16h

    cmp al, 0dh ;Enter code
    je start
    jne waitEnter

msg: db "Press ENTER to continue", 13, 10, 0
