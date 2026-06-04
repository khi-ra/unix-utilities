# Unix Utilities

In this project, I'm re-implementing basic unix commands e.g. `cat`, `wc`, `head`/`tail`, etc.
The purpose is to develop my C skills and gain a better understanding of low-level development.

# Usage

## Build/Compilation

`cat.c` is compiled with the following options:

```bash
gcc -o cat.out -Wall -Wpedantic -Werror -fsanitize=undefined -g cat.c
```

The `-fsanitize=address` flag is ommitted as it cannot be used alongside gdb (GNU Debugger).

- it can be added: `-fsanitize=undefined,address`

The `-g` flag is only necessary if debugging with gdb.

## Executing

The executable can be run with:

```bash
./cat.out [FILE]
```

# Limitations

## `Cat.c`

These features are currently not supported:

- Reading from standard input if file operand is `-`
- Running with options: `-e`, `-t`, `-v`
