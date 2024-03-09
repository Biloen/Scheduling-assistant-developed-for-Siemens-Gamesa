#include<generate.h>

/* Adds vacation-tag for the basic vacation days - summer and winter */
void addVacation(TeamShiftData *schedule) {
    /* In december its from the 23rd, till the 2nd jan
     * We go through every day till
     */
    Date date = {.day = 23, .month = 12};

    /* Counts up till the 23rd December */
    int i = 0;
    while (schedule->timeslotList.storage[i].date.day != date.day ||
           schedule->timeslotList.storage[i].date.month != date.month) {
        i++;
    }

    /* Inserts the vacation-worktime since it is paid vacation - 426 (7.1 hours) is for primo and 408 (6.8 hours) is for ultimo */
    schedule[0].accumulatedTime += 5*426;
    schedule[1].accumulatedTime += 5*426;
    schedule[2].accumulatedTime += 5*408;
    schedule[3].accumulatedTime += 5*408;

    /* Sets the next 11 days (till the 2nd Janurary) to vacation, and subtracts the worktime, that would be there */
    int j;
    for (j = 0; j < 11; j++, i++) {
        schedule[0].timeslotList.storage[i].editState = Vacation;
        schedule[0].accumulatedTime -= schedule[0].timeslotList.storage[i].workTime;
        schedule[0].timeslotList.storage[i].workTime = 0;

        schedule[1].timeslotList.storage[i].editState = Vacation;
        schedule[1].accumulatedTime -= schedule[1].timeslotList.storage[i].workTime;
        schedule[1].timeslotList.storage[i].workTime = 0;

        schedule[2].timeslotList.storage[i].editState = Vacation;
        schedule[2].accumulatedTime -= schedule[2].timeslotList.storage[i].workTime;
        schedule[2].timeslotList.storage[i].workTime = 0;

        schedule[3].timeslotList.storage[i].editState = Vacation;
        schedule[3].accumulatedTime -= schedule[3].timeslotList.storage[i].workTime;
        schedule[3].timeslotList.storage[i].workTime = 0;
    }

    /* Sets the date to the earliest possible date for week 27 to start,
     * and adds the amount of days it will take from 2nd jan, to this day
     */
    date.day = 5;
    date.month = 7;
    i += 184 + isLeapYear(schedule->timeslotList.storage[i].date.year);

    // then go to the start of 28
    while (schedule->timeslotList.storage[i].date.weekNumber != 28) {
        i++;
    }

    /* Stores the amount of worktime each primo teams works in variables,
     * respectable for week 28-30, and 29-31 */
    int dif28[] = {0, 0},
        dif29[] = {0, 0};
    for (j = 0; j < 21; j++, i++) {
        dif28[0] += schedule[0].timeslotList.storage[i].workTime;
        dif28[1] += schedule[1].timeslotList.storage[i].workTime;

        dif29[0] += schedule[0].timeslotList.storage[i+7].workTime;
        dif29[1] += schedule[1].timeslotList.storage[i+7].workTime;
    }

    /* Actual worktime with vacations in 28 and 29.
     * 6390 is 7.1*15, which is the amount of worktime you get for three weeks of vacation*/
    dif28[0] = schedule[0].accumulatedTime - dif28[0] + 6390;
    dif28[1] = schedule[1].accumulatedTime - dif28[1] + 6390;
    dif29[0] = schedule[0].accumulatedTime - dif29[0] + 6390;
    dif29[1] = schedule[1].accumulatedTime - dif29[1] + 6390;

    /* Takes the absolute value of week 28 and subtracts the absolute value of week 29 */
    int dif = abs((schedule[0].expectedHours - dif28[0]) - (schedule[1].expectedHours - dif28[1])) - 
              abs((schedule[0].expectedHours - dif29[0]) - (schedule[1].expectedHours - dif29[1]));

    /* If the difference is lower or equal to 0, set the 'i' value back to the start of week 28 (21 days).
     * Else set it to the start of week 29 (14 days) */
    if (dif <= 0)
        i -= 21;
    else
        i -= 14;

    //set the editState to Vacation and set the appropriate workTime
    for (j = 0; j < 21; j++, i++) {
        schedule[0].timeslotList.storage[i].editState = Vacation;
        schedule[0].accumulatedTime -= schedule[0].timeslotList.storage[i].workTime;

        schedule[1].timeslotList.storage[i].editState = Vacation;
        schedule[1].accumulatedTime -= schedule[1].timeslotList.storage[i].workTime;

        schedule[2].timeslotList.storage[i].editState = Vacation;
        schedule[2].accumulatedTime -= schedule[2].timeslotList.storage[i].workTime;

        schedule[3].timeslotList.storage[i].editState = Vacation;
        schedule[3].accumulatedTime -= schedule[3].timeslotList.storage[i].workTime;

        if (schedule->timeslotList.storage[i].date.day != Saturday &&
            schedule->timeslotList.storage[i].date.day != Sunday) {
            schedule[0].timeslotList.storage[i].workTime = 426;
            schedule[0].accumulatedTime += 426;
            schedule[1].timeslotList.storage[i].workTime = 426;
            schedule[1].accumulatedTime += 426;
            schedule[2].timeslotList.storage[i].workTime = 408;
            schedule[2].accumulatedTime += 408;
            schedule[3].timeslotList.storage[i].workTime = 408;
            schedule[3].accumulatedTime += 408;
        } else {
            schedule[0].timeslotList.storage[i].workTime = 0;
            schedule[1].timeslotList.storage[i].workTime = 0;
            schedule[2].timeslotList.storage[i].workTime = 0;
            schedule[3].timeslotList.storage[i].workTime = 0;
        }
    }
}

