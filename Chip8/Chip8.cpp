// Chip8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "c8.h"
#include <iostream>
#include <Windows.h>
#include <time.h>

int main(int argc, char* argv[])
{
	engine e;
	
	e.ConstructConsole(96,32,12,12);
	e.c.initialize();
	if (sizeof(argv) != 0) {
		e.c.loadFile(argv[1]);;
	}
	std::srand(time(NULL));
	e.Start();

	
	
    return 0;
}

