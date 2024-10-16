static struct clk_hw *hi3660_stub_clk_hw_get(struct of_phandle_args *clkspec,
					     void *data)
{
	unsigned int idx = clkspec->args[0];

	if (idx >= HI3660_CLK_STUB_NUM) {
		pr_err("%s: invalid index %u\n", __func__, idx);
		return ERR_PTR(-EINVAL);
	}

	return &hi3660_stub_clks[idx].hw;
}