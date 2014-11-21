#ifndef _ALARM_H_
#define _ALARM_H_

#ifdef DEBUG

#define INITIAL_ALARM_TIME (5)
#define ALARM_ACTIVE_TIME (5)
#define ALARM_WAIT_TIME (15)
#define SNOOZE_TIME (30)

#else

/*
#define INITIAL_ALARM_TIME (18000) // 5 hours (60 * 60 * 5)
#define ALARM_ACTIVE_TIME (20) // 10 seconds???
#define ALARM_WAIT_TIME (300) // 5 minutes
#define SNOOZE_TIME (60*30) // 30 minutes
*/


#define INITIAL_ALARM_TIME (60 * 60 * 8 + 60 * 15) // 8 hours (60 * 60 * 8) + 15 minutes
//#define INITIAL_ALARM_TIME (15)
#define ALARM_ACTIVE_TIME (15)
#define ALARM_WAIT_TIME (15)
#define SNOOZE_TIME (60 * 20) // 20 minutes



#endif

extern uint16_t alarm_time;

void alarm_init(void);
void alarm_handle(void);
void alarm_set_relative(uint8_t, uint8_t);
void alarm_snooze(void);

#endif
