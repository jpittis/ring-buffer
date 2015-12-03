#include <pthread.h>
#include <semaphore.h>

typedef struct {
    void **buffer; // a buffer of size length
    int length;
    pthread_mutex_t lock; // used to read or write to the buffer
    int head, tail; // index of start and end of the queue in the buffer
    sem_t spaceleft, currentsize; // acquired to enqueue or dequeue
} RingBuffer;

RingBuffer *RingBuffer_new(int length);

void RingBuffer_enqueue(RingBuffer *buf, void *value);

void *RingBuffer_dequeue(RingBuffer *buf);

int RingBuffer_enqueue_timed(RingBuffer *buf, void *value, const struct timespec *abs_timeout);

int RingBuffer_dequeue_timed(RingBuffer *buf, void **value, const struct timespec *abs_timeout);
