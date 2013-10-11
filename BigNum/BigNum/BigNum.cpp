// BigNum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BigNumCpu.h"
#include "BigInt.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using std::cout; using std::endl; using std::cin;

	BigInt a, b, c, d;

	//a.FillValue(4, 0xffffffff);
	//b.FillValue(3, 0xffffffff);
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

	srand(time(0));

	double timeToTest = 0.5;
	//int minToomSize = 2;
	while (true) {
		const size_t bigIntSize = 100;
		//cout << "How large should the integers be?";
		//cin >> bigIntSize;

		BigInt a, b, c, d;

		int reps;
		double repsPerSec, secs;
		a.FillRandom(bigIntSize);
		b.FillRandom(bigIntSize);

		Stopwatch s;
		for (reps = 0; s.Secs() < timeToTest; reps++) {
			if (reps % 100 == 0) {
				a.FillRandom(bigIntSize);
				b.FillRandom(bigIntSize);
			}
			//a.minToomSize = minToomSize;
			//b.minToomSize = minToomSize;

			c = a.TimesResult(b);
			//c = a.Toom2(b);
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

