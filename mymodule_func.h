#ifndef STACK_H
#define STACK_H

#include <linux/types.h>

#define STACK_SIZE 200

struct Leksema {
	char type; // 0, если число, '+', и тд если операция
	float value; // 0, если операция, число-иначе
};

struct Stack {
	struct Leksema items[STACK_SIZE];
	int top;
};

struct Stack* create_stack(void);
void destroy_stack(struct Stack* s);
int push(struct Stack* s, struct Leksema item);
int pop(struct Stack* s);
int is_empty(struct Stack* s);
struct Leksema top(struct Stack* s);

#endif

