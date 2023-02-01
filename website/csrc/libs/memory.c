static inline asm_memory_t new_asm_memory(uint16_t size)
{
    uint8_t *b = (uint8_t *)calloc(size, 1);
    return (asm_memory_t){
        .size = size,
        .buffer = b,
        .dataoffset = b,
    };
}

static inline void asm_memory_clear(asm_memory_t mem)
{
    memset(mem.buffer, 0, mem.size);
}

uint8_t *asm_memory_alloc(asm_instance_t *ins, uint16_t size)
{
    ins->memory.dataoffset = (uint8_t *)((uint64_t)ins->memory.dataoffset + (uint64_t)size);
    return (uint8_t *)((uint64_t)ins->memory.dataoffset - (uint64_t)size);
}