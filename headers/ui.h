#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>

typedef enum {
    UI_ACTION_NONE,
    UI_ACTION_QUIT,
    UI_ACTION_PAUSE,
    UI_ACTION_INCREASE,
    UI_ACTION_DECREASE,
    UI_ACTION_SHOW_DESIGN,
    UI_ACTION_SHOW_TEST,
    UI_ACTION_SHOW_STATISTICS
} UI_ACTIONS;

UI_ACTIONS ui_handle_event(SDL_Event *event);

#endif
