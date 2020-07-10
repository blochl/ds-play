#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#define MAX_NODES (1 << 20)
#define MEMBER_SIZE (1 << 8)

typedef struct node_t {
    void* s;  /* Must come first! */
    struct node_t* next;
} node_t;

static node_t* pool;
static uint32_t node_count;
pthread_mutex_t lock;
pthread_mutex_t init_lock;
pthread_mutex_t add_lock;

void* add_node() {
    node_t* tmp = NULL;

    pthread_mutex_lock(&add_lock);
    if (node_count == MAX_NODES) {
        /* Do whatever we do if no more nodes are allowed */
    }

    tmp = (node_t*)malloc(sizeof(node_t));
    assert(tmp);
    tmp->s = malloc(MEMBER_SIZE);
    assert(tmp->s);

    pthread_mutex_lock(&lock);
    tmp->next = pool;
    pool = tmp;
    node_count++;
    pthread_mutex_unlock(&lock);
    pthread_mutex_unlock(&add_lock);

    return tmp->s;
}

void rm_node(void* x) {
    node_t* curr = (node_t*)(&x);
    node_t* tmp = NULL;

    assert(curr->next);

    tmp = curr->next;

    pthread_mutex_lock(&lock);
    curr->next = tmp->next;
    free(curr->s);
    curr->s = tmp->s;
    node_count--;
    pthread_mutex_unlock(&lock);

    free(tmp);
}

int init_pool() {
    pthread_mutex_lock(&init_lock);
    if (pool) {
        /* The pool is already initiated */
        return -1;
    }

    /* A dummy node for enabling the removal of "actual" nodes
     * (which must not be the last one). node_count is not increased for it. */
    add_node();
    node_count = 0;
    pthread_mutex_unlock(&init_lock);

    return 0;
}

int free_pool() {
    node_t* tmp = NULL;

    pthread_mutex_lock(&init_lock);
    pthread_mutex_lock(&lock);
    while (pool) {
        free(pool->s);
        tmp = pool;
        pool = pool->next;
        free(tmp);
    }
    node_count = 0;
    pthread_mutex_unlock(&lock);
    pthread_mutex_unlock(&init_lock);

    return 0;
}
