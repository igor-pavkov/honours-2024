static NOINLINE double my_SQRT(double X)
{
	union {
		float   f;
		int32_t i;
	} v;
	double invsqrt;
	double Xhalf = X * 0.5;

	/* Fast and good approximation to 1/sqrt(X), black magic */
	v.f = X;
	/*v.i = 0x5f3759df - (v.i >> 1);*/
	v.i = 0x5f375a86 - (v.i >> 1); /* - this constant is slightly better */
	invsqrt = v.f; /* better than 0.2% accuracy */

	/* Refining it using Newton's method: x1 = x0 - f(x0)/f'(x0)
	 * f(x) = 1/(x*x) - X  (f==0 when x = 1/sqrt(X))
	 * f'(x) = -2/(x*x*x)
	 * f(x)/f'(x) = (X - 1/(x*x)) / (2/(x*x*x)) = X*x*x*x/2 - x/2
	 * x1 = x0 - (X*x0*x0*x0/2 - x0/2) = 1.5*x0 - X*x0*x0*x0/2 = x0*(1.5 - (X/2)*x0*x0)
	 */
	invsqrt = invsqrt * (1.5 - Xhalf * invsqrt * invsqrt); /* ~0.05% accuracy */
	/* invsqrt = invsqrt * (1.5 - Xhalf * invsqrt * invsqrt); 2nd iter: ~0.0001% accuracy */
	/* With 4 iterations, more than half results will be exact,
	 * at 6th iterations result stabilizes with about 72% results exact.
	 * We are well satisfied with 0.05% accuracy.
	 */

	return X * invsqrt; /* X * 1/sqrt(X) ~= sqrt(X) */
}