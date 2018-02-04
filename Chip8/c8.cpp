#include "stdafx.h"
#include "c8.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <windows.h>

bool c8::getBitInMemory(int pos) {
	unsigned char a = memory[pos / 8];
	unsigned char b = a & (0x80 >> (pos % 8));
	if (b > 0)
		return true;
	else
		return false;
}

void c8::setBitInMemory(int pos,bool value) {
	unsigned char a = memory[pos / 8];
	unsigned char b = a & (0x80 >> (pos % 8));
	unsigned char v = value << (pos % 8);

	if ((b == 1)) {
		if (value == false) {
			a ^= 1 << (pos % 8);
		}
	}
	else {
		a |= v;
	}


	memory[pos / 8] = a;
}

bool c8::getBitInDisplay(int pos) {
	unsigned char a = display[pos / 8];
	unsigned char b = (a >> (pos%8)) & 0x1;
	if (b > 0) 
		return true;
	else
		return false;
	
}

void c8::setBitInDisplay(int pos, bool value) {
	unsigned char a = display[pos / 8];
	unsigned char b = ((a >> (pos % 8)) & 0x1);
	unsigned char v = value << (pos % 8);

	if ((b == 1)){
		if (value == false){
			a ^= 1 << (pos % 8);
		}
	}
	else {
		a |= v;
	}


	display[pos / 8] = a;
}

void c8::initialize()
{
	for (int i = 0; i < 0x1000; i++)
		memory[i]  = 0x0;

	for (int i = 0; i < 8 * 32; i++)
		display[i] = 0x0;

	for (int i = 0; i < 0x10; i++){
		V[i]       = 0x0;
		stack[i]   = 0x0;
		keypad[i]  = false;
	}

	PC             = 0x200;
	I              = 0x0;
	SP             = 0x0;
	delay_timer    = 0x0;
	sound_timer    = 0x0;

	for (int i = 0; i < 0x50; i++)
		memory[i] = chip8_fontset[i];

}

void c8::loadFile(const char * path)
{
	std::ifstream file(path,std::ios::binary);
	
	file.seekg(0, std::ios::end);
	int fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	char* buffer = new char[fileSize];
	file.read(buffer,fileSize);

	for (int i = 0; i < 0x800; ++i)
		memory[i + 0x200] = buffer[i];
}

