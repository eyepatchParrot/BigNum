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

	int getBorrow(Limb a, Limb b) const
	{
		if (a < b) {
			return 1;
		} else {
			return 0;
		}
	}

	bool isBigEnoughForToom(size_t a_s, size_t b_s) const
	{
		return a_s > minToomSize || b_s > minToomSize;
	}

	bool isLopsided(size_t a_s, size_t b_s) const
	{
		const double minLopsided = 1.4; // no higher than 2
		return (double)a_s / b_s > minLopsided;
	}

	size_t toom2M() const
	{
		return (this->limbs.Size() + 1) / 2; // divide by 2 and round up
	}

	BigInt splitLow(size_t m) const
	{
		BigInt r;
		r.GrowTo(m - 1);
		for (int i = 0; i < m; i++) {
			r.Set(this->Get(i), i);
		}
		return r;
	}

	BigInt splitHigh(size_t m, size_t sz) const
	{
		BigInt r;
		size_t r_s = sz - m;
		r.GrowTo(r_s - 1);
		for (size_t i = 0; i < r_s; i++) {
			r.Set(this->Get(i + m), i);
		}
		return r;
	}

public:
	int minToomSize;

	BigInt(void)
	{
		this->limbs = Deque<Limb>(UIntToHexStringWithZero);
		minToomSize = 64;
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

	bool operator>(const BigInt &b) const {
		size_t a_s = this->Size();
		size_t b_s = b.Size();
		if (a_s > b_s) {
			return true;
		} else if (a_s < b_s) {
			return false;
		}

		// a_s = b_s
		if (a_s == 0) {
			return false;
		}

		for (size_t i = 0; i + 1 < a_s; i++) {
			Limb a_i = this->Get(a_s - i - 1);
			Limb b_i = b.Get(b_s - i - 1);
			if (a_i < b_i) {
				return false;
			}
		}
		return this->Get(0) > b.Get(0);
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

	void FillValue(size_t size, Limb value)
	{
		this->limbs.Clear();
		for (size_t i = 0; i < size; i++) {
			this->Set(value, i);
		}
	}

	void FillRandom(const size_t size)
	{
		// std::cout << size << " ";
		this->limbs.Clear();
		for (size_t i = 0; i < size; i++) {
			this->Set(rand(), i);
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

	void SubtractLimb(Limb b, unsigned e)
	{
		if (e >= this->limbs.Size()) {
			throw std::invalid_argument("b must < a");
		}

		Limb aLimb = this->Get(e);
		if (aLimb < b && e == this->limbs.Size() - 1) {
			throw std::invalid_argument("b must < a");
		}

		int borrow = this->getBorrow(aLimb, b);
		this->Set(aLimb - b, e);
		if (borrow != 0) {
			this->SubtractLimb(borrow, e + 1);
		}
	}


	BigInt Minus(BigInt b) const
	{
		if (b > *this) {
			throw std::invalid_argument("BigInt::Minus only works if a >= b");
		}

		BigInt r = *this;
		int borrow = 0;
		for (size_t i = 0; i < b.Size(); i++) {
			r.SubtractLimb(b.Get(i), i);
		}

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
		// multiply by 0
		if (this->limbs.Size() == 0 || b.limbs.Size() == 0) {
			return r;
		}

		size_t resultSize = this->limbs.Size() + b.limbs.Size() - 2 + 1;
		r.GrowTo(resultSize - 1);
		DoubleLimb carry = 0; // may overflow if maxVal(Limb) additions are performed
		for (size_t rIdx = 0; rIdx < resultSize; rIdx++) {
			DoubleLimb tmp = carry & ~(Limb)0;
			carry >>= (sizeof(Limb) * 8);
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

	// B = 2^32
	BigInt Toom2(BigInt b) const
	{
		size_t a_s = this->Size();
		size_t b_s = b.Size();

		if (a_s == 0 || b_s == 0) {
			return BigInt();
		}

		if (!isBigEnoughForToom(a_s, b_s)) {
			return this->TimesResult(b);
		}

		if (this->isLopsided(b_s, a_s)) {
			return b.Toom2(*this);
		} else if (this->isLopsided(a_s, b_s)) {
			// lopsided a
			// a = a_1B^m + a_0
			// b = b
			// r = b(a_1)(B^m) + b(a_0)
			size_t m = this->toom2M();
			BigInt a_0 = this->splitLow(m);
			BigInt a_1 = this->splitHigh(m, a_s);
			BigInt r_0 = a_0.Toom2(b);
			BigInt r_1 = a_1.Toom2(b).LimbShiftLeft(m);
			BigInt r = r_1.Plus(r_0);
			return r;
		}

		// even enough
		// a = a_1(B^m) + a_0
		// b = b_1(B^m) + b_0
		// r = r_2(B^2m) + r_1(B^m) + r_0
		// r_0 = a_0(b_0)
		// r_1 = (a_1 + a_0)(b_1 + b_0) - r_2 - r_0
		// r_2 = a_1(b_1)
		size_t m;
		if (a_s > b_s) {
			m = this->toom2M();
		} else {
			m = b.toom2M();
		}

		BigInt a_0 = this->splitLow(m);
		BigInt a_1 = this->splitHigh(m, a_s);
		BigInt b_0 = b.splitLow(m);
		BigInt b_1 = b.splitHigh(m, b_s);
		BigInt r_0 = a_0.Toom2(b_0);
		BigInt r_2 = a_1.Toom2(b_1);
		BigInt r_1_0 = a_1.Plus(a_0);
		BigInt r_1_1 = b_1.Plus(b_0);
		BigInt r_1 = (r_1_0).Toom2(r_1_1);
		r_1 = r_1.Minus(r_2);
		r_1 = r_1.Minus(r_0);
		r_1 = r_1.LimbShiftLeft(m);
		r_2 = r_2.LimbShiftLeft(m * 2);
		BigInt r = r_2.Plus(r_1).Plus(r_0);
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