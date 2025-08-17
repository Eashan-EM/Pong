#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

#define SPRITE "imgs/bar.png"
#define SPRITE_OBSTACLE "imgs/obstacle.png"
#define SPRITE_BALL "imgs/ball.png"

Uint32 last_time, current_time;
Uint32 ms_per_frame = 10;
Uint32 fps = 10;

enum KEY_PRESS {
    LEFT = -1,
    RIGHT = 1,
    NONE = 0
};

enum BALL_COLLISION {
    COLL_WALL_LEFT,
    COLL_WALL_RIGHT,
    COLL_WALL_TOP,
    COLL_WALL_BOTTOM,
    COLL_BAR_TOP,
    COLL_BAR_LEFT,
    COLL_BAR_RIGHT,
    COLL_OBSTACLE_LEFT,
    COLL_OBSTACLE_RIGHT,
    COLL_OBSTACLE_TOP,
    COLL_OBSTACLE_BOTTOM,
    COLL_NONE
};

struct Obstacle {
    char exists;
    unsigned short x;
    unsigned short y;
};

struct Ball {
    char xMov;
    char yMov;
};

struct {
    int canvasWidth, barImgWidthOffset;
    int canvasHeight, barImgHeightOffset;

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *sprite_texture, *obstacle_texture, *ball_texture;
    SDL_Rect bar_dest, obs_dest, ball_dest;
    struct Ball ball;
    struct {
        struct Obstacle *obs_array;
        unsigned obs_array_len, obs_rows, obs_cols, obs_col_off, obs_row_off, obs_to_be_killed, obs_killed;
        unsigned short x, y, w, h;
    } obstacleData;
} gameData;

enum KEY_PRESS read_input_event() {
    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                return LEFT;
            case SDLK_RIGHT:
                return RIGHT;
        }
    }
    return NONE;
}

void draw_bar() {
    SDL_RenderClear(gameData.renderer);
    SDL_RenderCopy( gameData.renderer, gameData.sprite_texture, NULL, &gameData.bar_dest );
}

void draw_obstacle() {
    for (int i=0; i<gameData.obstacleData.obs_array_len; i++) {
        if (gameData.obstacleData.obs_array[i].exists) {
            gameData.obs_dest.x = gameData.obstacleData.obs_array[i].x;
            gameData.obs_dest.y = gameData.obstacleData.obs_array[i].y;
            SDL_RenderCopy( gameData.renderer, gameData.obstacle_texture, NULL, &gameData.obs_dest );
        }
    }
}

void draw_ball() {
    SDL_RenderCopy(gameData.renderer, gameData.ball_texture, NULL, &gameData.ball_dest);
    SDL_RenderPresent(gameData.renderer);
}

void bar_animation() {
    enum KEY_PRESS key = read_input_event();
    current_time = SDL_GetTicks();
    int ms = current_time-last_time;
    if (ms<ms_per_frame) {
        return;
    }

    last_time = current_time;

    gameData.bar_dest.x += 10*key;
    if (gameData.bar_dest.x+gameData.bar_dest.w-gameData.barImgWidthOffset>gameData.canvasWidth) {
        gameData.bar_dest.x = gameData.canvasWidth-gameData.bar_dest.w+gameData.barImgWidthOffset;
    } else if (gameData.bar_dest.x<-gameData.barImgWidthOffset) {
        gameData.bar_dest.x = -gameData.barImgWidthOffset;
    }
 
    draw_bar();
}

void obstacle_animation() {
    draw_obstacle();
}

void game_exit() {
    emscripten_cancel_main_loop();
}

enum BALL_COLLISION has_ball_collided() {
    if (gameData.ball_dest.x+gameData.ball_dest.w+2>gameData.canvasWidth) {
        return COLL_WALL_RIGHT;
    }
    if (gameData.ball_dest.x-2<0) {
        return COLL_WALL_LEFT;
    }
    if (gameData.ball_dest.y-2<0) {
        return COLL_WALL_TOP;
    }
    if (gameData.ball_dest.y+gameData.ball_dest.h+2>gameData.canvasHeight) {
        return COLL_WALL_BOTTOM;
    }

