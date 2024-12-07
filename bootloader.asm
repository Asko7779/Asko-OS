[BITS 16]
[ORG 0x7C00]

START:
    cli                
    mov ax, 0x0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00      

    mov ah, 0x02        
    mov al, 1           
    mov ch, 0           
    mov cl, 2          

    mov dh, 0           
    mov dl, 0x80       
    int 0x13            

    jc load_error      

    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1          
    mov cr0, eax
    jmp 0x08:protected_mode

load_error:
    hlt                 
[BITS 32]
protected_mode:
    mov ax, 0x10         
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, 0x90000   

    jmp 0x10000

gdt_start:
    dq 0x0                      
    dq 0x00CF9A000000FFFF       
    dq 0x00CF92000000FFFF       

gdt_descriptor:
    dw gdt_end - gdt_start - 1  
    dd gdt_start                

gdt_end:

TIMES 510 - ($ - $$) DB 0
DW 0xAA55
