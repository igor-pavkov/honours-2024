struct dvb_frontend *mb86a20s_attach(const struct mb86a20s_config *config,
				    struct i2c_adapter *i2c)
{
	struct mb86a20s_state *state;
	u8	rev;

	dev_dbg(&i2c->dev, "%s called.\n", __func__);

	/* allocate memory for the internal state */
	state = kzalloc(sizeof(struct mb86a20s_state), GFP_KERNEL);
	if (state == NULL) {
		dev_err(&i2c->dev,
			"%s: unable to allocate memory for state\n", __func__);
		goto error;
	}

	/* setup the state */
	state->config = config;
	state->i2c = i2c;

	/* create dvb_frontend */
	memcpy(&state->frontend.ops, &mb86a20s_ops,
		sizeof(struct dvb_frontend_ops));
	state->frontend.demodulator_priv = state;

	/* Check if it is a mb86a20s frontend */
	rev = mb86a20s_readreg(state, 0);

	if (rev == 0x13) {
		dev_info(&i2c->dev,
			 "Detected a Fujitsu mb86a20s frontend\n");
	} else {
		dev_dbg(&i2c->dev,
			"Frontend revision %d is unknown - aborting.\n",
		       rev);
		goto error;
	}

	return &state->frontend;

error:
	kfree(state);
	return NULL;
}