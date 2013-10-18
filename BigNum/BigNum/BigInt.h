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

	bool isBigEnoughForToom3(size_t a_s, size_t b_s) const
	{
		return a_s > minToom3Size || b_s > minToom3Size;
	}

	bool isLopsided(size_t a_s, size_t b_s) const
	{
		const double minLopsided = 1.4; // no higher than 2
		return (double)a_s / b_s > minLopsided;
	}

	size_t toomM(size_t a_s, size_t b_s, size_t k) const
	{
		return ceilDivide(max(a_s, b_s), k);
	}

	BigInt split(size_t min, size_t max) const
	{
		if (max <= min) {
			return BigInt();
		}

		BigInt r;
		size_t r_s = max - min;
		size_t a_s = this->limbs.size();
		r.GrowTo(r_s - 1);
		for (size_t i = 0; i + min < max && i + min < a_s; i++) {
			r.Set(this->Get(i + min), i);
		}
		return r;
	}

public:
	size_t minToomSize;
	size_t minToom3Size;

	BigInt(void)
	{
		this->limbs = vector<Limb>();
		minToomSize = 0x180;
		minToom3Size = 3;
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

	BigInt operator*(const Limb b) const {
		return this->ScaledBy(b);
	}

	BigInt operator/(const Limb b) const {
		switch(b) {
		case 2:
			return this->BitShiftLeft(1);
			break;

		case 3:
			return this->DividedBy3();
			break;

		case 6:
			return (this->DividedBy3()).BitShiftLeft(1);
			break;

#ifdef _DEBUG
		default:
			throw std::invalid_argument("division not by 2, 3, or 6");
#endif
		}
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
		// multiply by 0
		if (this->limbs.size() == 0 || b.limbs.size() == 0) {
			return BigInt();
		}

		BigInt r;
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

	BigInt TimesLopsided(const BigInt b) const
	{
		size_t a_s = this->Size();
		size_t b_s = b.Size();

		if (this->isLopsided(b_s, a_s)) {
			return b.TimesLopsided(*this);
		}

		// lopsided a
		// a = a_1B^m + a_0
		// b = b
		// r = b(a_1)(B^m) + b(a_0)
		size_t m = this->toomM(a_s, b_s, 2);
		BigInt a_0 = this->split(0, m);
		BigInt a_1 = this->split(m, a_s);
		a_0.minToomSize = a_1.minToomSize = this->minToomSize;
		BigInt r = ((a_1 * b) << m) + a_0 * b;
		return r;
	}

	// B = 2^32
	BigInt Toom2(const BigInt b) const
	{
		size_t a_s = this->Size();
		size_t b_s = b.Size();

		if (!isBigEnoughForToom(a_s, b_s)) {
			return this->TimesResult(b);
		}

		if (this->isLopsided(b_s, a_s) || this->isLopsided(a_s, b_s)) {
			return this->TimesLopsided(b);
		}

		// even enough
		// a = a_1(B^m) + a_0
		// b = b_1(B^m) + b_0
		// r = r_2(B^2m) + r_1(B^m) + r_0
		// r_0 = a_0(b_0)
		// r_1 = (a_1 + a_0)(b_1 + b_0) - r_2 - r_0
		// r_2 = a_1(b_1)
		size_t m = this->toomM(a_s, b_s, 2);

		BigInt a_0 = this->split(0, m);
		BigInt a_1 = this->split(m, a_s);
		BigInt b_0 = b.split(0, m);
		BigInt b_1 = b.split(m, b_s);
		a_0.minToomSize = a_1.minToomSize = this->minToomSize;
		BigInt r_0 = a_0 * b_0;
		BigInt r_2 = a_1 * b_1;
		BigInt r_1_0 = a_1 + a_0;
		r_1_0.minToomSize = this->minToomSize;
		BigInt r_1 = r_1_0 * (b_1 + b_0);
		r_1 -= r_2;
		r_1 -= r_0;
		r_1 <<= m;
		r_2 <<= 2*m;
		r_2 += r_1;
		r_2 += r_0;
		return r_2;
	}

	BigInt Toom3(const BigInt b) const
	{
		size_t a_s = this->Size();
		size_t b_s = b.Size();

		if (!isBigEnoughForToom3(a_s, b_s)) {
			return this->Toom2(b);
		}

		if (this->isLopsided(b_s, a_s) || this->isLopsided(a_s, b_s)) {
			return this->TimesLopsided(b);
		}

		///////////
		// split //
		///////////
		
		// even enough
		// p(x) = a_2(x^2) + a_1(x) + a_0
		// q(x) = b_2(x^2) + b_1(x) + b_0
		// A = p(m)
		// B = q(m)
		// m = ceil(max(a_s, b_s) / 3)

		size_t m = this->toomM(a_s, b_s, 3);
		BigInt a_0 = this->split(0, m);
		BigInt a_1 = this->split(m, 2*m);
		BigInt a_2 = this->split(2*m, a_s);
		BigInt b_0 = b.split(0, m);
		BigInt b_1 = b.split(m, 2*m);
		BigInt b_2 = b.split(2*m, b_s);

		//////////////
		// Evaluate //
		//////////////

		// p(0) = a_0
		// p(1) = a_0 + a_1 + a_2
		// p(-1) = a_0 - a_1 + a_2
		// p(2) = a_0 + 2a_1 + 4a_2
		// p(inf) = a_2

		BigInt p_0 = a_0;
		BigInt p_1 = a_0 + a_1 + a_2;
		BigInt p_2 = a_0 + a_1 * 2 + a_2 * 4;
		BigInt p_inf = a_2;
		BigInt p_n1 = a_0 + a_2;
		bool p_n1_isNeg = false;
		if (a_1 > p_n1) {
			p_n1 = a_1 - p_n1;
			p_n1_isNeg = true;
		} else {
			p_n1 = p_n1 - a_1;
		}

		BigInt q_0 = b_0;
		BigInt q_1 = b_0 + b_1 + b_2;
		BigInt q_2 = b_0 + b_1 * 2 + b_2 * 4;
		BigInt q_inf = b_2;
		BigInt q_n1 = b_0 + b_2;
		bool q_n1_isNeg = false;
		if (b_1 > q_n1) {
			q_n1 = b_1 - q_n1;
			q_n1_isNeg = true;
		} else {
			q_n1 = q_n1 - b_1;
		}

		////////////////////////
		// pointwise multiply //
		////////////////////////

		// r(x) = p(x)q(x)
		bool r_n1_isNeg = p_n1_isNeg ^ q_n1_isNeg;
		BigInt r_0 = p_0 * q_0;
		BigInt r_1 = p_1 * q_1;
		BigInt r_n1 = p_n1 * q_n1;
		BigInt r_2 = p_2 * q_2;
		BigInt r_inf = p_inf * q_inf;

		/////////////////
		// interpolate //
		/////////////////

		// c_0 = r(0)
		// c_4 = r(inf)
		// c_1 = r(0)/2 + r(1)/3 - r(-1) + r(-2)/6 - 2r(inf)
		// c_2 = -r(0) + r(1)/2 + r(-1)/2 - r(inf)
		// c_3 = -r(0)/2 + r(1)/6 + r(-1)/2 - r(-2)/6 + 2r(inf)
		// c = c_4(m^4) + c_3(m^3) + c_2(m^2) + c_1(m) + r_0

		BigInt c_0 = r_0;
		BigInt c_4 = r_inf;
		BigInt c_1 = r_0 / 2 + r_1 / 3 + r_n2 / 6 - r_inf * 2; // - r_n1
		BigInt c_2 = BigInt() - r_0 + r_1 / 2 - r_inf; // + r_n1 / 2
		BigInt c_3 = BigInt() - r_0 / 2 + r_1 / 6 - r_n2 / 6 + r_inf * 2; // + r_n1 / 2
		if (r_n1_isNeg) {
			c_1 += r_n1;
			c_2 -= r_n1 / 2;
			c_3 -= r_n1 / 2;
		} else {
			c_1 -= r_n1;
			c_2 += r_n1 / 2;
			c_3 += r_n1 / 2;
		}

		BigInt c = c_0 + (c_1 << m) + (c_2 << (2 * m)) + (c_3 << (3 * m)) + (c_4 << (4 * m));
		return c;
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

	BigInt DividedBy3() const
	{
		const Limb modInverse3 = 0xAAAAAAAB;
		const Limb thirdMax = 0x55555556;
		const Limb twoThirdsMax = 0xAAAAAAAB;

		BigInt r;
		Limb borrow = 0;
		for (size_t i = 0; i < this->limbs.size(); i++) {
			Limb rLimb = this->Get(i) - borrow;
			borrow = this->getBorrow(this->Get(i), borrow);
			rLimb = rLimb * modInverse3;
			r.Set(rLimb, i);

			borrow += (rLimb >= thirdMax);
			borrow += (rLimb >= twoThirdsMax);
		}
		return r;
	}

	BigInt LimbShiftLeft(unsigned v) const
	{
		if (v == 0) {
			return *this;
		}

		BigInt r;
		r.GrowTo(v + this->limbs.size() - 1);
		for (size_t i = 0; i < this->limbs.size(); i++) {
			r.Set(this->Get(i), i + v);
		}

		return r;
	}

	BigInt BitShiftLeft(unsigned n) const
	{
		BigInt r;
		const size_t limbWidth = 8 * sizeof(Limb);
		//const Limb mask = ~0;
		const size_t numBits = n % limbWidth;
		if (numBits > 0) {
			r.GrowTo(this->limbs.size());
			Limb carry = 0;
			for (size_t i = 0; i < this->limbs.size(); i++) {
				Limb newLimb = (this->Get(i) << numBits) | carry;
				carry = this->Get(i) >> (limbWidth - numBits);
				r.Set(newLimb, i);
			}
			r.Set(carry, this->limbs.size());
			r = r.LimbShiftLeft(n / 8);
		} else {
			r = this->LimbShiftLeft(n / 8);
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