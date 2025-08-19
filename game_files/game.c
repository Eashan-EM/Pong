#include "game.h"
#include "bar.c"
#include "ball.c"

void draw_to_screen() {
    SDL_RenderClear(gameData.renderer);
    draw_bar();
    draw_ball();
    SDL_SetRenderDrawColor(gameData.renderer, 0, 0, 0, 255);
    SDL_RenderPresent(gameData.renderer);
}

void show_animation() {
    bar_animation();
    ball_animation();
    draw_to_screen();
}

void game_data_setup() {
    gameData.windowHeight = 0.8*EM_ASM_INT({return document.body.clientHeight});
    gameData.windowWidth = 0.8*EM_ASM_INT({return document.body.clientWidth});

    gameData.points.bar1 = gameData.points.bar2 = 0;
    setup_components();
}

void setup_components() {
    bar_setup();
    ball_setup();
}

void game_over(enum BALL_COLLISION coll) {
    switch (coll)
    {
    case COLL_WALL_LEFT:
        gameData.points.bar2++;
        break;
    case COLL_WALL_RIGHT:
        gameData.points.bar1++;
        break;
    default:
        break;
    }
    printf("%d %d\n", gameData.points.bar1, gameData.points.bar2);
}

void game_setup() {
    game_data_setup();
    SDL_Init( SDL_INIT_VIDEO );
    SDL_CreateWindowAndRenderer(gameData.windowWidth, gameData.windowHeight, 0, &gameData.window, &gameData.renderer);
    SDL_SetRenderDrawColor(gameData.renderer, 0, 0, 0, 255);
    draw_to_screen();
}