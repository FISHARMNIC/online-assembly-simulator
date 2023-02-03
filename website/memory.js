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

function js_pass_registers(a, b, c, d) {
    document.getElementById("r1").innerText = a;
    document.getElementById("r2").innerText = b;
    document.getElementById("r3").innerText = c;
    document.getElementById("r4").innerText = d;

    var arg = Object.values(arguments)
    var showunsigned = document.getElementById("signed").checked
    var convertUC = x => showunsigned ? x : (x < 0 ? 256 + x : x)
    var convertUS = x => showunsigned ? x : (x < 0 ? 65536 + x : x)
    var convertUL = x => showunsigned ? x : (x < 0 ? 4294967296 + x : x)

    var i8l = Array.from(new Int8Array([...arg]))
        .map(convertUC)
    var i8h = Array.from(new Int8Array([...arg.map(x => x >> 8)]))
        .map(convertUS)
    var i16 = Array.from(new Int16Array([...arg]))
        .map(convertUL)

    for (var i = 1; i <= 4; i++) {
        document.getElementById(`r${i}_32`).innerText = arg[i - 1];
        document.getElementById(`r${i}_16`).innerText = i16[i - 1];
        document.getElementById(`r${i}_8h`).innerText = i8h[i - 1];
        document.getElementById(`r${i}_8l`).innerText = i8l[i - 1];
    }

    //console.log(a, b, c, d)
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
    formattedmem = formattedmem.slice(0).map(x => x + " ".repeat(3 - (String(x).length > 3 ? String(x).length - 40 - col.length : String(x).length))).join(" ")
    //console.log(formattedmem)
    document.getElementById("wasm_memory").innerHTML = formattedmem;
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