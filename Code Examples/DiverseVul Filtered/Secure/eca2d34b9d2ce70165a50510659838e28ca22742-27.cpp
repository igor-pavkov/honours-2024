static void mb86a20s_reset_frontend_cache(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	/* Fixed parameters */
	c->delivery_system = SYS_ISDBT;
	c->bandwidth_hz = 6000000;

	/* Initialize values that will be later autodetected */
	c->isdbt_layer_enabled = 0;
	c->transmission_mode = TRANSMISSION_MODE_AUTO;
	c->guard_interval = GUARD_INTERVAL_AUTO;
	c->isdbt_sb_mode = 0;
	c->isdbt_sb_segment_count = 0;
}