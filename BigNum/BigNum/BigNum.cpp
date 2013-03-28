// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl;

	BigNumCpu<unsigned int> testInt(UIntToHexStringWithZero, UIntToHexStringNoZero);

	testInt.Set(1, 0x40);
	testInt.Set(0, 3);
	testInt.Set(-1, 2);

	std::cout << testInt.ToString() << endl;

	std::cin.get();

	return 0;
}

