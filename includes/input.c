#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "input.h"

#define NONE 'x'
#define TEAM_COUNT 4

/*
 * Structures and functions for storing references to SH-day names.
 * All of these items with the exception of the `freeShNameCache`
 * function are local to this translation unit.
 */

/*
 * The cache is implemented as a linked list.
 */
typedef struct ShName {
    char* name;
    struct ShName* next;
} ShNameNode;

// The initial list node.
static ShNameNode* shNameList = NULL;

/*
 * Attempts to push a name to the list. If an error is encountered
 * it will be set at `error_out` and null will be returned.
 */
char* pushName(char* name, Error* errorOut) {
    static const char* _allocErrMsg = "Unable to allocate memory for name caching.";
    ShNameNode* node = shNameList;
    if (node) {
        while (node->next != NULL) {
            node = node->next;
        }

        node->next = malloc(sizeof(ShNameNode));
        node = node->next;
    } else {
        node = malloc(sizeof(ShNameNode));
        shNameList = node;
    }

    if (node == NULL) {
        *errorOut = (Error) {ErrAlloc, _allocErrMsg};
        return NULL;
    }

    node->next = NULL;
    node->name = strdup(name);
    if (node->name == NULL) {
        *errorOut = (Error) {ErrAlloc, _allocErrMsg};
        return NULL;
    }

    return node->name;
}

/* Frees all the memory used for the SH-cache. */
void freeShNameCache() {
    ShNameNode* current = shNameList;
    ShNameNode* next;

    while (current != NULL) {
        next = current->next;
        if (current->name != NULL)
            free(current->name);
        free(current);

        current = next;
    }

    shNameList = NULL;
}

/* Parsing functions. */

/* Reads a day from a string containing a weekday.
 * Pre: a string containing a weekday initialised.
 * Out: the Weekday enum that was contained in the string.
 */
Weekday readWeekday(const char* strBuffer) {
    switch (strBuffer[0]) {
        case 'M':
            return Monday;
        case 'T':
            return strBuffer[1] == 'i' ? Tuesday : Thursday;
        case 'O':
            return Wednesday;
        case 'F':
            return Friday;
        case 'L':
            return Saturday;
        case 'S':
            return Sunday;
        default:
            return Monday;
    }
}

/* Reads a time from a string containing a time.
 * Pre: a string containing a time initialised, in format hh/mm.
 * Out: a time struct containing the hours and minutes.
 */
Time readTime(const char* strBuffer) {
    Time timeConverter;
    /*The characters that represent the hours are converted to integers*/
    int firstHourDigit = strBuffer[0] - '0', secondHourDigit = strBuffer[1] - '0';
    if (firstHourDigit > 0) {
        firstHourDigit *= 10;
    }
    timeConverter.hours = firstHourDigit + secondHourDigit;

    /*The characters that represent the minutes are converted to integers*/
    int firstMinuteDigit = strBuffer[3] - '0', secondMinuteDigit = strBuffer[4] - '0';
    if (firstMinuteDigit > 0) {
        firstMinuteDigit *= 10;
    }
    timeConverter.minutes = firstMinuteDigit + secondMinuteDigit;
    return timeConverter;
}

// takes a string buffer as a parameter and returns the number of hours as the type unsigned
unsigned readWorkTime(const char* buffer) {
    double value = 0.0;
    unsigned decimals = 0;
    char c;

    while ((c = *buffer++) != 0) {
        // Skip thousand separators
        if (c == '.')
            continue;

        if (c == ',') {
            // If a decimal stop has already been encountered stop reading
            if (decimals > 0)
                break;

            decimals = 1;
            continue;
        }

        // If c is not a digit between 0-9 stop reading.
        c -= '0';
        if (c > 9)
            break;

        if (decimals > 0) {
            decimals += 1;
        }

        value *= 10;
        value += c;
    }

    value /= pow(10, decimals - 1);
    return (unsigned) round(value * 60.0);
}

