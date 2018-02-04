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
	
	e.ConstructConsole(70,32,12,12);
	e.c.initialize();
	if (argc > 1) {
		e.c.loadFile(argv[1]);;

		e.Start();
	}
	else {
		std::cout << "Please choose a ROM. Execute this app by using: \"Chip8.exe <path>\"" << std::endl;
	}
	
	

	
	
    return 0;
}