bool c8::cycle()
{
	unsigned char X, Y, Z, KK,result;
	opcode = memory[PC] << 8 | memory[PC + 1];
	switch (opcode & 0xF000)
	{
	case 0x0000:
		Z = opcode & 0x00FF;
		if (Z == 0x00E0) {
			//CLS - Clear the display
			for (int i = 0; i < 8 * 32; i++)
				display[i] = 0x0;
			PC += 2;
		}
		else if (Z == 0x00EE)
		{
			//RET - Return from subroutine
			--SP;
			PC = stack[SP];
			PC += 2;
		}
		else {
			PC += 2;
		}
		break;
	case 0x1000:
		//JP - Jump to address
		PC = ((opcode & 0x0FFF));
		break;
	case 0x2000:
		//CALL - Call a subroutine on NNN address. Get stack ready.

		stack[SP] = PC;
		++SP;
		PC = opcode & 0x0FFF;
		break;
	case 0x3000:
		//SE - Skip next intruction if V[Z] = KK
		Z = (opcode & 0x0F00) >> 8;
		KK = opcode & 0x00FF;

		if (V[Z] == KK)
			PC += 4;
		else
			PC += 2;

		break;
	case 0x4000:
		//SNE - Skip next intruction if V[Z] != KK
		Z = (opcode & 0x0F00) >> 8;
		KK = opcode & 0x00FF;

		if (V[Z] != KK)
			PC += 4;
		else
			PC += 2;

		break;
	case 0x5000:
		//SE - Skip next intruction if V[X] = V[Y]
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;

		if (V[X] == V[Y])
			PC += 4;
		else
			PC += 2;
		break;
	case 0x6000:
		//LD - Load value KK into V[Z]
		Z = (opcode & 0x0F00) >> 8;
		KK = opcode & 0x00FF;
		V[Z] = KK;
		PC += 2;
		break;
	case 0x7000:
		//ADD - Add value KK into V[Z]
		Z = (opcode & 0x0F00) >> 8;
		KK = opcode & 0x00FF;
		V[Z] += KK;
		PC += 2;
		break;
	case 0x8000:
		Z = opcode & 0x000F;
		X = (opcode & 0x0F00) >> 8;
		Y = (opcode & 0x00F0) >> 4;
		switch (Z) {
		case 0:
			//LD - Load value from V[Y] into V[X]
			V[X] = V[Y];
			PC += 2;
			break;
		case 1:
			//OR - Perform a bitwise OR on the contents of V[X] and V[Y] and store it in V[X]
			V[X] |= V[Y];
			PC += 2;
			break;
		case 2:
			//AND - Perform a bitwise AND on the contents of V[X] and V[Y] and store it in V[X]
			V[X] &= V[Y];
			PC += 2;
			break;
		case 3:
			//XOR - Perform a bitwise XOR on the contents of V[X] and V[Y] and store it in V[X]
			V[X] ^= V[Y];
			PC += 2;
			break;
		case 4:
			//ADD - The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
				if ((V[Y] + V[X]) > 0xFF)
					V[0xF] = 0x1;
				else
					V[0xF] = 0x0;
				V[X] += V[Y];
				PC += 2;
				break;
			case 5:
				//SUB - If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
				if (V[X] > V[Y])
					V[0xF] = 0x1;
				else
					V[0xF] = 0x0;
				V[X] -= V[Y];

				PC += 2;
				break;
			case 6:
				//SHR - If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
				V[0xF] = V[X] & 0x1;
				V[X] = V[X]/2;
				PC += 2;
				break;
			case 7:
				//SUBN - Set Vx = Vx SHR 1. If the least - significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
				if (V[Y] > V[X]) {
					V[0xF] = 0x1;
				}
				else {
					V[0xF] = 0x0;
				}
				V[X] = V[Y] - V[X];
				PC += 2;
				break;
			case 0xE:
				//SHL - If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
				V[0xF] = V[X] >> 7;
				V[X] = V[X] * 2;
				PC += 2;
				break;
			default:
				PC += 2;
			}
			break;
		case 0x9000:
			//SNE - The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
			X = opcode & 0x0F00 >> 8;
			Y = opcode & 0x00F0 >> 4;
			if (V[X] != V[Y]) {
				PC += 4;
			} else {
				PC += 2;
			}
			break;
		case 0xA000:
			//LD - The value of register I is set to nnn.
			I = opcode & 0x0FFF;
			PC += 2;
			break;
		case 0xB000:
			//JP - The program counter is set to nnn plus the value of V0.
			PC = (opcode & 0x0FFF) + V[0];
			break;
		case 0xC000:
			//RND - Set Vx = random byte AND kk.
			X = (opcode & 0x0F00) >> 8;
			KK = opcode & 0x00FF;

			result = std::rand() % 255;
			

			V[X] = KK & result;
			PC += 2;
			break;
		case 0xD000:
			//Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			Z = opcode & 0x000F;
			X = (opcode & 0x0F00) >> 8;
			Y = (opcode & 0x00F0) >> 4;
			result = 0x0;
			for (int i = 0; i < Z; i++)
			{
				int newX = V[X];
				int newY = V[Y] + i;
				for (int j = 0; j < 8; j++)
				{
					unsigned char a = getBitInMemory(((I + i)*8)+j);
					unsigned char b = getBitInDisplay((newY * 64) + newX + j);
					if(a == 0x1 && b == 0x1){
						setBitInDisplay((newY * 64) + V[X] + j, false);
						result = 0x1;
					}
					else if ((a == 0x0 && b == 0x1) || (a == 0x1 && b == 0x0)) {
						setBitInDisplay((newY * 64) + V[X] + j, true);
						//result = 0x0;
					}
					else {
						setBitInDisplay((newY * 64) + V[X] + j, false);
						//result = 0x0;
					}
					
				}

			}
			V[0xF] = result;
			PC += 2;

			break;
		case 0xE000:
			Z = opcode & 0x00FF;
			X = (opcode & 0x0F00) >> 8;
			switch (Z) {
				case 0x009E:
				//SKP - Skip next instruction if key with the value of Vx is pressed.
					if (keypad[V[X]]){ 
						PC += 2;
					}
						
					break;
				case 0x00A1:
				//SKNP - Skip next instruction if key with the value of Vx is not pressed.
					if (!keypad[V[X]]) { 
						PC += 2; 
					}
						
					break;
			}
			//PC += 2;
		case 0xF000:
			Z = opcode & 0x00FF;
			X = (opcode & 0x0F00) >> 8;
			switch (Z) {
				case 0x0007:
					//LD - The value of DT is placed into Vx.
					V[X] = delay_timer;
					PC += 2;
					break;
				case 0x000A:
					//INPUT SHIT! DONT LOOK o.O
					for (int i = 0; i < 0x10; i++) {
						if (keypad[i]) {
							X = i;
							PC += 2;
							break;
						}
					}
					break;
				case 0x0015:
					//LD - Set delay timer = Vx.
					delay_timer = V[X];
					PC += 2;
					break;
				case 0x0018:
					//ADD - ST is set equal to the value of Vx.
					sound_timer = V[X];
					PC += 2;
					break;
				case 0x001E:
					//ADD - The values of I and Vx are added, and the results are stored in I.
					I += V[X];
					PC += 2;
					break;
				case 0x0029:
					//LD - Set I = location of sprite for digit Vx.
					I = V[X] * 0x5;
					PC += 2;
					break;
				case 0x0033:
					//LD - Store BCD representation of Vx in memory locations I, I+1, and I+2.
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					PC += 2;
					break;
				case 0x0055:
					//LD - Store registers V0 through Vx in memory starting at location I.
					for (int i = 0; i < X + 1; i++)
						memory[I + i] = V[i];
					PC += 2;
					break;
				case 0x0065:
					//LD - Read registers V0 through Vx from memory starting at location I.
					for (int i = 0; i < X + 1; i++)
						V[i] = memory[I + i];
					PC += 2;
					break;
				default:
					PC += 2;
					break;
			}

			break;
		default:
			PC += 2;
			break;
	}
	//std::cout << PC << std::endl;
	if(sound_timer > 0)
		sound_timer -= 1;
	if(delay_timer > 0)
		delay_timer -= 1;
	return false;
}

