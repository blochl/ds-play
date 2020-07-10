#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_NODES (1 << 20)
#define MEMBER_SIZE (1 << 8)

typedef struct node_t {
    void* s;
    struct node_t* next;
} node_t;

static node_t* pool;
static uint32_t node_count;

int init_pool() {
    if (pool) {
        /* The pool is already initiated */
        return -1;
    }

    /* A dummy node for enabling the removal of "actual" nodes
     * (which must not be the last one). node_count is not increased for it. */
    pool = (node_t*)malloc(sizeof(node_t));
    if (!pool) {
        return -1;  /* error */
    }
    pool->s = (void*)((node_t**)malloc(MEMBER_SIZE + sizeof(node_t*)) + 1);
    if (!((node_t**)pool->s - 1)) {
        return -1;  /* error */
    }
    *((node_t**)pool->s - 1) = pool;
    pool->next = NULL;

    return 0;
}

int free_pool() {
    node_t* tmp = NULL;

    while (pool) {
        free((node_t**)pool->s - 1);
        tmp = pool;
        pool = pool->next;
        free(tmp);
    }
    node_count = 0;

    return 0;
}

void* add_node() {
    node_t* tmp = NULL;

    if (node_count == MAX_NODES) {
        /* Do whatever we do if no more nodes are allowed */
    }

    tmp = (node_t*)malloc(sizeof(node_t));
    assert(tmp);
    tmp->s = (void*)((node_t**)malloc(MEMBER_SIZE + sizeof(node_t*)) + 1);
    assert((node_t**)tmp->s - 1);

    *((node_t**)tmp->s - 1) = tmp;

    tmp->next = pool;
    pool = tmp;
    node_count++;

    return pool->s;
}

void rm_node(void* x) {
    node_t* curr = *((node_t**)x - 1);
    node_t* tmp = NULL;

    assert(curr->next);

    tmp = curr->next;
    curr->next = tmp->next;
    curr->s = tmp->s;

    free(tmp);
    node_count--;
}
