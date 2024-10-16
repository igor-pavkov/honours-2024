static long hi3660_stub_clk_round_rate(struct clk_hw *hw, unsigned long rate,
				       unsigned long *prate)
{
	/*
	 * LPM3 handles rate rounding so just return whatever
	 * rate is requested.
	 */
	return rate;
}