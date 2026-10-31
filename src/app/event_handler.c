#include "../../headers/event_handler.h"
#include "../../headers/main.h"
#include "../../headers/game_state.h"

void event_handler(UI_ACTIONS action) {
    switch (action) {
        case UI_ACTION_QUIT:
            quit_game();
            break;
        case UI_ACTION_PAUSE:
            gameState.paused = !gameState.paused;
            break;
        case UI_ACTION_INCREASE:
            if (gameState.thrust_level < 1) {
                gameState.thrust_level += 0.05;
            }
            break;

        case UI_ACTION_DECREASE:
            if (gameState.thrust_level > 0) {
                gameState.thrust_level -= 0.05;
            }
            break;

        case UI_ACTION_SHOW_DESIGN:
            gameState.gameMode = GAME_MODE_DESIGN;
            break;
        case UI_ACTION_SHOW_TEST:
            gameState.gameMode = GAME_MODE_TEST;
            break;
        case UI_ACTION_SHOW_STATISTICS:
            gameState.gameMode = GAME_MODE_STATISTICS;
            break;
        case UI_ACTION_NONE:
            break;
    }
}
