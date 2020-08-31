#include <stdio.h>
#include <SDL.h>
#include <windows.h> 
#include <iostream>
#include <string>
#include "chip8.h"

//Imports for sound
#include <Mmsystem.h>
#include <mciapi.h>
#pragma comment(lib, "Winmm.lib")

using namespace std;

//Color for the emulator
#define RED 184
#define BLUE 233
#define GREEN 184

//Keyboard mapping
int read_keys(const Uint8 *keystate);

//The chip8 emulator
chip8 emulator;

int main(int argc, char * argv[]) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) { 
		printf("SDL_Init failed to intialize.\n");
		return 1;
	}

	//Create window
	SDL_Window *window = SDL_CreateWindow("Martin's Chip8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH*WINDOW_SCALE, WINDOW_HEIGHT*WINDOW_SCALE, 0);
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
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	
	//Black out the window
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_SetRenderDrawColor(renderer, RED, BLUE, GREEN, 255);

	//Initialize the keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Get the path to the game
	char game_file[20];
	printf("Enter the game's name\n");
	cin >> game_file;

	//Try and load the game
	if (emulator.load(game_file)) {
		printf("Closing program\n");
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	//Main game loop
	int game_loop = 1;
	while (game_loop && window != NULL) {
		emulator.cycle();

		//Draw on the screen
		if (emulator.draw) {

			//Clear screen
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, RED, BLUE, GREEN, 255);

			//Draw the pixels to scale
			for (int pixel = 0; pixel < WINDOW_WIDTH*WINDOW_HEIGHT; pixel++) {
				if (emulator.window_state[pixel]) { //Draw if pixel is meant to be on
					
					for (int i = 0; i < WINDOW_SCALE*WINDOW_SCALE; i++) { //Draw up to the modified window
						SDL_RenderDrawPoint(renderer, (pixel % WINDOW_WIDTH)*WINDOW_SCALE + (i % WINDOW_SCALE), (pixel / WINDOW_WIDTH)*WINDOW_SCALE + (i / WINDOW_SCALE));
					}
				}
			}

			//Show the new window
			SDL_RenderPresent(renderer);

			emulator.draw = 0;
		}

		//Play sounds
		if (emulator.sound) {
			sndPlaySound("beep.wav", SND_FILENAME | SND_ASYNC);
			emulator.sound = 0;
		}

		//Small delay to keep game slow
		Sleep(DELAY);
		
		//Read the keys
		if (read_keys(keystate)) {
			game_loop = 0;
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	printf("SDL exit\n");

	return 0;
}


/*
	Read the keyboard input and set the input 
*/
int read_keys(const Uint8 *keystate) {

	//	SDL_SCANCODE_1	SDL_SCANCODE_2	SDL_SCANCODE_3	SDL_SCANCODE_4
	//	SDL_SCANCODE_Q	SDL_SCANCODE_W	SDL_SCANCODE_E	SDL_SCANCODE_R
	//	SDL_SCANCODE_A	SDL_SCANCODE_S	SDL_SCANCODE_D	SDL_SCANCODE_F
	//	SDL_SCANCODE_Z	SDL_SCANCODE_X	SDL_SCANCODE_C	SDL_SCANCODE_V

	// The above maps to the keys

	//	1	2	3	C
	//	4	5	6	D
	//	7	8	9	E
	//	A	0	B	F

	SDL_PumpEvents();

	//Check for exit key (ESC)
	if (keystate[SDL_SCANCODE_ESCAPE]) return 1;

	//Clear inputs
	for (int i = 0; i < 16; i++) emulator.key_state[i] = 0;

	//Read keys
	if (keystate[SDL_SCANCODE_1]) emulator.key_state[0x1] = 1;
	if (keystate[SDL_SCANCODE_2]) emulator.key_state[0x2] = 1;
	if (keystate[SDL_SCANCODE_3]) emulator.key_state[0x3] = 1;
	if (keystate[SDL_SCANCODE_4]) emulator.key_state[0xC] = 1;

	if (keystate[SDL_SCANCODE_Q]) emulator.key_state[0x4] = 1;
	if (keystate[SDL_SCANCODE_W]) emulator.key_state[0x5] = 1;
	if (keystate[SDL_SCANCODE_E]) emulator.key_state[0x6] = 1;
	if (keystate[SDL_SCANCODE_R]) emulator.key_state[0xD] = 1;

	if (keystate[SDL_SCANCODE_A]) emulator.key_state[0x7] = 1;
	if (keystate[SDL_SCANCODE_S]) emulator.key_state[0x8] = 1;
	if (keystate[SDL_SCANCODE_D]) emulator.key_state[0x9] = 1;
	if (keystate[SDL_SCANCODE_F]) emulator.key_state[0xE] = 1;

	if (keystate[SDL_SCANCODE_Z]) emulator.key_state[0xA] = 1;
	if (keystate[SDL_SCANCODE_X]) emulator.key_state[0x0] = 1;
	if (keystate[SDL_SCANCODE_C]) emulator.key_state[0xB] = 1;
	if (keystate[SDL_SCANCODE_V]) emulator.key_state[0xF] = 1;

	return 0;
}