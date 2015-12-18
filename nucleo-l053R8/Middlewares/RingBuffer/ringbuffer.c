#include "ringbuffer.h"
#include <string.h>

uint16_t RingBuffer_GetFreeSpace(RingBuffer *buf) {
	if(buf->tail == buf->head)
		return RING_BUFFER_LENGTH - 1;

	if(buf->head > buf->tail)
		return RING_BUFFER_LENGTH - ((buf->head - buf->tail) + 1);
	else
		return  (buf->tail - buf->head) - 1;
}

uint16_t RingBuffer_GetDataLength(RingBuffer *buf) {
	return RING_BUFFER_LENGTH - (RingBuffer_GetFreeSpace(buf) + 1);
}


void RingBuffer_Init(RingBuffer *buf) {
	buf->head = buf->tail = 0;
	memset(buf->buf, 0, RING_BUFFER_LENGTH);
}

uint16_t RingBuffer_Read(RingBuffer *buf, uint8_t *data, uint16_t len) {
	uint16_t counter = 0;

	while(buf->tail != buf->head && counter < len) {
		data[counter++] = buf->buf[buf->tail];
		buf->tail = (buf->tail + 1) % RING_BUFFER_LENGTH;
	}
	return counter;
}

uint8_t RingBuffer_Write(RingBuffer *buf, uint8_t *data, uint16_t len) {
	uint16_t counter = 0;
	uint16_t freeSpace = RingBuffer_GetFreeSpace(buf);

	if(freeSpace == 0)
		return RING_BUFFER_FULL;
	else if (freeSpace < len)
		return RING_BUFFER_NO_SUFFICIENT_SPACE;

	while(counter < len) {
		buf->buf[buf->head] = data[counter++];
		buf->head = (buf->head + 1) % RING_BUFFER_LENGTH;
	}
 	return RING_BUFFER_OK;
}
