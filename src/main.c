#include <stdbool.h>

#include "../headers/game.h"
#include "../headers/window.h"

/*
PROJECT SchORBIT by Bartha-Tóth Simon
Date of latest update: 2026.11.09


TO BE REMOVED FROM FILES OTHER THAN PROJECT SUBMISSION:

NEPTUN: MGNNGD

*/

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

    exit_window();
    return 0;
}

void quit_game(void) {
    running = false;
}
