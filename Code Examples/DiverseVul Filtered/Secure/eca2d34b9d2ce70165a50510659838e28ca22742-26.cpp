static int mb86a20s_initfe(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	u64 pll;
	u32 fclk;
	int rc;
	u8  regD5 = 1, reg71, reg09 = 0x3a;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 0);

	/* Initialize the frontend */
	rc = mb86a20s_writeregdata(state, mb86a20s_init1);
	if (rc < 0)
		goto err;

	if (!state->inversion)
		reg09 |= 0x04;
	rc = mb86a20s_writereg(state, 0x09, reg09);
	if (rc < 0)
		goto err;
	if (!state->bw)
		reg71 = 1;
	else
		reg71 = 0;
	rc = mb86a20s_writereg(state, 0x39, reg71);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x71, state->bw);
	if (rc < 0)
		goto err;
	if (state->subchannel) {
		rc = mb86a20s_writereg(state, 0x44, state->subchannel);
		if (rc < 0)
			goto err;
	}

	fclk = state->config->fclk;
	if (!fclk)
		fclk = 32571428;

	/* Adjust IF frequency to match tuner */
	if (fe->ops.tuner_ops.get_if_frequency)
		fe->ops.tuner_ops.get_if_frequency(fe, &state->if_freq);

	if (!state->if_freq)
		state->if_freq = 3300000;

	pll = (((u64)1) << 34) * state->if_freq;
	do_div(pll, 63 * fclk);
	pll = (1 << 25) - pll;
	rc = mb86a20s_writereg(state, 0x28, 0x2a);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x29, (pll >> 16) & 0xff);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x2a, (pll >> 8) & 0xff);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x2b, pll & 0xff);
	if (rc < 0)
		goto err;
	dev_dbg(&state->i2c->dev, "%s: fclk=%d, IF=%d, clock reg=0x%06llx\n",
		__func__, fclk, state->if_freq, (long long)pll);

	/* pll = freq[Hz] * 2^24/10^6 / 16.285714286 */
	pll = state->if_freq * 1677721600L;
	do_div(pll, 1628571429L);
	rc = mb86a20s_writereg(state, 0x28, 0x20);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x29, (pll >> 16) & 0xff);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x2a, (pll >> 8) & 0xff);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x2b, pll & 0xff);
	if (rc < 0)
		goto err;
	dev_dbg(&state->i2c->dev, "%s: IF=%d, IF reg=0x%06llx\n",
		__func__, state->if_freq, (long long)pll);

	if (!state->config->is_serial)
		regD5 &= ~1;

	rc = mb86a20s_writereg(state, 0x50, 0xd5);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x51, regD5);
	if (rc < 0)
		goto err;

	rc = mb86a20s_writeregdata(state, mb86a20s_init2);
	if (rc < 0)
		goto err;


err:
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);

	if (rc < 0) {
		state->need_init = true;
		dev_info(&state->i2c->dev,
			 "mb86a20s: Init failed. Will try again later\n");
	} else {
		state->need_init = false;
		dev_dbg(&state->i2c->dev, "Initialization succeeded.\n");
	}
	return rc;
}