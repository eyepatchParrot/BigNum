// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl;

	BigNumCpu<unsigned int, UIntToHexString> testInt;
	testInt.Set(0, 3);

	std::cout << testInt.ToString() << endl;

	std::cin.get();

	return 0;
}

