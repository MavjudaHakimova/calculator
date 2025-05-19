
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include "mymodule_func.h"

MODULE_LICENSE("GPL");

struct Stack* create_stack(void) {
	struct Stack* s = kmalloc(sizeof(struct Stack), GFP_KERNEL);
	if(!s) return NULL;
	s->top = -1;
	return s;
}

void destroy_stack(struct Stack* s) {
	kfree(s);
}

int push(struct Stack* s, struct Leksema item) {
	s->items[++s->top] = item;
	return 0;
}

int pop(struct Stack* s) {
	s->top--;
	return 0;
}

int is_empty(struct Stack* s) {
	return s->top == -1;
}

struct Leksema top(struct Stack* s) {
	return s->items[s->top];
}

