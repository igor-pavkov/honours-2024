static int mb86a20s_get_main_CNR(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 cnr_linear, cnr;
	int rc, val;

	/* Check if CNR is available */
	rc = mb86a20s_readreg(state, 0x45);
	if (rc < 0)
		return rc;

	if (!(rc & 0x40)) {
		dev_dbg(&state->i2c->dev, "%s: CNR is not available yet.\n",
			 __func__);
		return -EBUSY;
	}
	val = rc;

	rc = mb86a20s_readreg(state, 0x46);
	if (rc < 0)
		return rc;
	cnr_linear = rc << 8;

	rc = mb86a20s_readreg(state, 0x46);
	if (rc < 0)
		return rc;
	cnr_linear |= rc;

	cnr = interpolate_value(cnr_linear,
				cnr_to_db_table, ARRAY_SIZE(cnr_to_db_table));

	c->cnr.stat[0].scale = FE_SCALE_DECIBEL;
	c->cnr.stat[0].svalue = cnr;

	dev_dbg(&state->i2c->dev, "%s: CNR is %d.%03d dB (%d)\n",
		__func__, cnr / 1000, cnr % 1000, cnr_linear);

	/* CNR counter reset */
	rc = mb86a20s_writereg(state, 0x45, val | 0x10);
	if (rc < 0)
		return rc;
	rc = mb86a20s_writereg(state, 0x45, val & 0x6f);

	return rc;
}