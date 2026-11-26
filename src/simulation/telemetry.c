#include "../../headers/telemetry.h"
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include "../../headers/debugmalloc.h"

/*
    Correct (?) workflow for the given telemetry recording session
    init -> create the file
*/

static FILE *records;

static DataLine *memoryRecords;
static int memoryRecordsCapacity = 0;
static int memoryRecordsCount;
static bool memory_recording_failed;

TelemetryStatus init_telemetry(void) {
    time_t now = time(NULL);
    if (now == (time_t)-1) {
        fprintf(stderr, "telemetry: could not read the system clock\n");
        return TELEMETRY_STATUS_ERROR;
    }

    struct tm *t = localtime(&now);
    if (t == NULL) {
        fprintf(stderr, "telemetry: could not convert the system time\n");
        return TELEMETRY_STATUS_ERROR;
    }

    char filename[64];

    if (strftime(filename, sizeof(filename), "record_%Y-%m-%d_%H-%M-%S.csv", t) == 0) {
        fprintf(stderr, "telemetry: could not create the recording filename\n");
        return TELEMETRY_STATUS_ERROR;
    }

    records = fopen(filename, "w");
    if (records == NULL) {
        fprintf(stderr, "telemetry: could not open '%s': ", filename);
        perror("telemetry");
        return TELEMETRY_STATUS_ERROR;
    }

    return TELEMETRY_STATUS_OK;
}

static bool parse_line(const char *line, DataLine *record) {
    return sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf",
        &record->timestamp,
        &record->acceleration,
        &record->mass,
        &record->altitude,
        &record->speed,
        &record->thrust) == 6;
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
        if (parse_line(buff, &line)) {
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

TelemetryStatus record_telemetry(DataLine dataEntry) {
    if (memory_recording_failed) {
        return TELEMETRY_STATUS_ERROR;
    }

    if (memoryRecordsCount == memoryRecordsCapacity) {
        if (memoryRecordsCapacity > INT_MAX / 2) {
            memory_recording_failed = true;
            return TELEMETRY_STATUS_ERROR;
        }

        int new_capacity = memoryRecordsCapacity == 0
            ? 1
            : memoryRecordsCapacity * 2;
        if ((size_t)new_capacity > SIZE_MAX / sizeof(*memoryRecords)) {
            memory_recording_failed = true;
            return TELEMETRY_STATUS_ERROR;
        }

        DataLine *memptr = realloc(memoryRecords, sizeof(*memoryRecords) * new_capacity);

        if (memptr == NULL) {
            memory_recording_failed = true;
            return TELEMETRY_STATUS_ERROR;
        }

        memoryRecords = memptr;
        memoryRecordsCapacity = new_capacity;
    }

    memoryRecords[memoryRecordsCount++] = dataEntry;
    return TELEMETRY_STATUS_OK;
}

TelemetryStatus save_telemetry(void) {
    if (records == NULL) {
        fprintf(stderr, "telemetry: no recording file is open\n");
        free(memoryRecords);
        memoryRecords = NULL;
        memoryRecordsCapacity = 0;
        memoryRecordsCount = 0;
        memory_recording_failed = false;
        return TELEMETRY_STATUS_ERROR;
    }

    bool write_ok = fprintf(
        records,
        "time,acceleration,mass,altitude,speed,thrust\n"
    ) >= 0;

    for (int rec = 0; write_ok && rec < memoryRecordsCount; ++rec) {
        write_ok = fprintf(
            records,
            "%.17g,%.17g,%.17g,%.17g,%.17g,%.17g\n",
            memoryRecords[rec].timestamp,
            memoryRecords[rec].acceleration,
            memoryRecords[rec].mass,
            memoryRecords[rec].altitude,
            memoryRecords[rec].speed,
            memoryRecords[rec].thrust
        ) >= 0;
    }

    int close_result = fclose(records);
    records = NULL;
    free(memoryRecords);
    memoryRecords = NULL;
    memoryRecordsCapacity = 0;
    memoryRecordsCount = 0;
    memory_recording_failed = false;

    if (!write_ok || close_result != 0) {
        fprintf(stderr, "telemetry: failed while writing the recording file: ");
        perror("telemetry");
        return TELEMETRY_STATUS_ERROR;
    }

    return TELEMETRY_STATUS_OK;
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
