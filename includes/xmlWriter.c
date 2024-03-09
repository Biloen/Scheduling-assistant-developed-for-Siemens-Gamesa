#include <xmlWriter.h>
#include <util.h>
#include <input.h>
#include <weight_day.h>

#include <stdio.h>

/*
 * Pointers to the start and end of the embedded
 * header and footer file objects.
 */
extern char _binary_header_start[];
extern char _binary_header_end[];
extern char _binary_footer_start[];
extern char _binary_footer_end[];

/*
 * Constant strings used for table generation.
 * These are mainly strings that are repeated often
 */
static const char* standardRowStart = "<Row ss:AutoFitHeight=\"0\" ss:Height=\"16.3125\">\n";
static const char* rowEnd = "</Row>\n";
static const char* emptyCell = "<Cell ss:StyleID=\"ts%d\"/>\n";
static const char* teamNames[] = {"PH1", "PH2", "UH1", "UH2"};

typedef enum {
    // We use the two first bits to determine the cell type.
    CellDateLeft = 0u,
    CellDateRight = 1u,
    CellShift = 2u,

    CellWeekend = 1u << 2u,

    // We use the fourth and fifth byte to determine
    // the store the edit state.
    CellRemoved = 1u << 3u,
    CellAdded = 2u << 3u,
    CellShDay = 3u << 3u,
    CellVacation = 4u << 3u,
} TimeslotCellStyle;

/*
 * Private function declarations
 */

/*
 * Writes the heading row for the next six months
 * starting with the given month and year.
 */
static void writeHeadingRow(FILE*, unsigned*, unsigned*);

// Writes one row of shifts.
static void writeShiftRow(FILE*, Timeslot*[4], unsigned, size_t);

// Writes the date section for a given timeslot.
static void writeTimeslotDate(FILE*, Timeslot*);

// Writes a shift cell to the file.
static void writeShift(FILE*, Timeslot*);

/*
 * Function implementations.
 */

void writeXmlScheduleTable(FILE* target, TeamShiftData data[]) {
    Timeslot* first = &data->timeslotList.storage[0];
    Timeslot* last = &data->timeslotList.storage[data->timeslotList.length - 1];

    fwrite(_binary_header_start, sizeof(char), _binary_header_end - _binary_header_start, target);
    fprintf(target,
            "<Row ss:AutoFitHeight=\"0\" ss:Height=\"41.4375\">\n"
            "<Cell ss:MergeAcross=\"35\" ss:StyleID=\"tsSheetTitle\">\n"
            "<Data ss:Type=\"String\">Fast skifteholdsplan %s %d - %s %d for timelønnet medarbejdere på 144 timer</Data>\n"
            "</Cell>%s\n",
            monthNumberToString(first->date.month), first->date.year,
            monthNumberToString(last->date.month), last->date.year,
            rowEnd);

    unsigned month = first->date.month;
    unsigned year = first->date.year;
    size_t offset = 0;
    Timeslot* rawPointers[] = {data[0].timeslotList.storage,
                               data[1].timeslotList.storage,
                               data[2].timeslotList.storage,
                               data[3].timeslotList.storage};

    for (unsigned i = 0; i < 2; i++) {
        writeHeadingRow(target, &month, &year);

        for (unsigned j = 0; j < 31; j++) {
            fputs(standardRowStart, target);
            writeShiftRow(target, rawPointers, j, offset);
            fputs(rowEnd, target);
        }
        // Inserts row as spacing
        fputs(standardRowStart, target);
        fputs(rowEnd, target);

        for (unsigned j = 0; j < 6; j++) {
            offset += daysInMonth(rawPointers[0]->date.month, rawPointers[0]->date.year);
        }
    }

    // Writes a summary containing (the total hour count per team).
    for (unsigned i = 0; i < 4; i++) {
        fputs(standardRowStart, target);
        fprintf(target, "<Cell/>"
                        "<Cell><Data ss:Type=\"String\">%s: %.2f</Data></Cell>",
                        teamNames[i], (double) data[i].accumulatedTime / 60.0);
        fputs(rowEnd, target);
    }

    fwrite(_binary_footer_start, sizeof(char), _binary_footer_end - _binary_footer_start, target);
}

