static int mb86a20s_get_pre_ber(struct dvb_frontend *fe,
				unsigned layer,
				u32 *error, u32 *count)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	int rc, val;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (layer >= NUM_LAYERS)
		return -EINVAL;

	/* Check if the BER measures are already available */
	rc = mb86a20s_readreg(state, 0x54);
	if (rc < 0)
		return rc;

	/* Check if data is available for that layer */
	if (!(rc & (1 << layer))) {
		dev_dbg(&state->i2c->dev,
			"%s: preBER for layer %c is not available yet.\n",
			__func__, 'A' + layer);
		return -EBUSY;
	}

	/* Read Bit Error Count */
	rc = mb86a20s_readreg(state, 0x55 + layer * 3);
	if (rc < 0)
		return rc;
	*error = rc << 16;
	rc = mb86a20s_readreg(state, 0x56 + layer * 3);
	if (rc < 0)
		return rc;
	*error |= rc << 8;
	rc = mb86a20s_readreg(state, 0x57 + layer * 3);
	if (rc < 0)
		return rc;
	*error |= rc;

	dev_dbg(&state->i2c->dev,
		"%s: bit error before Viterbi for layer %c: %d.\n",
		__func__, 'A' + layer, *error);

	/* Read Bit Count */
	rc = mb86a20s_writereg(state, 0x50, 0xa7 + layer * 3);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*count = rc << 16;
	rc = mb86a20s_writereg(state, 0x50, 0xa8 + layer * 3);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*count |= rc << 8;
	rc = mb86a20s_writereg(state, 0x50, 0xa9 + layer * 3);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	*count |= rc;

	dev_dbg(&state->i2c->dev,
		"%s: bit count before Viterbi for layer %c: %d.\n",
		__func__, 'A' + layer, *count);


	/*
	 * As we get TMCC data from the frontend, we can better estimate the
	 * BER bit counters, in order to do the BER measure during a longer
	 * time. Use those data, if available, to update the bit count
	 * measure.
	 */

	if (state->estimated_rate[layer]
	    && state->estimated_rate[layer] != *count) {
		dev_dbg(&state->i2c->dev,
			"%s: updating layer %c preBER counter to %d.\n",
			__func__, 'A' + layer, state->estimated_rate[layer]);

		/* Turn off BER before Viterbi */
		rc = mb86a20s_writereg(state, 0x52, 0x00);

		/* Update counter for this layer */
		rc = mb86a20s_writereg(state, 0x50, 0xa7 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51,
				       state->estimated_rate[layer] >> 16);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x50, 0xa8 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51,
				       state->estimated_rate[layer] >> 8);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x50, 0xa9 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x51,
				       state->estimated_rate[layer]);
		if (rc < 0)
			return rc;

		/* Turn on BER before Viterbi */
		rc = mb86a20s_writereg(state, 0x52, 0x01);

		/* Reset all preBER counters */
		rc = mb86a20s_writereg(state, 0x53, 0x00);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x53, 0x07);
	} else {
		/* Reset counter to collect new data */
		rc = mb86a20s_readreg(state, 0x53);
		if (rc < 0)
			return rc;
		val = rc;
		rc = mb86a20s_writereg(state, 0x53, val & ~(1 << layer));
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x53, val | (1 << layer));
	}

	return rc;
}