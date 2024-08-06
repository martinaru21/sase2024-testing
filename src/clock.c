/*-----INCLUDE LIBRARIES-----*/
#include "clock.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>
#include <stdint.h>

/*-----DEFINE GLOBAL VARIABLES-----*/

struct clock_s {
    uint8_t time[TIME_SIZE];
    uint16_t ticks_count;
    uint16_t tick_limit;
    uint8_t alarm_time[TIME_SIZE];
    uint8_t next_alarm_time[TIME_SIZE];
    bool valid;
    bool alarm_state;
    bool alarm_active;
    clock_event_t eventHandler;
};

static struct clock_s instances;

/*-----CONSTRUCTOR | GETTER | SETTER-----*/
clock_t clockCreate(uint16_t ticks, clock_event_t eventHandler) {
    memset(&instances, 0, sizeof(instances));
    instances.ticks_count = 0;
    instances.valid = false;
    instances.alarm_active = false;
    instances.tick_limit = ticks;
    instances.eventHandler = eventHandler;
    return &instances;
}

bool clockGetTime(clock_t clock, uint8_t time[], int size) {
    memcpy(time, clock->time, size);
    return clock->valid;
}

void clockSetTime(clock_t clock, const uint8_t time[], int size) {
    memcpy(clock->time, time, size);
    clock->valid = true;
}

/*-----TIME PASSAGE-----*/
void tick(clock_t clock) {
    clock->ticks_count++;
    if (clock->ticks_count == clock->tick_limit) {
        elapseSecond(clock);
        clock->ticks_count = 0;
    }
}

void elapseSecond(clock_t clock) {
    if (clock->time[5] < 9) {
        clock->time[5]++;
    } else {
        clock->time[5] = 0;
        if (clock->time[4] < 5) {
            clock->time[4]++;
        } else {
            clock->time[4] = 0;
            checkAlarm(clock);
            elapseMinute(clock->time);
        }
    }
}

void elapseMinute(uint8_t time[]) {
    if (time[3] < 9) {
        time[3]++;
    } else {
        time[3] = 0;
        if (time[2] < 5) {
            time[2]++;
        } else {
            time[2] = 0;
            elapseHour(time);
        }
    }
}

void elapseHour(uint8_t time[]) {
    if (time[1] < 9) {
        if (time[0] == 2 && time[1] == 3) {
            memset(time, 0, TIME_SIZE);
        } else {
            time[1]++;
        }
    } else {
        time[1] = 0;
        time[0]++;
    }
}

/*-----ALARM HANDLING-----*/
void alarmSetTime(clock_t clock, const uint8_t alarm[], int size) {
    clock->alarm_active = true;
    memcpy(clock->alarm_time, alarm, size);
    memcpy(clock->next_alarm_time, alarm, size);
}

void deactivateAlarm(clock_t clock){
    clock->alarm_active = false;
}

void checkAlarm(clock_t clock) {
    if (clock->alarm_active && memcmp(clock->next_alarm_time, clock->time, ALARM_SIZE)) {
        clock->eventHandler(clock, true);
        clock->alarm_state = true;
    }
}

void stopAlarm(clock_t clock) {
    clock->eventHandler(clock, false);
    clock->alarm_state = false;
}

void snoozeAlarm(clock_t clock) {
    if (clock->alarm_state == true) {
        stopAlarm(clock);
        memcpy(clock->next_alarm_time, clock->time, TIME_SIZE);

        for (int i = 0; i < SNOOZE_SIZE; i++) {
            elapseMinute(clock->next_alarm_time);
        }
    }
}