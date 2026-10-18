#ifndef COMPONENTS_H
#define COMPONENTS_H

typedef enum {
    NOSE_STANDARD,
    NOSE_SHARP
} NoseType;

typedef enum {
    BODY_STANDARD,
    BODY_LIGHTWEIGHT
} BodyType;

typedef enum {
    THRUSTER_FAST,
    THRUSTER_EFFICIENT
} ThrusterType;

typedef struct {
    int mass;
    float drag_coefficient;
} NoseProperties;

typedef struct {
    int mass;
    int fuel_storage;
} BodyProperties;

typedef struct {
    int mass;
    int thrust; 
    int propellant_flow_rate;
} ThrusterProperties;

NoseProperties get_nose_properties(NoseType nose);
BodyProperties get_body_properties(BodyType body);
ThrusterProperties get_thruster_properties(ThrusterType thruster);

#endif