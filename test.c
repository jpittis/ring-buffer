#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include"ring_buffer.h"

void test_RingBuffer_is_a_fifo_queue() {
    RingBuffer *buf = RingBuffer_new(4);

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    // enqueue the numbers
    RingBuffer_enqueue(buf, &a);
    RingBuffer_enqueue(buf, &b);
    RingBuffer_enqueue(buf, &c);
    RingBuffer_enqueue(buf, &d);

    // confirm they come out in the same order as enqueued
    assert(1 == *(int*)RingBuffer_dequeue(buf));
    assert(2 == *(int*)RingBuffer_dequeue(buf));
    assert(3 == *(int*)RingBuffer_dequeue(buf));
    assert(4 == *(int*)RingBuffer_dequeue(buf));
}

void *concurrent_dequeue_four(void *b) {
    RingBuffer *buf = (RingBuffer*)b;

    // attempt to dequeue four numbers
    // they should block until a concurrent enqueue
    assert(1 == *(int*)RingBuffer_dequeue(buf));
    assert(2 == *(int*)RingBuffer_dequeue(buf));
    assert(3 == *(int*)RingBuffer_dequeue(buf));
    assert(4 == *(int*)RingBuffer_dequeue(buf));

    return NULL;
}

void test_RingBuffer_concurrent_dequeue_waits_for_enqueue() {
    RingBuffer *buf = RingBuffer_new(4);

    // start concurrent dequeue
    pthread_t concurrent_dequeue;
    if (pthread_create(&concurrent_dequeue, NULL,
                concurrent_dequeue_four, buf)) {
        fprintf(stderr, "error: failed to create thread");
        exit(1);
    }

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    // enqueue four numbers to be dequeued concurrently
    RingBuffer_enqueue(buf, &a);
    RingBuffer_enqueue(buf, &b);
    RingBuffer_enqueue(buf, &c);
    RingBuffer_enqueue(buf, &d);

    if (pthread_join(concurrent_dequeue, NULL)) {
        fprintf(stderr, "error: failed to join thread");
        exit(1);
    }
}

void test_RingBuffer_concurrent_enqueue_waits_for_dequeue() {
    RingBuffer *buf = RingBuffer_new(4);

    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int e = 5;
    int f = 6;
    int g = 7;
    int h = 8;

    // enqueue four numbers to fill queue
    RingBuffer_enqueue(buf, &a);
    RingBuffer_enqueue(buf, &b);
    RingBuffer_enqueue(buf, &c);
    RingBuffer_enqueue(buf, &d);

    // start concurrent dequeue
    pthread_t concurrent_dequeue;
    if (pthread_create(&concurrent_dequeue, NULL,
                concurrent_dequeue_four, buf)) {
        fprintf(stderr, "error: failed to create thread");
        exit(1);
    }

    // attempt to enqueue four more numbers
    RingBuffer_enqueue(buf, &e);
    RingBuffer_enqueue(buf, &f);
    RingBuffer_enqueue(buf, &g);
    RingBuffer_enqueue(buf, &h);

    if (pthread_join(concurrent_dequeue, NULL)) {
        fprintf(stderr, "error: failed to join thread");
        exit(1);
    }

    // confirm the last four numbers were enqueued
    assert(5 == *(int*)RingBuffer_dequeue(buf));
    assert(6 == *(int*)RingBuffer_dequeue(buf));
    assert(7 == *(int*)RingBuffer_dequeue(buf));
    assert(8 == *(int*)RingBuffer_dequeue(buf));
}

void test_RingBuffer_does_not_wait_to_dequeue() {
    RingBuffer *buf = RingBuffer_new(2);
    void *value = RingBuffer_dequeue_timed(buf, NULL);
    assert(value == NULL);
}

void test_RingBuffer_does_not_wait_to_enqueue() {
    RingBuffer *buf = RingBuffer_new(2);

    int a = 1;
    int b = 2;
    int c = 3;

    // fille the queue
    RingBuffer_enqueue(buf, &a);
    RingBuffer_enqueue(buf, &b);

    int err = RingBuffer_enqueue_timed(buf, &c, NULL);
    assert(err != EAGAIN);
}

int main() {
    test_RingBuffer_is_a_fifo_queue();
    test_RingBuffer_concurrent_dequeue_waits_for_enqueue();
    test_RingBuffer_concurrent_enqueue_waits_for_dequeue();
    test_RingBuffer_does_not_wait_to_dequeue();
    test_RingBuffer_does_not_wait_to_enqueue();
}
