#include "../../headers/telemetry.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../../headers/debugmalloc.h"

/*
    Correct (?) workflow for the given telemetry recording session
    init -> create the file
*/

static FILE *records;

static DataLine *memoryRecords;
static int memoryRecordsCapacity = 0;
static int memoryRecordsCount;

int init_telemetry(void) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char filename[64];

    strftime(filename, sizeof(filename), "record_%Y-%m-%d_%H-%M-%S.csv", t);

    records = fopen(filename, "w");
    
    if (records == NULL) {
        return 500;
    }

    return 0;
}

int record_telemetry(DataLine dataEntry) {
    if (memoryRecordsCount == memoryRecordsCapacity) {

        int new_capacity = memoryRecordsCapacity;
        if (new_capacity == 0) {
            new_capacity = 1;
        } else {
            new_capacity = new_capacity*2;
        }

        DataLine *memptr = realloc(memoryRecords, sizeof(*memoryRecords) * new_capacity);

        if (memptr == NULL) {
            return 500;
        }

        memoryRecords = memptr;
        memoryRecordsCapacity = new_capacity;
    }

    memoryRecords[memoryRecordsCount++] = dataEntry;
    return 0;
}

int save_telemetry(void) {
    if (records == NULL) {
        return 500;
    }
    fprintf(records, "%s,", "time");
    fprintf(records, "%s,", "acceleration");
    fprintf(records, "%s,", "mass");
    fprintf(records, "%s,", "altitude");
    fprintf(records, "%s,", "speed");
    fprintf(records, "%s\n", "thrust");

    for (int rec = 0; rec < memoryRecordsCount; rec++) {
        fprintf(records, "%f,", memoryRecords[rec].timestamp);
        fprintf(records, "%f,", memoryRecords[rec].acceleration);
        fprintf(records, "%f,", memoryRecords[rec].mass);
        fprintf(records, "%f,", memoryRecords[rec].altitude);
        fprintf(records, "%f,", memoryRecords[rec].speed);
        fprintf(records, "%f\n", memoryRecords[rec].thrust);
    }

    int close_result = fclose(records);
    records = NULL;
    free(memoryRecords);
    return close_result == 0 ? 0 : 500;
}

double *create_record_array(DataType recordType, int *recordSize) {
    *recordSize = memoryRecordsCount;
    double *returnArray = malloc(sizeof(double)*memoryRecordsCount);
    if (returnArray == NULL) {
        return NULL;
    }
    for (int i = 0; i<memoryRecordsCount; i++) {
        switch (recordType) {
            case TIMESTAMP:     returnArray[i] = memoryRecords[i].timestamp; break;
            case MASS:          returnArray[i] = memoryRecords[i].mass; break;
            case ACCELERATION:  returnArray[i] = memoryRecords[i].acceleration; break;
            case ALTITUDE:      returnArray[i] = memoryRecords[i].altitude; break;
            case SPEED:         returnArray[i] = memoryRecords[i].speed; break;
            case THRUST:        returnArray[i] = memoryRecords[i].thrust; break;
            default:
                free(returnArray);
                *recordSize = 0;
                return NULL;
        }
    }
    return returnArray;
}