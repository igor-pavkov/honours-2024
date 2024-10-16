static int mb86a20s_set_frontend(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int rc, if_freq;
	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (!c->isdbt_layer_enabled)
		c->isdbt_layer_enabled = 7;

	if (c->isdbt_layer_enabled == 1)
		state->bw = MB86A20S_1SEG;
	else if (c->isdbt_partial_reception)
		state->bw = MB86A20S_13SEG_PARTIAL;
	else
		state->bw = MB86A20S_13SEG;

	if (c->inversion == INVERSION_ON)
		state->inversion = true;
	else
		state->inversion = false;

	if (!c->isdbt_sb_mode) {
		state->subchannel = 0;
	} else {
		if (c->isdbt_sb_subchannel >= ARRAY_SIZE(mb86a20s_subchannel))
			c->isdbt_sb_subchannel = 0;

		state->subchannel = mb86a20s_subchannel[c->isdbt_sb_subchannel];
	}

	/*
	 * Gate should already be opened, but it doesn't hurt to
	 * double-check
	 */
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);
	fe->ops.tuner_ops.set_params(fe);

	if (fe->ops.tuner_ops.get_if_frequency)
		fe->ops.tuner_ops.get_if_frequency(fe, &if_freq);

	/*
	 * Make it more reliable: if, for some reason, the initial
	 * device initialization doesn't happen, initialize it when
	 * a SBTVD parameters are adjusted.
	 *
	 * Unfortunately, due to a hard to track bug at tda829x/tda18271,
	 * the agc callback logic is not called during DVB attach time,
	 * causing mb86a20s to not be initialized with Kworld SBTVD.
	 * So, this hack is needed, in order to make Kworld SBTVD to work.
	 *
	 * It is also needed to change the IF after the initial init.
	 *
	 * HACK: Always init the frontend when set_frontend is called:
	 * it was noticed that, on some devices, it fails to lock on a
	 * different channel. So, it is better to reset everything, even
	 * wasting some time, than to loose channel lock.
	 */
	mb86a20s_initfe(fe);

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 0);

	rc = mb86a20s_writeregdata(state, mb86a20s_reset_reception);
	mb86a20s_reset_counters(fe);
	mb86a20s_stats_not_ready(fe);

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);

	return rc;
}