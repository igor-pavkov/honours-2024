static int hi3660_stub_clk_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct resource *res;
	unsigned int i;
	int ret;

	/* Use mailbox client without blocking */
	stub_clk_chan.cl.dev = dev;
	stub_clk_chan.cl.tx_done = NULL;
	stub_clk_chan.cl.tx_block = false;
	stub_clk_chan.cl.knows_txdone = false;

	/* Allocate mailbox channel */
	stub_clk_chan.mbox = mbox_request_channel(&stub_clk_chan.cl, 0);
	if (IS_ERR(stub_clk_chan.mbox))
		return PTR_ERR(stub_clk_chan.mbox);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return -EINVAL;
	freq_reg = devm_ioremap(dev, res->start, resource_size(res));
	if (!freq_reg)
		return -ENOMEM;

	freq_reg += HI3660_STUB_CLOCK_DATA;

	for (i = 0; i < HI3660_CLK_STUB_NUM; i++) {
		ret = devm_clk_hw_register(&pdev->dev, &hi3660_stub_clks[i].hw);
		if (ret)
			return ret;
	}

	return devm_of_clk_add_hw_provider(&pdev->dev, hi3660_stub_clk_hw_get,
					   hi3660_stub_clks);
}