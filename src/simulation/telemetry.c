#include "../../headers/telemetry.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
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

static int parseLine(const char *line, DataLine *returnLine) {
    if (sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf", 
            &returnLine->timestamp,
            &returnLine->acceleration,
            &returnLine->mass,
            &returnLine->altitude,
            &returnLine->speed,
            &returnLine->thrust) == 6) {
        return 0;
    }
    else {
        return 500;
    }
}

static DataLine *open_telemetry(int *sizeOf) {
    if (sizeOf == NULL) {
        return NULL;
    }
    *sizeOf = 0;

    FILE *file = fopen("example.csv", "r");
    if (file == NULL) {
        return NULL;
    }    

    char buff[512];
    int builtRecordSize = 0;
    int capacity = 0;
    DataLine *builtRecords = NULL;
    while (fgets(buff, sizeof(buff), file) != NULL) {
        DataLine line;
        if (parseLine(buff, &line) == 0) {
            if (builtRecordSize == INT_MAX) {
                free(builtRecords);
                fclose(file);
                return NULL;
            }

            if (builtRecordSize == capacity) {
                int new_capacity = capacity == 0 ? 64 : capacity;
                if (capacity != 0) {
                    new_capacity = capacity > INT_MAX / 2
                        ? INT_MAX
                        : capacity * 2;
                }

                DataLine *ptr = realloc(
                    builtRecords,
                    sizeof(*builtRecords) * (size_t)new_capacity
                );
                if (ptr == NULL) {
                    free(builtRecords);
                    fclose(file);
                    return NULL;
                }
                builtRecords = ptr;
                capacity = new_capacity;
            }

            builtRecords[builtRecordSize++] = line;
        } 
    }

    if (ferror(file)) {
        free(builtRecords);
        fclose(file);
        return NULL;
    }

    if (fclose(file) != 0) {
        free(builtRecords);
        return NULL;
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

static double get_record_value(const DataLine *record, DataType recordType) {
    switch (recordType) {
        case TIMESTAMP:     return record->timestamp;
        case MASS:          return record->mass;
        case ACCELERATION:  return record->acceleration;
        case ALTITUDE:      return record->altitude;
        case SPEED:         return record->speed;
        case THRUST:        return record->thrust;
        default:            return 0.0;
    }
}

static int is_valid_record_type(DataType recordType) {
    return recordType >= TIMESTAMP && recordType <= THRUST;
}

double *create_record_array(
    DataType recordType,
    int *recordSize,
    TelemetrySource source
) {
    if (recordSize == NULL) {
        return NULL;
    }

    *recordSize = 0;
    if (!is_valid_record_type(recordType)) {
        return NULL;
    }

    const DataLine *source_records = memoryRecords;
    int source_count = memoryRecordsCount;
    DataLine *file_records = NULL;

    if (source == TELEMETRY_SOURCE_EXAMPLE_FILE) {
        file_records = open_telemetry(&source_count);
        if (file_records == NULL) {
            return NULL;
        }
        source_records = file_records;
    } else if (source != TELEMETRY_SOURCE_CURRENT_RUN) {
        return NULL;
    }

    if (source_count <= 0 || source_records == NULL) {
        free(file_records);
        return NULL;
    }

    double *returnArray = malloc(sizeof(*returnArray) * (size_t)source_count);
    if (returnArray == NULL) {
        free(file_records);
        return NULL;
    }

    for (int i = 0; i < source_count; ++i) {
        returnArray[i] = get_record_value(&source_records[i], recordType);
    }

    *recordSize = source_count;
    free(file_records);
    return returnArray;
}
