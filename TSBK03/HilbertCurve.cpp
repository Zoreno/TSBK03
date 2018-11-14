#include "HilbertCurve.h"

void hilbert_rot(int n, int *x, int *y, int rx, int ry)
{
	if (ry == 0)
	{
		if (rx == 1)
		{
			*x = n - 1 - *x;
			*y = n - 1 - *y;
		}

		int t = *x;
		*x = *y;
		*y = t;
	}
}

int hilbert_xy2d(int n, int x, int y)
{
	int rx;
	int ry;
	int s;
	int d = 0;

	for (s = n / 2; s > 0; s /= 2)
	{
		rx = (x & s) > 0;
		ry = (y & s) > 0;
		d += s * s * ((3 * rx) ^ ry);
		hilbert_rot(s, &x, &y, rx, ry);
	}

	return d;
}

void hilbert_d2xy(int n, int d, int *x, int *y)
{
	int rx;
	int ry;
	int s;
	int t = d;

	*x = *y = 0;

	for (s = 1; s < n; s *= 2)
	{
		rx = 1 & (t / 2);
		ry = 1 & (t ^ rx);

		hilbert_rot(s, x, y, rx, ry);

		*x += s * rx;
		*y += s * ry;
		t /= 4;
	}
}