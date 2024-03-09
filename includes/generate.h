#ifndef P1_GENERATE_H
#define P1_GENERATE_H

#include <types.h>
#include <weight_day.h>
#include <input.h>
#include <math.h>

void addVacation(TeamShiftData* schedule);

void adjustSchedule(TeamShiftData*, char* [5]);

void calculateWeekNum(TsList*);

#endif
