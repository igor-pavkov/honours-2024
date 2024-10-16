R_API double r_bin_java_raw_to_double(const ut8 *raw, ut64 offset) {
	ut64 bits = R_BIN_JAVA_LONG (raw, offset);
	int s = ((bits >> 63) == 0) ? 1 : -1;
	int e = (int) ((bits >> 52) & 0x7ffL);
	long m = (e == 0) ?
	(bits & 0xfffffffffffffLL) << 1 :
	(bits & 0xfffffffffffffLL) | 0x10000000000000LL;
	double res = 0.0;
	IFDBG eprintf("Convert Long to Double: %08"PFMT64x "\n", bits);
	if (bits == 0x7ff0000000000000LL) {
		return INFINITY;
	}
	if (bits == 0xfff0000000000000LL) {
		return -INFINITY;
	}
	if (0x7ff0000000000001LL <= bits && bits <= 0x7fffffffffffffffLL) {
		return NAN;
	}
	if (0xfff0000000000001LL <= bits && bits <= 0xffffffffffffffffLL) {
		return NAN;
	}
	res = s * m * my_pow (2, e - 1075);// XXXX TODO Get double to work correctly here
	IFDBG eprintf("	High-bytes = %02x %02x %02x %02x\n", raw[0], raw[1], raw[2], raw[3]);
	IFDBG eprintf("	Low-bytes = %02x %02x %02x %02x\n", raw[4], raw[5], raw[6], raw[7]);
	IFDBG eprintf("Convert Long to Double s: %d, m: 0x%08lx, e: 0x%08x, res: %f\n", s, m, e, res);
	return res;
}