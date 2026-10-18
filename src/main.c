#include <stdbool.h>

#include "../headers/game.h"
#include "../headers/window.h"

bool running = true;

int main(void)
{
    if (init_window() > 0) {
        return 1;
    }

    init_game();

    while (running) {
        game_loop();
    }

    return 0;
}

void quit_game(void) {
    running = false;
}