#pragma once

#include "stdafx.h"

template <class Limb>
class BigNumCpu
{
	std::string (*LimbToStringWithZero)(Limb);
	std::string (*LimbToStringNoZero)(Limb);
	Deque<Limb> limbs;
	// the placeValue of limbs[0]
	int exponent;

	bool IdxIsValid(int placeValue)
	{
		return placeValue >= exponent && GetDequeIdx(placeValue) < limbs.size();
	}

	int GetDequeIdx(int placeValue)
	{
		return placeValue - exponent;
	}

public:
	BigNumCpu(std::string (*LimbToStringWithZeroArg)(Limb), std::string (*LimbToStringNoZeroArg)(Limb))
	{
		LimbToStringWithZero = LimbToStringWithZeroArg;
		LimbToStringNoZero = LimbToStringNoZeroArg;
		exponent = 0;
		limbs = Deque<Limb>(LimbToStringWithZero);
	}

	~BigNumCpu(void) { }

	Limb Get(int placeValue)
	{
		if (IdxIsValid(placeValue)) {
			return limbs[GetDequeIdx(placeValue)];
		} else {
			throw "Out of bounds.";
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
			while (placeValue < Min()) {
				limbs.PushFront(0);
				exponent--;
			}
			while (placeValue > Max()) {
				limbs.PushBack(0);
				//exponent++;
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

		if (LimbToStringWithZero == NULL) {
			throw "No ToStringWithZero func provided.";
		} else if (LimbToStringNoZero == NULL) {
			throw "No ToStringNoZero func provided.";
		}

		std::cout << limbs.ToString() << std::endl;

		if (Max() < 0) {
			retVal += "0 . ";
		}

		// Fill out the string with zeros until we reach the max place value.
		for (int i = 0; i < 0 - Max() - 1; i++) {
			// loop from place value
			retVal += LimbToStringWithZero(0);
		}

		// Fill out the string with zeros until we reach the min place value.
		for (int i = 0; i < Min(); i++) {
			retVal += LimbToStringWithZero(0);
		}

		// Loop over the digits highest to lowest
		for (size_t i = 0; i < limbs.Size(); i++) {
			int ix_digit = limbs.Size() - 1 - i;
			int placeValue = ix_digit + exponent;

			Limb digitValue = limbs.Get(ix_digit);
			if (placeValue >= 0 && placeValue == Max()) {
				retVal += LimbToStringNoZero(digitValue);
			} else {
				retVal += LimbToStringWithZero(digitValue);
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

		while (limbs.Size() > 0 && limbs.Get(limbs.Size - 1) == 0) {
			limbs.PopBack();
		}
	}
};

