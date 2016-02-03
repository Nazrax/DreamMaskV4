#include "ringbuffer.h"

#include <stdio.h>
#include <stdlib.h>

void ringbuffer_clear_uint16(ringbuffer_uint16_t *ringbuffer) {
  ringbuffer->head = 0;
  ringbuffer->count = 0;
}

ringbuffer_uint16_t* ringbuffer_new_uint16(int size) {
  ringbuffer_uint16_t *ringbuffer = (ringbuffer_uint16_t*)malloc(sizeof(ringbuffer_uint16_t));

  ringbuffer->data = calloc(size, sizeof(uint16_t));
  ringbuffer->size = size;
  ringbuffer_clear_uint16(ringbuffer);

  return ringbuffer;
}

void ringbuffer_append_uint16(ringbuffer_uint16_t *ringbuffer, uint16_t data) {
  ringbuffer->data[ringbuffer->head] = data;
  ringbuffer->head = (ringbuffer->head + 1) % ringbuffer->size;
  if (ringbuffer->count < ringbuffer->size) {
    ringbuffer->count++;
  }
}

void ringbuffer_dump_uint16(ringbuffer_uint16_t *ringbuffer) {
  int i, ri;
  for(i=0; i<ringbuffer->count; i++) {
    if (ringbuffer->size == ringbuffer->count) {
      ri = (i + ringbuffer->head) % ringbuffer->size;
    } else {
      ri = i;
    }
    printf("%02d ", ringbuffer->data[ri]);
  }
  printf("\n");
}

void ringbuffer_clear_double(ringbuffer_double_t *ringbuffer) {
  ringbuffer->head = 0;
  ringbuffer->count = 0;
}  

ringbuffer_double_t* ringbuffer_new_double(int size) {
  ringbuffer_double_t *ringbuffer = (ringbuffer_double_t*)malloc(sizeof(ringbuffer_double_t));
  ringbuffer->data = calloc(size, sizeof(double));
  ringbuffer->size = size;
  ringbuffer_clear_double(ringbuffer);

  return ringbuffer;
}

void ringbuffer_append_double(ringbuffer_double_t *ringbuffer, double data) {
  ringbuffer->data[ringbuffer->head] = data;
  ringbuffer->head = (ringbuffer->head + 1) % ringbuffer->size;
  if (ringbuffer->count < ringbuffer->size) {
    ringbuffer->count++;
  }
}

void ringbuffer_dump_double(ringbuffer_double_t *ringbuffer) {
  int i, ri;
  for(i=0; i<ringbuffer->count; i++) {
    if (ringbuffer->size == ringbuffer->count) {
      ri = (i + ringbuffer->head) % ringbuffer->size;
    } else {
      ri = i;
    }
    printf("%f ", ringbuffer->data[ri]);
  }
  printf("\n");
}
