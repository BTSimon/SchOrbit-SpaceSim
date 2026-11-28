#include "../../headers/components.h"

static const NoseProperties noses[NOSE_TYPE_COUNT] = {
    [NOSE_STANDARD] = {
        .name = "Standard nose",
        .mass = 500.0,
        .drag_properties = {
            .area = 0.8,
            .drag_coefficient = 0.40
        }
    },
    [NOSE_SHARP] = {
        .name = "Pointed nose",
        .mass = 300.0,
        .drag_properties = {
            .area = 0.45,
            .drag_coefficient = 0.25
        }
    },
    [NOSE_BLUNT] = {
        .name = "Blunt re-entry nose",
        .mass = 850.0,
        .drag_properties = {
            .area = 2.2,
            .drag_coefficient = 0.80
        }
    }
};

static const BodyProperties bodies[BODY_TYPE_COUNT] = {
    [BODY_STANDARD] = {
        .name = "Standard tank",
        .mass = 39000.0,
        .fuel_storage = 25000.0
    },
    [BODY_LIGHTWEIGHT] = {
        .name = "Lightweight tank",
        .mass = 12500.0,
        .fuel_storage = 8000.0
    },
    [BODY_HEAVY] = {
        .name = "Extended heavy tank",
        .mass = 93000.0,
        .fuel_storage = 65000.0
    }
};

static const ThrusterProperties thrusters[THRUSTER_TYPE_COUNT] = {
    [THRUSTER_HIGH_THRUST] = {
        .name = "High-thrust engine",
        .mass = 3500.0,
        .thrust = 1500000.0,
        .specific_impulse = 285.0,
        .drag_properties = {
            .area = 0.9,
            .drag_coefficient = 0.85
        }
    },
    [THRUSTER_EFFICIENT] = {
        .name = "Efficient upper-stage engine",
        .mass = 600.0,
        .thrust = 150000.0,
        .specific_impulse = 355.0,
        .drag_properties = {
            .area = 0.15,
            .drag_coefficient = 0.80
        }
    },
    [THRUSTER_BALANCED] = {
        .name = "Balanced stage engine",
        .mass = 1800.0,
        .thrust = 650000.0,
        .specific_impulse = 315.0,
        .drag_properties = {
            .area = 0.55,
            .drag_coefficient = 0.82
        }
    },
    [THRUSTER_HEAVY] = {
        .name = "Heavy booster engine",
        .mass = 5000.0,
        .thrust = 2000000.0,
        .specific_impulse = 300.0,
        .drag_properties = {
            .area = 1.4,
            .drag_coefficient = 0.85
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
