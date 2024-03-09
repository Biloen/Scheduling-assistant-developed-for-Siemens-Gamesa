#include <stdlib.h>
#include <limits.h>
#include "weight_day.h"

/*return the index to the timeslot with the lowest score aka the day we should add to the schedule*/
int dayToAdd(Timeslot slots[], int slotLen, char* modifiers[]) {
    int min = -1,
        minScore = INT_MAX,
        i,
        workHours;

    /*find the first day where the workers have time off*/
    for (i = 0; i < slotLen; i++) {
        if ((slots + i)->workTime == 0) {
            min = i;
            break;
        }
    }

    /*if we don't find any return -1*/
    if (min == -1) return min;
    workHours = hoursInWeek(slots, slotLen, i);
    /*go through the rest of the array to find the minimum*/
    for (int i = min; i < slotLen; i++) { 
        
        if (slots[i].editState == Removed || slots[i].editState == Added) {
            continue;
        }
        
        if ((slots + i)->weekday == Monday) workHours = hoursInWeek(slots, slotLen, i);

        int tempScore = weigh(slots, slotLen, modifiers, workHours, i);


        if ((slots + i)->workTime == 0 && tempScore < minScore) {
            min = i;
            minScore = tempScore; 
        }
    }

    return min;
}

/*return the index to the timeslot with the highest score aka the day we should remove to the schedule*/
int dayToRemove(Timeslot slots[], int slotLen, char* modifiers[]) {
    int max = -1,
        maxScore = INT_MIN,
        i,
        workHours;

    /*find the first day where the workers are working*/
    for (i = 0; i < slotLen; i++) {
        if ((slots + i)->workTime > 0 && (slots + i)->editState != Vacation) {
            max = i;
            break;
        }
    }

    /*if we don't find any return -1*/
    if (max == -1) return max;
    workHours = hoursInWeek(slots, slotLen, i);
    /*go through the rest of the timeslots and find the timeslot with the highest score*/
    for (int i = max; i < slotLen; i++) {
        if (slots[i].editState == Removed || slots[i].editState == Added) {
            continue;
        }
        
        if ((slots + i)->weekday == Monday) workHours = hoursInWeek(slots, slotLen, i);

        int tempScore = weigh(slots, slotLen, modifiers, workHours, i);
        


        if ((slots + i)->workTime > 0  && (slots + i)->editState != Vacation && tempScore > maxScore) {
            max = i;
            maxScore = tempScore;
        }
    }

    return max;
}

/*given an array of timeslots the length of the array and and an index, return the score of the timeslot of that index*/
int weigh(Timeslot slots[], int slotsLen, char* modifiers[5], int workHours, int slotIndex) {
    /*the constant scores and multipliers should optionally be changed by flags*/
    int score = 0, 
        scanned_modifiers[] = {500, 50, 200, 2, 1};

    if (workHours > 4 * 690) {
        score = INT_MAX / 2;
    } else {
        score += 5 * workHours;
    }

    /*if we have gotten any flags specifying values for the modifiers scan them and assign the value*/
    for (int i = 0; i < 5; i++) {
        if (modifiers[i] != NULL) {
            sscanf(modifiers[i], "%d", &scanned_modifiers[i]); 
        }
    }

    /*if it is an shDay add to the score if it is not find the closest sh day and subtract from the score*/
    if ((slots + slotIndex)->editState == ShDay) {
        score += scanned_modifiers[0];
    } else {
        score -= scanned_modifiers[3] * nDaysTill(slots, slotsLen, slotIndex, ShDay);
    }

    /*if it is an vacation add to the score if it is not find the closest vacation and subtract from the score*/
    if ((slots + slotIndex)->editState == Vacation) {
        score += scanned_modifiers[2];
    } else {
        score -= scanned_modifiers[4] * nDaysTill(slots, slotsLen, slotIndex, Vacation);
    }

    /*if it is a Sunday add to the score*/
    if ((slots + slotIndex)->weekday == Sunday) score += scanned_modifiers[1];

    return score;
}

int hoursInWeek(Timeslot slots[], int slotsLen, int slotIndex) {
    int hours = 0;

    do {
        hours += slots[slotIndex].workTime;
        slotIndex++;
    } while ((slots + slotIndex)->weekday != Monday && slotIndex <= slotsLen);

    return hours;
}

/*given the on array of timeslots the length of the array and an index return the number of days to a sh-day*/
int nDaysTill(Timeslot slots[], int slotLen, int slotIndex, int editStateDay) {
    /* days until the next sh-day
     * index variable for going backwards 
     * index vairable for going forwards */
    int days = 0, 
        i = slotIndex - 1,
        j = slotIndex + 1;


    /*if the day itself is not an sh-day we want to find the nearest day which is*/
    for (;;) {
        /*if i is larger than or equal to zero then if the day at index i as sh-day set day equals i and break, otherrwise go to the previous day*/
        if (i >= 0) {
            if ((slots + i)->editState == editStateDay) {
                days = daysBetweenDates((slots + i)->date, (slots + slotIndex)->date);
                break;
            } else {
                i--;
            }
            
        }

        /*the same as before but now we go forwards*/
        if (j < slotLen) {
            if ((slots + j)->editState == editStateDay) {
                days = daysBetweenDates((slots + slotIndex)->date, (slots + j)->date);
                break;
            } else {
                j++;
            }
        }
        
        /*this should really never happen*/
        if (i < 0 && j >= slotLen) {
            days = -1;
            break; 
        } 
    } 

    return days;
}

/*calculate the number of days between two dates we assume that d1 is before d2*/
int daysBetweenDates(Date d1, Date d2) {
    int days = 0;
    //whlie the two days are not the same go to the next day and add one to our days counter
    while((d1.day != d2.day) || (d1.month != d2.month) || (d1.year != d2.year)) {
        d1 = tomorrow(d1);
        days++;
    } 

    return days;
}

/*returns the number of days in a mounth given the wounth m and the year y*/
int daysInMonth(int month, int year) {
    int days;
    switch (month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            days = 31; 
            break;
        case 4: case 6: case 9: case 11:
            days = 30;
            break;
        case 2:
            if (isLeapYear(year)) {
                days = 29;
            } else {
                days = 28;
            }
            break;
        default:
            printf("Error in days in month\n");
            days = -1;
    }

    return days;
}

/*given the year return the boolean value for is this year a leap year?*/
int isLeapYear(int year) {
    int res;

    if (year % 400 == 0) {
        res = 1;
    } else if (year % 100 == 0) {
        res = 0;
    } else if (year % 4 == 0) {
        res = 1;
    } else {
        res = 0;
    }

    return res;
}

/*Given a date return the next day */
Date tomorrow(Date date) {
    Date nextDay = date;
    int daysInThisMonth = daysInMonth(date.month, date.year);

    if (date.day < daysInThisMonth) {
        nextDay.day++;
    } else if (date.day == daysInThisMonth) {
        nextDay.day = 1;
        if (date.month != 12) {
            nextDay.month++;
        } else {
            nextDay.month = 1;
            nextDay.year++;
        }
    } else {
        printf("Error in tomorrow\n");
    }
    
    return nextDay;
}
