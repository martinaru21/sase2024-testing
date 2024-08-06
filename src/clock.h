/*-----INCLUDE LIBRARIES-----*/
#include <stdint.h>
#include <stdbool.h>

/*-----DEFINE GLOBAL VARIABLES-----*/
#define TIME_SIZE 6
#define ALARM_SIZE 4
#define SNOOZE_SIZE 5
typedef struct clock_s * clock_t;
typedef void (*clock_event_t) (clock_t clock, bool status); 
clock_t clockCreate(uint16_t ticks, clock_event_t eventHandler);


/*-----FUNCTIONS-----*/
bool clockGetTime(clock_t clock, uint8_t time[], int size);

void clockSetTime(clock_t clock, const uint8_t time[], int size);

void alarmGetTime (clock_t clock, uint8_t alarm[], int size);

void alarmSetTime(clock_t clock, const uint8_t alarm[], int size);

void tick(clock_t clock);

void elapseSecond(clock_t clock);

void elapseMinute(uint8_t time[]);

void elapseHour(uint8_t time[]);

void checkAlarm(clock_t clock);

void stopAlarm(clock_t clock);

void deactivateAlarm(clock_t clock);

void snoozeAlarm(clock_t clock);