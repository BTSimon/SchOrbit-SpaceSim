#include "../../headers/event_handler.h"
#include "../../headers/main.h"
#include "../../headers/window.h"

void event_handler(UI_ACTIONS action) {
    switch (action) {
        case UI_ACTION_QUIT:
            quit_game();
            exit_window();
        case UI_ACTION_PAUSE:
    }
}