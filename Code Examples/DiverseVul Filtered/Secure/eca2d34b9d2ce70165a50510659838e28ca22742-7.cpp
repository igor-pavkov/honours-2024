static void mb86a20s_stats_not_ready(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int layer;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	/* Fill the length of each status counter */

	/* Only global stats */
	c->strength.len = 1;

	/* Per-layer stats - 3 layers + global */
	c->cnr.len = NUM_LAYERS + 1;
	c->pre_bit_error.len = NUM_LAYERS + 1;
	c->pre_bit_count.len = NUM_LAYERS + 1;
	c->post_bit_error.len = NUM_LAYERS + 1;
	c->post_bit_count.len = NUM_LAYERS + 1;
	c->block_error.len = NUM_LAYERS + 1;
	c->block_count.len = NUM_LAYERS + 1;

	/* Signal is always available */
	c->strength.stat[0].scale = FE_SCALE_RELATIVE;
	c->strength.stat[0].uvalue = 0;

	/* Put all of them at FE_SCALE_NOT_AVAILABLE */
	for (layer = 0; layer < NUM_LAYERS + 1; layer++) {
		c->cnr.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->pre_bit_error.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->pre_bit_count.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->post_bit_error.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->post_bit_count.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->block_error.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
		c->block_count.stat[layer].scale = FE_SCALE_NOT_AVAILABLE;
	}
}