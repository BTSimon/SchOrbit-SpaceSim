#include "../../headers/event_handler.h"
#include "../../headers/game.h"
#include "../../headers/main.h"
#include "../../headers/game_state.h"
#include "../../headers/telemetry.h"

#include <stdio.h>

static int wrap_index(int index, int count) {
    return ((index % count) + count) % count;
}

static void change_selected_design(int step) {
    ShipComponents *draft = &gameState.designParams.draft;
    switch (gameState.designParams.selected) {
        case NOSE:
            draft->nose = (NoseType)wrap_index((int)draft->nose + step, NOSE_TYPE_COUNT);
            break;
        case BODY:
            draft->body = (BodyType)wrap_index((int)draft->body + step, BODY_TYPE_COUNT);
            break;
        case THRUSTER:
            draft->thruster = (ThrusterType)wrap_index((int)draft->thruster + step, THRUSTER_TYPE_COUNT);
            break;
    }
}

void event_handler(UI_ACTIONS action) {
    //Same event for all gamestates 
    switch (action) {
        case UI_ACTION_QUIT:
            quit_game();
            break;
        case UI_ACTION_SHOW_DESIGN:
            gameState.gameMode = GAME_MODE_DESIGN;
            gameState.paused = true;
            gameState.designParams.selected = NOSE;
            gameState.designParams.draft = gameState.shipComponents;
            break;
        case UI_ACTION_SHOW_TEST:
            gameState.gameMode = GAME_MODE_TEST;
            gameState.paused = false;
            break;
        case UI_ACTION_SHOW_STATISTICS:
            gameState.gameMode = GAME_MODE_STATISTICS;
            gameState.paused = true;
            gameState.statisticsParams.graph_changed = true;
            break;
        case UI_ACTION_NONE:
        case UI_ACTION_PAUSE:
        case UI_ACTION_INCREASE:
        case UI_ACTION_DECREASE:
        case UI_ACTION_SOURCE_PREVIOUS:
        case UI_ACTION_SOURCE_NEXT:
        case UI_ACTION_GRAPH2_INCREASE:
        case UI_ACTION_GRAPH2_DECREASE:
        case UI_ACTION_CONFIRM_DESIGN:
            break;
    }

    //events based on gamestate
    switch (gameState.gameMode) {
        case GAME_MODE_DESIGN:
            switch (action) {
            case UI_ACTION_DECREASE:
                gameState.designParams.selected =
                    (Components)wrap_index((int)gameState.designParams.selected + 1, 3);
                break;
            case UI_ACTION_INCREASE:
                gameState.designParams.selected =
                    (Components)wrap_index((int)gameState.designParams.selected - 1, 3);
                break;
            case UI_ACTION_SOURCE_NEXT:
                change_selected_design(1);
                break;
            case UI_ACTION_SOURCE_PREVIOUS:
                change_selected_design(-1);
                break;
            case UI_ACTION_CONFIRM_DESIGN:
                if (start_new_flight() != APP_STATUS_OK) {
                    fputs("design: could not start the new flight\n", stderr);
                }
                break;
            default:
                break;
            }
            break;
        case GAME_MODE_STATISTICS:
            switch (action)
            {
                case UI_ACTION_INCREASE:
                    gameState.statisticsParams.y_axis = graph_axis_next(gameState.statisticsParams.y_axis, +1);
                    gameState.statisticsParams.graph_changed = true;
                    break;

                case UI_ACTION_DECREASE:
                    gameState.statisticsParams.y_axis = graph_axis_next(gameState.statisticsParams.y_axis, -1);
                    gameState.statisticsParams.graph_changed = true;
                    break;
                case UI_ACTION_GRAPH2_INCREASE:
                    gameState.statisticsParams.y2_axis = graph_axis_next(gameState.statisticsParams.y2_axis, +1);
                    gameState.statisticsParams.graph_changed = true;
                    break;
                case UI_ACTION_GRAPH2_DECREASE:
                    gameState.statisticsParams.y2_axis = graph_axis_next(gameState.statisticsParams.y2_axis, -1);
                    gameState.statisticsParams.graph_changed = true;
                    break;
                case UI_ACTION_SOURCE_PREVIOUS:
                case UI_ACTION_SOURCE_NEXT:
                    gameState.statisticsParams.source =
                        gameState.statisticsParams.source == TELEMETRY_SOURCE_CURRENT_RUN
                            ? TELEMETRY_SOURCE_EXAMPLE_FILE
                            : TELEMETRY_SOURCE_CURRENT_RUN;
                    gameState.statisticsParams.graph_changed = true;
                    break;
                default:
                    break;
            }
            break;

        case GAME_MODE_TEST:
            switch (action)
            {
                case UI_ACTION_PAUSE:
                    gameState.paused = !gameState.paused;
                    break;
                case UI_ACTION_INCREASE:
                    if (gameState.thrust_level < 1.0) {
                        gameState.thrust_level += 0.05;
                    }
                    break;

                case UI_ACTION_DECREASE:
                    if (gameState.thrust_level > 0.0) {
                        gameState.thrust_level -= 0.05;
                    }
                    break;
                case UI_ACTION_SOURCE_NEXT:
                    if (gameState.simulation_speed < 8.0) {
                        gameState.simulation_speed *= 2.0;
                    }
                    break;
                case UI_ACTION_SOURCE_PREVIOUS:
                    if (gameState.simulation_speed > 0.25) {
                        gameState.simulation_speed *= 0.5;
                    }
                    break;
                default:
                    break;
            }
            break;

    }
}
