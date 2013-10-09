#pragma once
#include "stdafx.h"

class DequeIndexOutOfBounds : std::out_of_range
{
public:
	DequeIndexOutOfBounds() :
		std::out_of_range("Deque index is out of bounds.") { }
};

template<class DequeType>
class Deque
{
	std::string (*DequeTypeToString)(DequeType);
	std::vector<DequeType> buffer;
	size_t sz_deque;
	size_t ix_start;

	bool IdxIsValid(int ix_deque)
	{
		return ix_deque >= 0 && ix_deque < (int)sz_deque;
	}

	int GetBufferIdx(int ix_deque)
	{
		int ix_tmp = ix_deque + ix_start;
		if (ix_tmp >= (int)buffer.size()) {
			ix_tmp = ix_tmp - buffer.size();
		} else if (ix_tmp < 0) {
			ix_tmp = ix_tmp + buffer.size();
		}
		return ix_tmp;
	}

	void Grow(int numElements = 1)
	{
		if (IsFull(numElements - 1)) {
			size_t newBufferSize = buffer.size() != 0 ? buffer.size() * 2 : 16;
			while (buffer.size() + numElements > newBufferSize) {
				newBufferSize = newBufferSize * 2;
			}

			// Grow the buffer
			std::vector<DequeType> newBuffer(newBufferSize);
			for (size_t i = 0; i < sz_deque; i++) {
				newBuffer[i] = Get(i);
			}
			buffer = newBuffer;
			ix_start = 0;
		}
		sz_deque += numElements;
	}

public:
	Deque(std::string (*DequeTypeToStringArg)(DequeType) = NULL)
	{
		DequeTypeToString = DequeTypeToStringArg;
		sz_deque = 0;
		ix_start = 0;
	}
	~Deque(void) { }

	void Fill(int min, int max, DequeType value)
	{
		if (max <= min || max < 0 || min > (int)sz_deque) {
			throw DequeIndexOutOfBounds();
		}

		int numNegElements = min * -1;
		numNegElements = numNegElements > 0 ? numNegElements : 0;
		int numPosElements = max - sz_deque;
		numPosElements = numPosElements > 0 ? numPosElements : 0;
		int numNewElements = numNegElements + numPosElements;
		Grow(numNewElements);
		ix_start = ix_start - numNegElements;
		ix_start = ix_start < 0 ? ix_start + buffer.size() : ix_start;
		min += numNegElements;
		max += numNegElements;

		for (int i = min; i < max; i++) {
			Set(i, value);
		}
	}

	DequeType Get(int ix_deque)
	{
		if (IdxIsValid(ix_deque)) {
			return buffer[GetBufferIdx(ix_deque)];
		} else {
			throw DequeIndexOutOfBounds();
		}
	}

	void Set(size_t ix_deque, DequeType val)
	{
		if (ix_deque >= sz_deque) {
			throw std::out_of_range("ix_deque in Deque::Set out of range.");
		}

		buffer[GetBufferIdx(ix_deque)] = val;
	}

	DequeType PushBack(DequeType val)
	{
		Grow();

		buffer[GetBufferIdx(sz_deque - 1)] = val;

		return val;
	}

	DequeType PushFront(DequeType val)
	{
		Grow();
		ix_start = GetBufferIdx(-1);

		buffer[GetBufferIdx(0)] = val;

		return val;
	}

	bool PopBack()
	{
		if (!IsEmpty()) {
			sz_deque--;
			
			return true;
		} else {
			return false;
		}
	}

	bool PopFront()
	{
		if (!IsEmpty()) {
			sz_deque--;
			ix_start = GetBufferIdx(1);

			return true;
		} else {
			return false;
		}
	}

	bool IsFull(int withNewElements = 0)
	{
		return (sz_deque + withNewElements) >= buffer.size();
	}

	bool IsEmpty()
	{
		return sz_deque <= 0;
	}

	size_t Size()
	{
		return sz_deque;
	}

	std::string ToString()
	{
		std::string retVal;
		if (DequeTypeToString == NULL) {
			throw ToStringFuncExpected();
		}

		for (size_t i = 0; i < sz_deque; i++) {
			retVal = retVal + (*DequeTypeToString)(Get(i)) + " ";
		}

		return retVal;
	}
};

