#include "../../headers/ui.h"

UI_ACTIONS ui_handle_event(SDL_Event *event) {
    switch (event->key.key) {
        case SDLK_ESCAPE:
            return UI_ACTION_QUIT;
        case SDLK_SPACE:
            return UI_ACTION_PAUSE;
        default:
            return UI_ACTION_NONE;
    }
    return UI_ACTION_NONE;
}