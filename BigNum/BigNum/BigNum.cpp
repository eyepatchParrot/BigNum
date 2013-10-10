// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"
#include "BigInt.h"

void setRandomBigInt(BigInt &n, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		n.Set(rand(), i);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl; using std::cin;

	BigInt a, b, c, d;

	a.SetString("FFFFFFFF");
	b.SetString("FFFFFFFF");
	c = a.Times(b);
//	d = a.TimesResult(b);

	cout << c.String() << endl;

	srand(time(0));

	double timeToTest = 0.5;
	while (true) {
		size_t bigIntSize = 100;
		//cout << "How large should the integers be?";
		//cin >> bigIntSize;

		BigInt a, b, c;
		a.SetString("FFFFFFFF");
		b.SetString("FFFFFFFF");
		c = a.Times(b);

		int reps;
		double repsPerSec, secs;

		setRandomBigInt(a, bigIntSize);
		setRandomBigInt(b, bigIntSize);
		Stopwatch s;
		for (reps = 0; s.Secs() < timeToTest; reps++) {
			c = a.TimesResult(b);
		}
		secs = s.Secs();
		repsPerSec = (double)reps / secs;
		cout << "times : " << reps << "\tsecs : " << secs << "\treps / secs : " << repsPerSec << endl;

		//s = Stopwatch();
		//for (reps = 0; s.Secs() < timeToTest; reps++) {
		//	c = a.TimesResult(b);
		//}
		//secs = s.Secs();
		//repsPerSec = (double)reps / secs;
		//cout << "timesR : " << reps << "\tsecs : " << secs << "\treps / secs : " << repsPerSec << endl;
		timeToTest *= 2.0;
	}

	cout << "We're done here." << endl;
	//cin.get();

	return 0;
}

