#pragma once

typedef unsigned __int32 Limb;
typedef unsigned __int64 DoubleLimb;

using std::vector;

class BigInt
{
	vector<Limb> limbs;

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
		return (this->limbs.size() + 1) / 2; // divide by 2 and round up
	}

	BigInt splitLow(size_t m) const
	{
		BigInt r;
		r.GrowTo(m - 1);
		for (size_t i = 0; i < m; i++) {
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
	size_t minToomSize;

	BigInt(void)
	{
		this->limbs = vector<Limb>();
		minToomSize = 0x180;
	}

	~BigInt(void) {
	}

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

		for (size_t i = 0; i < a_s; i++) {
			Limb a_i = this->Get(a_s - i - 1);
			Limb b_i = b.Get(b_s - i - 1);
			if (a_i < b_i) {
				return false;
			} else if (a_i > b_i) {
				return true;
			}
		}
		return false;
	}

	BigInt& operator+=(const BigInt &b) {
		this->Add(b);
		return *this;
	}

	BigInt& operator-=(const BigInt &b) {
		this->Subtract(b);
		return *this;
	}

	BigInt operator+(const BigInt &b) const {
		return BigInt(*this) += b;
	}

	BigInt operator-(const BigInt &b) const {
		return BigInt(*this) -= b;
	}

	BigInt operator*(const BigInt &b) const {
		return this->Toom2(b);
	}

	BigInt operator<<(const size_t n) const {
		return this->LimbShiftLeft(n);
	}

	BigInt operator<<=(const size_t n) const {
		return BigInt(*this).LimbShiftLeft(n);
	}

	void GrowTo(unsigned e)
	{
		if (e + 1 > this->limbs.size()) {
			this->limbs.resize(e + 1, 0);
		}
	}

	void Set(Limb l, unsigned e)
	{
		this->GrowTo(e);
		this->limbs[e] = l;
	}

	void SetString(const std::string s)
	{
		this->limbs.clear();
		size_t numSize = (s.size() + 1) / 9;
		if ((s.size() + 1) % 9 != 0) {
			throw std::invalid_argument("string must be in form 'FFFFFFFF FFFFFFFF'");
		}
		for (size_t i = 0; i < numSize; i++) {
			std::string limbText = s.substr(i * 9, 8);
			Limb curLimb = std::stoul(limbText, 0, 16);
			this->Set(curLimb, numSize - i - 1);
		}
	}

	void FillValue(size_t size, Limb value)
	{
		this->limbs.clear();
		for (size_t i = 0; i < size; i++) {
			this->Set(value, i);
		}
	}

	void FillRandom(const size_t size)
	{
		// std::cout << size << " ";
		this->limbs.clear();
		for (size_t i = 0; i < size; i++) {
			this->Set(rand(), i);
		}
	}

	Limb Get(unsigned e) const
	{
#ifdef _DEBUG
		if (e >= this->limbs.size()) {
			throw std::out_of_range("e is out of range.");
		}
#endif

		return this->limbs[e];
	}

	size_t Size() const
	{
		size_t realSize = this->limbs.size();
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

	void Add(const BigInt &b)
	{
		this->GrowTo(b.limbs.size() - 1);
		Limb carry = 0;
		const Limb mask = ~0;
		const int limbWidth = sizeof(Limb) * 8;
		const size_t b_s = b.limbs.size();
		for (size_t i = 0; i < b_s; i++) {
			DoubleLimb aLimb = this->Get(i);
			Limb bLimb = b.Get(i);
			DoubleLimb rLimb = aLimb + bLimb + carry;
			this->Set(rLimb & mask, i);
			carry = rLimb >> limbWidth;
		}
		this->AddLimb(carry, b_s);
	}

	void Add2(const BigInt &b)
	{
		this->GrowTo(b.limbs.size() - 1);
		Limb carry = 0;
		const Limb mask = ~0;
		const int limbWidth = sizeof(Limb) * 8;
		const size_t b_s = b.limbs.size();
		for (size_t i = 0; i * 2 < b_s; i++) {
			DoubleLimb aLimb = this->Get(i * 2) | ((DoubleLimb)this->Get(i * 2 + 1) << limbWidth);
			DoubleLimb bLimb = b.Get(i * 2) | ((DoubleLimb)b.Get(i * 2 + 1) << limbWidth);
			DoubleLimb rLimb = aLimb + bLimb + carry;
			this->Set(rLimb & mask, i);
			this->Set(rLimb >> limbWidth, i + 1);
			carry = (aLimb != 0 || carry != 0) && rLimb <= bLimb;
		}
		if (b_s % 2 == 1) {
			Limb aLimb = this->Get(b_s - 1);
			Limb bLimb = b.Get(b_s - 1);
			Limb rLimb = aLimb + bLimb + carry;
			this->Set(rLimb, b_s - 1);
			carry = (aLimb != 0 || carry != 0) && rLimb <= bLimb;
		}
		this->AddLimb(carry, b_s);
	}

	BigInt Plus(const BigInt b) const
	{
		BigInt r = *this;
		
		r.Add(b);

		return r;
	}

	void SubtractLimb(Limb b, unsigned e)
	{
#ifdef _DEBUG
		if (e >= this->limbs.size()) {
			throw std::invalid_argument("b must < a");
		}
#endif

		Limb aLimb = this->Get(e);

#ifdef _DEBUG
		if (aLimb < b && e == this->limbs.size() - 1) {
			throw std::invalid_argument("b must < a");
		}
#endif

		int borrow = this->getBorrow(aLimb, b);
		this->Set(aLimb - b, e);
		if (borrow != 0) {
			this->SubtractLimb(borrow, e + 1);
		}
	}

	void Subtract(const BigInt &b)
	{
#ifdef _DEBUG
		if (b > *this) {
			throw std::invalid_argument("BigInt::Minus only works if a >= b");
		}
#endif

		int borrow = 0;
		const size_t limbWidth = sizeof(Limb) * 8;
		size_t b_s = b.Size();
		for (size_t i = 0; i < b_s; i++) {
			Limb aLimb = this->Get(i);
			Limb bLimb = b.Get(i);
			DoubleLimb rLimb = (DoubleLimb)aLimb - bLimb - borrow;
			this->Set((Limb)rLimb, i);
			borrow = rLimb >> (limbWidth * 2 - 1);

		}
		if (borrow > 0) {
			this->SubtractLimb(borrow, b.Size());
		}
	}

	void Subtract2(const BigInt &b)
	{
#ifdef _DEBUG
		if (b > *this) {
			throw std::invalid_argument("BigInt::Minus only works if a >= b");
		}
#endif

		int borrow = 0;
		const size_t limbWidth = sizeof(Limb) * 8;
		size_t b_s = b.Size();
		for (size_t i = 0; i < b_s; i++) {
			Limb aLimb = this->Get(i);
			Limb bLimb = b.Get(i);
			DoubleLimb rLimb = (DoubleLimb)aLimb - bLimb - borrow;
			this->Set((Limb)rLimb, i);
			borrow = rLimb >> (limbWidth * 2 - 1);

		}
		if (borrow > 0) {
			this->SubtractLimb(borrow, b.Size());
		}
	}


	BigInt Minus(const BigInt &b) const
	{
		BigInt r = *this;
		r.Subtract(b);

		return r;
	}

	BigInt Times(const BigInt b) const
	{
		BigInt r;
		for (unsigned bIdx = 0; bIdx < b.limbs.size(); bIdx++) {
			BigInt tmp = this->ScaledBy(b.Get(bIdx)).LimbShiftLeft(bIdx);
			r = r.Plus(tmp);
		}

		return r;
	}

	BigInt TimesResult(const BigInt b) const
	{
		BigInt r;
		// multiply by 0
		if (this->limbs.size() == 0 || b.limbs.size() == 0) {
			return r;
		}

		size_t resultSize = this->limbs.size() + b.limbs.size() - 2 + 1;
		r.GrowTo(resultSize - 1);
		DoubleLimb carry = 0; // may overflow if maxVal(Limb) additions are performed
		for (size_t rIdx = 0; rIdx < resultSize; rIdx++) {
			DoubleLimb tmp = carry & ~(Limb)0;
			carry >>= (sizeof(Limb) * 8);
			int startIdx = rIdx - b.limbs.size() + 1;
			startIdx = startIdx < 0 ? 0 : startIdx;
			for (size_t aIdx = startIdx; aIdx < this->limbs.size() && rIdx >= aIdx; aIdx++) {
				int bIdx = (int)rIdx - aIdx;
				Limb aLimb = this->Get(aIdx);
				DoubleLimb aDblLimb = aLimb;
				DoubleLimb bLimb = b.Get(bIdx);
				DoubleLimb mulLimb = aDblLimb * bLimb;
				tmp += mulLimb;
				carry += tmp >> (sizeof(Limb) * 8);
				tmp &= ~(Limb)0;
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
	BigInt Toom2(const BigInt b) const
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
			a_0.minToomSize = a_1.minToomSize = this->minToomSize;
			BigInt r = (a_1.Toom2(b) << m) + a_0.Toom2(b);
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
		a_0.minToomSize = a_1.minToomSize = this->minToomSize;
		BigInt r_0 = a_0.Toom2(b_0);
		BigInt r_2 = a_1.Toom2(b_1);
		BigInt r_1_0 = a_1 + a_0;
		r_1_0.minToomSize = this->minToomSize;
		BigInt r_1 = r_1_0.Toom2(b_1 + b_0);
		r_1 -= r_2;
		r_1 -= r_0;
		r_1 <<= m;
		r_2 <<= 2*m;
		r_2 += r_1;
		r_2 += r_0;
		return r_2;
	}

	BigInt ScaledBy(Limb b) const
	{
		BigInt r;
		r.GrowTo(this->limbs.size() - 1);
		Limb carry = 0;
		for (size_t i = 0; i < this->limbs.size(); i++) {
			DoubleLimb tmp = (DoubleLimb)this->Get(i) * b + carry;
			Limb lowLimb = tmp & ~0;
			carry = tmp >> (sizeof(Limb) * 8);
			r.Set(lowLimb, i);
		}
		r.Set(carry, r.limbs.size());

		return r;
	}

	BigInt LimbShiftLeft(unsigned v) const
	{
		BigInt r;
		r.GrowTo(v + this->limbs.size() - 1);
		for (size_t i = 0; i < this->limbs.size(); i++) {
			r.Set(this->Get(i), i + v);
		}

		return r;
	}

	void Trim()
	{
		while (this->limbs.size() > this->Size()) {
			this->limbs.pop_back();
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