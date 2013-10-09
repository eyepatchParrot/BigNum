#pragma once

typedef unsigned __int32 Limb;

class BigInt
{
	Deque<Limb> limbs;

public:
	BigInt(void)
	{
		this->limbs = Deque<Limb>(UIntToHexStringWithZero);
	}

	~BigInt(void) { }

	void Set(Limb l, unsigned e)
	{
		while (e >= this->limbs.Size()) {
			this->limbs.PushBack(0);
		}

		this->limbs.Set(e, l);
	}

	Limb Get(unsigned e)
	{
		if (e >= this->limbs.Size()) {
			throw std::out_of_range("e is out of range.");
		}

		return this->limbs.Get(e);
	}

	size_t Size()
	{
		return this->limbs.Size();
	}
};

