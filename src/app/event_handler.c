#include "../../headers/event_handler.h"
#include "../../headers/main.h"
#include "../../headers/game_state.h"
#include "../../headers/telemetry.h"

void event_handler(UI_ACTIONS action) {
    //Same event for all gamestates 
    switch (action) {
        case UI_ACTION_QUIT:
            quit_game();
            break;
        case UI_ACTION_SHOW_DESIGN:
            gameState.gameMode = GAME_MODE_DESIGN;
            gameState.paused = true;
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
            break;
    }

    //events based on gamestate
    switch (gameState.gameMode) {
        case GAME_MODE_DESIGN:
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
                default:
                    break;
            }
            break;

    }
}
