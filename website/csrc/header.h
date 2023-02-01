typedef struct
{
    uint8_t zero : 1;
    uint8_t sign : 1;
    uint8_t overflow : 1;
} flags_t;

typedef struct
{
    uint16_t size;
    uint8_t *buffer;
    uint8_t *dataoffset;
    flags_t flags;
} asm_memory_t;

typedef union
{
    uint8_t b8[2];
    uint16_t b16;
    uint32_t b32;
} asm_register_t;

typedef struct
{
    asm_register_t a;
    asm_register_t b;
    asm_register_t c;
    asm_register_t d;
} register_struct;

typedef struct
{
    char bits;
    uint32_t *value;
} register_read_prt;

typedef struct
{
    char bits;
    uint32_t value;
} register_read_vt;

typedef struct
{
    asm_memory_t memory;
    register_struct registers;
    char code[20][20];
} asm_instance_t;

#include "libs/memory.c"
#include "libs/registers.c"
#include "libs/parser.c"
#include "libs/instance.c"