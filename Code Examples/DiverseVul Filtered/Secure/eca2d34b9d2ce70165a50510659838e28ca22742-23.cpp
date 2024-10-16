static int mb86a20s_read_signal_strength(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int rc;
	unsigned rf_max, rf_min, rf;

	if (state->get_strength_time &&
	   (!time_after(jiffies, state->get_strength_time)))
		return c->strength.stat[0].uvalue;

	/* Reset its value if an error happen */
	c->strength.stat[0].uvalue = 0;

	/* Does a binary search to get RF strength */
	rf_max = 0xfff;
	rf_min = 0;
	do {
		rf = (rf_max + rf_min) / 2;
		rc = mb86a20s_writereg(state, 0x04, 0x1f);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x05, rf >> 8);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x04, 0x20);
		if (rc < 0)
			return rc;
		rc = mb86a20s_writereg(state, 0x05, rf);
		if (rc < 0)
			return rc;

		rc = mb86a20s_readreg(state, 0x02);
		if (rc < 0)
			return rc;
		if (rc & 0x08)
			rf_min = (rf_max + rf_min) / 2;
		else
			rf_max = (rf_max + rf_min) / 2;
		if (rf_max - rf_min < 4) {
			rf = (rf_max + rf_min) / 2;

			/* Rescale it from 2^12 (4096) to 2^16 */
			rf = rf << (16 - 12);
			if (rf)
				rf |= (1 << 12) - 1;

			dev_dbg(&state->i2c->dev,
				"%s: signal strength = %d (%d < RF=%d < %d)\n",
				__func__, rf, rf_min, rf >> 4, rf_max);
			c->strength.stat[0].uvalue = rf;
			state->get_strength_time = jiffies +
						   msecs_to_jiffies(1000);
			return 0;
		}
	} while (1);
}