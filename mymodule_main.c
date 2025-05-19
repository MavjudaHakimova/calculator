#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include "mymodule_func.h"
#include <linux/types.h>
#include <linux/math.h>


#define PI 3.14159265358979323846
#define TWO_PI (2.0 * PI)
#define PI_2 (PI / 2.0)
#define EXP 2.71828182845904523536


static char* expression = "2+3";
module_param(expression, charp, 0);
MODULE_PARM_DESC(expression, "Mathematical expression to evaluate");

struct Stack* stack_n;
struct Stack* stack_o;

static float normalize_angle(float x) {
	while (x >= TWO_PI) {
		x -= TWO_PI;
	}
	while (x < 0) {
		x += TWO_PI;
	}
	return x;
}

static float kernel_sin(float x) {
	float angle = normalize_angle(x);
	int sign = 1;

	if(angle > PI) {
		angle -= PI;
		sign = -1; //sin(x)= -sin(pi+x)
	}

	if(angle > PI_2) {
		angle = PI - angle; //sin(pi - x) = sin(x)
	}

	if(angle < 1e-6f) {
		return sign * angle;
	}

	float x2 = angle * angle;
	return sign * (angle - (angle * x2) / 6.0f + (angle * x2 * x2) / 120.0f - (angle * x2 * x2 * x2) / 5040.0f);
}

static float kernel_cos(float x) {
	float angle = normalize_angle(x);
	int sign = 1;

	if (angle > PI) {
		angle -= PI;
		sign = -1; //cos(pi+x) = -cos(x)
	}

	if (angle > PI_2) {
		angle = PI - angle; //cos(pi-x)=-cos(x)
		sign = -1;
	}

	if (angle < 1e-6f) {
		return 1;
	}

	float x2 = angle * angle;
	return sign * (1.0f - x2 / 2.0f + (x2 * x2) / 24.0f - (x2 * x2 * x2) / 720.0f);
}

float float_pow(float base, int exp) {
	float result = 1.0f;
	int i;
	if (exp < 0) {
		base = 1.0f / base;
		exp = -exp;
	}

	for(i = 0; i < exp; i++) {
		result *= base;
	}

	return result;
}

static int get_rang(char ch) {
	switch(ch) {
		case '+':
		case '-':
			return 1;
		case '*':
		case '/':
			return 2;
		case '^':
			return 3;
		case 's':
		case 'c':
		case 't':
		case 'g':
		case 'e':
			return 4;
		default:
			return 0;
	}
}

bool calculate(struct Stack* stack_n, struct Stack* stack_o, struct Leksema* item) {
	printk(KERN_ALERT "calculating...\n");
	float a;
	float b;
	float c;
	item->type = '0';

	if(is_empty(stack_n)) {
		printk(KERN_WARNING "Stack of numbers is empty!\n");
		return false;
	}

	kernel_fpu_begin();
	*item = top(stack_n);
	a = item->value;
	pop(stack_n);
	kernel_fpu_end();
	
	char ch = top(stack_o).type;

	switch(ch) {
		case '+':
			if(is_empty(stack_n)) {
				printk(KERN_WARNING "Stack of numbers is empty!\n");
				return false;
			}

			kernel_fpu_begin();
			b = top(stack_n).value;
			pop(stack_n);
			c = b + a;
			item->value = c;
			push(stack_n, *item);
			pop(stack_o);
			kernel_fpu_end();
			break;

		case '-':
			if(is_empty(stack_n)) {
                                printk(KERN_WARNING "Stack of numbers is empty!\n");
                                return false;
                        }

			kernel_fpu_begin();
			b = top(stack_n).value;
			pop(stack_n);
			c = b - a;
			item->value = c;
			push(stack_n, *item);
			pop(stack_o);
			kernel_fpu_end();
			break;

		case '^':
                        if(is_empty(stack_n)) {
                                printk(KERN_WARNING "Stack of numbers is empty!\n");
                                return false;
                        }

                        kernel_fpu_begin();
                        b = top(stack_n).value;
                        pop(stack_n);
                        c = float_pow(b, a);
                        item->value = c;
                        push(stack_n, *item);
                        pop(stack_o);
                        kernel_fpu_end();
                        break;

		case 'e':
			kernel_fpu_begin();
			c = float_pow(EXP, a);
			item->value = c;
			push(stack_n, *item);
			pop(stack_o);
			kernel_fpu_end();
			break;

		case '*':
			if(is_empty(stack_n)) {
                                printk(KERN_WARNING "Stack of numbers is empty!\n");
                                return false;
                        }

			kernel_fpu_begin();
                        b = top(stack_n).value;
                        pop(stack_n);
                        c = b * a;
                        item->value = c;
                        push(stack_n, *item);
			pop(stack_o);
                        kernel_fpu_end();
                        break;

		case '/':
			if(is_empty(stack_n)) {
                                printk(KERN_WARNING "Stack of numbers is empty!\n");
                                return false;
                        }

                        kernel_fpu_begin();
                        b = top(stack_n).value;
                        pop(stack_n);
			if (a == 0) {
				printk(KERN_WARNING "Division by zero!\n");
				kernel_fpu_end();
				return false;
			}
                        c = b / a;
                        item->value = c;
                        push(stack_n, *item);
			pop(stack_o);
                        kernel_fpu_end();
                        break;

		case 's':
			kernel_fpu_begin();
			c = kernel_sin(a);
			item->value = c;
			push(stack_n, *item);
			pop(stack_o);
			kernel_fpu_end();
			break;

		case 'c':
			kernel_fpu_begin();
                        c = kernel_cos(a);
                        item->value = c;
                        push(stack_n, *item);
                        pop(stack_o);
                        kernel_fpu_end();
			break;

		case 't':
			kernel_fpu_begin();
			if (kernel_cos(a) == 0) {
				printk(KERN_WARNING "Division by cos(x)=0!!!\n");
				kernel_fpu_end();
				return false;
			}
                        c = kernel_sin(a) / kernel_cos(a);
                        item->value = c;
                        push(stack_n, *item);
                        pop(stack_o);
                        kernel_fpu_end();
			break;

		case 'g':
			kernel_fpu_begin();
			if (kernel_sin(a) == 0) {
				printk(KERN_WARNING "Division by sin(x)=0!!!\n");
                                kernel_fpu_end();
                                return false;
			}
                        c = kernel_cos(a) / kernel_sin(a);
                        item->value = c;
                        push(stack_n, *item);
                        pop(stack_o);
                        kernel_fpu_end();
			break;
		default:
			printk(KERN_WARNING "Unknown operator!\n");
			return false;
	}

	return true;
}

