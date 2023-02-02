extern void js_pass_virtual_memory(char* addr);
extern void js_pass_free_buffer(char* addr);
extern void js_return_value(char val);
extern char returnAddr;
extern void console(char* str);
extern void console_char(char ch);
extern void console_int(int in);

extern void js_pass_registers(int32_t a, int32_t b, int32_t c, int32_t d);