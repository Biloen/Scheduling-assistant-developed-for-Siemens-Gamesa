#include <types.h>
#include <memory.h>

/*
 * Allocates storage for the tsList. If storage is already
 * allocated the old storage is copied into the new allocation.
 */
Error tsListRealloc(TsList* list, size_t size) {
    if (size <= list->capacity)
        return NO_ERROR;

    list->capacity = size;
    if (list->storage == NULL)
        list->storage = malloc(sizeof(Timeslot) * size);
    else
        list->storage = realloc(list->storage, size * sizeof(Timeslot));

    if (list->storage == NULL) {
        list->capacity = 0;
        return blankError(ErrAlloc);
    }

    return NO_ERROR;
}

/*
 * Creates a storage object and allocates
 * storage of given size.
 */
TsList tsListNew(size_t size) {
    Timeslot* storage = size ? malloc(sizeof(Timeslot) * size) : NULL;
    return (TsList) {0, size, storage};
}

/* 
 * Pushes an element to the end of the list.
 * If the allocated storage is insufficient additional
 * storage will be allocated.
 */
Error tsListPush(TsList* list, Timeslot val) {
    if (list->capacity == list->length) {
        Error e = tsListRealloc(list, list->capacity * 2);
        if (e.kind != ErrNone)
            return e;
    }

    list->storage[list->length++] = val;
    return NO_ERROR;
}

/* Frees the storage used by the list. */
void tsListFree(TsList* list) {
    if (list->storage != NULL) {
        free(list->storage);
        list->storage = NULL;
    }
}

//takes a Date struct and return a pointer to a string with the date in the format dd-mm-yy
char* dateToString(Date date) {
    char* out = malloc(8 * sizeof(char));
    sprintf(out, "%02d-%02d-%04d", date.day, date.month, date.year);
    return out;
}

/* Converts a string containing a date into a Date struct.
 * Pre: a string containing a date in dd-mm-yyyy format.
 * Out: a Date struct containing a date, month and year.
 */
Date stringToDate(const char* date) {
    Date out;
    char* end;
    out.day = strtol(date, &end, 10);
    out.month = strtol(end + 1, &end, 10);
    out.year = strtol(end + 1, NULL, 10);

    return out;
}

// Compares two Date objects for equality.
int dateEq(Date* a, Date* b) {
    return a->month == b->month && a->day == b->day && a->year == b->year;
}

// returns 0 if the two dates are equal and 1 othewise
int dateCompare(const void* a, const void* b) {
    return dateCompareTrue(a, b);
}

// returns 0 if the two dates are equal, positive integer if a comes before b and a negtive integer otherwise.
int dateCompareTrue(const Date* a, const Date* b) {
    
    if (a->year != b->year)
        return b->year - a->year;
    
    if (a->month != b->month)
        return b->month - a->month;
 
    if (a->day != b->day)
        return a->day - b->day;
     
    return 0;
}