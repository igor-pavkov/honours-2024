static diy_fp_t multiply(diy_fp_t x, diy_fp_t y)
{
	uint64_t a,b,c,d,ac,bc,ad,bd,tmp;
	diy_fp_t r;
	uint64_t M32 = 0xFFFFFFFF;
	a = x.f >> 32; b = x.f & M32;
	c = y.f >> 32; d = y.f & M32;
	ac = a*c; bc = b*c; ad = a*d; bd = b*d;
	tmp = (bd>>32) + (ad&M32) + (bc&M32);
	tmp += 1U << 31;
	r.f = ac+(ad>>32)+(bc>>32)+(tmp >>32);
	r.e = x.e + y.e + 64;
	return r;
}