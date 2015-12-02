#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct {
    void **buffer; // a buffer of size length
    int length;
    pthread_mutex_t lock; // used to read or write to the buffer
    int head, tail; // index of start and end of the queue in the buffer
    sem_t spaceleft, currentsize; // acquired to enqueue or dequeue
} RingBuffer;

RingBuffer *RingBuffer_new(int length) {
    // buffer length should be a power of 2
    if (length % 2 != 0) {
        return NULL;
    }

    RingBuffer *buf = malloc(sizeof(RingBuffer));
    buf->length = length;
    buf->head = 0;
    buf->tail = 0;
    buf->buffer = malloc(sizeof(void*));

    // buffer starts with all its space left
    sem_init(&buf->spaceleft, 0, length);

    // buffer starts with a size of zero
    sem_init(&buf->currentsize, 0, 0);

    return buf;
}

void RingBuffer_enqueue(RingBuffer *buf, void *value) {
    // wait for buffer to have space
    sem_wait(&buf->spaceleft);

    // lock and store the value at head
    pthread_mutex_lock(&buf->lock);
    buf->buffer[ (buf->head++) & (buf->length - 1) ] = value;
    pthread_mutex_unlock(&buf->lock);

    // the buffer now has one more value
    sem_post(&buf->currentsize);
}

void *RingBuffer_dequeue(RingBuffer *buf) {
    // wait for the buffer to have a value
    sem_wait(&buf->currentsize);

    // lock and remove the tail value
    pthread_mutex_lock(&buf->lock);
    void *value = buf->buffer[ (buf->tail++) & (buf->length - 1) ];
    pthread_mutex_unlock(&buf->lock);

    // the buffer now has one less space
    sem_post(&buf->spaceleft);

    return value;
}