static void writeHeadingRow(FILE* target, unsigned* month, unsigned* year) {
    fputs(standardRowStart, target);
    for (unsigned i = 0; i < 6; i++) {
        // Prints the month name and year.
        fprintf(target,
                "<Cell ss:MergeAcross=\"1\" ss:StyleID=\"tsHeading\">"
                "<Data ss:Type=\"String\">%s %d</Data></Cell>\n",
                monthNumberToString(*month), *year);

        for (unsigned j = 0; j < sizeof teamNames / sizeof(char*); j++)
            fprintf(target, "<Cell ss:StyleID=\"tsHeading\"><Data ss:Type=\"String\">%s</Data></Cell>\n", teamNames[j]);

        if (*month == 12)
            *year += 1;
        *month = *month % 12 + 1;
    }

    fputs(rowEnd, target);
}

static void writeShiftRow(FILE* target, Timeslot* data[4], unsigned day, size_t offset) {
    for (unsigned i = 0; i < 6; i++) {
        unsigned daysInCurrentMonth = daysInMonth(data[0][offset].date.month, data[0][offset].date.year);
        if (day < daysInCurrentMonth) {
            writeTimeslotDate(target, &data[0][offset+day]);
            for (unsigned j = 0; j < 4; j++)
                writeShift(target, &data[j][offset + day]);
        } else {
            writeTimeslotDate(target, NULL);
            for (unsigned j = 0; j < 4; j++)
                writeShift(target, NULL);
        }

        offset += daysInCurrentMonth;
    }

}

static void writeShift(FILE* target, Timeslot* timeslot) {
    static const char teamSymbol[] = {'P', 'P', 'U', 'U'};
    if (timeslot == NULL) {
        fprintf(target, emptyCell, CellShift);
        return;
    }

    // Ignores the ShDay state since it does not apply to shifts
    unsigned editState = timeslot->editState != ShDay ? timeslot->editState : None;
    unsigned cellStyle = (unsigned) CellShift
                                  | editState << 3u;

    if (cellStyle == CellShift && timeslot->weekday > Friday)
        cellStyle |= (unsigned) CellWeekend;

    if (timeslot->editState == Vacation || (timeslot->workTime == 0 && timeslot->editState != Removed))
        fprintf(target, emptyCell, cellStyle);
    else
        fprintf(target, "<Cell ss:StyleID=\"ts%d\"><Data ss:Type=\"String\">%c</Data></Cell>\n",
                cellStyle, teamSymbol[timeslot->team]);
}

static void writeTimeslotDate(FILE* target, Timeslot* timeslot) {
    unsigned cellStyle = CellDateLeft;
    if (timeslot == NULL) {
        fprintf(target, emptyCell, cellStyle);
        fprintf(target, emptyCell, CellDateRight);
        return;
    }

    if (timeslot->shName != NULL)
        cellStyle |= (unsigned) CellShDay;
    else if (timeslot->weekday > Friday)
        cellStyle |= (unsigned) CellWeekend;

    // Print the left half of the date cell
    fprintf(target, "<Cell ss:StyleID=\"ts%d\"><Data ss:Type=\"String\">", cellStyle);
    if (timeslot->weekday == Monday)
        fprintf(target, "(%d) M", timeslot->date.weekNumber);
    else
        fputc(getWeekdayInitial(timeslot->weekday), target);

    fprintf(target, " %2d</Data></Cell>\n", timeslot->date.day);

    // Print the right half of the date cell
    cellStyle |= (unsigned) CellDateRight;
    if (timeslot->shName != NULL)
        fprintf(target, "<Cell ss:StyleID=\"ts%d\"><Data ss:Type=\"String\">%s</Data></Cell>\n",
                cellStyle, timeslot->shName);
    else fprintf(target, emptyCell, cellStyle);

}
