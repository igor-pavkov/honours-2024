static diy_fp_t minus(diy_fp_t x, diy_fp_t y)
{
	diy_fp_t r;
	assert(x.e == y.e);
	assert(x.f >= y.f);
	r.f = x.f - y.f;
	r.e = x.e;
	return r;
}