#include "../../headers/ui.h"

UI_ACTIONS ui_handle_event(SDL_Event *event) {
    switch (event->key.key) {
        case SDLK_ESCAPE:
            return UI_ACTION_QUIT;
        case SDLK_SPACE:
            return UI_ACTION_PAUSE;
        case SDLK_UP:
            return UI_ACTION_INCREASE;
        case SDLK_DOWN:
            return UI_ACTION_DECREASE;
        case SDLK_1:
            return UI_ACTION_SHOW_DESIGN;
        case SDLK_2:
            return UI_ACTION_SHOW_TEST;
        case SDLK_3:
            return UI_ACTION_SHOW_STATISTICS;
        default:
            return UI_ACTION_NONE;
    }
    return UI_ACTION_NONE;
}
