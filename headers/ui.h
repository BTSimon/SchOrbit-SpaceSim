#ifndef UI_H
#define UI_H

#include <SDL3/SDL.h>

typedef enum {
    UI_ACTION_NONE,
    UI_ACTION_QUIT,
    UI_ACTION_PAUSE
} UI_ACTIONS;

UI_ACTIONS ui_handle_event(SDL_Event *event);

#endif