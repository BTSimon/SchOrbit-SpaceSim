#ifndef COMPONENTS_H
#define COMPONENTS_H

typedef struct {
    double drag_coefficient;
    double area;
} Drag;

typedef enum {
    NOSE_STANDARD,
    NOSE_SHARP,
    NOSE_BLUNT,
    NOSE_TYPE_COUNT
} NoseType;

typedef enum {
    BODY_STANDARD,
    BODY_LIGHTWEIGHT,
    BODY_HEAVY,
    BODY_TYPE_COUNT
} BodyType;

typedef enum {
    THRUSTER_HIGH_THRUST,
    THRUSTER_EFFICIENT,
    THRUSTER_BALANCED,
    THRUSTER_HEAVY,
    THRUSTER_TYPE_COUNT
} ThrusterType;

#define STANDARD_GRAVITY_M_S2 9.80665

typedef struct {
    const char *name;
    double mass;
    Drag drag_properties;
} NoseProperties;

typedef struct {
    const char *name;
    double mass;
    double fuel_storage;
} BodyProperties;

typedef struct {
    const char *name;
    double mass;
    double thrust; 
    double specific_impulse;
    Drag drag_properties;
} ThrusterProperties;

NoseProperties get_nose_properties(NoseType nose);
BodyProperties get_body_properties(BodyType body);
ThrusterProperties get_thruster_properties(ThrusterType thruster);

#endif
