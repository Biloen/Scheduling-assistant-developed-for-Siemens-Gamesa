

#ifndef P1_PROJECT_CRITERIA_H
#define P1_PROJECT_CRITERIA_H

#include <stdlib.h>
#include <stdio.h>
#include <types.h>
#include <limits.h>

typedef struct Criteria{
    Date date;
    int forceSchedule;
    TeamName team;
} Criteria;
int dateInCriteria(Date, Criteria*, int);
int checkWeight(int, Timeslot , Criteria*, int);
int getCriteria(FILE*, Criteria**);


#endif //P1_PROJECT_CRITERIA_H
