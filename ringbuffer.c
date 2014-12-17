#include "ringbuffer.h"

#include <stdio.h>
#include <stdlib.h>

ringbuffer_uint16_t* ringbuffer_new_uint16(int size) {
  ringbuffer_uint16_t *ringbuffer = (ringbuffer_uint16_t*)malloc(sizeof(ringbuffer_uint16_t));

  ringbuffer->head = 0;
  ringbuffer->size = size;
  ringbuffer->data = calloc(size, sizeof(uint16_t));

  return ringbuffer;
}

void ringbuffer_append_uint16(ringbuffer_uint16_t *ringbuffer, uint16_t data) {
  ringbuffer->data[ringbuffer->head] = data;
  ringbuffer->head = (ringbuffer->head + 1) % ringbuffer->size;
}

void ringbuffer_dump(ringbuffer_uint16_t *ringbuffer) {
  int i, ri;
  for(i=0; i<ringbuffer->size; i++) {
    ri = (i + ringbuffer->head) % ringbuffer->size;
    printf("%d ", ringbuffer->data[ri]);
  }
  printf("\n");
}

ringbuffer_double_t* ringbuffer_new_double(int size) {
  ringbuffer_double_t *ringbuffer = (ringbuffer_double_t*)malloc(sizeof(ringbuffer_double_t));

  ringbuffer->head = 0;
  ringbuffer->size = size;
  ringbuffer->data = calloc(size, sizeof(double));

  return ringbuffer;
}

void ringbuffer_append_double(ringbuffer_double_t *ringbuffer, double data) {
  ringbuffer->data[ringbuffer->head] = data;
  ringbuffer->head = (ringbuffer->head + 1) % ringbuffer->size;
}

void ringbuffer_dump_double(ringbuffer_double_t *ringbuffer) {
  int i, ri;
  for(i=0; i<ringbuffer->size; i++) {
    ri = (i + ringbuffer->head) % ringbuffer->size;
    printf("%f ", ringbuffer->data[ri]);
  }
  printf("\n");
}
