#ifndef P1_PROJECT_OUTPUTTABLE_H
#define P1_PROJECT_OUTPUTTABLE_H

#include <stdlib.h>
#include <stdio.h>
#include "types.h"
#include "input.h"
#include "weight_day.h"

char* weekdayToStr(Timeslot ts);
void printHeader (FILE *file,TeamShiftData tsd[]);
int printDates (FILE *file, TeamShiftData tsd[], int modifier);
void printTable(FILE *file, TeamShiftData tsd[]);

#endif