    if (gameData.ball_dest.x<gameData.bar_dest.x+gameData.bar_dest.w-gameData.barImgWidthOffset &&
        !(gameData.ball_dest.y+gameData.ball_dest.h<gameData.bar_dest.w+gameData.barImgHeightOffset ||
          gameData.ball_dest.y>gameData.bar_dest.w-gameData.barImgHeightOffset)
    ) {
        return COLL_BAR_RIGHT;
    }
    if (gameData.ball_dest.x+gameData.ball_dest.w>gameData.bar_dest.x+gameData.barImgWidthOffset &&
        !(gameData.ball_dest.y+gameData.ball_dest.h<gameData.bar_dest.w+gameData.barImgHeightOffset ||
          gameData.ball_dest.y>gameData.bar_dest.w-gameData.barImgHeightOffset)
    ) {
        return COLL_BAR_LEFT;
    }
    if (gameData.ball_dest.y+gameData.ball_dest.h>gameData.bar_dest.y+gameData.barImgHeightOffset &&
        !(gameData.ball_dest.x>gameData.bar_dest.x+gameData.bar_dest.w-gameData.barImgWidthOffset ||
          gameData.ball_dest.x+gameData.ball_dest.w<gameData.bar_dest.x+gameData.barImgWidthOffset
        )) {
        return COLL_BAR_TOP;
    }

    struct Obstacle temp_obs;
    if (gameData.ball_dest.y<gameData.obstacleData.y+gameData.obstacleData.h) {
        for (int i=gameData.obstacleData.obs_rows-1; i>=0; i--) {
            for (int j=0; j<gameData.obstacleData.obs_cols; j++) {
                temp_obs = gameData.obstacleData.obs_array[j+i*gameData.obstacleData.obs_cols];
                if (!(gameData.ball_dest.y<temp_obs.y+gameData.obs_dest.h)) {
                    break;
                }
                if (temp_obs.exists) {
                    if ((
                        gameData.ball_dest.y+gameData.ball_dest.h>=temp_obs.y &&
                        gameData.ball_dest.y<=temp_obs.y) &&
                        !(gameData.ball_dest.x+gameData.ball_dest.w<temp_obs.x ||
                          gameData.ball_dest.x>temp_obs.x+gameData.obs_dest.w)) {
                        gameData.obstacleData.obs_array[j+i*gameData.obstacleData.obs_cols].exists = 0;
                        return COLL_OBSTACLE_TOP;
                    } /*else if (gameData.ball_dest.x+gameData.ball_dest.w>temp_obs.x && 
                        !(gameData.ball_dest.y+gameData.ball_dest.h<temp_obs.y ||
                          gameData.ball_dest.y>temp_obs.y+gameData.obs_dest.h)) {
                        gameData.obstacleData.obs_array[j+i*gameData.obstacleData.obs_cols].exists = 0;
                        return COLL_OBSTACLE_LEFT;
                    } else if ((
                        gameData.ball_dest.x<=temp_obs.y+gameData.obs_dest.w &&
                        gameData.ball_dest.x+gameData.ball_dest.w>=temp_obs.y+gameData.obs_dest.w) &&
                        !(gameData.ball_dest.y+gameData.ball_dest.h<temp_obs.x ||
                          gameData.ball_dest.y>temp_obs.y+gameData.obs_dest.h)) {
                        gameData.obstacleData.obs_array[j+i*gameData.obstacleData.obs_cols].exists = 0;
                        return COLL_OBSTACLE_RIGHT;
                    } else*/ if (
                        (gameData.ball_dest.y<=temp_obs.y+gameData.obs_dest.h &&
                         gameData.ball_dest.y+gameData.ball_dest.h>=temp_obs.y+gameData.obs_dest.h) && 
                        !(gameData.ball_dest.x+gameData.ball_dest.w<temp_obs.x ||
                          gameData.ball_dest.x>temp_obs.x+gameData.obs_dest.w)) {
                        gameData.obstacleData.obs_array[j+i*gameData.obstacleData.obs_cols].exists = 0;
                        gameData.obstacleData.obs_killed++;
                        printf("collide\n");
                        return COLL_OBSTACLE_BOTTOM;
                    }
                    //printf("%d %d\n", gameData.obstacleData.obs_killed, gameData.obstacleData.obs_to_be_killed);
                    if (gameData.obstacleData.obs_killed==gameData.obstacleData.obs_to_be_killed) {
                        game_exit();
                    }
                }
            }
        }
    }
    return COLL_NONE;
}

void ball_animation() {
    gameData.ball_dest.x += gameData.ball.xMov;
    gameData.ball_dest.y += gameData.ball.yMov;

    switch (has_ball_collided()) {
        case COLL_WALL_RIGHT:
        case COLL_WALL_LEFT:
        case COLL_BAR_LEFT:
        case COLL_BAR_RIGHT:
        case COLL_OBSTACLE_LEFT:
        case COLL_OBSTACLE_RIGHT:
            gameData.ball.xMov = -gameData.ball.xMov;
            break;
        case COLL_WALL_TOP:
        case COLL_BAR_TOP:
        case COLL_OBSTACLE_BOTTOM:
        case COLL_OBSTACLE_TOP:
            gameData.ball.yMov = -gameData.ball.yMov;
            break;
        case COLL_WALL_BOTTOM:
            game_exit();
            break;
        case COLL_NONE:
            break;
    }
    draw_ball();
}

