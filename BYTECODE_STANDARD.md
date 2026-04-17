# Medusa Bytecode Standard

| Identifier                | Opcode | Notes                                                                                                                                             |
| ------------------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `EOD`                     | -1     | This marks the end of the program data in memory                                                                                                  |
| `_NULL`                   | 0      | Can be used for padding, or as a NOP                                                                                                              |
| `INT_LITERAL`             | 1      | Marks the following integer as a literal instead of an instruction                                                                                |
| `IDENT`                   | 2      | Label name                                                                                                                                        |
| `VARIABLE`                | 3      |                                                                                                                                                   |
| `DATA_PTR_REF`            | 4      |                                                                                                                                                   |
| `PROG_PTR_REF`            | 5      |                                                                                                                                                   |
| `ASSIGN`                  | 6      | Assign/reassign value of a variable                                                                                                               |
| `ADDRESS_ASSIGN`          | 7      | Assign/reassign address of a variable                                                                                                             |
| `DECLARE_REF`             | 8      | Used to store the address of a variable instead of the value                                                                                      |
| `DEREF`                   | 9      | In Medusa, this is the `*` symbol, which is shared with the `MULTI` opcode                                                                        |
| `MULTI`                   | 10     | In Medusa, this is the `*` symbol, which is shared with the `DEREF` opcode                                                                        |
| `DIV`                     | 11     | Integer division, Medusa doesn't support floats natively                                                                                          |
| `PLUS`                    | 12     | Addition                                                                                                                                          |
| `INC`                     | 13     | Increment                                                                                                                                         |
| `MINUS`                   | 14     | Subtraction                                                                                                                                       |
| `DEC`                     | 15     | Decrement                                                                                                                                         |
| `EQUIV`                   | 16     | Checks for equivalence between two values/variables                                                                                               |
| `NOT_EQUIV`               | 17     | Checks if two values/variables are NOT equivalent                                                                                                 |
| `LESS`                    | 18     |                                                                                                                                                   |
| `GREATER`                 | 19     |                                                                                                                                                   |
| `LESS_EQUAL`              | 20     |                                                                                                                                                   |
| `GREATER_EQUAL`           | 21     |                                                                                                                                                   |
| `MOVE_RIGHT`              | 22     | Moves a variable's address to the right once                                                                                                      |
| `MOVE_LEFT`               | 23     | Moves a variable's address to the left once                                                                                                       |
| `BEGIN_SCOPE`             | 24     | Marks the beginning of a scope                                                                                                                    |
| `END_SCOPE`               | 25     | Marks the end of a scope                                                                                                                          |
| `DEFINE_SCOPE_ITERATIONS` | 26     |                                                                                                                                                   |
| `BREAK_FROM_SCOPE`        | 27     |                                                                                                                                                   |
| `INFINITE_LOOP`           | 28     |                                                                                                                                                   |
| `L_PAREN`                 | 29     |                                                                                                                                                   |
| `R_PAREN`                 | 30     |                                                                                                                                                   |
| `PRINT`                   | 31     |                                                                                                                                                   |
| `PRINT_LITERAL`           | 32     |                                                                                                                                                   |
| `DEFINE_DATA_MEM_SIZE`    | 33     | This is a meta instruction that defines how many bytes to allocate for the interpreter's date section (if this is excluded, default to 128 bytes) |
| `EOS`                     | 34     | End of statement, clears the runtime stack                                                                                                        |
