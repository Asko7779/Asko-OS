[BITS 16]
[ORG 0x7C00]

; Initial Setup in Real Mode
START:
    cli                 ; Disable interrupts
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00      ; Setup stack

    ; Load Kernel (using BIOS interrupt)
    mov ah, 0x02        ; Function to read sectors
    mov al, 1           ; Read 1 sector
    mov ch, 0           ; Cylinder 0
    mov cl, 2           ; Start reading from sector 2
    mov dh, 0           ; Head 0
    mov dl, 0x80        ; First hard disk
    int 0x13            ; BIOS Disk Interrupt

    ; Check for error in loading
    jc load_error       ; Jump if error

    ; Setup GDT and switch to protected mode
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1         ; Enable protected mode
    mov cr0, eax
    jmp 0x08:protected_mode

load_error:
    hlt                 ; Halt on error

[BITS 32]
protected_mode:
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000    ; Setup stack in protected mode

    ; Jump to kernel entry point
    jmp 0x10000

; Define the Global Descriptor Table (GDT)
gdt_start:
    dq 0x0                      ; Null descriptor
    dq 0x00CF9A000000FFFF       ; Code segment descriptor
    dq 0x00CF92000000FFFF       ; Data segment descriptor

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

gdt_end:

TIMES 510 - ($ - $$) DB 0
DW 0xAA55
