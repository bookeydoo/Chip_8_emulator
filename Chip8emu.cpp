//TODO : organize files and compile them together 
#include "chip.hpp"
#include<stdint.h>
#include<cstdint>
#include<random>
#include<chrono>
#include <cstring>

const uint8_t video_width	=64;
const uint8_t video_height	=32;
const uint8_t FONT_SET_SIZE = 80;
const uint8_t FONT_SET_STARTADDRESS = 0x50;
const uint8_t start_address = 0x200;
uint8_t fontset[FONT_SET_SIZE] =
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()) {
	for (int i = 0; i < FONT_SET_SIZE; i++) {
		Mem[FONT_SET_STARTADDRESS + i] = fontset[i];
	}
	randByte = std::uniform_int_distribution<uint16_t>(0, 255u); // initialize RNG register
}
	
void Chip8::loadRom(char const* filename)
{
	// Open the file as a stream of binary and move the file pointer to the end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char* buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			Mem[start_address + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
	else {
		std::cout << "sth went wrong while loading the rom";
	}
}
	void Chip8::OP_00E0() //clear op
	{
		memset(Screen, 0, sizeof(Screen));
	}

	void Chip8::OP_00EE()// RET  (return func)
	{
		SP--;
		PC = stack[SP];
	}
	void Chip8::OP_1nnn()// jp to addr
	{
		uint16_t address = opcode & 0x0FFFu;
		PC = address;
	}

	void Chip8::OP_2nnn() //call addr
	{
		uint16_t address = opcode & 0x0FFFu;
		stack[SP] = PC;
		SP++;
		PC = address;
	}

	void Chip8::OP_3xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);
		if (VX == byte) {
			PC += 2;
		}
	}

	void Chip8::OP_4xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);
		if (VX != byte) {
			PC += 2;
		}
	}

	void Chip8::OP_5xy0()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t VY = (opcode & 0x00F0u) >> 4u;
		if (VX == VY) {
			PC += 2;
		}
	}

	void Chip8::OP_6xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);

		registers [VX] = byte;
	}

	void Chip8::OP_7xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);

		VX += byte;
	}

	void Chip8::OP_8xy0()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t VY = (opcode & 0x00F0u) >> 4u;

		registers[VX] =registers[VY];
	}

	// OR Vx, Vy
	void Chip8::OP_8xy1()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		registers[VX] = (registers[VX] | registers[VY]);
	}

	// AND Vx, Vy
	void Chip8::OP_8xy2() {


		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		registers[VX] = (registers[VX] & registers[VY]);
	}

	// XOR Vx, Vy
	void Chip8::OP_8xy3() {

		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		registers[VX] = (registers[VX] ^ registers[VY]); //^ stands for xor
	}

	// ADD Vx, Vy
	void Chip8::OP_8xy4() {
		//VF here is flag for carry 
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		
		uint16_t sum = registers[VX] + registers[VY];

		if (sum > 255u) {
			registers[0xF] = 1; //the 16th reg
		}
		else 
			registers[0xF] = 0;

		registers[VX] = 0x0FFu & sum; 
	}

	// SUB Vx, Vy
	void Chip8::OP_8xy5() { //VF is gonna be a flag for not borrow
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (VX > VY) {
			registers[0xF] = 1;
		}
		else registers[0xF] = 0; //borrow occured
		
		registers[VX] -= registers[VY];
	}

	// SHR Vx
	void Chip8::OP_8xy6() {	//shift right operation
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		
		registers[0xF] = (registers[VX] & 0x1u);
		
		registers[VX] >>= 1;
			
		
	
	}

	// SUBN Vx, Vy
	void Chip8::OP_8xy7() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (VY > VX) {
			registers[0xF] = 1;
		}
		else registers[0xF] = 0; //borrow occured

		registers[VY] -= registers[VX];
	}

	// SHL Vx
	void Chip8::OP_8xyE() {

		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		registers[0xF] = registers[VX] & 0x1u;
		registers[VX] <<= 1;
	}

	// SNE Vx, Vy
	void Chip8::OP_9xy0() { //skip next instruction
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (registers[VX] != registers[VY]) {
			PC += 2;
		}

	}

	// LD I, address
	void Chip8::OP_Annn() {
		uint16_t Address = (opcode & 0x0FFFu);//memory is 12 bit so we will have to use 16 bit address register
		AR = Address;
	}

	// JP V0, address
	void Chip8::OP_Bnnn() {
		uint16_t jmp_location = (opcode & 0x0FFFu);
		
		PC = registers[0] + jmp_location;

	}

	// RND Vx, byte
	void Chip8::OP_Cxkk() {
		uint8_t VX = (opcode & 0x0F00u)>>8u;
		
		registers[VX] = randByte(randGen) & (opcode & 0x00FFu); //right side variable is the kk value

	}

	// DRW Vx, Vy, height
	void Chip8::OP_Dxyn() { //might be hardest function
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00F0u) >> 4u;
		uint8_t height = opcode & 0x000Fu;
	
		uint8_t Pos_X = registers[VX] % video_width;
		uint8_t Pos_Y = registers[VY] % video_height;

		registers[0xF] = 0;


		for (int row = 0;row < height;row++) {
			uint8_t sprite_byte = Mem[AR + row];
			for (int col = 0;col < 8;col++) {

				uint8_t spritePixel = sprite_byte & (0x80u >> col);
				uint32_t* screenPixel = &Screen[(Pos_Y + row) * video_width + (Pos_X + col)];

				// Sprite pixel is on
				if (spritePixel)
				{
					// Screen pixel also on - collision
					if (*screenPixel == 0xFFFFFFFF)
					{
						registers[0xF] = 1;
					}

					// Effectively XOR with the sprite pixel
					*screenPixel ^= 0xFFFFFFFF;
				}
			}
		}

	}

	// SKP Vx
	void Chip8::OP_Ex9E() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t key = registers[VX];

		if (keys[key]) {
			PC += 2;
		}
	}
	

	// SKNP Vx
	void Chip8::OP_ExA1() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t key = registers[VX];

		if (!keys[key]) {
			PC += 2;
		}
	}

	// LD Vx, DT
	void Chip8::OP_Fx07() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		registers[VX] = delaytimer;
		
	}

	// LD Vx, K
	void Chip8::OP_Fx0A() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		if (keys[0]) {
			registers[VX] = 0;
		}
		else if (keys[1]) {
			registers[VX] = 1;
		}
		else if (keys[2]) {
			registers[VX] = 2;
		}
		else if (keys[3]) {
			registers[VX] = 3;
		}
		else if (keys[4]) {
			registers[VX] = 4;
		}
		else if (keys[5]) {
			registers[VX] = 5;
		}
		else if (keys[6]) {
			registers[VX] = 6;
		}
		else if(keys[7]) {
			registers[VX] = 7;
		}
		else if (keys[8]) {
			registers[VX] = 8;
		}
		else if (keys[9]) {
			registers[VX] = 9;
		}
		else if (keys[10]) {
			registers[VX] = 10;
		}
		else if (keys[11]) {
			registers[VX] = 11;
		}
		else if (keys[12]) {
			registers[VX] = 12;
		}
		else if (keys[13]) {
			registers[VX] = 13;
		}
		else if (keys[14]) {
			registers[VX] = 14;
		}
		else if (keys[15]) {
			registers[VX] = 15;
		}
		else {
			PC -= 2; //for waiting as to repeat instruction
		}
	}

	// LD DT, Vx
	void Chip8::OP_Fx15() {
		uint8_t VX = (opcode & 0x0F00u)>>8u;
		delaytimer = registers[VX];
	}

	// LD ST, Vx
	void Chip8::OP_Fx18() {

		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		soundtimer = registers[VX];

	}

	// ADD I, Vx
	void Chip8::OP_Fx1E() {
		
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		AR += registers[VX];
		
	}

	// LD F, Vx
	void Chip8::OP_Fx29() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t digit = registers[VX];

		AR = FONT_SET_STARTADDRESS + (5 * digit);

	}

	// LD B, Vx
	void Chip8::OP_Fx33() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t value = registers[VX];
		Mem[AR + 2] = value % 10;
		value /= 2;
		Mem[AR + 1] = value % 10;
		value /= 2;
		Mem[AR] = value % 10;
	}

	// LD [I], Vx
	void Chip8::OP_Fx55() {
		uint8_t VX = (opcode & 0x0F00u)>>8u;

		for (int i = 0;i < 16;i++) {
			Mem[AR+i] = registers[i];
		}
	}

	// LD Vx, [I]
	void Chip8::OP_Fx65() {
		
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		for (int i = 0;i < 16; i++) {
			registers[i] = Mem[AR + i];
		}
	}

	void Chip8::cycle(){
		//fetch
		opcode = (Mem[PC]<<8u) ;
		
		decode();
		PC += 2;

		// Decrement the delay timer if it's been set
		if (delaytimer > 0)
		{
			--delaytimer;
		}

		// Decrement the sound timer if it's been set
		if (soundtimer > 0)
		{
			--soundtimer;
		}
	}

	void Chip8::decode() {
		uint8_t first_byte = opcode & 0xF000;
		if (first_byte == 1) {
			OP_1nnn();
		}
		else if (first_byte == 2) {
			OP_2nnn();
		}
		else if (first_byte == 3) {
			OP_3xkk();
		}
		else if (first_byte == 4) {
			OP_4xkk();
		}
		else if (first_byte == 5) {
			OP_5xy0();
		}
		else if (first_byte == 6) {
			OP_6xkk();
		}
		else if (first_byte == 7) {
			OP_7xkk();
		}
		else if (first_byte == 8) {
			uint8_t lastbyte = (opcode & 0x000F);
			if (lastbyte == 0) {
				OP_8xy0();
			}
			else if (lastbyte == 1) {
				OP_8xy1();
			}
			else if (lastbyte == 2) {
				OP_8xy2();
			}
			else if (lastbyte == 3) {
				OP_8xy3();
			}
			else if (lastbyte == 4) {
				OP_8xy4();
			}
			else if (lastbyte == 5) {
				OP_8xy5();
			}
			else if (lastbyte == 6) {
				OP_8xy6();
			}
			else if (lastbyte == 7) {
				OP_8xy7();
			}
			else {
				OP_8xyE();
			}

		}
		else if (first_byte == 0) {
			uint8_t lastbyte = (opcode & 0x000F);
			if (lastbyte == 0) {
				OP_00E0();
			}
			else {
				OP_00EE();
			}
		}
		else if (first_byte == 0xE) {
			uint8_t lastbyte = (opcode & 0x000F);
			if (lastbyte == 1) {
				OP_ExA1();
			}
			else {
				OP_Ex9E();
			}
		}
		else if (first_byte == 0xF) {
			uint8_t thirdbyte = (opcode & 0x00F0);
			uint8_t lastbyte = (opcode & 0x000F);
			if (lastbyte == 7) {
				OP_Fx07();
			}
			else if (lastbyte == 0xA) {
				OP_Fx0A();
			}
			else if (thirdbyte == 1) {
				if (lastbyte == 5) {
					OP_Fx15();
				}
				else if (lastbyte == 8) {
					OP_Fx18();
				}
				else {
					OP_Fx1E();
				}
			}
			else if (lastbyte == 9) {
				OP_Fx29();
			}
			else if (lastbyte == 3) {
				OP_Fx33();
			}
			else if (thirdbyte == 5) {
				OP_Fx55();
			}
			else if (thirdbyte == 6) {
				OP_Fx65();
			}
			else {
				std::cerr << "unknown opcode" << std::hex << opcode << "\n";
				return;
			}
		}
	}