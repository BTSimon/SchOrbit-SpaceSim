#ifndef COMPONENTS_H
#define COMPONENTS_H

typedef struct {
    double drag_coefficient;
    double area;
} Drag;

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
    double mass;
    Drag drag_properties;
} NoseProperties;

typedef struct {
    double mass;
    double fuel_storage;
} BodyProperties;

typedef struct {
    double mass;
    double thrust; 
    double propellant_flow_rate;
    Drag drag_properties;
} ThrusterProperties;

NoseProperties get_nose_properties(NoseType nose);
BodyProperties get_body_properties(BodyType body);
ThrusterProperties get_thruster_properties(ThrusterType thruster);

#endif