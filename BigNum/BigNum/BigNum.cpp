// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"
#include "BigInt.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl; using std::cin;

	//BigInt a, b, c, d;

	//a.FillValue(2, 0xffffffff);
	//b.FillValue(2, 0xffffffff);
	//c = a.Times(b);
	//d = a.Toom2(b);

	//cout << (c==d) << " c : " << c.String() << endl << "d : " << d.String() << endl;

	//for (int x = 1; x < 7; x++) {
	//	for (int y = 1; y < 7; y++) {
	//		a.FillValue(x, 0xFFFFFFFF);
	//		b.FillValue(y, 0xFFFFFFFF);
	//		c = a.Times(b);
	//		d = a.Toom2(b);

	//		if (!(c==d)) {
	//			cout << "x: " << x << " y: " << y << endl << "c : " << c.String() << endl << "d : " << d.String() << endl;
	//		}
	//	}
	//}

	double timeToTest = 1.0;
	int minToomSize = 512;
	while (true) {
		const size_t bigIntSize = 768 * 5;
		//cout << "How large should the integers be?";
		//cin >> bigIntSize;

		BigInt a, b, c, d;

		int reps;
		double repsPerSec, secs;
		a.FillRandom(bigIntSize);
		b.FillRandom(bigIntSize);

		Stopwatch s;

		srand(5);
		s = Stopwatch();
		for (reps = 0; s.Secs() < timeToTest; reps++) {
			if (reps % 100 == 0) {
				a.FillRandom(bigIntSize);
				b.FillRandom(bigIntSize);
			}
			a.minToomSize = b.minToomSize = minToomSize;

			c = a.Toom2(b);
			//c = a;
			//c.Add(a);
		}
		secs = s.Secs();
		repsPerSec = (double)reps / secs;
		cout << "toomSize : " << minToomSize << "\ttimes : " << reps << "\tsecs : " << secs << "\treps / secs : " << repsPerSec << endl;

//#define COMPARE
#ifdef COMPARE
		srand(5);
		s = Stopwatch();
		for (reps = 0; s.Secs() < timeToTest; reps++) {
			if (reps % 100 == 0) {
				a.FillRandom(bigIntSize);
				b.FillRandom(bigIntSize);
			}
			a.minToomSize = b.minToomSize = minToomSize;

			c = a;
			c.Add2(b);
		}
		secs = s.Secs();
		repsPerSec = (double)reps / secs;
		cout << "toomSize : " << minToomSize << "\ttoom2 : " << reps << "\tsecs : " << secs << "\treps / secs : " << repsPerSec << endl;
#endif

		cout << endl;

		timeToTest *= 2.0;
		//minToomSize *= 0.9;
	}

	cout << "We're done here." << endl;
	//cin.get();

	return 0;
}

