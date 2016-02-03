#ifndef _ALARM_H_
#define _ALARM_H_

#ifdef DEBUG

#define ALARM_INITIAL_TIME (30) // Time 
#define ALARM_WAIT_TIME (15)
#define ALARM_SNOOZE_TIME (30)

#else

//#define ALARM_INITIAL_TIME (60 * 60 * 8 + 60 * 15) // 8 hours (60 * 60 * 8) + 15 minutes
//#define ALARM_INITIAL_TIME (3 * 60 * 60 + 30 * 60) // 3.5 hours
#define ALARM_INITIAL_TIME (5 * 60 * 60) // 5 hours
//#define ALARM_WAIT_TIME (2 * 60) // 2 minutes
#define ALARM_WAIT_TIME (10 * 60) // 10 minutes
#define ALARM_SNOOZE_TIME (60 * 30) // 20 minutes

#endif

//#define ALARM_STRING "DREAM DREAM"
#define ALARM_STRING "O O"

extern uint16_t alarm_time;

void alarm_init(void);
void alarm_handle(void);
void alarm_set_relative(uint8_t, uint8_t);
void alarm_snooze(uint16_t);

#endif
