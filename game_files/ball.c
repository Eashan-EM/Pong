enum BALL_COLLISION check_ball_collision(int index) {
    struct Ball ball = gameData.ballData.ballArray[index];
    SDL_Rect bar1 = gameData.barData.barArray[0], bar2 = gameData.barData.barArray[1];
    if (ball.x-ball.radius<0)
        return COLL_WALL_LEFT;
    if (ball.y-ball.radius<0)
        return COLL_WALL_TOP;
    if (ball.x+ball.radius>gameData.windowWidth)
        return COLL_WALL_RIGHT;
    if (ball.y+ball.radius>gameData.windowHeight)
        return COLL_WALL_BOTTOM;
    if ((ball.x-ball.radius<bar1.x+gameData.barData.maxBarWidth && ball.x-ball.radius>bar1.x) && !(
        ball.y<bar1.y ||
        ball.y>bar1.y+bar1.h
    ))
        return COLL_BAR_1;
    if ((ball.x+ball.radius>bar2.x && ball.x+ball.radius<bar2.x+gameData.barData.maxBarWidth) && !(
        ball.y<bar2.y ||
        ball.y>bar2.y+bar2.h
    ))
        return COLL_BAR_2;
    return COLL_NONE;
}

void ball_animation() {
    for (int i=0; i<gameData.ballData.ballArrayFilled; i++) {
        gameData.ballData.ballArray[i].x += gameData.ballData.ballArray[i].movX;
        gameData.ballData.ballArray[i].y += gameData.ballData.ballArray[i].movY;
        enum BALL_COLLISION info = check_ball_collision(i);
        switch (info) {
            case COLL_WALL_LEFT:
            case COLL_WALL_RIGHT:
                game_over(info);
            case COLL_BAR_1:
            case COLL_BAR_2:
                gameData.ballData.ballArray[i].movX = -gameData.ballData.ballArray[i].movX;
                break;
            case COLL_WALL_TOP:
            case COLL_WALL_BOTTOM:
                gameData.ballData.ballArray[i].movY = -gameData.ballData.ballArray[i].movY;
                break;
            case COLL_NONE:
                break;
        }
    }
}

void draw_ball() {
    for (int i=0; i<gameData.ballData.ballArrayFilled; i++)
        filledCircleRGBA(gameData.renderer, gameData.ballData.ballArray[i].x, gameData.ballData.ballArray[i].y, gameData.ballData.ballArray[i].radius, 255, 255, 255, 255);
}

void ball_setup() {
    gameData.ballData.ballArrayLen = 50;
    gameData.ballData.ballArrayFilled = 1;
    gameData.ballData.minRadius = 8;
    gameData.ballData.minMovX = 2;
    gameData.ballData.maxMovX = 10;
    gameData.ballData.minMovY = 2;
    gameData.ballData.maxMovY = 10;
    gameData.ballData.ballArray = (struct Ball *) malloc(gameData.ballData.ballArrayLen*sizeof(struct Ball));

    gameData.ballData.ballArray[0] = (struct Ball) {10, 10, gameData.ballData.minRadius, 4, 4};
}