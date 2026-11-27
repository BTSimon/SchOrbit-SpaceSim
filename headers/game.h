#ifndef GAME_H
#define GAME_H

#include "./app_status.h"

AppStatus init_game(void);
AppStatus start_new_flight(void);
void game_loop(void);

#endif
