#include <stdio.h>
#include <string.h>
#include "chip8.h"

void chip8::init() {
	pc = 0;
	opcode = 0;
	I = 0;
	sp = 0;
	draw = 0;

	delay_timer = 0;
	sound_timer = 0;

	memset(V, 0, REGISTER_SIZE);
	memset(stack, 0, STACK_SIZE);
	memset(memory, 0, MEMORY_SIZE);
	memset(window_state, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
	memset(key_state, 0, 16);
}

void chip8::cycle()
{
	// Opcodes are two bytes large, and so the program counter holds opcodes in two indexes
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Check the first 4 bits of program counter (big-endian) and act accordingly
	switch (opcode & 0x0FFF) {
		case 0x0000: // 0x00NN
			switch (opcode & 0xFF) {

				case 0x0E0: // Clear the screen

					break;

				case 0x0EE: // Return from a subroutine

					break;
			}
		break;
		

		case 0x1000: // 0x1NNN
			// Go to the address represented by the last 12 bits
		
			break;


		case 0x2000: // 0x2NNN
			// Call subroutine represented by the last 12 bits

			break;


		case 0x3000: // 0x3XNN
			// if(Vx==NN), skip the next instruction

			break;


		case 0x4000: // 0x4XNN
			// if(Vx!=NN), skip the next instruction

			break;


		case 0x5000: // 0x1XY0
			// if(Vx==Vy), skip the next instruction

			break;


		case 0x6000: // 0x6XNN
			// Vx = NN, assign Vx with NN

			break;


		case 0x7000: // 0x7XNN
			// Vx += NN, add NN to Vx

			break;


		case 0x8000: // 0x8XY0
			switch (opcode & 0xF) {
				case 0x0: // Vx=Vy
	
					break;


				case 0x1: // Vx=Vx|Vy
	
					break;


				case 0x2: // Vx=Vx&Vy
	
					break;


				case 0x3: // Vx=Vx^Vy
	
					break;


				case 0x4: // Vx += Vy
	
					break;


				case 0x5: // Vx -= Vy
	
					break;


				case 0x6: // Vx>>=1
	
					break;


				case 0x7: // Vx=Vy-Vx
	
					break;


				case 0xE: // Vx<<=1
	
					break;
			}

			break;


		case 0x9000: // 0x9XY0
			// if(Vx!=Vy), skip the next instruction

			break;


		case 0xA000: // 0xANNN
			// I = NNN

			break;


		case 0xB000: // 0xBNNN
			// PC=V0+NNN

			break;


		case 0xC000: // 0xCXNN
			// Vx=rand()&NN, bitwise AND between NN and a random number between 0 and 255

			break;


		case 0xD000: // 0xDXYN
			// draw(Vx,Vy,N)

			break;


		case 0xE000: // 0xEXNN
			switch (opcode & 0xFF) {
				case 0x9E: // if(key()==Vx), skip instruction if key at Vx is pressed

					break;


				case 0xA1: // if(key()!=Vx), skip instruction if key at Vx is not pressed

					break;
			}

			break;


		case 0xF000: // 0xFXNN
			switch (opcode & 0xFF) {

				case 0x07: // Vx = get_delay(), Vx is set to the delay value

					break;


				case 0x0A: // Vx = get_key(), block and send next key press to Vx 

					break;


				case 0x15: // delay_timer(Vx), set delay timer to Vx

					break;


				case 0x18: // sound_timer(Vx), set sound timer to Vx

					break;


				case 0x1E: // I +=Vx

					break;


				case 0x29: // I = sprite_addr[Vx]

					break;

				
				case 0x33: // https://en.wikipedia.org/wiki/CHIP-8

					break;


				case 0x55: // reg_dump(Vx,&I)

					break;

				case 0x65: // reg_load(Vx,&I)

					break;

			}

			break;
	}
}

int chip8::load(const char *game)
{
	//Initialize the emulator
	init();

	// Open the game file and read it into our memory
	FILE *game_file = fopen(game, "rb");
	if (game_file == NULL) {
		printf("Failed to open game file\n");
		return 1;
	}

	// Find the file size
	fseek(game_file, 0, SEEK_END);
	int size = ftell(game_file);
	fseek(game_file, 0, SEEK_SET);

	// Read the file into memory
	if (size <= MEMORY_SIZE - MEMORY_OFFSET) {
		fread(memory, 1, size, game_file);
	} else {
		printf("Game size is too large\n");
		fclose(game_file);
		return 1;
	}

	// Close the game file
	fclose(game_file);

	return 0;
}
