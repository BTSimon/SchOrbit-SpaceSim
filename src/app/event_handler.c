#include "../../headers/event_handler.h"
#include "../../headers/main.h"
#include "../../headers/game_state.h"

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
            break;
        case UI_ACTION_NONE:
        case UI_ACTION_PAUSE:
        case UI_ACTION_INCREASE:
        case UI_ACTION_DECREASE:
            break;
    }

    //events based on gamestate
    switch (gameState.gameMode) {
        case GAME_MODE_DESIGN:
            break;
        case GAME_MODE_STATISTICS:
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
