static diy_fp_t normalize_boundary(diy_fp_t in)
{
	diy_fp_t res = in;
	/* Normalize now */
	/* the original number could have been a denormal. */
	while (! (res.f & (DP_HIDDEN_BIT << 1))) {
		res.f <<= 1;
		res.e--;
	}
	/* do the final shifts in one go. Don't forget the hidden bit (the '-1') */
	res.f <<= (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
	res.e = res.e - (DIY_SIGNIFICAND_SIZE - DP_SIGNIFICAND_SIZE - 2);
	return res;
}