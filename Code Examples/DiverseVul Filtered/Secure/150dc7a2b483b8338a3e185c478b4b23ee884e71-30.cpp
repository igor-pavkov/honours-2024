static double LOG2D(int a)
{
	if (a < 0)
		return 1.0 / (1UL << -a);
	return 1UL << a;
}