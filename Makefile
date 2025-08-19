compile:
	emcc --emrun -o Pong.html --shell-file shell.html -sUSE_SDL=2 -sUSE_SDL_GFX=2 -sUSE_SDL_TTF=2 main.c

run:
	emrun --browser /usr/bin/opera-developer Pong.html