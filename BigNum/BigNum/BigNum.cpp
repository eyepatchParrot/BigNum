// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"
#include "BigInt.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl;

	BigInt a;
	a.Set(13, 0);

	BigNumCpu testInt;

	testInt.Set(1, 0x40);
	std::cout << testInt.ToString() << endl;
	testInt.Set(0, 3);
	std::cout << testInt.ToString() << endl;
	testInt.Set(-1, 2);

	std::cout << testInt.ToString() << endl;

	Deque<int> testDeque(IntToHexString);
	testDeque.Fill(-1, 3, 5);

	std::cout << testDeque.ToString() << endl;

	testDeque.Fill(-3, 0, 2);
	std::cout << testDeque.ToString() << endl;

	testDeque.Fill(testDeque.Size(), testDeque.Size() + 3, 7);
	std::cout << testDeque.ToString() << endl;

	std::cin.get();

	return 0;
}

