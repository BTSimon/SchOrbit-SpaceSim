#include "../../headers/components.h"

const NoseProperties noses[] = {
    [NOSE_SHARP] = {
        .mass = 60,
        .drag_coefficient = 0.2f 
    },

    [NOSE_STANDARD] = {
        .mass = 100,
        .drag_coefficient = 0.4f
    }
};

const BodyProperties bodies[] = {
    [BODY_LIGHTWEIGHT] = {
        .mass = 1000,
        .fuel_storage = 850 
    },

    [BODY_STANDARD] = {
        .mass = 1400,
        .fuel_storage = 1300
    }
};

const ThrusterProperties thrusters[] = {
    [THRUSTER_EFFICIENT] = {
        .mass = 250,
        .thrust = 100000,
        .propellant_flow_rate = 20 
    },

    [THRUSTER_FAST] = {
        .mass = 300,
        .thrust = 18000,
        .propellant_flow_rate = 40
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