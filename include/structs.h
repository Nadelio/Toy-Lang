#ifndef MEDUSA_STRUCTS_H
#define MEDUSA_STRUCTS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef char* string;
typedef int32_t i32;

typedef struct {
	i32 key;
	i32 value;
	i32 position;
} MapElement;

typedef struct {
	size_t size;
	MapElement* pairs;
} Map;

void put(Map* map, i32 key, i32 value, i32 position);
MapElement* get(Map* map, i32 key);
void set(Map* map, i32 indent, i32 value, i32 address);

typedef struct {
	i32 value;
	i32 indent;
	i32 address;
	bool is_literal;
} Variable;

typedef struct {
	Variable* stack[64];
	int top;
	size_t size;
} StackVariable;

StackVariable* build_varstack(void);
void deconstruct(StackVariable* varstack);
bool is_varstack_empty(StackVariable* stack);
bool is_varstack_full(StackVariable* stack);
Variable* pop_varstack(StackVariable* stack);
Variable* peek_varstack(StackVariable* stack);
void push_varstack(StackVariable* stack, Variable* value);

typedef struct {
	i32 stack[64];
	int top;
	size_t size;
} Stacki32;

Stacki32* build(void);
bool is_empty(Stacki32* stack);
bool is_full(Stacki32* stack);
i32 pop(Stacki32* stack);
i32 peek(Stacki32* stack);
void push(Stacki32* stack, i32 value);

bool is_out_of_bounds(i32* ptr, i32* lower_bound, i32* upper_bound);

#endif
