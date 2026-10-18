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
    float mass;
    float drag_coefficient;
} NoseProperties;

typedef struct {
    float mass;
    float fuel_storage;
} BodyProperties;

typedef struct {
    float mass;
    float thrust; 
    float propellant_flow_rate;
} ThrusterProperties;

NoseProperties get_nose_properties(NoseType nose);
BodyProperties get_body_properties(BodyType body);
ThrusterProperties get_thruster_properties(ThrusterType thruster);

#endif