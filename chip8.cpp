#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

unsigned char chip8_fontset[FONT_SET_SIZE] =
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

void chip8::init() {
	pc = 0;
	opcode = 0;
	I = 0;
	sp = 0;
	draw = 0;

	delay_timer = 0;
	sound_timer = 0;

	memset(registers, 0, REGISTER_SIZE);
	memset(stack, 0, STACK_SIZE);
	memset(memory, 0, MEMORY_SIZE);
	memset(window_state, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
	memset(key_state, 0, 16);

	memcpy(memory, chip8_fontset, FONT_SET_SIZE); // Store the fontset at the start of the memory
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
					memset(window_state, 0, WINDOW_WIDTH * WINDOW_HEIGHT);
					draw = 1;
					pc += 2;
					break;

				case 0x0EE: // Return from a subroutine
					stack[sp--] = 0;
					pc = stack[sp] + 2;
					break;
			}
		break;
		

		case 0x1000: // 0x1NNN
			// Go to the address represented by the last 12 bits
			pc = opcode & 0x0FFF;
			break;


		case 0x2000: // 0x2NNN
			// Call subroutine represented by the last 12 bits
			stack[sp++] = pc;
			pc = opcode & 0x0FFF;
			break;


		case 0x3000: // 0x3XNN
			// if(Vx==NN), skip the next instruction
			if (registers[(opcode & 0x0F00) >> 8] == opcode & 0x00FF) {
				pc += 2;
			}
			pc += 2;
			break;


		case 0x4000: // 0x4XNN
			// if(Vx!=NN), skip the next instruction
			if (registers[(opcode & 0x0F00) >> 8] != opcode & 0x00FF) {
				pc += 2;
			}
			pc += 2;
			break;


		case 0x5000: // 0x5XY0
			// if(Vx==Vy), skip the next instruction
			if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			pc += 2;
			break;


		case 0x6000: // 0x6XNN
			// Vx = NN, assign Vx with NN
			registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
			break;


		case 0x7000: // 0x7XNN
			// Vx += NN, add NN to Vx
			registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
			break;


		case 0x8000: // 0x8XY0
			switch (opcode & 0xF) {
				case 0x0: // Vx=Vy
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x1: // Vx=Vx|Vy
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] | registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x2: // Vx=Vx&Vy
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] & registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x3: // Vx=Vx^Vy
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] ^ registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x4: // Vx += Vy, VF = 1 when there is carry
					if (registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4] > 0x00FF) {
						registers[0xF] = 1;
					} else {
						registers[0xF] = 0;
					}
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] + registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x5: // Vx -= Vy, VF = 1 when there is no borrow
					if (registers[(opcode & 0x0F00) >> 8] < registers[(opcode & 0x00F0) >> 4]) {
						registers[0xF] = 0;
					} else {
						registers[0xF] = 1;
					}
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] - registers[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;


				case 0x6: // Vx>>=1
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] >> 1;
					pc += 2;
					break;


				case 0x7: // Vx=Vy-Vx, VF = 1 when there is no borrow
					if (registers[(opcode & 0x00F0) >> 4] < registers[(opcode & 0x0F00) >> 8]) {
						registers[0xF] = 0;
					}
					else {
						registers[0xF] = 1;
					}
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x00F0) >> 4] - registers[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;


				case 0xE: // Vx<<=1
					registers[(opcode & 0x0F00) >> 8] = registers[(opcode & 0x0F00) >> 8] << 1;
					pc += 2;
					break;
			}

			break;


		case 0x9000: // 0x9XY0
			// if(Vx!=Vy), skip the next instruction
			if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			pc += 2;
			break;


		case 0xA000: // 0xANNN
			// I = NNN
			I = opcode & 0x0FFF;
			break;


		case 0xB000: // 0xBNNN
			// PC=V0+NNN
			pc = registers[0] + (opcode & 0x0FFF);
			break; 


		case 0xC000: // 0xCXNN
			// Vx=rand()&NN, bitwise AND between NN and a random number between 0 and 255
			int random = rand() % 256;
			registers[(opcode & 0x0F00) >> 8] = random & (opcode & 0x00FF);
			break;


		case 0xD000: // 0xDXYN
			// draw(Vx,Vy,N)

			int x = (opcode & 0x0F00) >> 8;
			int y = (opcode & 0x00F0) >> 4;
			int height = (opcode & 0x000F);

			for (int row = 0; row < height; row++) {
				int row_state = memory[I + row]; //This contains 8 bits that we can compare to the window state
				for (int column = 0; column < 8; column++) {
					if ((row_state & (0x80 >> column)) != 0) { //Check if the column-th bit from the left is 1
						// Check if the bit is already 1
						if (window_state[(x + column) + (64 * (y + row))]) {
							registers[0xF] = 1;
						}
						window_state[(x + column) + (64 * (y + row))] = window_state[(x + column) + (64 * (y + row))] ^ 1;
					}
				}
			}

			draw = 1;
			pc += 2;
			break;


		case 0xE000: // 0xEXNN
			switch (opcode & 0xFF) {
				case 0x9E: // if(key()==Vx), skip instruction if key at Vx is pressed
					if (key_state[registers[(opcode & 0x0F00) >> 8]]) {
						pc += 2;
					}
					pc += 2;
					break;


				case 0xA1: // if(key()!=Vx), skip instruction if key at Vx is not pressed
					if (!key_state[registers[(opcode & 0x0F00) >> 8]]) {
						pc += 2;
					}
					pc += 2;
					break;
			}

			break;


		case 0xF000: // 0xFXNN
			switch (opcode & 0x00FF) {

				case 0x0007: // Vx = get_delay(), Vx is set to the delay value
					registers[(opcode & 0x0F00) >> 8] = delay_timer;
					break;


				case 0x000A: // Vx = get_key(), block and send next key press to Vx 

					// We cannot busy wait since we need to head out of the operation to check input

					int key_press = 0;
					for (int i = 0; i < 16; i++) {
						if (key_state[i]) {
							registers[(opcode & 0x0F00) >> 8] = i;
							key_press = 1;
						}
					}

					if (key_press) {
						pc += 2;
					}

					break;


				case 0x0015: // delay_timer(Vx), set delay timer to Vx
					delay_timer = registers[(opcode & 0x0F00) >> 8];
					break;


				case 0x0018: // sound_timer(Vx), set sound timer to Vx
					sound_timer = registers[(opcode & 0x0F00) >> 8];
					break;


				case 0x001E: // I +=Vx
					I += registers[(opcode & 0x0F00) >> 8];
					break;


				case 0x0029: // I = sprite_addr[Vx]
					// The location of 0x0 - 0xF in the font set is a multiple of 5, since each character has 5 bytes
					// See the chip8_fontset above for a better understanding of why this is the case
					I = registers[(opcode & 0x0F00) >> 8] * 5;
					pc += 2;
					break;

				
				case 0x0033: // https://en.wikipedia.org/wiki/CHIP-8
					memory[I] = registers[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (registers[(opcode & 0x0F00) >> 8] % 100) / 10;
					memory[I + 2] = (registers[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
					break;


				case 0x0055: // reg_dump(Vx,&I)
					for (int i = 0; i < registers[(opcode & 0x0F00) >> 8]; i++) {
						memory[I + i] = registers[i];
					}
					pc += 2;
					break;

				case 0x0065: // reg_load(Vx,&I)
					for (int i = 0; i < registers[(opcode & 0x0F00) >> 8]; i++) {
						registers[i] = memory[I + i];
					}
					pc += 2;
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
		fread(&memory[512], 1, size, game_file);
	} else {
		printf("Game size is too large\n");
		fclose(game_file);
		return 1;
	}

	// Close the game file
	fclose(game_file);

	return 0;
}
