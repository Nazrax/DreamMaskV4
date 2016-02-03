#ifndef _DETECTOR_H_
#define _DETECTOR_H_

#include <stdint.h>

#include "ringbuffer.h"
#include "types.h"

#define DETECTOR_MOVEMENT_DETECTION_WINDOW 10
#define DETECTOR_THRESHOLD_MINUTES 10
#define DETECTOR_THRESHOLD_MULTIPLIER 5
#define DETECTOR_THRESHOLD_MINIMUM 10
#define DETECTOR_TICK_TIMEOUT 90*TICKS_PER_SECOND
#define DETECTOR_REM_THRESHOLD 100
#define TICKS_PER_SECOND 8

#ifdef DEBUG
#undef DETECTOR_TICK_TIMEOUT
#undef DETECTOR_REM_THRESHOLD

#define DETECTOR_TICK_TIMEOUT 15*TICKS_PER_SECOND
#define DETECTOR_REM_THRESHOLD 5
#endif


void detector_init(void);
void detector_update(uint16_t left_reading, uint16_t right_reading);

typedef struct {
  ringbuffer_uint16_t *readingBuffer;
  ringbuffer_double_t *movementAvgBuffer;
  uint16_t threshold;
  uint32_t movementSum;
  uint32_t movementCount;
  uint16_t lastMovement;
  uint16_t ticksSinceMovement;
  uint8_t cooldown;
  bool_t dreaming;
} reading_data_t;

#endif
