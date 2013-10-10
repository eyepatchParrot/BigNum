#pragma once

typedef unsigned __int32 Limb;
typedef unsigned __int64 DoubleLimb;

class BigInt
{
	Deque<Limb> limbs;

	int getCarry(Limb a, Limb b) const
	{
		if (a + b < a) {
			return 1;
		} else {
			return 0;
		}
	}

public:
	BigInt(void)
	{
		this->limbs = Deque<Limb>(UIntToHexStringWithZero);
	}

	~BigInt(void) { }

	void GrowTo(unsigned e)
	{
		while (this->limbs.Size() <= e) {
			this->limbs.PushBack(0);
		}
	}

	void Set(Limb l, unsigned e)
	{
		this->GrowTo(e);
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
		if (b != 0) {
			this->GrowTo(e);
			Limb aLimb = this->Get(e);

			this->Set(aLimb + b, e);
			AddLimb(this->getCarry(aLimb, b), e + 1);
		}
	}

	void FinalAdd(BigInt b)
	{
		for (size_t i = 0; i < b.Size(); i++) {
			this->AddLimb(b.Get(i), i);
		}
	}

	BigInt Plus(BigInt b) const
	{
		BigInt r = *this;
		BigInt carry;
		r.GrowTo(b.Size());
		for (unsigned i = 0; i < b.Size(); i++) {
			carry.AddLimb(this->getCarry(r.Get(i), b.Get(i)), i + 1);
			r.Set(r.Get(i) + b.Get(i), i);
		}
		r.FinalAdd(carry);

		return r;
	}

	BigInt Times(BigInt b) const
	{
		BigInt r;
		for (unsigned bIdx = 0; bIdx < b.Size(); bIdx++) {
			BigInt tmp = this->ScaledBy(b.Get(bIdx)).LimbShiftLeft(bIdx);
			r = r.Plus(tmp);
		}

		return r;
	}

	BigInt ScaledBy(Limb b) const
	{
		BigInt r;
		r.GrowTo(this->Size() - 1);
		for (size_t i = 0; i < this->Size(); i++) {
			DoubleLimb tmp = (DoubleLimb)this->Get(i) * b;
			Limb lowLimb = tmp & (Limb)-1;
			Limb highLimb = (tmp >> (sizeof(Limb) * 8)) & (Limb)-1;
			r.AddLimb(lowLimb, i);
			r.AddLimb(highLimb, i + 1);
		}

		return r;
	}

	BigInt LimbShiftLeft(unsigned v) const
	{
		BigInt r;
		r.GrowTo(v + this->Size() - 1);
		for (size_t i = 0; i < this->Size(); i++) {
			r.Set(this->Get(i), i + v);
		}
		return r;
	}

	void Trim()
	{
		for (; this->Size() > 0 && this->Get(this->Size() - 1) == 0; this->limbs.PopBack() ) {
		}
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

