// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Deque.h"

unsigned int StripSign(int val)
{
	return val & 0x7FFFFFFF;
}

std::string UIntToHexString(unsigned int val, bool includeZero = false)
{
	std::string retVal;

	// An unsigned int has 8 hex digits, start at the highest, and work your way
	// down.
	bool hasPrintedNonZero = false;
	for (int i = 0; i < 8; i++) {
		int curDigit = 7 - i;
		int digit = (val & (0xF << (4 * curDigit))) >> (4 * curDigit);
		if (hasPrintedNonZero || includeZero || digit != 0 || curDigit == 0) {
			char digitChar = digit >= 10 ? 'A' + (digit - 10) : '0' + digit;
			retVal = retVal + digitChar;

			hasPrintedNonZero = true;
		}
	}

	return retVal;
}

std::string IntToHexString(int val)
{
	std::string retVal;

	if (val < 0) {
		retVal = "-";
		val = val * -1;
	}
	retVal = retVal + UIntToHexString(val);
	
	return retVal;
}

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl;

	Deque<int, IntToHexString> myDeque;

	for (int i = 0; i < 15; i++) {
		myDeque.PushBack(i);

		cout << myDeque.ToString() << endl;
	}

	for (int i = 0; i < 5; i++) {
		myDeque.PopBack();

		cout << myDeque.ToString() << endl;
	}

	for (int i = 0; i < 10; i++) {
		myDeque.PushFront(i * -1 - 1);

		cout << myDeque.ToString() << endl;
	}

	for (int i = 0; i < 5; i++) {
		myDeque.PopFront();

		cout << myDeque.ToString() << endl;
	}

	std::cin.get();

	return 0;
}