bool engine::OnUserCreate()
{
	curTime = 0;
	return true;
}

bool engine::OnUserUpdate(float fElapsedTime)
{
	curTime += fElapsedTime;

	/*GET THE KEYS*/
	c.keypad[0x1] = GetKey('1').bHeld;
	c.keypad[0x2] = GetKey('2').bHeld;
	c.keypad[0x3] = GetKey('3').bHeld;
	c.keypad[0xC] = GetKey('4').bHeld;
	c.keypad[0x4] = GetKey('Q').bHeld;
	c.keypad[0x5] = GetKey('W').bHeld;
	c.keypad[0x6] = GetKey('E').bHeld;
	c.keypad[0xD] = GetKey('R').bHeld;
	c.keypad[0x7] = GetKey('A').bHeld;
	c.keypad[0x8] = GetKey('S').bHeld;
	c.keypad[0x9] = GetKey('D').bHeld;
	c.keypad[0xE] = GetKey('F').bHeld;
	c.keypad[0xA] = GetKey('Z').bHeld;
	c.keypad[0x0] = GetKey('X').bHeld;
	c.keypad[0xB] = GetKey('C').bHeld;
	c.keypad[0xF] = GetKey('V').bHeld;
	

		c.cycle();

		/*DRAW THE DISPLAY*/
		//Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 32; y++) {
				bool b = c.getBitInDisplay(((y * 64) + x));
				if (b)
					Draw(x, y, PIXEL_SOLID);
				else
					Draw(x, y, L' ');
			}
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (c.keypad[(i * 4) + j])
					Draw(j + 64, i, L'#');
				else
					Draw(j + 64, i, L'X');
			}
		}

		for (int i = 0; i < 0x10; i++) {
			DrawString(64, i + 5, std::to_wstring(i));
			DrawString(67, i + 5, std::to_wstring(c.V[i]));
		}
			
		curTime = 0;
	return true;
}
