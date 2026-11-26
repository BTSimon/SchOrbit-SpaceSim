#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdio.h>

#define TELEMETRY_LINE_SIZE 2

typedef struct {
    double timestamp;
    double mass;
    double acceleration;
    double altitude;
    double speed;
    double thrust;
} DataLine;

typedef enum {
    TIMESTAMP,
    MASS,
    ACCELERATION,
    ALTITUDE,
    SPEED,
    THRUST,
    DATA_NONE
} DataType;

typedef enum {
    TELEMETRY_SOURCE_CURRENT_RUN,
    TELEMETRY_SOURCE_EXAMPLE_FILE
} TelemetrySource;

typedef enum {
    TELEMETRY_STATUS_OK = 0,
    TELEMETRY_STATUS_ERROR = 1
} TelemetryStatus;

TelemetryStatus init_telemetry(void);
TelemetryStatus record_telemetry(DataLine dataEntry);
TelemetryStatus save_telemetry(void);
double *create_record_array(
    DataType recordType,
    int *recordSize,
    TelemetrySource source
);

#endif
