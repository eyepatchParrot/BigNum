#pragma once

template <class Limb>
class BigNumCpu
{
	Deque<Limb> limbs;
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
		limbs = Deque<Limb>(1);
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

	bool Set(int placeValue, Limb value)
	{
		if (limbs.Size() == 0 || (limbs.Size() == 1 && limbs.Get(0) == 0)) {
			// There are no significant members in limbs.
		}
		return Limb();
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
		return std::string();
	}
};

