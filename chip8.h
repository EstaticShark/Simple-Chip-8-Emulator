//Hardware settings
#define REGISTER_SIZE 16
#define STACK_SIZE 16

//Memory Settings
#define MEMORY_SIZE 4096
#define MEMORY_OFFSET 0x200

//Window Settings
#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32
#define WINDOW_SCALE 10

//Font set
#define FONT_SET_SIZE 80
#define FONT_OFFSET 0x50

//Timing settings
#define DELAY 1

class chip8 {
	private:
		unsigned short pc;								// Program counter
		unsigned short opcode;							// Current opcode
		unsigned short I;								// Index register
		unsigned short sp;								// Stack pointer

		unsigned char  delay_timer;						// Delay timer
		unsigned char  sound_timer;						// Sound timer

		unsigned char  registers[REGISTER_SIZE];		// Registers (V0-VF). The VF register contains the carry bit
		unsigned short stack[STACK_SIZE];				// Stack (16 levels)
		unsigned char  memory[MEMORY_SIZE];				// Memory (4 KiB)		

		void init();									// Initialization function
	public:
		unsigned char  window_state[64 * 32];			// Screen pixels in an array
		unsigned char  key_state[16];					// Keyboard input

		/*
			Having getters and setters slows the emulator down too much
		*/
		bool sound;										// Flag to make sounds
		bool draw;										// Flag to draw

		void cycle();									// Run one cycle of the emulator
		int load(const char *game);						// Load the game, return 0 for success, -1 on error
};
