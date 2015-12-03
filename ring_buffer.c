#include <errno.h>
#include <stdlib.h>
#include "ring_buffer.h"

RingBuffer *RingBuffer_new(int length) {
    // buffer length should be a power of 2
    if (length % 2 != 0) {
        return NULL;
    }

    RingBuffer *buf = malloc(sizeof(RingBuffer));
    buf->length = length;
    buf->head = 0;
    buf->tail = 0;
    buf->buffer = malloc(sizeof(void*) * length);

    pthread_mutex_init(&buf->lock, NULL);

    // buffer starts with all its space left
    sem_init(&buf->spaceleft, 0, length);

    // buffer starts with a size of zero
    sem_init(&buf->currentsize, 0, 0);

    return buf;
}

void RingBuffer_free(RingBuffer *buf) {
    free(buf->buffer);
    free(buf);
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

int RingBuffer_enqueue_timed(RingBuffer *buf, void *value,
        const struct timespec *abs_timeout) {
    int err;

    // wait for buffer to have space
    if (abs_timeout == NULL) {
        err = sem_trywait(&buf->spaceleft);
    } else {
        err = sem_timedwait(&buf->spaceleft, abs_timeout);
    }

    if (err) {
        return err;
    }

    // lock and store the value at head
    pthread_mutex_lock(&buf->lock);
    buf->buffer[ (buf->head++) & (buf->length - 1) ] = value;
    pthread_mutex_unlock(&buf->lock);

    // the buffer now has one more value
    sem_post(&buf->currentsize);

    return 0;
}

int RingBuffer_dequeue_timed(RingBuffer *buf, void **value,
        const struct timespec *abs_timeout) {
    int err;

    // wait for the buffer to have a value
    if (abs_timeout == NULL) {
        err = sem_trywait(&buf->currentsize);
    } else {
        err = sem_timedwait(&buf->currentsize, abs_timeout);
    }

    if (err) {
        return err;
    }

    // lock and remove the tail value
    pthread_mutex_lock(&buf->lock);
    *value = buf->buffer[ (buf->tail++) & (buf->length - 1) ];
    pthread_mutex_unlock(&buf->lock);

    // the buffer now has one less space
    sem_post(&buf->spaceleft);

    return 0;
}

