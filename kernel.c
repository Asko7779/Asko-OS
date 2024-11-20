#include <stdint.h>

#define VIDEO_MEMORY 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F  

static uint16_t* const VIDEO = (uint16_t*) VIDEO_MEMORY;
static uint16_t cursor_pos = 0;

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

void kernel_main();
void clear_screen();
void print_char(char c);
void print_string(const char* str);
void show_system_info();
void memory_dump();
uint8_t read_keyboard();
void print_key();
char scancode_to_char(uint8_t scancode);
void uint8_to_hex(uint8_t value, char* output);
static inline uint8_t inb(uint16_t port);

void kernel_main() {
    clear_screen();
    print_string("[AOS] AskoOS Kernel\n");
    print_string("Press any key to continue...\n");

    while (1) {
        print_key();
    }
}

void clear_screen() {
    for (int i = 0; i < MAX_ROWS * MAX_COLS; ++i) {
        VIDEO[i] = 0x0F00; 
    }
    cursor_pos = 0;
}

void print_char(char c) {
    if (c == '\n') {
        cursor_pos += (MAX_COLS - cursor_pos % MAX_COLS);
    } else {
        VIDEO[cursor_pos] = (WHITE_ON_BLACK << 8) | c;
        cursor_pos++;
    }
}

void print_string(const char* str) {
    while (*str) {
        print_char(*str++);
    }
}

void show_system_info() {
    print_string("[OS] AskoOS - Beta Version\n");
    print_string("[RUNNING] - Protected Mode.\n");
    print_string("[ARCHITECTURE] - 64-bit\n");
}

void memory_dump() {
    print_string("Memory Dump (first 128 bytes):\n");
    uint8_t* mem = (uint8_t*) 0x00000000;  
    char buffer[4];

    for (int i = 0; i < 128; i++) {
        uint8_to_hex(mem[i], buffer);
        print_string(buffer);
        print_string(" ");
        if ((i + 1) % 16 == 0) print_string("\n");
    }
}

void uint8_to_hex(uint8_t value, char* output) {
    const char* hex_digits = "0123456789ABCDEF";
    output[0] = hex_digits[(value >> 4) & 0xF];
    output[1] = hex_digits[value & 0xF];
    output[2] = '\0';
}

uint8_t read_keyboard() {
    while ((inb(KEYBOARD_STATUS_PORT) & 0x01) == 0);
    return inb(KEYBOARD_PORT);
}

void print_key() {
    uint8_t scancode = read_keyboard();
    char key = scancode_to_char(scancode);
    print_char(key);
}

char scancode_to_char(uint8_t scancode) {
    static char scancode_map[] = {
        [0x1E] = 'a', [0x30] = 'b', [0x2E] = 'c', [0x20] = 'd',
        [0x12] = 'e', [0x21] = 'f', [0x22] = 'g', [0x23] = 'h',
        [0x17] = 'i', [0x24] = 'j', [0x25] = 'k', [0x26] = 'l',
        [0x32] = 'm', [0x31] = 'n', [0x18] = 'o', [0x19] = 'p',
        [0x10] = 'q', [0x13] = 'r', [0x1F] = 's', [0x14] = 't',
        [0x16] = 'u', [0x2F] = 'v', [0x11] = 'w', [0x2D] = 'x',
        [0x15] = 'y', [0x2C] = 'z', [0x39] = ' ',  
    };

    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }
    return 0;
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0" : "=a" (value) : "dN" (port));
    return value;
}