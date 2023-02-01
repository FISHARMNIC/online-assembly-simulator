#include "wasm.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>

#include "header.h"

asm_memory_t memory;
asm_instance_t instance;
char js_string[100];
char *js_string_pointer;
void asm_init()
{
    memory = new_asm_memory(100);
    instance = new_asm_instance(memory);
    js_pass_free_buffer(js_string);
    js_pass_virtual_memory((char*)memory.buffer);
    js_string_pointer = js_string;
}

void asm_memory_reset()
{
    memset(memory.buffer, 0, 100);
}

int asm_run()
{
    int out = asm_instance_run_single(&instance, js_string_pointer);
    js_return_value(out);
    return 0;
}