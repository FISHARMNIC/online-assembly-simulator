/*
Library written for simple C-JS Interaction
Nicolas Quijano 2023
*/

// to do memory highlighting compare what chnaged new memory vs old memory
window.WASM_SOURCE = 'wasm/main.wasm';
window.MAX_STR_SIZE = 1000;
window.wasm_global = 0;
window.memory = [];
window.returnAddr = 0;

var env = {
    MAX_STR_SIZE,
    js_update_memory: () => memory_highlight(),
    js_pass_virtual_memory,
    js_pass_free_buffer,
    js_return_value,
    console: (str) => console.log(read_wasm_string(str)),
    console_char: (ch) => console.log(ch),
    console_int: (i) => console.log(i),
    js_pass_registers
}

function js_pass_registers(a, b, c, d) {
    document.getElementById("r1").innerText = a;
    document.getElementById("r2").innerText = b;
    document.getElementById("r3").innerText = c;
    document.getElementById("r4").innerText = d;

    var arg = Object.values(arguments);
    
    var i8l = new Int8Array([...arg])
    var i8h = new Int8Array([...arg.map(x => x >> 8)])
    var i16 = new Int16Array([...arg])
    for (var i = 1; i <= 4; i++) {
        document.getElementById(`r${i}_32`).innerText = arg[i - 1];
        document.getElementById(`r${i}_16`).innerText = i16[i - 1];
        document.getElementById(`r${i}_8h`).innerText = i8h[i - 1];
        document.getElementById(`r${i}_8l`).innerText = i8l[i - 1];
    }

    console.log(a, b, c, d)
}

function wasm_write_memory(address, data) {
    var mem = new Uint8Array(memory.buffer, address, MAX_STR_SIZE)
    mem.set(new Uint8Array(data))
}

function wasm_write_string(address, str) {
    str = str.split("").map(x => x.charCodeAt(0))
    str.push(0)
    wasm_write_memory(address, str)
    return address;
}

function read_wasm_string(base) {
    var strBuf = new Uint8Array(memory.buffer, base, MAX_STR_SIZE);

    var i = 0;
    while (strBuf[i] != 0 && i < MAX_STR_SIZE)
        i++

    return new TextDecoder().decode(strBuf).slice(0, i);
}

function wasm_read_memory(base) {
    return (new Uint8Array(memory.buffer, base, MAX_STR_SIZE))
}

function console_read_memory() {
    console.log(wasm_read_memory(virtual_address))
}


function wasm_runFunction(str_fn, params) {
    if (params) {
        wasm_global[str_fn](...params)
        return
    }
    wasm_global[str_fn]()
}

function wasm_init(url, imp) {
    return fetch(url)
        .then(res => res.arrayBuffer())
        .then(bytes => WebAssembly.compile(bytes))
        .then(module => WebAssembly.instantiate(module, imp))
        .then(instance => instance.exports);
}

window.runsem = false;
async function main() {
    wasm_init(String(WASM_SOURCE), { env })
        .then(m => {
            wasm_global = m;
            memory = m.memory;
            m.asm_init();
            reset();
        })
}

main()
