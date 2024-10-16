static unsigned long hi3660_stub_clk_recalc_rate(struct clk_hw *hw,
						 unsigned long parent_rate)
{
	struct hi3660_stub_clk *stub_clk = to_stub_clk(hw);

	/*
	 * LPM3 writes back the CPU frequency in shared SRAM so read
	 * back the frequency.
	 */
	stub_clk->rate = readl(freq_reg + (stub_clk->id << 2)) * MHZ;
	return stub_clk->rate;
}