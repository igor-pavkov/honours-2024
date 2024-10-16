static int mb86a20s_reset_counters(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int rc, val;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	/* Reset the counters, if the channel changed */
	if (state->last_frequency != c->frequency) {
		memset(&c->cnr, 0, sizeof(c->cnr));
		memset(&c->pre_bit_error, 0, sizeof(c->pre_bit_error));
		memset(&c->pre_bit_count, 0, sizeof(c->pre_bit_count));
		memset(&c->post_bit_error, 0, sizeof(c->post_bit_error));
		memset(&c->post_bit_count, 0, sizeof(c->post_bit_count));
		memset(&c->block_error, 0, sizeof(c->block_error));
		memset(&c->block_count, 0, sizeof(c->block_count));

		state->last_frequency = c->frequency;
	}

	/* Clear status for most stats */

	/* BER/PER counter reset */
	rc = mb86a20s_writeregdata(state, mb86a20s_per_ber_reset);
	if (rc < 0)
		goto err;

	/* CNR counter reset */
	rc = mb86a20s_readreg(state, 0x45);
	if (rc < 0)
		goto err;
	val = rc;
	rc = mb86a20s_writereg(state, 0x45, val | 0x10);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x45, val & 0x6f);
	if (rc < 0)
		goto err;

	/* MER counter reset */
	rc = mb86a20s_writereg(state, 0x50, 0x50);
	if (rc < 0)
		goto err;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		goto err;
	val = rc;
	rc = mb86a20s_writereg(state, 0x51, val | 0x01);
	if (rc < 0)
		goto err;
	rc = mb86a20s_writereg(state, 0x51, val & 0x06);
	if (rc < 0)
		goto err;

	goto ok;
err:
	dev_err(&state->i2c->dev,
		"%s: Can't reset FE statistics (error %d).\n",
		__func__, rc);
ok:
	return rc;
}