void adjustSchedule(TeamShiftData *scheduleData, char *modifiers[5]) {
    int delta = scheduleData->expectedHours - scheduleData->accumulatedTime;
    if (delta < 0)
        delta += abs(delta) % 690;
    else
        delta -= abs(delta) % 690;
    while (delta != 0) {
        if (delta < 0) {
            int index = dayToRemove(scheduleData->timeslotList.storage,
                                    scheduleData->timeslotList.length,
                                    modifiers);
            Timeslot *slot = &scheduleData->timeslotList.storage[index];

            delta += slot->workTime;
            scheduleData->accumulatedTime -= slot->workTime;

            slot->editState = Removed;
            slot->workTime = 0;
            slot->startTime = (Time) {0};
            slot->endTime = (Time) {0};
        } else {
            int index = dayToAdd(scheduleData->timeslotList.storage,
                                 scheduleData->timeslotList.length,
                                 modifiers);
            Timeslot *slot = &scheduleData->timeslotList.storage[index];
            delta -= (slot->workTime = 690);
            scheduleData->accumulatedTime += slot->workTime;

            slot->editState = Added;
        }
    }
}

void calculateWeekNum(TsList* list) {
    int i = 0;
    /* Counts up till we hit 6th September (Or 5th on a leap year), which is always in week 36 */
    while (list->storage[i].date.day != 6 - isLeapYear(list->storage[i].date.year) || 
           list->storage[i].date.month != 9) {
        i++;
    }
    
    /* Makes a date to check if there is 53 weeks in the year */
    Date dec = {26 + isLeapYear(list->storage[i].date.year), 0, 12, list->storage[i].date.year};

    int d = list->storage[i].weekday,
        j = 36;

    /* If it did not count up to 7, wee do not need to deisplay week 36, so we count up */
    if (i <= 6) {
        j++;
        i += 7;
    }

    /* Loops through the whole year and inputs the weeknumber */
    for (; i-d < 365 + isLeapYear(list->storage[i].date.year); i+=7, j++) {
        if (j > 52)
            j = 1;
        
        /* Inputs the weeknumber in everyday of said week */
        int k;
        for (k = 0; k < 7; k++) {
            list->storage[(i+k)-d].date.weekNumber = j;
            if ((i+k)-d >= 365+isLeapYear(list->storage[i].date.year))
                break;
        }
        /* If the 27th December is on a sunday, there is 53 weeks in the current year,
         * which we input */
        if (list->storage[i-d-1].date.day == dec.day &&
            list->storage[i-d-1].date.month == dec.month) {
            list->storage[i-d].date.weekNumber = 53;
            j--;
        }
    }
}
