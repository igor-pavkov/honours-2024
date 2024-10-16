static int mb86a20s_read_status_and_stats(struct dvb_frontend *fe,
					  enum fe_status *status)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	int rc, status_nr;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 0);

	/* Get lock */
	status_nr = mb86a20s_read_status(fe, status);
	if (status_nr < 7) {
		mb86a20s_stats_not_ready(fe);
		mb86a20s_reset_frontend_cache(fe);
	}
	if (status_nr < 0) {
		dev_err(&state->i2c->dev,
			"%s: Can't read frontend lock status\n", __func__);
		goto error;
	}

	/* Get signal strength */
	rc = mb86a20s_read_signal_strength(fe);
	if (rc < 0) {
		dev_err(&state->i2c->dev,
			"%s: Can't reset VBER registers.\n", __func__);
		mb86a20s_stats_not_ready(fe);
		mb86a20s_reset_frontend_cache(fe);

		rc = 0;		/* Status is OK */
		goto error;
	}

	if (status_nr >= 7) {
		/* Get TMCC info*/
		rc = mb86a20s_get_frontend(fe);
		if (rc < 0) {
			dev_err(&state->i2c->dev,
				"%s: Can't get FE TMCC data.\n", __func__);
			rc = 0;		/* Status is OK */
			goto error;
		}

		/* Get statistics */
		rc = mb86a20s_get_stats(fe, status_nr);
		if (rc < 0 && rc != -EBUSY) {
			dev_err(&state->i2c->dev,
				"%s: Can't get FE statistics.\n", __func__);
			rc = 0;
			goto error;
		}
		rc = 0;	/* Don't return EBUSY to userspace */
	}
	goto ok;

error:
	mb86a20s_stats_not_ready(fe);

ok:
	if (fe->ops.i2c_gate_ctrl)
		fe->ops.i2c_gate_ctrl(fe, 1);

	return rc;
}