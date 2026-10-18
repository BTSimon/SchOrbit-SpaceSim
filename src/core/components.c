#include "../../headers/components.h"

const NoseProperties noses[] = {
    [NOSE_SHARP] = {
        .mass = 60.0f,
        .drag_coefficient = 0.2f 
    },

    [NOSE_STANDARD] = {
        .mass = 100.0f,
        .drag_coefficient = 0.4f
    }
};

const BodyProperties bodies[] = {
    [BODY_LIGHTWEIGHT] = {
        .mass = 1000.0f,
        .fuel_storage = 850.0f 
    },

    [BODY_STANDARD] = {
        .mass = 1400.0f,
        .fuel_storage = 1300.0f
    }
};

const ThrusterProperties thrusters[] = {
    [THRUSTER_EFFICIENT] = {
        .mass = 250.0f,
        .thrust = 7000.0f,
        .propellant_flow_rate = 20.0f 
    },

    [THRUSTER_FAST] = {
        .mass = 300.0f,
        .thrust = 1800.0f,
        .propellant_flow_rate = 40.0f
    }
};

NoseProperties get_nose_properties(NoseType nose) {
    return noses[nose];
}

BodyProperties get_body_properties(BodyType body) {
    return bodies[body];
}

ThrusterProperties get_thruster_properties(ThrusterType thruster) {
    return thrusters[thruster];
}