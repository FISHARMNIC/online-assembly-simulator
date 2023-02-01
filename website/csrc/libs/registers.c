register_read_prt register_read(asm_instance_t *ins, char *str)
{
    //%eax
    register_read_prt ret;
    if (str[1] == 'e')
    {
        // printf("--t32:%c:%i--\n", str[2], *((uint32_t*) &ins->registers.a.b32));
        ret.bits = 32;
        switch (str[2])
        {
        case 'a':
        {
            ret.value = (uint32_t *)&(ins->registers.a.b32);
            return ret;
            break;
        };
        case 'b':
        {
            ret.value = (uint32_t *)&(ins->registers.b.b32);
            return ret;
            break;
        };
        case 'c':
        {
            ret.value = (uint32_t *)&(ins->registers.c.b32);
            return ret;
            break;
        };
        case 'd':
        {
            ret.value = (uint32_t *)&(ins->registers.d.b32);
            return ret;
            break;
        };
        }
    }
    else if (str[2] == 'x')
    {
        ret.bits = 16;
        switch (str[1])
        {
        case 'a':
        {
            ret.value = (uint32_t *)&(ins->registers.a.b16);
            return ret;
            break;
        };
        case 'b':
        {
            ret.value = (uint32_t *)&(ins->registers.b.b16);
            return ret;
            break;
        };
        case 'c':
        {
            ret.value = (uint32_t *)&(ins->registers.c.b16);
            return ret;
            break;
        };
        case 'd':
        {
            ret.value = (uint32_t *)&(ins->registers.d.b16);
            return ret;
            break;
        };
        }
    }
    else if (str[2] == 'l')
    {
        ret.bits = 8;
        switch (str[1])
        {
        case 'a':
        {
            ret.value = (uint32_t *)&(ins->registers.a.b8[0]);
            return ret;
            break;
        };
        case 'b':
        {
            ret.value = (uint32_t *)&(ins->registers.b.b8[0]);
            return ret;
            break;
        };
        case 'c':
        {
            ret.value = (uint32_t *)&(ins->registers.c.b8[0]);
            return ret;
            break;
        };
        case 'd':
        {
            ret.value = (uint32_t *)&(ins->registers.d.b8[0]);
            return ret;
            break;
        };
        }
    }
    else if (str[2] == 'h')
    {
        ret.bits = 8;
        switch (str[1])
        {
        case 'a':
        {
            ret.value = (uint32_t *)&(ins->registers.a.b8[1]);
            return ret;
            break;
        };
        case 'b':
        {
            ret.value = (uint32_t *)&(ins->registers.b.b8[1]);
            return ret;
            break;
        };
        case 'c':
        {
            ret.value = (uint32_t *)&(ins->registers.c.b8[1]);
            return ret;
            break;
        };
        case 'd':
        {
            ret.value = (uint32_t *)&(ins->registers.d.b8[1]);
            return ret;
            break;
        };
        }
    }
    ret.bits = -1;
    return ret;
}

register_read_vt register_read_value(register_read_prt r)
{
    // takes a power of 2 (8, 16, 32) and masks a value to that size
    return (register_read_vt){
        .value = ((2 * (uint32_t)pow(2, r.bits - 1) - 1) & (*r.value)),
        .bits = r.bits,
    };
}

static inline register_read_vt register_read_quick(asm_instance_t *ins, char *str)
{
    return register_read_value(register_read(ins, str));
}

void register_write_quick(asm_instance_t *ins, char *str, uint32_t val)
{
    register_read_prt out = register_read(ins, str);
    switch (out.bits)
    {
    case 8:
    {
        *((uint8_t *)out.value) = (uint8_t)val;
        break;
    };
    case 16:
    {
        *((uint16_t *)out.value) = (uint16_t)val;
        break;
    };
    case 32:
    {
        *out.value = (uint32_t)val;
        break;
    };
    }
}
