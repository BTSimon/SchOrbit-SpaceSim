#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/game.h"
#include "../headers/window.h"
#include "../headers/telemetry.h"

/*
PROJECT SchORBIT by Bartha-Tóth Simon
Date of latest update: 2026.11.09


TO BE REMOVED FROM FILES OTHER THAN PROJECT SUBMISSION:

NEPTUN: MGNNGD

*/

bool running = true;

int main(void)
{
    if (init_window() != APP_STATUS_OK) {
        return EXIT_FAILURE;
    }
    if (init_game() != APP_STATUS_OK) {
        exit_window();
        return EXIT_FAILURE;
    }

    while (running) {
        game_loop();
    }

    exit_window();
    return EXIT_SUCCESS;
}

void quit_game(void) {
    if (save_telemetry() != TELEMETRY_STATUS_OK) {
        fputs("The telemetry recording may be incomplete.\n", stderr);
    }
    running = false;
}
