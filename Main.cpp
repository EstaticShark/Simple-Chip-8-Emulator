#include <stdio.h>
#include <SDL.h>
#include "chip8.h"

unsigned char chip8_fontset[80] =
{
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

//Keyboard mapping
int key_input[16];
void read_keys(const Uint8 *keystate);

int main(int argc, char * argv[]) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { //Initialize SDL
		printf("SLD_Init failed to intialize.\n");
		return 1;
	}

	//Create window
	SDL_Window *window = SDL_CreateWindow("Martin's Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (window == NULL) { //Window has failed to create
		printf("SDL_CreateWindow failed to create a window.\n");
		SDL_Quit();
		return 1;
	}

	//Setup render
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);
	if (renderer == NULL) {
		printf("SDL_CreateRenderer failed to create a renderer.\n");
		SDL_Quit();
		return 1;
	}

	printf("SDL setup complete\n");

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Main game loop
	int game_loop = 1;
	while (game_loop) {

		
		read_keys(keystate);
	}


	SDL_Quit();

	printf("SDL exit\n");

	return 0;
}


/*
	Read the keyboard input and set the input 
*/
void read_keys(const Uint8 *keystate) {

	//	SDL_SCANCODE_1	SDL_SCANCODE_2	SDL_SCANCODE_3	SDL_SCANCODE_4
	//	SDL_SCANCODE_Q	SDL_SCANCODE_W	SDL_SCANCODE_E	SDL_SCANCODE_R
	//	SDL_SCANCODE_A	SDL_SCANCODE_S	SDL_SCANCODE_D	SDL_SCANCODE_F
	//	SDL_SCANCODE_Z	SDL_SCANCODE_X	SDL_SCANCODE_C	SDL_SCANCODE_V

	SDL_PumpEvents();

	for (int i = 0; i < 16; i++) key_input[i] = 0;

	if (keystate[SDL_SCANCODE_1]) key_input[0] = 1;
	if (keystate[SDL_SCANCODE_2]) key_input[1] = 1;
	if (keystate[SDL_SCANCODE_3]) key_input[2] = 1;
	if (keystate[SDL_SCANCODE_4]) key_input[3] = 1;

	if (keystate[SDL_SCANCODE_Q]) key_input[4] = 1;
	if (keystate[SDL_SCANCODE_W]) key_input[5] = 1;
	if (keystate[SDL_SCANCODE_E]) key_input[6] = 1;
	if (keystate[SDL_SCANCODE_R]) key_input[7] = 1;

	if (keystate[SDL_SCANCODE_A]) key_input[8] = 1;
	if (keystate[SDL_SCANCODE_S]) key_input[9] = 1;
	if (keystate[SDL_SCANCODE_D]) key_input[10] = 1;
	if (keystate[SDL_SCANCODE_F]) key_input[11] = 1;

	if (keystate[SDL_SCANCODE_Z]) key_input[12] = 1;
	if (keystate[SDL_SCANCODE_X]) key_input[13] = 1;
	if (keystate[SDL_SCANCODE_C]) key_input[14] = 1;
	if (keystate[SDL_SCANCODE_V]) key_input[15] = 1;

	for (int j = 0; j < 16; j++) {
		if (key_input[j]) {
			printf("Key %d was pressed\n", j);
		}
	}
}