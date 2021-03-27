#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>


#define NUM_ITR 1000
#define NUM_THREAD 1000

struct node {
	int val;
	struct node *next;
};

struct stack {
	struct node *head;
	pthread_mutex_t lock;
};

struct thread_data {
	int iterations;
	struct stack *st;
	unsigned int thread_id;
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

struct node *pop(struct stack *st)
{
	struct node *popped;

	if (!st)
		return NULL;

	pthread_mutex_lock(&st->lock);
	if (!st->head) {
		pthread_mutex_unlock(&st->lock);
		return NULL;
	}
	popped = st->head;
	st->head = st->head->next;
	pthread_mutex_unlock(&st->lock);

	return popped;
}

void free_stack(struct stack *st)
{
	struct node *n;

	while((n = pop(st)))
		free(n);
}

int print_list(struct node *head)
{
	int i = 0;

	while(head) {
		printf("%d ", head->val);
		head = head->next;
		i++;
	}
	if (i)
		printf("\n");

	return i;
}

void *thread1_main(void *data)
{
	struct thread_data *in = data;
	int i;

	for (i = 0; i < in->iterations; i++ ) {
		if (i%in->thread_id)
			push(in->st, i);
	}

	for (i = 0; i < in->iterations; i++ ) {
		if (i%in->thread_id)
			pop(in->st);
	}

	return NULL;
}

void test(int num_threads, int iterations)
{
	pthread_t *thread_ids = malloc(sizeof(pthread_t)*num_threads);
	struct stack *st = new_stack();
	int i;

	for (i = 0; i < num_threads; i++) {
		struct thread_data data;
		data.st = st;
		data.thread_id = i;
		data.iterations = iterations;
		pthread_create(&thread_ids[i], NULL, thread1_main, &data);
	}

	for (i = 0; i < num_threads; i++) {
		pthread_join(thread_ids[i], NULL);
	}
	free(thread_ids);
	assert(print_list(st->head) == 0);
}

int main(void)
{
	struct stack *st = new_stack();
	push(st, 100);
	push(st, 200);
	push(st, 300);
	assert(print_list(st->head) == 3);
	free_stack(st);
	assert(print_list(st->head) == 0);
	test(NUM_THREAD, NUM_ITR);
	return 0;
}
