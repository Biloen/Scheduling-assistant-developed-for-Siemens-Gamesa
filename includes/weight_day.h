#ifndef P1_WEIGHT_DAY_H
#define P1_WEIGHT_DAY_H

#include <stdio.h>
#include "types.h"

int dayToAdd(Timeslot slots[], int slotLen, char* modifiers[]);
int dayToRemove(Timeslot slots[], int slotLen, char* modifiers[]);
int weigh(Timeslot slots[], int slotsLen, char* modifiers[], int workHours, int slotIndex);
int hoursInWeek(Timeslot slots[], int slotLen, int slotIndex);
int nDaysTill(Timeslot slots[], int slotsLen, int slotIndex, int editStateDay);
int daysBetweenDates(Date d1, Date d2);
int daysInMonth(int month, int year);
int isLeapYear(int year);
Date tomorrow(Date date);
int dateBefore(Date d1, Date d2);

#endif
