#include <pthread.h>
#include <semaphore.h>

typedef struct {
    void *buffer;
    int length;
    pthread_mutex_t lock;
    int head, tail;
    sem_t spaceleft, currentsize;
} RingBuffer;

RingBuffer *RingBuffer_new(int length);

void RingBuffer_enqueue(RingBuffer *buf, void *value);

void *RingBuffer_dequeue(RingBuffer *buf);
