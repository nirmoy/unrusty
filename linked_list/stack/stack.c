#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

struct node {
	int val;
	struct node *next;
};

struct stack {
	struct node *head;
	pthread_mutex_t lock;
};

struct node *new_node(int val)
{
	struct node * new = malloc(sizeof(struct node));

	if (!new)
		return NULL;
	new->val = val;
	new->next = NULL;
	return new;
}
struct stack *new_stack()
{
	struct stack * new = malloc(sizeof(struct stack));

	if (!new)
		return NULL;
	new->head = NULL;
	pthread_mutex_init(&new->lock, NULL);
	return new;

}
void push(struct stack *st, int val)
{
	if (!st)
		return;

	struct node *new = new_node(val);

	if (!new)
		return;
	pthread_mutex_lock(&st->lock);
	if (!st->head) {
		st->head = new;
	pthread_mutex_unlock(&st->lock);
		return;
	}

	new->next = st->head;
	st->head = new;
	pthread_mutex_unlock(&st->lock);
	return;
}

void print_list(struct node *head)
{
	while(head) {
		printf("%d", head->val);
		head = head->next;
	}
	printf("\n");
}

void *thread1_main(void *data)
{
	struct stack *st = data;
	int i;

	for (i = 0; i < 10; i++ ) {
		if (i%2)
			push(st, i);
	}
	return NULL;
}

void *thread2_main(void *data)
{
	struct stack *st = data;
	int i;

	for (i = 0; i < 10; i++ ) {
		if (!(i%2))
			push(st, i);
	}
	return NULL;
}

int main(void)
{
	pthread_t thread_ids[2];
	struct stack *st = new_stack();

	pthread_create(&thread_ids[0], NULL, thread1_main, st);
	pthread_create(&thread_ids[1], NULL, thread2_main, st);
	pthread_join(thread_ids[0], NULL);
	pthread_join(thread_ids[1], NULL);

	print_list(st->head);

}
