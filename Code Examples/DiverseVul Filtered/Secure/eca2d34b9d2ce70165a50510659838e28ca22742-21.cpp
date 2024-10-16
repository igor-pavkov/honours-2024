static int mb86a20s_tune(struct dvb_frontend *fe,
			bool re_tune,
			unsigned int mode_flags,
			unsigned int *delay,
			enum fe_status *status)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	int rc = 0;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (re_tune)
		rc = mb86a20s_set_frontend(fe);

	if (!(mode_flags & FE_TUNE_MODE_ONESHOT))
		mb86a20s_read_status_and_stats(fe, status);

	return rc;
}