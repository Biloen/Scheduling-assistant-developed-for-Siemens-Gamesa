#ifndef P1_TYPES_H
#define P1_TYPES_H

#include <stdlib.h>
#include <stdio.h>
#include <error.h>

typedef enum {
    Monday = 0,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday
} Weekday;

typedef enum {
    None = 0,
    Removed,
    Added,
    ShDay,
    Vacation
} EditState;

typedef enum {
    PrimoOne = 0,
    PrimoTwo,
    UltimoOne,
    UltimoTwo
} TeamName;

// Struct representing a date
typedef struct {
    int day, weekNumber, month, year;
} Date;

// Converts a `Date` struct to a string of format "dd-mm-yyyy".
char* dateToString(Date);

// Converts a string of format "dd-mm-yyyy" to an instance of Date struct.
Date stringToDate(const char* date);

int dateEq(Date* a, Date* b);

int dateCompare(const void* a, const void* b);

int dateCompareTrue(const Date* a, const Date* b);

typedef struct {
    int hours, minutes;
} Time;

typedef struct {
    Date date;
    Weekday weekday;
    const char* shName;
    TeamName team;
    Time startTime;
    Time endTime;
    unsigned workTime;
    EditState editState;   
} Timeslot;

/*
 * A dynamically expandable array structure for storing `Timeslot` objects.
 * Note: should always be initialized through `TsListNew(size_T)`.
 */
typedef struct {
    size_t length;
    size_t capacity;
    Timeslot* storage;
} TsList;

/*
 * Reallocates the internal storage of the given `TsList` and copies
 * all elements to the new storage. If the given storage size is less than
 * or equal in size to the currently allocated nothing is done.
 */
Error tsListRealloc(TsList*, size_t);

// Initializes a new `TsList` with storage of a given size pre-allocated.
TsList tsListNew(size_t);

// Pushes one element to the back of the given list.
Error tsListPush(TsList*, Timeslot);

// Frees the dynamic memory used for storage by a given list.
void tsListFree(TsList*);

#endif
