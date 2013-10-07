#pragma once

#include "stdafx.h"

typedef unsigned __int32 Limb;

class BigNumCpu
{
	Deque<Limb> limbs;
	// the placeValue of limbs[0]
	int exponent;

	bool IdxIsValid(int placeValue)
	{
		int idx = GetDequeIdx(placeValue);
		return idx >= 0 && (unsigned)idx < limbs.Size();
	}

	int GetDequeIdx(int placeValue)
	{
		return placeValue - exponent;
	}

public:
	BigNumCpu()
	{
		exponent = 0;
		limbs = Deque<Limb>(UIntToHexStringWithZero);
	}

	~BigNumCpu(void) { }

	Limb Get(int placeValue)
	{
		if (IdxIsValid(placeValue)) {
			return limbs.Get(GetDequeIdx(placeValue));
		} else {
			return 0;
		}
	}

	void Set(int placeValue, Limb value)
	{
		if (limbs.Size() == 0 || (limbs.Size() == 1 && limbs.Get(0) == 0)) {
			// There are no significant members in limbs.
			if (limbs.Size() == 1) {
				limbs.PopBack();
			}

			// limbs is now guaranteed to be empty.
			limbs.PushBack(value);
			exponent = placeValue;
		} else {
			if (placeValue < Min()) {
				limbs.Fill(GetDequeIdx(placeValue), GetDequeIdx(Min()), 0);
				exponent = placeValue;
			}
			if (placeValue > Max()) {
				limbs.Fill(GetDequeIdx(Max() + 1), GetDequeIdx(placeValue + 1), 0);
			}
			limbs.Set(GetDequeIdx(placeValue), value);
		}
	}

	BigNumCpu Plus(BigNumCpu other)
	{
		return BigNumCpu();
	}

	BigNumCpu Minus(BigNumCpu other)
	{
		return BigNumCpu();
	}

	BigNumCpu Times(BigNumCpu other)
	{
		return BigNumCpu();
	}

	BigNumCpu Shl(int value)
	{
	}

	BigNumCpu Shr(int value)
	{
	}

	// Returns the highest place value
	int Max()
	{
		return exponent + limbs.Size() - 1;
	}

	// Returns the lowest place value
	int Min()
	{
		return exponent;
	}

	std::string ToString(void)
	{
		std::string retVal;

		if (Max() < 0) {
			retVal += "0 . ";
		}

		// Fill out the string with zeros until we reach the max place value.
		for (int i = 0; i < 0 - Max() - 1; i++) {
			// loop from place value
			retVal += UIntToHexStringWithZero(0);
		}

		// Fill out the string with zeros until we reach the min place value.
		for (int i = 0; i < Min(); i++) {
			retVal += UIntToHexStringWithZero(0);
		}

		// Loop over the digits highest to lowest
		for (size_t i = 0; i < limbs.Size(); i++) {
			int ix_digit = limbs.Size() - 1 - i;
			int placeValue = ix_digit + exponent;

			Limb digitValue = limbs.Get(ix_digit);
			if (placeValue >= 0 && placeValue == Max()) {
				retVal += UIntToHexStringNoZero(digitValue);
			} else {
				retVal += UIntToHexStringWithZero(digitValue);
			}
			retVal += " ";

			
			if (i + 1 < limbs.Size() && placeValue == 0) {
				retVal += ". ";
			}
		}

		return retVal;
	}

	void Prune()
	{
		while (limbs.Size() > 0 && limbs.Get(0) == 0) {
			limbs.PopFront();
		}

		while (limbs.Size() > 0 && limbs.Get(limbs.Size() - 1) == 0) {
			limbs.PopBack();
		}
	}
};

