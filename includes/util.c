#include <util.h>
#include <stdlib.h>

const char* monthNumberToString(unsigned month) {
    // A static list of month names.
    static const char* monthNames[] = {"Januar", "Februar", "Marts",
                                       "April", "Maj", "Juni", "Juli",
                                       "August", "September", "Oktober",
                                       "November", "December"};
    return month > 0 && month <= 12
           ? monthNames[month - 1]
           : NULL;
}

char getWeekdayInitial(Weekday day) {
    static const char initials[] = {'M', 'T', 'O', 'T', 'F', 'L', 'S'};
    return initials[day];
}
