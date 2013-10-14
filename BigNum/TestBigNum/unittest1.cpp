#include "stdafx.h"
#include "CppUnitTest.h"
#include "Deque.h"
#include "BigInt.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{
	template<> static std::wstring ToString<BigInt> (const BigInt& t) { RETURN_WIDE_STRING(t.String().c_str()); }
} } }

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
	//template<> static std::wstring ToString<BigInt> (const BigInt* t) { RETURN_WIDE_STRING(t.String().c_str()); }
	//template<> static std::wstring ToString<BigInt> (const BigInt* t) { RETURN_WIDE_STRING(t.String().c_str()); }

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
			for (unsigned i = 0; i < testDeque.Size(); i++) {
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

		TEST_METHOD(TestBigIntAdd)
		{
			BigInt a, b, c;
			// test normal add
			a.Set(13U, 1);
			b.Set(32U, 1);
			c = a.Plus(b);
			Assert::AreEqual(45U, c.Get(1));

			// test carry add
			b.Set(-1, 1);
			c = a.Plus(b);
			Assert::AreEqual(3U, c.Size());
			Assert::AreEqual(1U, c.Get(2));
			Assert::AreEqual(12U, c.Get(1));

			// make sure that a, b, and c are uncoupled
			a.Set(43, 0);
			b.Set(22, 0);
			c.Set(246, 0);
			Assert::AreNotEqual(a.Get(0), b.Get(0));
			Assert::AreNotEqual(a.Get(0), c.Get(0));
			Assert::AreNotEqual(b.Get(0), c.Get(0));

			a.FillValue(2, 0xFFFFFFFF);
			b.FillValue(2, 0xFFFFFFFF);
			c = a.Plus(b);
			Assert::AreEqual(3U, c.Size());
			Assert::AreEqual(0xFFFFFFFEU, c.Get(0));
			Assert::AreEqual(0x1U, c.Get(2));
		}

		TEST_METHOD(TestBigIntSet)
		{
			BigInt a;
			a.Set(13, 1);
			Assert::AreEqual(13U, a.Get(1));
			Assert::AreEqual(2U, a.Size());
		}

		TEST_METHOD(TestBigIntMultiply)
		{
			BigInt a, b, c, d, e;
			a.Set(17, 0);
			c = a.ScaledBy(19);
			Assert::AreEqual(17U * 19U, c.Get(0));
			
			// test simple multiply
			a.Set(17, 0);
			b.Set(19, 0);
			c = a.Times(b);
			d = a.TimesResult(b);
			e = a.Toom2(b);
			Assert::AreEqual(17U * 19U, c.Get(0));
			Assert::AreEqual(c, d);
			Assert::AreEqual(c, e);

			// test multiply carry
			a.SetString("FFFFFFFF");
			b.SetString("FFFFFFFF");
			c = a.Times(b);
			d = a.TimesResult(b);
			e = a.Toom2(b);
			Assert::AreEqual(2U, c.Size());
			Assert::AreEqual(0xFFFFFFFEU, c.Get(1));
			Assert::AreEqual(0x1U, c.Get(0));
			Assert::AreEqual(c, d);
			Assert::AreEqual(c, e);

			Assert::AreEqual(std::string("FFFFFFFE 00000001"), c.String());

			a.SetString("FFFFFFFF FFFFFFFF");
			b.SetString("FFFFFFFF FFFFFFFF");
			c = a.Times(b);
			d = a.TimesResult(b);
			e = a.Toom2(b);
			Assert::AreEqual(std::string("FFFFFFFF FFFFFFFE 00000000 00000001"), c.String());
			Assert::AreEqual(c, d);
			Assert::AreEqual(c, e);

			a = BigInt();
			b = BigInt();
			c = a.Times(b);
			d = a.TimesResult(b);
			e = a.Toom2(b);

			// write > tests
			// a_s > b_s
			a.SetString("FFFFFFFF FFFFFFFF");
			b.SetString("FFFFFFFF");
			Assert::IsTrue(a > b);

			// a_s < b_s
			a.SetString("FFFFFFFF");
			b.SetString("FFFFFFFF FFFFFFFF");
			Assert::IsFalse(a > b);

			// a_s == b_s && a < b
			a.SetString("FFFFFFF1");
			b.SetString("FFFFFFFF");
			Assert::IsFalse(a > b);

			// a_s == b_s && a > b
			a.SetString("FFFFFFFF");
			b.SetString("FFFFFFF1");
			Assert::IsTrue(a > b);

			// a == b
			b.SetString("FFFFFFFF");
			Assert::IsFalse(a > b);

			// write toom tests
			for (int x = 1; x < 10; x++) {
				for (int y = 1; y < 10; y++) {
					a.FillValue(x, 0xFFFFFFFF);
					b.FillValue(y, 0xFFFFFFFF);
					c = a.Times(b);
					d = a.TimesResult(b);
					
					Assert::AreEqual(c, d);
				}
			}
		}

	};
}