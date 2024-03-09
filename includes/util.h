#ifndef P1_PROJECT_UTIL_H
#define P1_PROJECT_UTIL_H

#include <types.h>

/*
 * Takes an unsigned integer between 1 and 12 (inclusive)
 * and coverts it to the corresponding month name (in danish).
 * NULL is returned if the given number is out of bounds of the range.
 */
const char* monthNumberToString(unsigned);

//Gets the initial letter of a weekday name (in danish)
char getWeekdayInitial(Weekday);

#endif
