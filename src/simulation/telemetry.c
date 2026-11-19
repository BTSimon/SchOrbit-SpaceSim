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

DataLine *parseLine(char *line) {
    DataLine *returnLine;
    if (sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf", 
            returnLine->timestamp, 
            returnLine->acceleration, 
            returnLine->mass, 
            returnLine->altitude, 
            returnLine->speed,
            returnLine->thrust) == 6) {
        sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf", 
            returnLine->timestamp, 
            returnLine->acceleration, 
            returnLine->mass, 
            returnLine->altitude, 
            returnLine->speed,
            returnLine->thrust);
    } else {
        return NULL;
    }
    return returnLine;
}

DataLine *open_telemetry(int *sizeOf) {
    FILE *file = fopen("example.csv", "r");
    if (file == NULL) {
        //recordsCount = NULL;
        return;
    }    

    char buff[512];
    int builtRecordSize = 0;
    DataLine *builtRecords = malloc(sizeof(DataLine) * builtRecordSize);
    while (fgets(buff, sizeof(buff), file) != NULL)
    {
        DataLine *parsedRecord = parseLine(buff);
        if (parsedRecord != NULL) {
            builtRecordSize++;
            DataLine *ptr = realloc(builtRecords, sizeof(DataLine)*builtRecordSize);
            if (ptr == NULL) {
                return NULL;
            }
            builtRecords = ptr;
            builtRecords[builtRecordSize-1] = *parsedRecord;
        } 
    }
    *sizeOf = builtRecordSize;
    return builtRecords;
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
    memoryRecords = open_telemetry(recordSize);
    double *returnArray = malloc(sizeof(double)*(*recordSize));
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