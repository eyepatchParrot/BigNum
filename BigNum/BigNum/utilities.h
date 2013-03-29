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