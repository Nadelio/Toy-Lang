# Medusa Bytecode Standard

| Identifier                | Opcode | Notes                                                                                                                                             |
| ------------------------- | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `EOD`                     | -1     | This marks the end of the program data in memory                                                                                                  |
| `_NULL`                   | 0      | Can be used for padding, or as a NOP                                                                                                              |
| `VARIABLE`                | 1      |                                                                                                                                                   |
| `DATA_PTR_REF`            | 2      |                                                                                                                                                   |
| `PROG_PTR_REF`            | 3      |                                                                                                                                                   |
| `ASSIGN`                  | 4      | Assign/reassign value of a variable                                                                                                               |
| `ADDRESS_ASSIGN`          | 5      | Assign/reassign address of a variable                                                                                                             |
| `DECLARE_REF`             | 6      | Used to store the address of a variable instead of the value                                                                                      |
| `DEREF`                   | 7      | In Medusa, this is the `*` symbol, which is shared with the `MULTI` opcode                                                                        |
| `MULTI`                   | 8      | In Medusa, this is the `*` symbol, which is shared with the `DEREF` opcode                                                                        |
| `DIV`                     | 9      | Integer division, Medusa doesn't support floats natively                                                                                          |
| `PLUS`                    | 10     | Addition                                                                                                                                          |
| `INC`                     | 11     | Increment                                                                                                                                         |
| `MINUS`                   | 12     | Subtraction                                                                                                                                       |
| `DEC`                     | 13     | Decrement                                                                                                                                         |
| `EQUIV`                   | 14     | Checks for equivalence between two values/variables                                                                                               |
| `NOT_EQUIV`               | 15     | Checks if two values/variables are NOT equivalent                                                                                                 |
| `LESS`                    | 16     |                                                                                                                                                   |
| `GREATER`                 | 17     |                                                                                                                                                   |
| `LESS_EQUAL`              | 18     |                                                                                                                                                   |
| `GREATER_EQUAL`           | 19     |                                                                                                                                                   |
| `MOVE_RIGHT`              | 20     | Moves a variable's address to the right once                                                                                                      |
| `MOVE_LEFT`               | 21     | Moves a variable's address to the left once                                                                                                       |
| `BEGIN_SCOPE`             | 22     | Marks the beginning of a scope                                                                                                                    |
| `END_SCOPE`               | 23     | Marks the end of a scope                                                                                                                          |
| `DEFINE_SCOPE_ITERATIONS` | 24     |                                                                                                                                                   |
| `BREAK_FROM_SCOPE`        | 25     |                                                                                                                                                   |
| `INFINITE_LOOP`           | 26     |                                                                                                                                                   |
| `DEFINE_DATA_MEM_SIZE`    | 27     | This is a meta instruction that defines how many bytes to allocate for the interpreter's date section (if this is excluded, default to 128 bytes) |
| `INT_LITERAL`             | 28     | Marks the following integer as a literal instead of an instruction                                                                                |
| `EOS`                     | 29     | End of statement (currently not used)                                                                                                             |
