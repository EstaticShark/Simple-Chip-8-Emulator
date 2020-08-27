#define REGISTER_SIZE 16
#define STACK_SIZE 16
#define MEMORY_SIZE 4096
#define MEMORY_OFFSET 512
#define WINDOW_WIDTH 64
#define WINDOW_HEIGHT 32

class chip8 {
	private:
		unsigned short pc;						// Program counter
		unsigned short opcode;					// Current opcode
		unsigned short I;						// Index register
		unsigned short sp;						// Stack pointer

		unsigned char  delay_timer;				// Delay timer
		unsigned char  sound_timer;				// Sound timer

		unsigned char  V[REGISTER_SIZE];		// Registers (V0-VF)
		unsigned short stack[STACK_SIZE];		// Stack (16 levels)
		unsigned char  memory[MEMORY_SIZE];		// Memory (4 KiB)		

		void init();
	public:
		bool draw;								// Flag to indicate drawing
		unsigned char  window_state[64 * 32];	// Screen pixels in an array
		unsigned char  key_state[16];			// Keyboard input

		void cycle();							// Run one cycle of the emulator
		int load(const char *game);				// Load the game, return 0 for success, -1 on error
};
