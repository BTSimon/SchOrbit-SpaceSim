#include "../../headers/components.h"

const NoseProperties noses[] = {
    [NOSE_SHARP] = {
        .mass = 2,
        .drag_properties = {
            .area = 0.05,
            .drag_coefficient = 0.3
        }
    },

    [NOSE_STANDARD] = {
        .mass = 100.0,
        .drag_properties = {
            .area = 8,
            .drag_coefficient = 0.4
        }
    }
};

const BodyProperties bodies[] = {
    [BODY_LIGHTWEIGHT] = {
        .mass = 65,
        .fuel_storage = 40 
    },

    [BODY_STANDARD] = {
        .mass = 1400.0,
        .fuel_storage = 1300.0
    }
};

const ThrusterProperties thrusters[] = {
    [THRUSTER_EFFICIENT] = {
        .mass = 8,
        .thrust = 5000,
        .propellant_flow_rate = 2.0,
        .drag_properties = {
            .area = 0.05,
            .drag_coefficient = 0.8
        }
    },

    [THRUSTER_FAST] = {
        .mass = 300.0,
        .thrust = 1800.0,
        .propellant_flow_rate = 40.0,
        .drag_properties = {
            .area = 10,
            .drag_coefficient = 0.8
        }
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