static void print_stack_n(struct Stack* stack) {
	struct Stack* temp_stack = create_stack();
	struct Leksema item;

	printk(KERN_ALERT "Stack of numbers:\n");

	while(!is_empty(stack)) {
		kernel_fpu_begin();
		item = top(stack);
		pop(stack);
		float result = item.value;
		kernel_fpu_end();
		int int_part = (int)result;
		int frac_part = (int)((result - int_part) * 1000);
		printk(KERN_ALERT " value is: %d.%03d\n", int_part, frac_part);
		kernel_fpu_begin();
		push(temp_stack, item);
		kernel_fpu_end();

	}
	while(!is_empty(temp_stack)) {
		kernel_fpu_begin();
		item = top(temp_stack);
		pop(temp_stack);
		push(stack, item);
		kernel_fpu_end();
	}

	destroy_stack(temp_stack);
}

static void print_stack_o(struct Stack* stack) {
	struct Stack* temp_stack = create_stack();
	struct Leksema item;

	printk(KERN_ALERT "Stack of operators:\n");

	while (!is_empty(stack)) {
		kernel_fpu_begin();
		item = top(stack);
		char c = item.type;
		pop(stack);
		kernel_fpu_end();

		printk(KERN_ALERT " operator is:%c\n", c);

		kernel_fpu_begin();
		push(temp_stack, item);
		kernel_fpu_end();
	}

	while (!is_empty(temp_stack)) {
		kernel_fpu_begin();
		item = top(temp_stack);
		pop(temp_stack);
		push(stack, item);
		kernel_fpu_end();
	}

	destroy_stack(temp_stack);
}

