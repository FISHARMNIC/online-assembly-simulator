var virtual_address = 0;
var stringbuffer = 0;

function run_c(arg) {
    wasm_write_string(stringbuffer, arg)
    oldMem = [...Array.from(wasm_read_memory(virtual_address))]
    wasm_global.asm_run()
}

function js_return_value(arg) {
    returnAddr = arg;
    if (returnAddr != -1) {
        linenum = returnAddr - 1
        highlight()
    }
}

function js_pass_free_buffer(ptr) {
    // address of free allocated space for writing strings
    stringbuffer = ptr;
}
function js_pass_virtual_memory(ptr) {
    // address of the memory buffer for the assembly program in the wasm program
    virtual_address = ptr;
}

function updateMemory() {
    var formattedmem = Array.from(wasm_read_memory(virtual_address).slice(0));
    var col = "orange";
    if (oldMem != -1) {
        formattedmem = formattedmem.map((x, i) => {
            return (x == oldMem[i]) ? x : `<span style="background-color:${col};">${x}</span>`
        })
    }
    //console.log(`<span style="background-color:${col};">${123}</span>`.length - 40 - col.length)
    formattedmem = formattedmem.slice(0).map(x => x + " ".repeat(3 - (String(x).length > 3 ? String(x).length - 40 - col.length: String(x).length))).join(" ")
    //console.log(formattedmem)
    document.getElementById("wasm_memory").innerHTML = formattedmem;
}

function memory_highlight() {

}

function memory_read_size(size, index) {
    var buff;
    if (size == 8)
        buffer = new Uint8Array(memory.buffer, index + virtual_address, 1)
    else if (size == 16)
        buffer = new Uint16Array(memory.buffer, index + virtual_address, 1)
    else if (size == 32)
        buffer = new Uint32Array(memory.buffer, index + virtual_address, 1)
    return buff[0];
}

function memory_write_size(size, index, value) {
    var buff;
    if (size == 8)
        buffer = new Uint8Array(memory.buffer, index + virtual_address, 1)
    else if (size == 16)
        buffer = new Uint16Array(memory.buffer, index + virtual_address, 1)
    else if (size == 32)
        buffer = new Uint32Array(memory.buffer, index + virtual_address, 1)
    buff[0] = value;
}