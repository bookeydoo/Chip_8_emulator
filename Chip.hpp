
#ifndef CHIP8_HPP  // Check if CHIP8_HPP is not defined
#define CHIP8_HPP  // Define CHIP8_HPP


#include<stdint.h>
#include<cstdint>
#include<random>
#include<chrono>
#include <cstring>




class Chip8 {
public:
	//implementing registers used
	uint8_t registers[16];
	uint16_t stack[16];
	uint16_t Mem[4096];
	uint16_t PC = 0x200; //Program counter starts at 0x20 then increments by 2 everytime
	uint16_t AR; //address register
	uint8_t SP;  //stackpointer
	uint8_t keys[16]; //we use hexadecimal keys from 0 to F 
	uint8_t delaytimer;
	uint8_t soundtimer;
	int32_t Screen[64 * 32]; //we can change it depending on preferences
	uint16_t opcode;

	std::default_random_engine randGen;
	std::uniform_int_distribution<uint16_t> randByte;

	Chip8();

	void cycle();

	void loadROM(char const* file);

	void decode();
	
	void OP_00E0(); //clear op

	void OP_00EE();// RET  (return func)

	void OP_1nnn();// jp to addr

	void OP_2nnn(); //call addr


	void OP_3xkk();


	void OP_4xkk();

	void OP_5xy0();

	void OP_6xkk();

	void OP_7xkk();

	void OP_8xy0();

	// OR Vx, Vy
	void OP_8xy1();

	// AND Vx, Vy
	void OP_8xy2();

	// XOR Vx, Vy
	void OP_8xy3();

	// ADD Vx, Vy
	void OP_8xy4();

	// SUB Vx, Vy
	void OP_8xy5();

	// SHR Vx
	void OP_8xy6();

	// SUBN Vx, Vy
	void OP_8xy7();

	// SHL Vx
	void OP_8xyE();

	// SNE Vx, Vy
	void OP_9xy0();

	// LD I, address
	void OP_Annn();

	// JP V0, address
	void OP_Bnnn();

	// RND Vx, byte
	void OP_Cxkk();

	// DRW Vx, Vy, height
	void OP_Dxyn();

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

#endif