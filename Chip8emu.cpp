//TODO : organize files and compile them together 

#include<stdint.h>
#include<cstdint>
#include<random>
#include<chrono>
#include <cstring>

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


class Chip8 {
	//implementing registers used
	uint16_t AR; //address register
	uint8_t registers[16];
	uint16_t stack[16];
	uint16_t Mem[4096];
	uint16_t PC = 0x20; //Program counter starts at 0x20 then increments by 2 everytime
	uint16_t AR; //address register
	uint8_t SP;  //stackpointer
	uint8_t keys[16]; //we use hexadecimal keys from 0 to F 
	uint8_t delaytimer;
	uint8_t soundtimer;
	int32_t Screen[64 * 32]; //we can change it depending on preferences
	uint16_t opcode;

	std::default_random_engine randGen;
	std::uniform_int_distribution<uint8_t> randByte;

	Chip8()
		: randGen(std::chrono::system_clock::now().time_since_epoch().count())
	{

		for (int i = 0;i < FONT_SET_SIZE;i++) {
			Mem[FONT_SET_STARTADDRESS + i] = fontset[i];
		}
		randByte = std::uniform_int_distribution<uint8_t>(0, 255u);// initalize RNG register
	}

	void OP_00E0() //clear op
	{
		memset(Screen, 0, sizeof(Screen));
	}

	void OP_00EE()// RET  (return func)
	{
		SP--;
		PC = stack[SP];
	}
	void OP_1nnn()// jp to addr
	{
		uint16_t address = opcode & 0x0FFFu;
		PC = address;
	}

	void OP_2nnn() //call addr
	{}

	void OP_3xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);
		if (VX = byte) {
			PC += 2;
		}
	}

	void OP_4xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);
		if (VX != byte) {
			PC += 2;
		}
	}

	void OP_5xy0()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t VY = (opcode & 0x00F0u) >> 4u;
		if (VX = VY) {
			PC += 2;
		}
	}

	void OP_6xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);

		VX = byte;
	}

	void OP_7xkk()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t byte = (opcode & 0x00FFu);

		VX += byte;
	}

	void OP_8xy0()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;// cuz x is in the second hexadecimal place so 
		uint8_t VY = (opcode & 0x00F0u) >> 4u;

		VX = VY;
	}

	// OR Vx, Vy
	void OP_8xy1()
	{
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		VX = (VX | VY);
	}

	// AND Vx, Vy
	void OP_8xy2() {


		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		VX = (VX & VY);
	}

	// XOR Vx, Vy
	void OP_8xy3() {

		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		VX = (VX ^ VY); //^ stands for xor
	}

	// ADD Vx, Vy
	void OP_8xy4() {
		//VF here is flag for carry 
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;
		
		uint16_t sum = registers[VX] + registers[VY];

		if (sum > 255u) {
			registers[0xF] = 1; //the 16th reg
		}
		else 
			registers[0xF] = 0;

		VX = 0x0FFu & sum; 
	}

	// SUB Vx, Vy
	void OP_8xy5() { //VF is gonna be a flag for not borrow
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (VX > VY) {
			registers[0xF] = 1;
		}
		else registers[0xF] = 0; //borrow occured
		
		registers[VX] -= registers[VY];
	}

	// SHR Vx
	void OP_8xy6() {	//shift right operation
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		
		registers[0xF] = (registers[VX] & 0x1u);
		
		registers[VX] >>= 1;
			
		
	
	}

	// SUBN Vx, Vy
	void OP_8xy7() {
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (VY > VX) {
			registers[0xF] = 1;
		}
		else registers[0xF] = 0; //borrow occured

		registers[VY] -= registers[VX];
	}

	// SHL Vx
	void OP_8xyE() {

		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		registers[0xF] = registers[VX] & 0x1u;
		registers[VX] <<= 1;
	}

	// SNE Vx, Vy
	void OP_9xy0() { //skip next instruction
		uint8_t VX = (opcode & 0x0F00u) >> 8u;
		uint8_t VY = (opcode & 0x00Fu) >> 4u;

		if (registers[VX] != registers[VY]) {
			PC += 2;
		}

	}

	// LD I, address
	void OP_Annn() {
		uint16_t Address = (opcode & 0x0FFFu);//memory is 12 bit so we will have to use 16 bit address register
		AR = Address;
	}

	// JP V0, address
	void OP_Bnnn() {
		uint16_t jmp_location = (opcode & 0x0FFFu);
		
		PC = registers[0] + jmp_location;

	}

	// RND Vx, byte
	void OP_Cxkk() {
		uint8_t VX = (opcode & 0x0F00u)>>8u;
		
		registers[VX] = (randByte(randGen) & (opcode & 0x00FFu)); //right side variable is the kk value

	}

	// DRW Vx, Vy, height
	void OP_Dxyn() {

	}

	// SKP Vx
	void OP_Ex9E();

	// SKNP Vx
	void OP_ExA1();

	// LD Vx, DT
	void OP_Fx07();

	// LD Vx, K
	void OP_Fx0A();

	// LD DT, Vx
	void OP_Fx15();

	// LD ST, Vx
	void OP_Fx18();

	// ADD I, Vx
	void OP_Fx1E();

	// LD F, Vx
	void OP_Fx29();

	// LD B, Vx
	void OP_Fx33();

	// LD [I], Vx
	void OP_Fx55();

	// LD Vx, [I]
	void OP_Fx65();
};
