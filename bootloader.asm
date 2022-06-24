[ORG 7c00h]         ; code starts at 7c00h

xor ax, ax          ; ax = 0
mov ds, ax          ; ds = 0
cld

mov si, msg         ; si contains msg

;iterate each character and print
next:
    mov al,[si]
    cmp al,0
    je waitEnter    ; jmp if character is \0
    call printchar  ; else print character
    inc si          ; increase index
    jmp next

printchar:
    mov ah,0x0e     ; print function
    int 0x10
    ret

waitEnter:
    mov ah, 00h     ; get keystate function
    int 16h

    cmp al, 0dh     ; Enter code
    jne waitEnter   ; if Enter not pressed, loop


; read sector 2 from disk to memory
mov ah, 2h          ; int13h function 2
mov al, 1           ; read 1 sectors
mov ch, 0           ; cylinder number 0
mov cl, 2           ; sector number 2 (starts from 1)
mov dh, 0           ; head number 0
xor bx, bx
mov es, bx          ; es = 0
mov bx, 7e00h       ; 512bytes from origin address 7c00h
int 13h
jmp 7e00h           ; jump to the next sector

msg: db "Press ENTER to continue . . .", 0

; to fill this sector and make it bootable:
times 510-($-$$) db 0
dw 0AA55h

; read sector 3 contain old MBR to memory
mov ah, 2h		; int13h function 2
mov al, 1		; read 1 sectors
mov ch, 0		; cylinder number 0
mov cl, 3       ; sector number 2 (starts from 1)
mov dh, 0       ; head number 0
xor bx, bx
mov es, bx      ; es = 0
mov bx, 7c00h   ; place old MBR to 7c00h
int 13h
jmp 7c00h       ; jump to old MBR

times 1024-($-$$) db 0
