static int hi3660_stub_clk_set_rate(struct clk_hw *hw, unsigned long rate,
				    unsigned long parent_rate)
{
	struct hi3660_stub_clk *stub_clk = to_stub_clk(hw);

	stub_clk->msg[0] = stub_clk->cmd;
	stub_clk->msg[1] = rate / MHZ;

	dev_dbg(stub_clk_chan.cl.dev, "set rate msg[0]=0x%x msg[1]=0x%x\n",
		stub_clk->msg[0], stub_clk->msg[1]);

	mbox_send_message(stub_clk_chan.mbox, stub_clk->msg);
	mbox_client_txdone(stub_clk_chan.mbox, 0);

	stub_clk->rate = rate;
	return 0;
}