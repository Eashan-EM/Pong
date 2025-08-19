int read_input_event() {
    int out = 0;
    const Uint8 *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_PAGEUP])
        out += BAR2UP;
    else if (keystate[SDL_SCANCODE_PAGEDOWN])
        out += BAR2DOWN;
    if (keystate[SDL_SCANCODE_W])
        out += BAR1UP;
    else if (keystate[SDL_SCANCODE_S])
        out += BAR1DOWN;

    return out;
}

void bar_animation() {
    int key = read_input_event();

    if (key & BAR1UP)
        gameData.barData.barArray[0].y = (gameData.barData.barArray[0].y-gameData.barData.barMove<0)?0:(gameData.barData.barArray[0].y-gameData.barData.barMove);
    if (key & BAR1DOWN)
        gameData.barData.barArray[0].y = (gameData.barData.barArray[0].y+gameData.barData.barArray[0].h+gameData.barData.barMove>gameData.windowHeight)?(gameData.windowHeight-gameData.barData.barArray[0].h):(gameData.barData.barArray[0].y+gameData.barData.barMove);
    if (key & BAR2UP)
        gameData.barData.barArray[1].y = (gameData.barData.barArray[1].y-gameData.barData.barMove<0)?0:(gameData.barData.barArray[1].y-gameData.barData.barMove);
    if (key & BAR2DOWN)
        gameData.barData.barArray[1].y = (gameData.barData.barArray[1].y+gameData.barData.barArray[1].h+gameData.barData.barMove>gameData.windowHeight)?(gameData.windowHeight-gameData.barData.barArray[1].h):(gameData.barData.barArray[1].y+gameData.barData.barMove);
}

void draw_bar() {
    SDL_SetRenderDrawColor(gameData.renderer, 255, 255, 255, 255);

    for (int i=0; i<gameData.barData.barArrayLen; i++) {
        SDL_RenderFillRect(gameData.renderer, gameData.barData.barArray+i);
    }
}

void bar_setup() {
    gameData.barData.barArrayLen = 2;
    gameData.barData.barArray = (SDL_Rect *) malloc(gameData.barData.barArrayLen*sizeof(SDL_Rect));

    gameData.barData.barOffset = 80;
    gameData.barData.maxBarHeight = 100;
    gameData.barData.minBarHeight = 50;
    gameData.barData.maxBarWidth = 5;
    gameData.barData.barMove = 3;

    gameData.barData.barArray[0] = (SDL_Rect) {gameData.barData.barOffset, (gameData.windowHeight-gameData.barData.maxBarHeight)/2, gameData.barData.maxBarWidth, gameData.barData.maxBarHeight};
    gameData.barData.barArray[1] = (SDL_Rect) {gameData.windowWidth-gameData.barData.barOffset, (gameData.windowHeight-gameData.barData.maxBarHeight)/2, gameData.barData.maxBarWidth, gameData.barData.maxBarHeight};
}