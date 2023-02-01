emcc --no-entry main.c -o ../wasm/main.wasm \
-s STANDALONE_WASM \
-s WARN_ON_UNDEFINED_SYMBOLS=0 \
-s EXPORTED_FUNCTIONS='[_asm_run, _asm_init, _asm_memory_reset]'