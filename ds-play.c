#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_NODES (1 << 20)
#define MEMBER_SIZE (1 << 8)

typedef struct node_t {
    void* s;  /* Must come first! */
    struct node_t* next;
} node_t;

static node_t* pool;
static uint32_t node_count;

void* add_node() {
    node_t* tmp = NULL;

    if (node_count == MAX_NODES) {
        /* Do whatever we do if no more nodes are allowed */
    }

    tmp = (node_t*)malloc(sizeof(node_t));
    assert(tmp);
    tmp->s = malloc(MEMBER_SIZE);
    assert(tmp->s);

    tmp->next = pool;
    pool = tmp;
    node_count++;

    return pool->s;
}

void rm_node(void* x) {
    node_t* curr = (node_t*)(&x);
    node_t* tmp = NULL;

    assert(curr->next);

    tmp = curr->next;
    curr->next = tmp->next;
    free(curr->s);
    curr->s = tmp->s;

    free(tmp);
    node_count--;
}

int init_pool() {
    if (pool) {
        /* The pool is already initiated */
        return -1;
    }

    /* A dummy node for enabling the removal of "actual" nodes
     * (which must not be the last one). node_count is not increased for it. */
    add_node();
    node_count = 0;

    return 0;
}

int free_pool() {
    node_t* tmp = NULL;

    while (pool) {
        free(pool->s);
        tmp = pool;
        pool = pool->next;
        free(tmp);
    }
    node_count = 0;

    return 0;
}
