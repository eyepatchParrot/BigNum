#include "stdafx.h"
#include "CppUnitTest.h"
#include "Deque.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestBigNum
{		
	TEST_CLASS(TestDeque)
	{
	public:
		
		TEST_METHOD(TestDequePushBack)
		{
			Deque<int, NULL> testDeque;
			const int pushBackIterations = 32;
			for (int i = 0; i < pushBackIterations; i++) {
				testDeque.PushBack(i);
			}
			int sz_expected = pushBackIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < testDeque.Size(); i++) {
				Assert::AreEqual(i, testDeque.Get(i));
			}

			const int popBackIterations = 5;
			for (int i = 0; i < popBackIterations; i++) {
				Assert::AreEqual(true, testDeque.PopBack());
			}
			sz_expected -= popBackIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < testDeque.Size(); i++) {
				Assert::AreEqual(i, testDeque.Get(i));
			}

			const int pushFrontIterations = 10;
			for (int i = 0; i < pushFrontIterations; i++) {
				testDeque.PushFront(i * -1 - 1);
			}
			sz_expected += pushFrontIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < testDeque.Size(); i++) {
				Assert::AreEqual(i - pushFrontIterations, testDeque.Get(i));
			}

			const int popFrontIterations = 5;
			for (int i = 0; i < popFrontIterations; i++) {
				Assert::AreEqual(true, testDeque.PopFront());
			}
			sz_expected -= popFrontIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < testDeque.Size(); i++) {
				Assert::AreEqual(i - pushFrontIterations + popFrontIterations, testDeque.Get(i));
			}
		}

	};
}