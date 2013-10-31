#include "globals.h"
#include "buttons.h"


static void _update(button_t *button);

void buttons_age() {
  int i;
  bool_t allButtonsUp = true;

  for(i=0; i<BUTTON_COUNT; i++) {
    buttons[i].old = buttons[i].current;
    if (buttons[i].current == DOWN)
      allButtonsUp = false;
  }

  if (allButtonsUp) 
    doublePress = longDoublePress = reallyLongDoublePress = false;
}

void buttons_update() {
  int i;
  bool_t allButtonsDown = true;
  uint32_t time = 0;

  for(i=0; i<BUTTON_COUNT; i++) {
    _update(&buttons[i]);
    if (buttons[i].update_time > time)
      time = buttons[i].update_time;

    if (buttons[i].current == UP)
      allButtonsDown = false;
  }

  if (allButtonsDown) {
    doublePress = true;
    uint32_t duration = clock_ticks - time;
    if (duration > TICKS_PER_SECOND * 4) {
      if (!reallyLongDoublePress) {
        reallyLongDoublePress = true;
      }
    } else if (duration > TICKS_PER_SECOND * 2) {
      if (!longDoublePress) {
        longDoublePress = true;
      }
    }
  }
}

bool_t pressed(button_t *button) {
  return (button->old == DOWN) && (button->current == UP);
}

static void _update(button_t *button) {
  button_state_t state = !(PIND & _BV(button->pin));

  if (state != button->current) {
    if (button->update_time + 1 < clock_ticks) {
      button->old = button->current;
      button->current = state;
      button->update_time = clock_ticks;

      if (state == DOWN) 
        button->down_time = clock_ticks;
    }
  }
}
