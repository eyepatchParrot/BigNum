#pragma once
#include "stdafx.h"

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
			for (size_t i = 0; i < buffer.size(); i++) {
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
		if (max <= min) {
			throw "Out of bounds. max <= min.";
		}

		int numNegElements = min * -1;
		numNegElements = numNegElements > 0 ? numNegElements : 0;
		int numPosElements = max - sz_deque;
		numPosElements = numPosElements > 0 ? numPosElements : 0;
		int numNewElements = numNegElements + numPosElements;
		Grow(numNewElements);

		max += (min < 0 ? min * -1 : 0);
		min = 0;

		for (int i = 0; i < max; i++) {
			Set(i, value);
		}
	}

	DequeType Get(int ix_deque)
	{
		if (IdxIsValid(ix_deque)) {
			return buffer[GetBufferIdx(ix_deque)];
		} else {
			throw "Out of bounds.";
		}
	}

	bool Set(size_t ix_deque, DequeType val)
	{
		if (ix_deque >= sz_deque) {
			return false;
		} else {
			buffer[GetBufferIdx(ix_deque)] = val;

			return true;
		}
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
			throw "No string function provided.";
		}

		for (size_t i = 0; i < sz_deque; i++) {
			retVal = retVal + (*DequeTypeToString)(Get(i)) + " ";
		}

		return retVal;
	}
};

