R_API double my_pow(ut64 base, int exp) {
	ut8 flag = 0;
	ut64 res = 1;
	if (exp < 0) {
		flag = 1;
		exp *= -1;
	}
	while (exp) {
		if (exp & 1) {
			res *= base;
		}
		exp >>= 1;
		base *= base;
		IFDBG eprintf("Result: %"PFMT64d ", base: %"PFMT64d ", exp: %d\n", res, base, exp);
	}
	if (flag == 0) {
		return 1.0 * res;
	}
	return (1.0 / res);
}