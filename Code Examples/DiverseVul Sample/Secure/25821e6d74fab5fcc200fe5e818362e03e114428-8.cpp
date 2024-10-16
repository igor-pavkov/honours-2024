static void normalized_boundaries(double d, diy_fp_t* out_m_minus, diy_fp_t* out_m_plus)
{
	diy_fp_t v = double2diy_fp(d);
	diy_fp_t pl, mi;
	int significand_is_zero = v.f == DP_HIDDEN_BIT;
	pl.f = (v.f << 1) + 1; pl.e = v.e - 1;
	pl = normalize_boundary(pl);
	if (significand_is_zero) {
		mi.f = (v.f << 2) - 1;
		mi.e = v.e - 2;
	} else {
		mi.f = (v.f << 1) - 1;
		mi.e = v.e - 1;
	}
	mi.f <<= mi.e - pl.e;
	mi.e = pl.e;
	*out_m_plus = pl;
	*out_m_minus = mi;
}