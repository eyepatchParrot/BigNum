#include "stdafx.h"
#include "utilities.h"


std::string UIntToHexString(unsigned int val, bool includeZero)
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

std::string UIntToHexStringWithZero(unsigned int val)
{
	return UIntToHexString(val, true);
}

std::string UIntToHexStringNoZero(unsigned int val)
{
	return UIntToHexString(val, false);
}

std::string IntToHexString(int val)
{
	std::string retVal;

	if (val < 0) {
		retVal = "-";
		val = val * -1;
	}
	retVal = retVal + UIntToHexString(val, false);
	
	return retVal;
}