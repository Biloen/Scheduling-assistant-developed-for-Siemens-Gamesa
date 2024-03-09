#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <libgen.h>

#include <types.h>
#include <input.h>
#include <criteria.h>
#include <outputtable.h>
#include <generate.h>
#include <xmlWriter.h>

/*
 * Data structure used for declaring program options.
 * The options are of format `-s value` where s is defined
 * by the symbol field. The value is passed to the dest field.
 */
typedef struct {
    char** dest;
    char symbol;
} Option;

// Parses program arguments according to the given options.
char* parseArgs(int, char* [], size_t, const Option*);

// In-fixes the .out between the name and extension of the file.
char* createOutName(const char*);

int main(int argc, char* argv[]) {
    // Program option target variables.
    char* inputFilePath = NULL;
    char* outputFilePath = NULL;
    char* criteriaFilePath = NULL;
    char* mode = NULL;
    char* modifiers[] = {NULL, NULL, NULL, NULL, NULL};

    // Used to determine if the output name string needs to be freed.
    int freeOutName = 0;

    /*
    * Program option definitions.
    * output file path option along with 5 options to change default modifiers in the weigh function
    * s score for being an sh-day
    * l score for being a saturday
    * f score for being a vacation day 
    * S multiplier for when it is not an sh-day 
    * F multiplier for when it is not a vacation day
    */
    const Option options[] = {
            {&outputFilePath, 'o'},
            {&criteriaFilePath, 'c'},
            {modifiers    , 's'},
            {modifiers + 1, 'l'},
            {modifiers + 2, 'f'},
            {modifiers + 3, 'S'},
            {modifiers + 4, 'F'},
            {&mode, 'm'}
    };

    inputFilePath = parseArgs(argc, argv, sizeof options / sizeof(Option), options);

    // Checks if a file path was specified.
    if (inputFilePath == NULL) {
        fprintf(stderr,
                "No file path given. Please supply a valid file path as an argument to the program.\n"
                "Example: %s /path/to/file", argv[0]);
        return EXIT_FAILURE;
    }

    // Opens the input file and checks for errors.
    FILE* inputFile = fopen(inputFilePath, "r");
    if (inputFile == NULL) {
        int err = errno;
        fprintf(stderr, "Unable to open the specified input file %s. (%s)\n",
                inputFilePath, strerror(err));
        return EXIT_FAILURE;
    }

    // Checks if a output_file_path was given. If not one will be generated.
    if (outputFilePath == NULL) {
        freeOutName = 1;
        outputFilePath = createOutName(inputFilePath);
    }

    FILE* outputFile = fopen(outputFilePath, "w");
    if (outputFile == NULL) {
        int err = errno;
        fprintf(stderr, "Unable to create output file %s. (%s)\n",
                outputFilePath, strerror(err));

        fclose(inputFile);
        return EXIT_FAILURE;
    }

    FILE* criteriaFile = NULL;
    if (criteriaFilePath != NULL) {
        criteriaFile = fopen(criteriaFilePath, "r");
        if (criteriaFile == NULL) {
            int err = errno;
            fprintf(stderr, "Unable to open file at given criteria file path %s."
                    "\n(%s)\nProceeding without criteria.",
                    criteriaFilePath, strerror(err));
        }
    }

    // Initialize input storage variables.
    TeamShiftData teamShifts[4] = {0};
    for (unsigned i = 0; i < 4; i++)
        teamShifts[i].timeslotList = tsListNew(365);

    // Read input file.
    Error err = readInput(inputFile, teamShifts);
    if (err.kind != ErrNone)
        goto cleanup;
    
    calculateWeekNum(&teamShifts->timeslotList);
    addVacation(teamShifts);

    if (criteriaFile != NULL) {
        // read criteria.
        Criteria *criteria;
        int criteria_size;
        criteria_size = getCriteria(criteriaFile, &criteria);

        qsort(criteria, criteria_size, sizeof(Criteria), dateCompare);

        size_t j = 0;
        for (int i = 0; i < criteria_size; i++) {
            int found = 0;
            while (!found && j < teamShifts->timeslotList.length) {
                if (dateEq(&teamShifts[criteria[i].team].timeslotList.storage[j].date, &criteria[i].date)) {
                    if (criteria[i].forceSchedule) {
                        teamShifts[criteria[i].team].timeslotList.storage[j].editState = Added;
                        teamShifts[criteria[i].team].timeslotList.storage[j].workTime = 690;
                    } else {
                        teamShifts[criteria[i].team].timeslotList.storage[j].editState = Removed;
                        teamShifts[criteria[i].team].timeslotList.storage[j].workTime = 0;
                    }
                    found++;
                } else
                    j++;
            }

            if (!found) {
                char *dateStr = dateToString(criteria[i].date);
                printf("Unable to apply criteria for shift on %s.", dateStr);
                free(dateStr);

                err = blankError(ErrInvalidCriteria);
                goto cleanup;
            }
        }
    }
    
    for (unsigned i = 0; i < 4; i++) {
        adjustSchedule(&teamShifts[i], modifiers);
    }
    
    /* Writes schedule to outputfile*/
    if (mode != NULL && *mode == 'x')
        writeXmlScheduleTable(outputFile, teamShifts);
    else
        printTable(outputFile, teamShifts);

cleanup:
    fclose(inputFile);
    fclose(outputFile);
	
    if (freeOutName && outputFilePath != NULL)
        free(outputFilePath);


    freeShNameCache();

    for (unsigned i = 0; i < 4; i++)
        tsListFree(&teamShifts[i].timeslotList);

    if (err.kind != ErrNone) {
        printErr(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

char* parseArgs(int argc, char* argv[], size_t optionCount, const Option* options) {
    char* mainArg = NULL;
    const Option* currentArg = NULL;

    for (int i = 1; i < argc; i++) {
        // If current argument is set the value will be set through the dest field.
        if (currentArg != NULL) {
            *currentArg->dest = argv[i];
            currentArg = NULL;
        }

        /*
         * If the '-' character is encountered the string is assumed to
         * signify the start of an option parameter. If a matching option
         * is found the currentArg will be set accordingly.
         */
        else if (argv[i][0] == '-') {
            /*
             * If no character follows the '-' or more than one character
             * follows the '-' they will treated as a main argument rather than
             * a options specifier.
             */
            if (argv[i][1] == 0 || argv[i][2] != 0) {
                mainArg = argv[i];
                continue;
            }

            for (size_t j = 0; j < optionCount; j++) {
                if (argv[i][1] == options[j].symbol) {
                    currentArg = &options[j];
                    break;
                }
            }
        }
            // Any remaining are assumed to be the main argument.
        else
            mainArg = argv[i];
    }

    return mainArg;
}


char* createOutName(const char* path) {
    static const char* infix = ".out";

    /*
     * We copy the string since the basename(char*) and
     * dirname(char*) functions may modify the contents
     * of the given string.
     */
    char* _dPath = strdup(path); // dirname
    char* _bPath = strdup(path); // basename

    char* directory = dirname(_dPath);
    char* filename = basename(_bPath);

    // Search for the position to insert the infix string.
    char* cursor = filename;
    char* infixPos = NULL;

    do {
        if (*cursor == '.')
            infixPos = cursor;
    } while (*++cursor != 0);

    // If '.' is not found or is the first character, set position to end.
    if (infixPos == filename)
        infixPos = NULL;

    char* result = malloc(strlen(directory) + strlen(filename) + strlen(infix) + 2);
    if (infixPos != NULL)
        sprintf(result, "%s/%.*s%s%s", directory, (int) (infixPos - filename), filename, infix, infixPos);
        // If no position is found add the infix as a suffix instead.
    else
        sprintf(result, "%s/%s%s", directory, filename, infix);

    free(_dPath);
    free(_bPath);

    return result;
}
