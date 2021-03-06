#pragma once
#include "olcConsoleGameEngine.h"
#include "olcNoiseMaker.h"

class c8
{
public:
	c8() {};
	~c8() {};

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

	unsigned char memory[0x1000];
	unsigned char V[0x10];
	
	unsigned short PC,I,SP,opcode;
	unsigned short stack[0x10];

	bool getBitInMemory(int pos);
	void setBitInMemory(int pos, bool value);
	bool getBitInDisplay(int pos);
	void setBitInDisplay(int pos, bool value);


	/*64x32******************
	char = 8bits.************
	1 bit = 1 pixel.*********
	*************************
	64/8 = 8 chars per line**
	32 lines****************/

	unsigned char display[8 * 32];
	unsigned char delay_timer, sound_timer;
	bool keypad[0x10];

	void initialize();
	void loadFile(const char * path);

	bool cycle();

};

class engine : public olcConsoleGameEngine {
public:
	engine() { m_sAppName = L"Chip-8 Interpreter"; vector<wstring> devices = olcNoiseMaker<short>::Enumerate(); sound = new olcNoiseMaker<short>(devices[0]); };


	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

	float curTime = 0;
	float maxTime = 0.016f;

	olcNoiseMaker<short>* sound;

	c8 c;
};
double GetNoise(double dTime);
