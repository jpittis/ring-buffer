#include<assert.h>
#include"ring_buffer.h"

int main() {
    RingBuffer *buf = RingBuffer_new(4);

    // insert numbers into queue
    int a = 1;
    int b = 2;
    int c = 3;
    RingBuffer_enqueue(buf, &a);
    RingBuffer_enqueue(buf, &b);
    RingBuffer_enqueue(buf, &c);

    // confirm it works as a queue
    assert(&a == RingBuffer_dequeue(buf));
    assert(&b == RingBuffer_dequeue(buf));
    assert(&c == RingBuffer_dequeue(buf));
}
