#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdlib.h>

struct Ball {
    int x, y, radius;
    int movX, movY;
};

struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    unsigned windowHeight, windowWidth;

    struct {
        SDL_Rect *barArray;
        unsigned barArrayLen;
        int barOffset, maxBarHeight, minBarHeight, maxBarWidth, barMove;
    } barData;

    struct {
        struct Ball *ballArray;
        int ballArrayLen, ballArrayFilled;
        int minRadius, minMovX, maxMovX, minMovY, maxMovY;
    } ballData;

    struct {
        unsigned bar1, bar2;
    } points;
} gameData;

enum KEY_PRESS {
    BAR1UP = 1,
    BAR1DOWN = 2,
    BAR2UP = 4,
    BAR2DOWN = 8,
};

enum BALL_COLLISION {
    COLL_WALL_LEFT,
    COLL_WALL_RIGHT,
    COLL_WALL_TOP,
    COLL_WALL_BOTTOM,
    COLL_BAR_1,
    COLL_BAR_2,
    COLL_NONE
};

void setup_components();
void game_over(enum BALL_COLLISION);

void bar_setup();
void draw_bar();
void bar_animation();

void ball_setup();
void draw_ball();
void ball_animation();