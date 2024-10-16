static uint64_t double_to_uint64(double d)
{
	union { double d; uint64_t n; } tmp;
	tmp.d = d;
	return tmp.n;
}