void show_animation() {
    bar_animation();
    obstacle_animation();
    ball_animation();
}

void setup_gamedata() {
    gameData.canvasHeight = EM_ASM_INT({return document.body.clientHeight});
    gameData.canvasWidth  = EM_ASM_INT({return document.body.clientWidth});

    gameData.canvasWidth = (gameData.canvasWidth<500)?gameData.canvasWidth:500;
}

void setup_bar() {
    gameData.barImgWidthOffset = 40;
    gameData.barImgHeightOffset = 20;

    SDL_Surface *temp_surface = IMG_Load(SPRITE);
    
    gameData.sprite_texture = SDL_CreateTextureFromSurface(gameData.renderer, temp_surface);

    SDL_QueryTexture( gameData.sprite_texture, NULL, NULL, &gameData.bar_dest.w, &gameData.bar_dest.h ); // query the width and height
    gameData.bar_dest.x = (gameData.canvasWidth-gameData.bar_dest.w)/2;
    gameData.bar_dest.y = gameData.canvasHeight-2*gameData.bar_dest.h;
    SDL_FreeSurface(temp_surface);
}

void setup_obstacles() {
    gameData.obstacleData.obs_rows = 10;
    gameData.obstacleData.obs_cols = 10;
    gameData.obstacleData.obs_array_len = gameData.obstacleData.obs_cols*gameData.obstacleData.obs_rows;
    gameData.obstacleData.obs_col_off = 25;
    gameData.obstacleData.obs_row_off = 20;
    unsigned char OBS_SPACE = 20;
    gameData.obstacleData.x = gameData.obstacleData.obs_col_off;
    gameData.obstacleData.y = gameData.obstacleData.obs_row_off;
    gameData.obstacleData.obs_to_be_killed = gameData.obstacleData.obs_array_len;
    gameData.obstacleData.obs_killed = 0;
    
    gameData.obstacleData.obs_array = (struct Obstacle *) malloc(sizeof(struct Obstacle)*gameData.obstacleData.obs_rows*gameData.obstacleData.obs_cols);

    SDL_Surface *temp_surface = IMG_Load(SPRITE_OBSTACLE);
    gameData.obstacle_texture = SDL_CreateTextureFromSurface(gameData.renderer, temp_surface);

    SDL_QueryTexture(gameData.obstacle_texture, NULL, NULL, &gameData.obs_dest.w, &gameData.obs_dest.h);
    
    gameData.obstacleData.w = gameData.obstacleData.obs_cols*(gameData.obs_dest.w+OBS_SPACE);
    gameData.obstacleData.h = gameData.obstacleData.obs_rows*(gameData.obs_dest.h+OBS_SPACE);

    gameData.obs_dest.x = 50;
    gameData.obs_dest.y = 50;
    
    for (int i=0; i<gameData.obstacleData.obs_rows; i++) {
        for (int j=0; j<gameData.obstacleData.obs_cols; j++) {
            gameData.obstacleData.obs_array[i*gameData.obstacleData.obs_cols+j].exists = 1;
            gameData.obstacleData.obs_array[i*gameData.obstacleData.obs_cols+j].x = gameData.obstacleData.obs_col_off+j*(gameData.obs_dest.w+OBS_SPACE);
            gameData.obstacleData.obs_array[i*gameData.obstacleData.obs_cols+j].y = gameData.obstacleData.obs_row_off+i*(gameData.obs_dest.h+OBS_SPACE);
        }
    }

    SDL_FreeSurface(temp_surface);
}

void setup_ball() {
    gameData.ball.xMov = 2;
    gameData.ball.yMov = 3;

    SDL_Surface *temp_surface = IMG_Load(SPRITE_BALL);
    gameData.ball_texture = SDL_CreateTextureFromSurface(gameData.renderer, temp_surface);

    SDL_QueryTexture(gameData.ball_texture, NULL, NULL, &gameData.ball_dest.w, &gameData.ball_dest.h);
    gameData.ball_dest.x = (gameData.canvasWidth-gameData.ball_dest.w)/2;
    gameData.ball_dest.y = gameData.bar_dest.y+3;
}

void setup_sprites() {
    setup_bar();
    setup_obstacles();
    setup_ball();
}

int game_setup() {
    setup_gamedata();
    SDL_Init( SDL_INIT_VIDEO );

    SDL_CreateWindowAndRenderer(gameData.canvasWidth, gameData.canvasHeight, 0, &gameData.window, &gameData.renderer );
    setup_sprites();

    SDL_SetRenderDrawColor( gameData.renderer, 0, 0, 0, 255 );
    SDL_RenderClear( gameData.renderer );

    draw_bar();

    last_time = SDL_GetTicks();
    return 0;
}