#include <emscripten.h>
#include "game_files/game.c"

int main() {
    game_setup();
    emscripten_set_main_loop(show_animation, 0, 0);
    return 0;
}