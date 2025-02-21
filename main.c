#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct stack {
	int val;
	char sig;
	struct stack* next;
};


void push(struct stack** stack, int val_, char sig_) {

	struct stack* tmp = malloc(sizeof(struct stack));
	tmp->val = val_;
	tmp->sig = sig_;
	tmp->next = (*stack);
	(*stack) = tmp;
}


void pop(struct stack** stack) {

	if ((*stack) == NULL)
		return;
	struct stack* tmp = (*stack);
	(*stack) = (*stack)->next;
	free(tmp);
}


void del_spaces(char* str) {
	for (int i = 0; str[i] != '\0'; i++) 
	{
		if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n')
		{
			for (int j = i; str[j] != '\0'; j++)
				str[j] = str[j + 1];
			i--;
		}
	}
}


int in_int(struct stack** digits) {

	if ((*digits) == NULL)
		return 0;

	int num = (*digits)->sig - '0';
	pop(digits);
	int c = 10;

	while ((*digits) != NULL)
	{
		if ((*digits)->sig == '-')
			num = num * (-1);
		else
			num = num + ((*digits)->sig - '0') * c;

		c = c * 10;
		pop(digits);
	}
	
	return num;
}


int ranK(char ch) {
	if (ch == '+' || ch == '-') return 1;
	if (ch == '*' || ch == '/') return 2;
	else return 0;
}


void calc_stack(struct stack** num, struct stack** sign) {

	int b = (*num)->val; pop(num);
	int a = (*num)->val; pop(num);

	switch ((*sign)->sig)
	{
	case '+': push(num, a + b, 0); break;
	case '-': push(num, a - b, 0); break;
	case '*': push(num, a * b, 0); break;
	case '/': push(num, a / b, 0); break;
	}
	pop(sign);
}


int count(char* str) {

	struct stack* num = NULL;
	struct stack* sign = NULL;
	struct stack* digits = NULL;

	for (int i = 0; str[i] != '\0'; i++)
	{
		if ((str[i] >= '0' && str[i] <= '9') || (str[i] == '-' && i == 0) || (str[i] == '-' && str[i - 1] == '(' && i != 0))
		{
			push(&digits, 0, str[i++]);

			for (; str[i] != '+' && str[i] != '-' && str[i] != '/' && str[i] != '*' && str[i] != ')' && str[i] != '(' && str[i] != '\0'; i++)
				push(&digits, 0, str[i]);

			push(&num, in_int(&digits), 0);
			i--;
		}

		else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/')
		{
			if (sign == NULL)
				push(&sign, 0, str[i]);

			else if (sign != NULL && (ranK(str[i]) > ranK(sign->sig)))
				push(&sign, 0, str[i]);

			else {
				calc_stack(&num, &sign); i--;
			}
		}

		else if (str[i] == '(')
		{
			push(&sign, 0, str[i]);
		}

		else if (str[i] == ')')
		{
			while (sign->sig != '(')
				calc_stack(&num, &sign);

			pop(&sign);
		}
	}

	while (sign != NULL)
		calc_stack(&num, &sign);

	int res = num->val;
	pop(&num);
	return res;
}


void get_str(char* str, int size) {
	bool stop = true;
	for (int i = 0; stop && (i < size); i++)
	{
		str[i] = getchar();
		if (str[i] == EOF)
		{
			stop = false;
			str[i] = '\0';
		}
	}
}


int main()
{
	char str[1024];
	get_str(str, 1024);

	del_spaces(str);
	printf("\n%d\n", count(str));
	return 0;
}
