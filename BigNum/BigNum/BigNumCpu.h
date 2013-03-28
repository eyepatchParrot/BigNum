#pragma once

#include "stdafx.h"

template <class Limb, std::string (*LimbToString)(Limb)>
class BigNumCpu
{
	Deque<Limb, LimbToString> limbs;
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
	BigNumCpu(void)
	{
		exponent = 0;
		limbs = Deque<Limb>(1, 0);
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
			while (placeValue < exponent) {
				limbs.PushFront(0);
				exponent--;
			}
			while (placeValue > (exponent + limbs.Size() - 1)) {
				limbs.PushBack(0);
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

	void Shl(int value)
	{
	}

	void Shr(int value)
	{
	}

	std::string ToString(void)
	{
		for (int i = 0; i < 
		return std::string();
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

