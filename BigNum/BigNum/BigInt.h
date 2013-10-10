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

	bool operator==(const BigInt &b) const {
		if (this->Size() != b.Size()) {
			return false;
		} else {
			for (size_t i = 0; i < this->Size(); i++) {
				if (this->Get(i) != b.Get(i)) {
					return false;
				}
			}
		}
		return true;
	}

	void GrowTo(unsigned e)
	{
		this->limbs.GrowTo(e + 1);
	}

	void Set(Limb l, unsigned e)
	{
		this->GrowTo(e);
		this->limbs.Set(e, l);
	}

	void SetString(const std::string s)
	{
		this->limbs.Clear();
		size_t numSize = (s.size() + 1) / 9;
		if ((s.size() + 1) % 9 != 0) {
			throw std::invalid_argument("string must be in form 'FFFFFFFF FFFFFFFF'");
		}
		for (int i = 0; i < numSize; i++) {
			std::string limbText = s.substr(i * 9, 8);
			Limb curLimb = std::stoul(limbText, 0, 16);
			this->Set(curLimb, numSize - i - 1);
		}
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
		size_t realSize = this->limbs.Size();
		for (; realSize > 0 && this->Get(realSize - 1) == 0; realSize--) {
		}
		return realSize;
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
		for (size_t i = 0; i < b.limbs.Size(); i++) {
			this->AddLimb(b.Get(i), i);
		}
	}

	BigInt Plus(BigInt b) const
	{
		BigInt r = *this;
		BigInt carry;
		r.GrowTo(b.limbs.Size() - 1);
		for (unsigned i = 0; i < b.limbs.Size(); i++) {
			carry.AddLimb(this->getCarry(r.Get(i), b.Get(i)), i + 1);
			r.Set(r.Get(i) + b.Get(i), i);
		}
		r.FinalAdd(carry);

		return r;
	}

	BigInt Times(BigInt b) const
	{
		BigInt r;
		for (unsigned bIdx = 0; bIdx < b.limbs.Size(); bIdx++) {
			BigInt tmp = this->ScaledBy(b.Get(bIdx)).LimbShiftLeft(bIdx);
			r = r.Plus(tmp);
		}

		return r;
	}

	BigInt TimesResult(BigInt b) const
	{
		BigInt r;
		size_t resultSize = this->limbs.Size() + b.limbs.Size() - 2 + 1;
		r.GrowTo(resultSize - 1);
		DoubleLimb carry = 0; // may overflow if maxVal(Limb) additions are performed
		for (size_t rIdx = 0; rIdx < resultSize; rIdx++) {
			DoubleLimb tmp = carry;
			carry = 0;
			for (size_t aIdx = 0; aIdx < this->limbs.Size(); aIdx++) {
				int bIdx = (int)rIdx - aIdx;
				if (bIdx >= 0 && bIdx < b.limbs.Size()) { // if is a valid idx
					Limb aLimb = this->Get(aIdx);
					DoubleLimb aDblLimb = aLimb;
					DoubleLimb bLimb = b.Get(bIdx);
					DoubleLimb mulLimb = aDblLimb * bLimb;
					tmp += mulLimb;
					carry += tmp >> (sizeof(Limb) * 8);
					tmp &= ~(Limb)0;
				}
			}
			r.Set((Limb)tmp, rIdx);
		}
		Limb lowCarry = carry & ~(Limb)0;
		Limb highCarry = carry >> (sizeof(Limb) * 8);
		if (lowCarry != 0) {
			r.Set(lowCarry, resultSize);
		}
		if (highCarry != 0) {
			r.Set(highCarry, resultSize + 1);
		}
			
		return r;
	}

	BigInt ScaledBy(Limb b) const
	{
		BigInt r;
		r.GrowTo(this->limbs.Size() - 1);
		Limb carry = 0;
		for (size_t i = 0; i < this->limbs.Size(); i++) {
			DoubleLimb tmp = (DoubleLimb)this->Get(i) * b + carry;
			Limb lowLimb = tmp & ~0;
			carry = tmp >> (sizeof(Limb) * 8);
			r.Set(lowLimb, i);
		}
		r.Set(carry, r.limbs.Size());

		return r;
	}

	BigInt LimbShiftLeft(unsigned v) const
	{
		BigInt r;
		r.GrowTo(v + this->limbs.Size() - 1);
		for (size_t i = 0; i < this->limbs.Size(); i++) {
			r.Set(this->Get(i), i + v);
		}
		return r;
	}

	void Trim()
	{
		while (this->limbs.Size() > this->Size()) {
			this->limbs.PopBack();
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