/* This function reads the given csv. input file and enters the data of the input file into an array of TeamShiftDatas.
 * Pre: A file opened that is in the right input format, and in csv. format, and a defined array of TeamShiftData, with one element for each team.
 * Out: A TeamShiftData array with one element for each team, and each team are filled with their shifts.
 */
Error readInput(FILE* inputFile, TeamShiftData* listOfShifts) {
    Error err = NO_ERROR;
    TimeslotField currentField = DatePart;
    Timeslot timeslotShift = {0};
    char strBuffer[512] = {0};
    int c;
    int i = 0, j, k = 0;

    //skip first 7 lines of the csv file
    while (i < 7) {
        c = fgetc(inputFile);
        if (c == '\n')
            i++;
    }

    //parse the work hours for each team
    for (i = 0; i < TEAM_COUNT; i++) {
        char* wPos = strBuffer;
        // Skip non numeric characters
        while (!isdigit(c)) {
            c = fgetc(inputFile);
            if (c == EOF)
                return (Error) {ErrUnexpectedEOF, "EOF encountered while scanning for expected work time."};
        }

        while (c == ',' || c == '.' || isdigit(c)) {
            *wPos++ = (char) c;
            c = fgetc(inputFile);
        }

        // Write null terminator
        *wPos = 0;

        listOfShifts[i].expectedHours = readWorkTime(strBuffer);
    }

    //skip 18 lines in the input file
    i = 0;
    while (i <= 18) {
        c = fgetc(inputFile);
        if (c == '\n')
            i++;
    }

    //parse date, weekday, shName, startTime, endTime and workhours for all the teams
    i = 0;
    j = 0;
    while (c != EOF) {
        c = fgetc(inputFile);
        if (c == ';') {
            //if (currentField != DatePart)
            strBuffer[i] = 0;

            switch (currentField) {
                case DatePart:
                    timeslotShift.date = stringToDate(strBuffer);
                    currentField++;
                    if (strBuffer[0] == 0)
                        return NO_ERROR;
                    break;

                case WeekdayPart:
                    timeslotShift.weekday = readWeekday(strBuffer);
                    currentField++;
                    strBuffer[0] = NONE;
                    break;

                case ShName:
                    if (strBuffer[0] != 0) {
                        strBuffer[i] = 0;

                        timeslotShift.shName = pushName(strBuffer, &err);
                        if (timeslotShift.shName == NULL)
                            return err;
                    } else {
                        timeslotShift.shName = NULL;
                    }
                    currentField++;
                    break;

                case IsDay:
                case IsWeekend:
                    currentField++;
                    break;

                case IsSHDay:
                    if (strBuffer[0] == '1')
                        timeslotShift.editState = ShDay;
                    currentField++;
                    break;

                case Empty:
                case ShiftName:
                    currentField++;
                    break;

                case StartTime:
                    if (isdigit(strBuffer[0]))
                        timeslotShift.startTime = readTime(strBuffer);
                    else timeslotShift.startTime = (Time) {0};
                    currentField++;
                    break;

                case EndTime:
                    if (isdigit(strBuffer[0]))
                        timeslotShift.endTime = readTime(strBuffer);
                    else timeslotShift.endTime = (Time) {0};
                    currentField++;
                    break;

                case WorkHours:
                    timeslotShift.workTime = readWorkTime(strBuffer);
                    timeslotShift.team = (TeamName) j;
                    listOfShifts[j].accumulatedTime += timeslotShift.workTime;

                    err = tsListPush(&listOfShifts[j].timeslotList, timeslotShift);
                    if (err.kind != ErrNone)
                        return err;

                    currentField = Empty;
                    j++;
                    break;
                default:
                    break;
            }

            i = 0;

        } else if (c == '\n') {
            i = 0;
            j = 0;
            k++;
            currentField = DatePart;
            strBuffer[0] = NONE;
            timeslotShift = (Timeslot) {0};
        } else {
            strBuffer[i] = (char) c;
            i++;
        }

    }

    return err;
}
