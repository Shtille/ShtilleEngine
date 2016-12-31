#include "../../util/saim_list.h"
#include "../../util/saim_memory.h"

#include <stdlib.h>
#include <stdio.h>

static int N = 10;

bool compare_func(const void* a,const void* b) {
  return *(int*)a <= *(int*)b;
}
void destroy_func(void* a) {
  SAIM_FREE((int*)a);
}

int *getnum(int val)
{
    int *p;

    p = SAIM_MALLOC(sizeof(*p));
    *p = val;
    return p;
}

int main()
{
    saim_list list;
    saim_list_node * node;

    saim_memory_init();

    saim_list_create(&list, destroy_func);
    printf("after list create %i\n", saim_memory_num_allocations());

    int i, *vals[N];

    srand(12345);

    for (i = 0; i < N; ++i) {
        vals[i] = getnum(rand() % N);
    }

    for (i = 0; i < N; ++i) {
        saim_list_push_back(&list, vals[i]);
    }
    printf("after back insertions %i\n", saim_memory_num_allocations());
    printf("list size = %u\n", list.length);

    // Show list
    node = list.head;
    if (!node)
        printf("no head!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->next;
    }
    putc('\n', stdout);

    printf("inverse:\n");
    node = list.tail;
    if (!node)
        printf("no tail!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->prev;
    }
    putc('\n', stdout);

    saim_list_sort(&list, compare_func);
    printf("after sort:\n");
    node = list.head;
    if (!node)
        printf("no head!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->next;
    }
    putc('\n', stdout);

    printf("inverse:\n");
    node = list.tail;
    if (!node)
        printf("no tail!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->prev;
    }
    putc('\n', stdout);

    saim_list_push_back(&list, getnum(11));
    saim_list_push_back(&list, getnum(12));
    saim_list_push_back(&list, getnum(13));
    destroy_func(saim_list_pop_front(&list));
    destroy_func(saim_list_pop_front(&list));
    destroy_func(saim_list_pop_front(&list));
    printf("after some push and pop:\n");
    node = list.head;
    if (!node)
        printf("no head!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->next;
    }
    putc('\n', stdout);

    printf("inverse:\n");
    node = list.tail;
    if (!node)
        printf("no tail!\n");
    while (node) {
        printf("%i ", *(int*)node->data);
        node = node->prev;
    }
    putc('\n', stdout);

    saim_list_destroy(&list);
    printf("after destroy %i\n", saim_memory_num_allocations());

    saim_memory_cleanup();

    return 0;
}