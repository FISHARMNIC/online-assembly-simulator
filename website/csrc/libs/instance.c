static inline asm_instance_t new_asm_instance(asm_memory_t memory)
{
    return (asm_instance_t){
        .memory = memory,
    };
}

void memory_write_chunk(asm_instance_t *ins, uint16_t index, uint8_t size, uint32_t data)
{
    if (size == 32)
        *(uint32_t *)(ins->memory.buffer + index) = data;
    else if (size == 16)
        *(uint16_t *)(ins->memory.buffer + index) = data;
    else
        ins->memory.buffer[index] = data;
}

uint32_t memory_read_chunk(asm_instance_t *ins, uint16_t index, uint8_t size)
{
    // most significant first
    if (size == 32)
        return *((uint32_t *)(ins->memory.buffer + index));
    else if (size == 16)
        return *((uint16_t *)(ins->memory.buffer + index));
    else
        return ins->memory.buffer[index];
}

typedef struct
{
    char bits;
    uint32_t value;
    char isRegister;
    char *original;
} operand_t;

operand_t op_to_number(asm_instance_t *ins, char *str)
{
    operand_t ret = {.bits = 32, .value = 0, .original = str, .isRegister = 0};
    if (str[0] == '%')
    {
        register_read_vt o = register_read_quick(ins, str);
        // //printf("REGISTER(%s) bits:%i value:%i\n", str, o.bits, o.value);
        ret.isRegister = 1;
        ret.bits = o.bits;
        ret.value = o.value;
    }
    else if(str[0] <= 57 && str[0] >= 48)
    {
        int num = atoi(str);
        ret.value = num;
        if ((int)((char)num) == num)
        {
            ret.value = (char)num;
            ret.bits = 8;
        }
        else if ((int)((short)num) == num)
        {
            ret.value = (short)num;
            ret.bits = 16;
        }
    }
    return ret;
}

