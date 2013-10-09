#pragma once

typedef unsigned __int32 Limb;
typedef unsigned __int64 DoubleLimb;

class BigInt
{
	Deque<Limb> limbs;

	bool needCarry(Limb a, Limb b)
	{
		return a + b < a;
	}

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

	Limb Get(unsigned e) const
	{
		if (e >= this->limbs.Size()) {
			throw std::out_of_range("e is out of range.");
		}

		return this->limbs.Get(e);
	}

	size_t Size() const
	{
		return this->limbs.Size();
	}

	void AddLimb(Limb b, unsigned e)
	{
		while (e >= this->Size()) {
			this->limbs.PushBack(0);
		}

		Limb aLimb = this->Get(e);

		this->Set(aLimb + b, e);

		if (this->needCarry(aLimb, b)) {
			AddLimb(1, e + 1);
		}
	}

	BigInt Plus(BigInt b) const
	{
		BigInt r = *this;
		for (unsigned i = 0; i < b.Size(); i++) {
			r.AddLimb(b.Get(i), i);
		}

		return r;
	}

	BigInt Times(BigInt b) const
	{
		BigInt r;
		for (unsigned aIdx = 0; aIdx < this->Size(); aIdx++) {
			for (unsigned bIdx = 0; bIdx < b.Size(); bIdx++) {
				DoubleLimb tmp = (DoubleLimb)this->Get(aIdx) * b.Get(bIdx);
				unsigned lowE = aIdx + bIdx;
				Limb lowLimb = tmp & (Limb)-1;
				unsigned highE = lowE + 1;
				Limb highLimb = (tmp >> (sizeof(Limb) * 8)) & (Limb)-1;
				r.AddLimb(lowLimb, lowE);
				r.AddLimb(highLimb, highE);
			}
		}

		return r;
	}

	std::string String() const
	{
		std::string r;

		bool needFiller = false;
		for (int i = (int)this->Size() - 1; i >= 0; i--) {
			r += UIntToHexString(this->Get(i), needFiller);
			if (i > 0) {
				r += " ";
			}

			if (this->Get(i) != 0) {
				needFiller = true;
			}
		}

		return r;
	}
};

