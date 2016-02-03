#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <stdint.h>

typedef struct {
  uint8_t size;
  uint8_t count;
  uint8_t head;
  uint16_t *data;
} ringbuffer_uint16_t;

typedef struct {
  uint8_t size;
  uint8_t count;
  uint8_t head;
  double *data;
} ringbuffer_double_t;

ringbuffer_uint16_t* ringbuffer_new_uint16(int size);
void ringbuffer_clear_uint16(ringbuffer_uint16_t *ringbuffer);
void ringbuffer_append_uint16(ringbuffer_uint16_t *ringbuffer, uint16_t data);
void ringbuffer_dump_uint16(ringbuffer_uint16_t *ringbuffer);

ringbuffer_double_t* ringbuffer_new_double(int size);
void ringbuffer_clear_double(ringbuffer_double_t *ringbuffer);
void ringbuffer_append_double(ringbuffer_double_t *ringbuffer, double data);
void ringbuffer_dump_double(ringbuffer_double_t *ringbuffer);

#endif
