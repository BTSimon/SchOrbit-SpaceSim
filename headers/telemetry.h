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
    THRUST
} DataType;

int init_telemetry(void);
int record_telemetry(DataLine dataEntry);
int save_telemetry(void);
double *create_record_array(DataType recordType, int *recordSize);

#endif