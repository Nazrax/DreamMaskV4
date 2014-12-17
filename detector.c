#include <stdlib.h>
#include <stdio.h>

#include "types.h"

#include "detector.h"
#include "ringbuffer.h"

reading_data_t *leftData, *rightData;

uint16_t counter;

uint16_t calculate_movement(ringbuffer_uint16_t* ringbuffer, uint16_t reading);
void update_movements(reading_data_t *data, uint16_t reading);
void update_threshold(reading_data_t *data);

reading_data_t* new_data(void) {
  reading_data_t *data = calloc(1, sizeof(reading_data_t));

  data->readingBuffer = ringbuffer_new_uint16(DETECTOR_MOVEMENT_DETECTION_WINDOW);
  data->movementAvgBuffer = ringbuffer_new_double(DETECTOR_THRESHOLD_MINUTES * 2);
  data->threshold = 30;

  int i;
  for(i=0; i<DETECTOR_THRESHOLD_MINUTES*2; i++) {
    data->movementAvgBuffer->data[i] = 10;
  }

  return data;
}

void detector_init() {
  leftData = new_data();
  rightData = new_data();
}

void detector_update(uint16_t leftReading, uint16_t rightReading) {
  update_movements(leftData, leftReading);
  update_movements(rightData, rightReading);

  counter++;
  if (counter > TICKS_PER_SECOND * 60) {
    update_threshold(leftData);
    update_threshold(rightData);
    counter = 0;
  }
}

void update_movements(reading_data_t *data, uint16_t reading) {
  ringbuffer_append_uint16(data->readingBuffer, reading);
  uint16_t movement = calculate_movement(data->readingBuffer, reading);
  data->lastMovement = movement;
  data->movementSum += movement;
  if (data->cooldown > 0) {
    data->cooldown--;
  } else {
    if (movement > data->threshold) {
      data->movementCount++;
      data->cooldown = DETECTOR_MOVEMENT_DETECTION_WINDOW-1;
      data->ticksSinceMovement = 0;
    }
  }

  if (data->movementCount > 0 && data->ticksSinceMovement > DETECTOR_TICK_TIMEOUT) {
    data->movementCount = 0;
  }
  data->dreaming = (data->movementCount > DETECTOR_REM_THRESHOLD);

  data->ticksSinceMovement++;
}

void update_threshold(reading_data_t *data) {
  int i, ri;
  double sum = 0;
  double avg = 1.0 * data->movementSum / counter;
  data->movementSum = 0;
  ringbuffer_append_double(data->movementAvgBuffer, avg);

  for(i=0; i<DETECTOR_THRESHOLD_MINUTES; i++) {
    ri = (i + data->movementAvgBuffer->head) % data->movementAvgBuffer->size;
    sum += data->movementAvgBuffer->data[ri];
  }
  uint16_t newThreshold = sum / DETECTOR_THRESHOLD_MINUTES * DETECTOR_THRESHOLD_MULTIPLIER;
  if (newThreshold < DETECTOR_THRESHOLD_MINIMUM) {
    newThreshold = DETECTOR_THRESHOLD_MINIMUM;
  }
  data->threshold = newThreshold;
}

uint16_t calculate_movement(ringbuffer_uint16_t* ringbuffer, uint16_t reading) {
  uint8_t i;
  uint16_t maxMovement = 0;

  for(i=0; i<ringbuffer->size; i++) {
    uint16_t movement = abs(ringbuffer->data[i] - reading);

    if (movement > maxMovement) {
      maxMovement = movement;
    }
  }
  return maxMovement;
}
