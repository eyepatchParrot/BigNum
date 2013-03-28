#pragma once
#include "stdafx.h"

template<class DequeType, std::string (*DequeTypeToString)(DequeType)>
class Deque
{
	std::vector<DequeType> buffer;
	size_t sz_deque;
	size_t ix_start;

	bool IdxIsValid(int ix_buffer)
	{
		return ix_buffer >= 0 && ix_buffer < (int)buffer.size();
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

	void Grow()
	{
		if (IsFull()) {
			// Grow the buffer
			size_t newBufferSize = buffer.size() ? buffer.size() * 2 : 16;
			std::vector<DequeType> newBuffer(newBufferSize);
			for (size_t i = 0; i < buffer.size(); i++) {
				newBuffer[i] = Get(i);
			}
			buffer = newBuffer;
			ix_start = 0;
		}
		sz_deque++;
	}

public:
	Deque(void)
	{
		sz_deque = 0;
		ix_start = 0;
	}
	~Deque(void) { }

	DequeType Get(int ix_deque)
	{
		int ix_buffer = GetBufferIdx(ix_deque);
		if (IdxIsValid(ix_buffer)) {
			return buffer[ix_buffer];
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

	bool IsFull()
	{
		return sz_deque >= buffer.size();
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

		for (size_t i = 0; i < sz_deque; i++) {
			retVal = retVal + (*DequeTypeToString)(Get(i)) + " ";
		}

		return retVal;
	}
};

