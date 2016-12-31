#include "../../util/saim_set.h"
#include "../../util/saim_memory.h"

#include <stdlib.h>
#include <stdio.h>

static int N = 10;

int compare_func(const void* a,const void* b) {
  if( *(int*)a > *(int*)b) return(1);
  if( *(int*)a < *(int*)b) return(-1);
  return(0);
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
    saim_set * set;
    saim_set_node * node;

    saim_memory_init();

    set = saim_set_create(compare_func, destroy_func);
    printf("after set_create %i\n", saim_memory_num_allocations());

    node = saim_set_get_first(set);
    if (node != set->nil)
        printf("fuck this is not empty\n");

    int i, *vals[N];

    srand(12345);

    for (i = 0; i < N; i++) {
        vals[i] = getnum(i);
    }

    for (i = 0; i < N; i++) {
        node = saim_set_insert(set, vals[i]);
    }
    printf("after insertions %i\n", saim_memory_num_allocations());
    printf("set size = %u\n", set->size);

    i = 0;
    node = saim_set_get_first(set);
    while (node != set->nil)
    {
        printf("%i, ", *(int*)node->data);
    	++i;
    	node = saim_set_get_next(set, node);
    }
    putc('\n', stdout);
    printf("after enumeration %i\n", saim_memory_num_allocations());
    printf("nodes count = %i\n", i);

    for (i = 0; i < N; i++) {
        node = saim_set_search(set, vals[i]);
        saim_set_delete(set, node);
    }
    printf("after delete %i\n", saim_memory_num_allocations());
    printf("set size = %u\n", set->size);

    //saim_set_destroy(set);
    saim_set_destroy(set);
    printf("after destroy %i\n", saim_memory_num_allocations());

    saim_memory_cleanup();

    return 0;
}