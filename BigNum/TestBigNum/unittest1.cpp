#include "stdafx.h"
#include "CppUnitTest.h"
#include "Deque.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestBigNum
{		
	class NoExceptionException : public std::runtime_error {
	public:
		NoExceptionException(std::string expectedExceptionName, std::string executing) :
			std::runtime_error(expectedExceptionName + " expected and not received. Executing " + executing + ".") { }
	};

	class NoExceptionDequeIndex : public NoExceptionException {
	public:
		NoExceptionDequeIndex(std::string executing) :
			NoExceptionException("DequeIndexOutOfBounds", executing) { }
	};

	//class WrongExceptionException : public std::runtime_error {
	//public:
	//	WrongExceptionException(std::string expectedExceptionName, std::string executing) :
	//		std::runtime_error(expectedExceptionName + " expected, but different one received. Executing " + executing + ".") { }
	//};

	TEST_CLASS(TestDeque)
	{
	public:
		TEST_METHOD(TestDequeFill)
		{
			Deque<int> testDeque;
			try {
				try {
					testDeque.Fill(4, 0, 7);
					throw NoExceptionDequeIndex("testDeque.Fill(4, 0, 7);");
				} catch (DequeIndexOutOfBounds) { }

				try {
					testDeque.Fill(-3, -2, 7);
					throw NoExceptionDequeIndex("testDeque.Fill(-3, -2, 7);");
				} catch (DequeIndexOutOfBounds) { }

				try {
					testDeque.Fill(2, 5, 7);
					throw NoExceptionDequeIndex("testDeque.Fill(-3, -2, 7);");
				} catch (DequeIndexOutOfBounds) { }

			} catch (NoExceptionException &e) {
				std::string errorMsg(e.what());
				std::wstring werrorMsg(errorMsg.begin(), errorMsg.end());
				Assert::Fail(werrorMsg.c_str());
			} catch (std::exception &e) {
				Assert::Fail(L"Wrong exception.");
			}

			// Handles case of multi-size Fill
			int sz_expected = 50;
			testDeque.Fill(0, sz_expected, 1);
			Assert::AreEqual((size_t)sz_expected, testDeque.Size(), L"Size wrong on Fill(0, 50, 1)");
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				Assert::AreEqual(1, testDeque.Get(i));
			}

			sz_expected += 20;
			testDeque.Fill(-20, 5, 2);
			Assert::AreEqual((size_t)sz_expected, testDeque.Size(), L"Size wrong on Fill(-20, 5, 2)");
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				if (i < 25) {
					Assert::AreEqual(2, testDeque.Get(i));
				} else {
					Assert::AreNotEqual(2, testDeque.Get(i));
				}
			}

			sz_expected += 20;
			testDeque.Fill(testDeque.Size() - 5, testDeque.Size() + 20, 3);
			Assert::AreEqual((size_t)sz_expected, testDeque.Size(), L"Size wrong on Fill(-20, 5, 2)");
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				if (i >= testDeque.Size() - 25) {
					Assert::AreEqual(3, testDeque.Get(i));
				} else {
					Assert::AreNotEqual(3, testDeque.Get(i));
				}
			}
		}

		TEST_METHOD(TestDequeGeneral)
		{
			Deque<int> testDeque(NULL);
			const int pushBackIterations = 32;
			for (int i = 0; i < pushBackIterations; i++) {
				testDeque.PushBack(i);
			}
			int sz_expected = pushBackIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				Assert::AreEqual(i, testDeque.Get(i));
			}

			const int popBackIterations = 5;
			for (int i = 0; i < popBackIterations; i++) {
				Assert::AreEqual(true, testDeque.PopBack());
			}
			sz_expected -= popBackIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				Assert::AreEqual(i, testDeque.Get(i));
			}

			const int pushFrontIterations = 10;
			for (int i = 0; i < pushFrontIterations; i++) {
				testDeque.PushFront(i * -1 - 1);
			}
			sz_expected += pushFrontIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				int offset = -pushFrontIterations;
				Assert::AreEqual(i + offset, testDeque.Get(i));
			}

			const int popFrontIterations = 5;
			for (int i = 0; i < popFrontIterations; i++) {
				Assert::AreEqual(true, testDeque.PopFront());
			}
			sz_expected -= popFrontIterations;
			Assert::AreEqual(sz_expected, (int)testDeque.Size());
			for (int i = 0; i < (int)testDeque.Size(); i++) {
				int offset = popFrontIterations - pushFrontIterations;
				Assert::AreEqual(i + offset, testDeque.Get(i));
			}
		}

	};
}