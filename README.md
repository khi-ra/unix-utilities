# Unix Utilities

In this project, I'm re-implementing basic unix commands e.g. `cat`, `wc`, `head`/`tail`, etc.
The purpose is to develop my C skills and gain a better understanding of low-level development.

# `Cat`

## Usage

### Build/Compilation

`cat.c` is compiled with the following options:

```bash
gcc -o cat.out -Wall -Wpedantic -Werror -fsanitize=undefined -g cat.c
```

The `-fsanitize=address` flag is ommitted as it cannot be used alongside gdb (GNU Debugger).

- it can be added: `-fsanitize=undefined,address`

The `-g` flag is only necessary if debugging with gdb.

### Running The Program

The executable can be run with:

```bash
./cat.out [FILE]
```

- multiple files can be taken as input, each separated by a space
- if no file is specified or file operand is `-`, the program takes input from standard input

## Implementation

### Error Handling

The program defines an enum of error codes `enum error_code`:   
```C
enum error_code 
{
  ERR_FOPEN = 1,
  ERR_FREAD = 2,
  ERR_FWRITE = 4,
  ERR_FACCESS = 4,
  ERR_FINVAL = 5,
};
```
Upon error, the program exits with the relevant error code.  

## Limitations

These features are currently not supported:

- Running with options: `-e`, `-t`, `-v`
