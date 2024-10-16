static int mb86a20s_get_post_ber(struct dvb_frontend *fe,
				 unsigned layer,
				  u32 *error, u32 *count)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	u32 counter, collect_rate;
	int rc, val;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (layer >= NUM_LAYERS)
		return -EINVAL;

	/* Check if the BER measures are already available */
	rc = mb86a20s_readreg(state, 0x60);
	if (rc < 0)
		return rc;

	/* Check if data is available for that layer */
	if (!(rc & (1 << layer))) {
		dev_dbg(&state->i2c->dev,
			"%s: post BER for layer %c is not available yet.\n",
			__func__, 'A' + layer);
		return -EBUSY;
	}

	/* Read Bit Error Count */
	rc = mb86a20s_readreg(state, 0x64 + layer * 3);
	if (rc < 0)
		return rc;
	*error = rc << 16;
	rc = mb86a20s_readreg(state, 0x65 + layer * 3);
	if (rc < 0)
		return rc;
	*error |= rc << 8;
	rc = mb86a20s_readreg(state, 0x66 + layer * 3);
	if (rc < 0)
		return rc;
	*error |= rc;

	dev_dbg(&state->i2c->dev,
		"%s: post bit error for layer %c: %d.\n",
		__func__, 'A' + layer, *error);

	/* Read Bit Count */
	rc = mb86a20s_writereg(state, 0x50, 0xdc + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	counter = rc << 8;
	rc = mb86a20s_writereg(state, 0x50, 0xdd + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	counter |= rc;
	*count = counter * 204 * 8;

	dev_dbg(&state->i2c->dev,
		"%s: post bit count for layer %c: %d.\n",
		__func__, 'A' + layer, *count);

	/*
	 * As we get TMCC data from the frontend, we can better estimate the
	 * BER bit counters, in order to do the BER measure during a longer
	 * time. Use those data, if available, to update the bit count
	 * measure.
	 */

	if (!state->estimated_rate[layer])
		goto reset_measurement;

	collect_rate = state->estimated_rate[layer] / 204 / 8;
	if (collect_rate < 32)
		collect_rate = 32;
	if (collect_rate > 65535)
		collect_rate = 65535;
	if (collect_rate != counter) {
		dev_dbg(&state->i2c->dev,
			"%s: updating postBER counter on layer %c to %d.\n",
			__func__, 'A' + layer, collect_rate);

		/* Turn off BER after Viterbi */
		rc = mb86a20s_writereg(state, 0x5e, 0x00);

		/* Update counter for this layer */
		rc = mb86a20s_writereg(state, 0x50, 0xdc + layer * 2);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, collect_rate >> 8);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x50, 0xdd + layer * 2);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, collect_rate & 0xff);
		if (rc < 0)
			return rc;

		/* Turn on BER after Viterbi */
		rc = mb86a20s_writereg(state, 0x5e, 0x07);

		/* Reset all preBER counters */
		rc = mb86a20s_writereg(state, 0x5f, 0x00);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x5f, 0x07);

		return rc;
	}

reset_measurement:
	/* Reset counter to collect new data */
	rc = mb86a20s_readreg(state, 0x5f);
	if (rc < 0)
		return rc;
	val = rc;
	rc = mb86a20s_writereg(state, 0x5f, val & ~(1 << layer));
	if (rc < 0)
		return rc;
	rc = mb86a20s_writereg(state, 0x5f, val | (1 << layer));

	return rc;
}