#include <stdio.h>
#include <types.h>
#include <input.h>
#include <weight_day.h>
#include <outputtable.h>
#include <util.h>

/* Formats and prints the given output data to the given file. */
void printTable(FILE *file, TeamShiftData tsd[]) {
    /* print header with string from first month of timeslots, to last month of timeslots
    * same with ints for years */
    fprintf(file, "Fast skifteholdsplan i perioden %s %d - %s %d "
                  "for timelønnede medarbejedere på 144 timer"
                  ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;\n",
            monthNumberToString(tsd->timeslotList.storage[0].date.month),
            tsd->timeslotList.storage[0].date.year,
            monthNumberToString(tsd->timeslotList.storage[tsd->timeslotList.length - 1].date.month),
            tsd->timeslotList.storage[tsd->timeslotList.length - 1].date.year);
    /* and then we print the dates.
     * the first call prints the last 6-12 months, but requires a modifier int
     * that returns from the latter call of the function, that prints the
     * first 1-6 months */
    printDates(file,tsd,printDates(file,tsd,0));
}



int printDates (FILE *file, TeamShiftData tsd[], int modifier){

    /* int i and j for for loops, days to count up days in a month, so we can skip a month
     * at a time */
    int i, j, days=0+modifier;

    /* first print 6 months that we print out the dates for */
    for (i = 0; i < 6; ++i) {
        fprintf(file, "%s %d;;PH1;PH2;UH1;UH2;", monthNumberToString(tsd->timeslotList.storage[days].date.month),
                tsd->timeslotList.storage[days].date.year);
        days += daysInMonth(tsd->timeslotList.storage[days].date.month, tsd->timeslotList.storage[days].date.year);
    }

    /* newline to separate */
    fprintf(file,"\n");

    /* we know we need to print out 31 lines. one for each possible day
     * how we handle days with less than 31 days we will handle later */
    for (i = 0; i < 31; ++i) {
        /* at the start of each print for every day line we reset the days
         * and add a days modifier, so that we either end up at the start of
         * the timeslot array, or 6 months into the timeslot array */
        days = 0+modifier;
        /* from here we print out 6 the days in rows of 6 months, so we run the print 6 times */
        for (j = 0; j < 6; j++) {
            /* if the day we are on is a date that can be found in the month we are printing,
             * run this code */
            if (i <
                daysInMonth(tsd->timeslotList.storage[days].date.month, tsd->timeslotList.storage[days].date.year)) {
                /* first print a char with the first letter of the day, and then the day */
                fprintf(file, "%s %d;%s; ", weekdayToStr(tsd->timeslotList.storage[i + days]), i + 1,
                        tsd->timeslotList.storage[i + days].shName ? tsd->timeslotList.storage[i + days].shName : " ");


                for (int k = 0; k < 4; ++k) {

                    switch (tsd[k].timeslotList.storage[i+days].editState) {
                        case 1:
                            fprintf(file, "RM;"); //removed
                            break;
                        case 2:
                            fprintf(file, "AD;"); //ADDED
                            break;
                        case 3:
                            fprintf(file, "SH  "); //SH day
                            if (k<2)
                                fprintf(file, "%c;", tsd[k].timeslotList.storage[i + days].workTime ? 'P' : ' ');
                            else
                                fprintf(file, "%c;", tsd[k].timeslotList.storage[i + days].workTime ? 'U' : ' ');
                            break;
                        case 4:
                            fprintf(file, "VC;"); //vacation
                            break;
                        default:
                            if (k < 2)
                                fprintf(file, "%c;", tsd[k].timeslotList.storage[i + days].workTime ? 'P' : ' ');
                            else
                                fprintf(file, "%c;", tsd[k].timeslotList.storage[i + days].workTime ? 'U' : ' ');
                            break;

                    }
                }

            } else
                /* if this day doesn't exist, just skip it entirely */
                fprintf(file, ";;;;;;");

            /* and add the days in the month, so we go to the next month */
            days += daysInMonth(tsd->timeslotList.storage[days].date.month, tsd->timeslotList.storage[days].date.year);
        }
        /* new line after every line */
        fprintf(file, "\n");
    }
    /* and return the days, for the modifier next time we run this function */
    return days;
}

/* this takes a weekday enum, and returns the relevant char */
char* weekdayToStr(Timeslot ts) {
    /* 8 is sizeof(char) */
    char* str = malloc(8);
    /* same as month number to string, we just go through until we find it */
    switch (ts.weekday) {
        case Monday:
            sprintf(str,"M (%d)",ts.date.weekNumber);
            return str;
        case Tuesday:
            return "T";
        case Wednesday:
            return "O";
        case Thursday:
            return "T";
        case Friday:
            return "F";
        case Saturday:
            return "L";
        case Sunday:
            return "S";
        default:
            return "-";
    }
}



