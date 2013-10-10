#pragma once
#include "stdafx.h"

std::string UIntToHexString(unsigned int val, bool includeZero);
std::string UIntToHexStringWithZero(unsigned int val);
std::string UIntToHexStringNoZero(unsigned int val);
std::string IntToHexString(int val);

class ToStringFuncExpected : std::invalid_argument {
public:
	ToStringFuncExpected() :
		std::invalid_argument("Did not pass ToString function.") { }
};

class Stopwatch {
	LARGE_INTEGER freq;
	LARGE_INTEGER startCounter;

public:
	Stopwatch(void)
	{
		QueryPerformanceFrequency(&this->freq);
		QueryPerformanceCounter(&this->startCounter);
	}
	~Stopwatch() { }

	double Secs()
	{
		LARGE_INTEGER curCounter;
		QueryPerformanceCounter(&curCounter);
		double r = (curCounter.QuadPart - this->startCounter.QuadPart) / (double)this->freq.QuadPart;
		return r;
	}
};

