#include "../includes/types.h"
#include "../includes/cutest/CuTest.h"
#include "../includes/weight_day.h"

void test_weight() {
    /*nDaysTill test*/
    Timeslot testArray[30];
    int i;
    Date testDate = {10, 7, 2020};
    Time end = {12 ,20};
    Time start = {6, 20};
    for (i = 0; i < 30; i++) {
        testArray[i].date = testDate;
        testArray[i].weekday = i % 7;
        testArray[i].shName = NULL;
        testArray[i].startTime = start;
        testArray[i].endTime = end;
        testArray[i].workHours = 6;
        testArray[i].editState = Added;
        testDate = tomorrow(testDate);
    }
    
    testArray[0].editState = ShDay;
    testArray[29].editState = ShDay;
    testArray[10].editState = ShDay;
    /*printf("number of days %d\n", nDaysTill(testArray, 30, 8, ShDay));*/

    testArray[10].editState = Vacation;
    /*printf("number of days %d\n", nDaysTill(testArray, 30, 8, ShDay));
    printf("number of days %d\n", nDaysTill(testArray, 30, 8, Vacation));
    
    printf("number of days %d\n", nDaysTill(testArray, 30, 0, ShDay));
    printf("number of days %d\n", nDaysTill(testArray, 30, 29, ShDay));*/

    for (i = 0; i < 30; i++) {
        printf("Final score: %d\n", weigh(testArray, 30, i));
    }

    /*days between dates test*/
    /*Date d1 = {1, 1, 2020};
    Date d2 = {31, 1, 2020};
    printf("number of days: %d\n", daysBetweenDates(d1, d2));

    d2.day = 1;
    d2.year = 2021;
    printf("number of days: %d\n", daysBetweenDates(d1, d2));*/

    /*tomorrow test*/
    /*Date tomorrowTest = {1, 1, 2020};
    for (int i = 0; i < 400; i++) {
        printf("D: %d, M: %d, Y: %d\n", tomorrowTest.day, tomorrowTest.month, tomorrowTest.year);
        tomorrowTest = tomorrow(tomorrowTest);
    }*/
}