static int __init silent_init(void) {
	printk(KERN_ALERT "EXPRESSION TO CALCULATE: %s\n", expression);

	stack_n = create_stack();
	stack_o = create_stack();

	float num = 0.0f;
	bool building_num = false;
	struct Leksema item;
	bool flag = 1; //для обработки отрицательных чисел
	int sign1 = 1;
	while(*expression) {
		if(*expression >= '0' && *expression <= '9' || *expression == '-' && flag == 1) {
			if (*expression == '-' && flag == 1) {
				sign1 = -1;
				expression++;
			} else {
				sign1 = 1;
			}
			while(*expression >= '0' && *expression <= '9') {
				kernel_fpu_begin();
				num = num * 10 + (*expression - '0');
				kernel_fpu_end();
				expression++;
			}

			if (*expression == '.') {
				expression++;
				float frac_part = 0.0f;
				float divisor = 10.0f;

				while(*expression >= '0' && *expression <= '9') {
					kernel_fpu_begin();
					frac_part += (*expression - '0') / divisor;
					divisor *= 10;
					kernel_fpu_end();
					expression++;
				}
				kernel_fpu_begin();
				num += frac_part;
				kernel_fpu_end();
			}

			kernel_fpu_begin();
			item.value = num * sign1;
			push(stack_n, item);
			num = 0.0f;
			flag = 0;
			kernel_fpu_end();
			print_stack_n(stack_n);
			continue;
		}

		if (*expression == 'e' || *expression == 's' || *expression == 'c' || *expression == 't') {
			char foo[3];
			foo[0] = *expression;
			foo[1] = *(expression + 1);
			foo[2] = *(expression + 2);

			if (foo[0] == 's' && foo[1] == 'i' && foo[2] == 'n') {
				kernel_fpu_begin();
				item.type = 's';
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();
				expression += 3;
				continue;
			}

			if (foo[0] == 'c' && foo[1] == 'o' && foo[2] == 's') {
				kernel_fpu_begin();
				item.type = 'c';
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();
				expression += 3;
				continue;
			}

			if (foo[0] == 'c' && foo[1] == 't' && foo[2] == 'g') {
				kernel_fpu_begin();
				item.type = 'g';
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();
				expression += 3;
				continue;
			}

			if (foo[0] == 't' && foo[1] == 'a' && foo[2] == 'n') {
				kernel_fpu_begin();
				item.type = 't';
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();
				expression += 3;
				continue;
			}

			if (foo[0] == 'e' && foo[1] == 'x' && foo[2] == 'p') {
				kernel_fpu_begin();
				item.type = 'e';
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();
				expression += 3;
				continue;
			}
		}
		if (*expression == '+' || *expression == '-' && flag == 0 || *expression == '/' || *expression == '*' || *expression == '^') {
			int x = get_rang(*expression);
			printk(KERN_ALERT "rang is: %d\n", x);
			if (is_empty(stack_o)) {
				printk(KERN_ALERT "First operator!\n");
				kernel_fpu_begin();
				item.type = *expression;
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();

				expression++;
				continue;
			}
			if (!is_empty(stack_o) && get_rang(*expression) > get_rang(top(stack_o).type)) {
				printk(KERN_ALERT "Range of current is greater than top of stack of operators!!!");
				kernel_fpu_begin();
				item.type = *expression;
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();

				expression++;
				continue;
			}
			if (!is_empty(stack_o) && get_rang(*expression) <= get_rang(top(stack_o).type)) {
				if (calculate(stack_n, stack_o, &item) == false) {
					printk(KERN_ERR "calculate FAILED!!!\n");
					return -EINVAL;
				}
				printk(KERN_ALERT "Let's calculate the interim operator!!!\n");
				kernel_fpu_begin();
				item.type = *expression;
				item.value = 0.0f;
				push(stack_o, item);
				kernel_fpu_end();

				expression++;
				continue;
			}
			expression++;
		}
		if (*expression == '(') {
			flag = 1;
			kernel_fpu_begin();
			item.type = *expression;
			item.value = 0.0f;
			push(stack_o, item);
			kernel_fpu_end();
			expression++;
			continue;
		}
		if (*expression == ')') {
			printk(KERN_ALERT "evaluate the expression inside the brackets\n");
			while(top(stack_o).type != '(') {
				if (calculate(stack_n, stack_o, &item) == false) {
					printk(KERN_ERR "calculate inside brackets FAILED\n");
					return -EINVAL;
				}
			}
			pop(stack_o);
			expression++;
		}

		if (*expression == 'p') {
			kernel_fpu_begin();
			item.type = '0';
			item.value = PI;
			push(stack_n, item);
			kernel_fpu_end();
			expression++;
			continue;
		}
	}

	while (!is_empty(stack_o)) {
		struct Leksema result;
		calculate(stack_n, stack_o, &result);
	}

	kernel_fpu_begin();
	struct Leksema result = top(stack_n);
	float result_value = result.value;
	kernel_fpu_end();

	int sign = (result_value < 0) ? -1 : 1;
	float abs_value = (sign == -1) ? -result_value : result_value;

	int int_part = (int)abs_value;
	kernel_fpu_begin();
	int frac_part = (int)((abs_value - int_part) * 1000 + 0.5f);
	kernel_fpu_end();
	if (sign == -1) {
		printk(KERN_ALERT "CALCULATION RESULT: -%d.%03d\n", int_part, frac_part);
	} else {
		printk(KERN_ALERT "CALCULATION RESULT: %d.%03d\n", int_part, frac_part);
	}

	return 0;
}

static void __exit silent_exit(void) {
	destroy_stack(stack_n);
	destroy_stack(stack_o);
	printk(KERN_ALERT "Stack module unloaded\n");
}

module_init(silent_init);
module_exit(silent_exit);

MODULE_LICENSE("GPL");
