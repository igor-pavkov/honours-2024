static int mb86a20s_get_blk_error(struct dvb_frontend *fe,
			    unsigned layer,
			    u32 *error, u32 *count)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	int rc, val;
	u32 collect_rate;
	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (layer >= NUM_LAYERS)
		return -EINVAL;

	/* Check if the PER measures are already available */
	rc = mb86a20s_writereg(state, 0x50, 0xb8);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;

	/* Check if data is available for that layer */

	if (!(rc & (1 << layer))) {
		dev_dbg(&state->i2c->dev,
			"%s: block counts for layer %c aren't available yet.\n",
			__func__, 'A' + layer);
		return -EBUSY;
	}

	/* Read Packet error Count */
	rc = mb86a20s_writereg(state, 0x50, 0xb9 + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*error = rc << 8;
	rc = mb86a20s_writereg(state, 0x50, 0xba + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*error |= rc;
	dev_dbg(&state->i2c->dev, "%s: block error for layer %c: %d.\n",
		__func__, 'A' + layer, *error);

	/* Read Bit Count */
	rc = mb86a20s_writereg(state, 0x50, 0xb2 + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*count = rc << 8;
	rc = mb86a20s_writereg(state, 0x50, 0xb3 + layer * 2);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*count |= rc;

	dev_dbg(&state->i2c->dev,
		"%s: block count for layer %c: %d.\n",
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

	if (collect_rate != *count) {
		dev_dbg(&state->i2c->dev,
			"%s: updating PER counter on layer %c to %d.\n",
			__func__, 'A' + layer, collect_rate);

		/* Stop PER measurement */
		rc = mb86a20s_writereg(state, 0x50, 0xb0);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, 0x00);
		if (rc < 0)
			return rc;

		/* Update this layer's counter */
		rc = mb86a20s_writereg(state, 0x50, 0xb2 + layer * 2);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, collect_rate >> 8);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x50, 0xb3 + layer * 2);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, collect_rate & 0xff);
		if (rc < 0)
			return rc;

		/* start PER measurement */
		rc = mb86a20s_writereg(state, 0x50, 0xb0);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, 0x07);
		if (rc < 0)
			return rc;

		/* Reset all counters to collect new data */
		rc = mb86a20s_writereg(state, 0x50, 0xb1);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, 0x07);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51, 0x00);

		return rc;
	}

reset_measurement:
	/* Reset counter to collect new data */
	rc = mb86a20s_writereg(state, 0x50, 0xb1);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	val = rc;
	rc = mb86a20s_writereg(state, 0x51, val | (1 << layer));
	if (rc < 0)
		return rc;
	rc = mb86a20s_writereg(state, 0x51, val & ~(1 << layer));

	return rc;
}