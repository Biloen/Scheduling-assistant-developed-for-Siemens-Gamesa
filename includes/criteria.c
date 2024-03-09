#include <criteria.h>

/*Function Reads Criteria from File and sets it into the criteriaOutput parameter, the function also returns the arraySize*/
int getCriteria(FILE* inputFile, Criteria** criteriaOutput) {
    char buffer[512] = {0};
    int i = 0, lineCounter = 0;
    char team[10];
    char forceSchedule;

    //find the number of lines so we can allocate memory for criteriaOutput
    int c;
    while ((c = fgetc(inputFile)) != EOF) {
        if (c == '\n') // Increment count if this character is newline
            lineCounter++;
    }

    *criteriaOutput = malloc(sizeof(Criteria) * lineCounter);

    rewind(inputFile);

    //parse inputFile to criteriaOutput
    while (fscanf(inputFile, "%[^;]; %c ; %s", buffer, &forceSchedule, team) == 3) {
        (*criteriaOutput)[i].date = stringToDate(buffer);
        (*criteriaOutput)[i].forceSchedule = forceSchedule == 'Y';

        if (team[0] == 'P') {
            if (team[5] == 'O') criteriaOutput[0][i].team = PrimoOne;
            else (*criteriaOutput)[i].team = PrimoTwo;
        } else {
            if (team[6] == 'O') criteriaOutput[0][i].team = UltimoOne;
            else (*criteriaOutput)[i].team = UltimoTwo;
        }
        i++;

    }

    if (feof(inputFile)) {
        return i;
    } else {
        /* Errors */
        return 0;
    }
}

/*Checks whether the weight needs to be changed according to the criteria input and returns the "adjusted" weight as an int*/
int checkWeight(int weight, Timeslot timeslot, Criteria* criteria, int arraySize) {
    int check = dateInCriteria(timeslot.date, criteria, arraySize);
    if (check != 0) {
        if (criteria[check - 1].forceSchedule && criteria[check - 1].team == timeslot.team) return INT_MAX;
        else return INT_MIN;
    } else return weight;
}

/*Checks whether the date is in the criteria and returns it as an int*/
int dateInCriteria(Date date, Criteria* criteria, int arraySize) {
    for (int i = 0; i < arraySize; ++i) {
        if (dateEq(&date, &criteria[i].date)) {
            return i + 1;
        }
    }
    return 0;
}
