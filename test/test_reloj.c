/*-----INCLUDE LIBRARIES-----*/
#include "unity.h"
#include "clock.h"
#include <stdint.h>

/*-----DEFINE GLOBAL VARIABLES-----*/
#define TICKS_PER_SECOND 5
#define TICKS_PER_MINUTE 60 * TICKS_PER_SECOND
#define TICKS_PER_HOUR 60 * TICKS_PER_MINUTE
#define TICKS_PER_DAY 24 * TICKS_PER_HOUR
clock_t clock;
static bool alarm_on;

/*-----FUNCTIONS-----*/
void elapseTicks(uint32_t ticks){
    for (int i = 0; i < ticks; i++){
        tick(clock);
    }
}

void alarmEvent(clock_t reloj, bool status){
    alarm_on = status;
}

void setUp(){
    static const uint8_t INITIALIZER[] = {0, 0, 0, 0, 0, 0};
    alarm_on = false;
    clock = clockCreate(TICKS_PER_SECOND, alarmEvent);
    clockSetTime(clock, INITIALIZER, sizeof(INITIALIZER));
}


/*-----GENERAL TEST-----*/
void test_start_up(void){
    static const uint8_t VALID[] = {0, 0, 0, 0, 0, 0};        
    uint8_t time[TIME_SIZE] = {0xFF};                          
    clock = clockCreate(TICKS_PER_SECOND, alarmEvent);              
    
    TEST_ASSERT_FALSE(clockGetTime(clock, time, TIME_SIZE));    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);      
}

void test_set_time(){
    static const uint8_t VALID[] = {0, 1, 0, 0, 0, 0};  
    uint8_t time[TIME_SIZE] = {0xFF};   
    clockSetTime(clock, VALID, TIME_SIZE);        

    TEST_ASSERT_TRUE(clockGetTime(clock, time, TIME_SIZE));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}


/*-----TIME PASSAGE TEST-----*/
void test_one_second(void){
    static const uint8_t VALID[] = {0, 0, 0, 0, 0, 1};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_SECOND);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_ten_seconds(void){
    static const uint8_t VALID[] = {0, 0, 0, 0, 1, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_SECOND*10);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_one_minute(void){
    static const uint8_t VALID[] = {0, 0, 0, 1, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_MINUTE);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_ten_minutes(void){
    static const uint8_t VALID[] = {0, 0, 1, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_MINUTE*10);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_one_hour(void){
    static const uint8_t VALID[] = {0, 1, 0, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_HOUR);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_ten_hours(void){
    static const uint8_t VALID[] = {1, 0, 0, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_HOUR*10);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}

void test_one_day(void){
    static const uint8_t VALID[] = {0, 0, 0, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    elapseTicks(TICKS_PER_DAY);
    clockGetTime(clock, time, TIME_SIZE);
    
    TEST_ASSERT_EQUAL_UINT8_ARRAY(VALID, time, TIME_SIZE);
}


/*-----ALARM TEST-----*/
void test_alarm_turn_on(void){
    static const uint8_t ALARM[] = {0, 0, 0, 5, 0, 0};
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_MINUTE*5);
    TEST_ASSERT_TRUE(alarm_on);
}

void test_alarm_turn_off(void){
    static const uint8_t ALARM[] = {0, 0, 0, 5, 0, 0};
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_MINUTE*5);
    stopAlarm(clock);
    TEST_ASSERT_FALSE(alarm_on);
}

void test_alarm_deactivate(void){
    static const uint8_t ALARM[] = {0, 0, 0, 5, 0, 0};
    alarmSetTime(clock, ALARM, TIME_SIZE);
    deactivateAlarm(clock);
    elapseTicks(TICKS_PER_MINUTE*5);
    TEST_ASSERT_FALSE(alarm_on);
}


/*-----SNOOZE TEST-----*/
void test_alarm_snooze(void){
    static const uint8_t ALARM[] = {0, 0, 0, 5, 0, 0};
    static const uint8_t SNOOZE[] = {0, 0, 1, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_MINUTE*5);
    snoozeAlarm(clock);

    TEST_ASSERT_FALSE(alarm_on);

    elapseTicks(TICKS_PER_MINUTE*5);
    clockGetTime(clock, time, TIME_SIZE);

    TEST_ASSERT_TRUE(alarm_on);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(SNOOZE, time, TIME_SIZE);
}

void test_alarm_snooze_overflow(void){
    static const uint8_t ALARM[] = {0, 0, 5, 6, 0, 0};
    static const uint8_t SNOOZE[] = {0, 1, 0, 1, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_MINUTE*56);
    snoozeAlarm(clock);

    TEST_ASSERT_FALSE(alarm_on);

    elapseTicks(TICKS_PER_MINUTE*5);
    clockGetTime(clock, time, TIME_SIZE);

    TEST_ASSERT_TRUE(alarm_on);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(SNOOZE, time, TIME_SIZE);
}

void test_alarm_snooze_next_day(void){
    static const uint8_t ALARM[] = {0, 1, 0, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_HOUR);
    snoozeAlarm(clock);

    elapseTicks(TICKS_PER_MINUTE*5);
    stopAlarm(clock);

    elapseTicks(TICKS_PER_HOUR*23+TICKS_PER_MINUTE*55);
    clockGetTime(clock, time, TIME_SIZE);

    TEST_ASSERT_TRUE(alarm_on);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(ALARM, time, TIME_SIZE);
}

void test_alarm_snooze_invalid(void){
    static const uint8_t ALARM[] = {0, 1, 0, 0, 0, 0};
    uint8_t time[TIME_SIZE] = {0xFF};
    
    alarmSetTime(clock, ALARM, TIME_SIZE);
    elapseTicks(TICKS_PER_MINUTE*10);
    snoozeAlarm(clock);

    TEST_ASSERT_FALSE(alarm_on);
}

void test_alarm_snooze_alarm_off(void){
    snoozeAlarm(clock);
    TEST_ASSERT_FALSE(alarm_on);
}