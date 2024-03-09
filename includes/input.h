#ifndef P1_INPUT_H
#define P1_INPUT_H

#include <stdlib.h>
#include <stdio.h>
#include "types.h"

typedef enum timeslotField {
    DatePart = 0,
    WeekdayPart,
    ShName,
    IsDay,
    IsWeekend,
    IsSHDay,
    Empty,
    ShiftName,
    StartTime,
    EndTime,
    WorkHours
} TimeslotField;

typedef struct TeamShiftData {
    unsigned expectedHours;
    unsigned accumulatedTime;
    TsList timeslotList;
} TeamShiftData;

Weekday readWeekday(const char*);

Time readTime(const char*);

Error readInput(FILE*, TeamShiftData*);

void freeShNameCache();

#endif