int asm_instance_run_single(asm_instance_t *ins, char *str)
{
    char **parsed = parser_parse(str);
    operand_t op2; // mne op1, op2
    operand_t op1;
    uint8_t op2ind = 0;
    uint8_t op1ind = 0;
    uint8_t offset = 2; // mov a b

    uint8_t len = 0;
    for (; parsed[len] != NULL; len++)
        ;

    // splits into operand 1 and 2
    if (len >= 2)
    {
        if (parsed[1][0] == '[')
        {
            op1 = op_to_number(ins, parsed[2]);
            op1ind = 1;
            offset = 3;
        }
        else
        {
            op1 = op_to_number(ins, parsed[1]);
        }
    }
    // //printf("LO %i %i %s\n", len, offset, parsed[offset - 1]);
    if (len > offset)
    {
        if (parsed[offset][0] == '[')
        {
            op2 = op_to_number(ins, parsed[offset + 1]);
            op2ind = 1;
        }
        else
        {
            // //printf("OP2: %s\n", parsed[offset]);
            op2 = op_to_number(ins, parsed[offset]);
        }
    }

    // finds postfix (b,w,l)
    uint8_t postfix = parsed[0][strlen(parsed[0]) - 1];
    if (postfix == 'b') // 8
    {
        postfix = 8;
        goto wr;
    }
    else if (postfix == 'w') // 16
    {
        postfix = 16;
        goto wr;
    }
    else if (postfix == 'l') // 32
    {
        postfix = 32;
        goto wr;
    }
    else
    {
        postfix = 0;
        goto ex;
    }
wr:
    parsed[0][strlen(parsed[0]) - 1] = 0;
ex:
    // main switch
    if (strcmp(parsed[0], "mov") == 0) // mov
    {
        if (op2ind)
        {
            // read bits from memory specified by the pointer requested by op1
            // mov %ax, [%edx] - read 32 bits pointed by edx
            op2.value = memory_read_chunk(ins, op2.value, op1.bits);
        }
        // //printf("%s\n", op2.original); fflush(stdout);
        if (op1ind)
        {
            // mov [%eax], %ebx - gave register size, no postfix needed
            if (op2.isRegister) // if we used a register
            {
                memory_write_chunk(ins, op1.value, op2.bits, op2.value);
            }
            // movb [%eax], 123 - need postfix
            else
            { // we need a postfix
                if (postfix == 0)
                    console("Error(0): suffix operator needed");
                memory_write_chunk(ins, op1.value, postfix, op2.value);
            }
        }
        else
        {
            // mov label, 123
            if (op1.isRegister)
            {
                register_write_quick(ins, op1.original, op2.value);
            }
            // mov %eax, 123
            else
            {
                //printf("Error(1): suffix operator needed\n");
                memory_write_chunk(ins, op1.value, postfix, op2.value);
            }
        }
    }
    else if (parsed[0][strlen(parsed[0]) - 1] == ':') // label
    {
        if(op1.original[0] == '.')
        {
            if(strcmp(op1.original, ".asciz") == 0)
            {
                // removes quotes from "word" and copies it to allocated memory
                strcpy((char*)asm_memory_alloc(ins, strlen(op2.original)), strtok(op2.original + 1, "\""));
            }
            else if(strcmp(op1.original, ".int") == 0)
            {
                *((uint32_t*)asm_memory_alloc(ins, sizeof(uint32_t))) = op2.value;
            }
            else if(strcmp(op1.original, ".short") == 0)
            {
                *((uint16_t*)asm_memory_alloc(ins, sizeof(uint16_t))) = (uint16_t)op2.value;
            }
            else if(strcmp(op1.original, ".byte") == 0)
            {
                *((uint8_t*)asm_memory_alloc(ins, sizeof(uint8_t))) = (uint8_t)op2.value;
            }
        }
    }
    else if (strcmp(parsed[0], "cmp") == 0) // cmp
    {
        int32_t res = 0;
        if(op1ind && op2ind)
            console("Error: too many memory references\n");

        char swap = 0;
        if(op2ind)
        {
            // swap
            operand_t temp = op1;
            op1 = op2;
            op2 = temp;
            swap = 0;
        }

        if (op1ind)
        {
            // cmp [%eax], %ebx
            if (op2.isRegister)
            {
                // use bits of op2 to specify size to read form memory
                res = memory_read_chunk(ins, op1.value, op2.bits) - op2.value;
                goto setFlags;
            }
            // cmpb [%eax], 123
            else
            {
                if (postfix == 0)
                    console("Error(3): suffix operator needed");
                // use postfix to specify size
                op1.value = memory_read_chunk(ins, op1.value, postfix);
            }
        }
        // cmp %eax, %ebx
        if(op1.bits != op2.bits && (op1.isRegister && op2.isRegister)) 
            console("Warning: comparing uneven sizes\n");
        res = (int32_t)op1.value - (int32_t)op2.value;

        setFlags:
        if(swap) res = -res;
        ins->memory.flags.zero = res == 0;
        ////printf("DEBUG COMPARE OUT: %i\n", res);
    }
    else if (parsed[0][0] == 'j') // jump
    {
        switch (parsed[0][1])
        {
        case 'm':
        {
            return op1.value;
            break;
        };
        case 'n':
        {
            if (parsed[0][2] == 'e')
            {
                // jne addr;
                //console("hello");
                //console_char();
                if (ins->memory.flags.zero == 0)
                    return op1.value;
            }
            break;
        };
        case 'e': 
        {
            if (ins->memory.flags.zero == 1) return op1.value;
            break;
        }
        }
    }
    else if (strcmp(parsed[0], "inc") == 0 || strcmp(parsed[0], "dec") == 0) // inc
    {
        char change = (strcmp(parsed[0], "inc") == 0) * 2 - 1; // 1 when inc, -1 when dec
        // incb [%eax]
        if(op1ind)
        {
            if(postfix == 0) console("Error: postfix needed\n");
            memory_write_chunk(ins, op1.value, postfix, memory_read_chunk(ins, op1.value, postfix) + change);
        } 
        // inc %eax
        else 
        {
            register_write_quick(ins, op1.original, op1.value + change);
        }
    }
    else {
        //printf("UNKNOWN\n");
    }
    return -1;
}

int asm_instance_run(asm_instance_t *ins, char **str, uint16_t len)
{
    int output = -1;
    for (int i = 0; i < len; i++)
    {
        output = asm_instance_run_single(ins, str[i]);
        if (output != -1)
            i = output - 1;
    }
    return 0;
}
