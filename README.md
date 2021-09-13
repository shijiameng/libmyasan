My Address Sanitizer
====================

## Build

Build MyASan runtime library `libmyasan.so` simply by: 

```sh
make
```

## HOWTO

Instrument each `load/store` instruction with following MyASan memory check APIs:

| API                 | Description                  |
| ------------------- | ---------------------------- |
| `asan_load8_check`  | Check before loading 8 bytes |
| `asan_load4_check`  | Check before loading 4 bytes |
| `asan_load2_check`  | Check before loading 2 bytes |
| `asan_load1_check`  | Check before loading 1 byte  |
| `asan_store8_check` | Check before storing 8 bytes |
| `asan_store4_check` | Check before storing 4 bytes |
| `asan_store2_check` | Check before storing 2 bytes |
| `asan_store1_check` | Check before storing 1 byte  |

Hook each `malloc/free` with its MyASan version: `asan_malloc` and `asan_free`.

## Samples

`use-after-free` and `heap overflow` sample programs are provided in `samples` directory to verify the functionality of MyASan.

Build samples

```sh
cd sample
gcc uaf.c -o uaf -I.. -L.. -lmyasan
gcc heap_overflow.c -o heap_overflow -I.. -L.. -lmyasan
```
Run `use-after-free` sample
```sh
LD_LIBRARY_PATH=.. ./uaf
```

Run `heap overflow` sample
```sh
LD_LIBRARY_PATH=.. ./heap_overflow
```