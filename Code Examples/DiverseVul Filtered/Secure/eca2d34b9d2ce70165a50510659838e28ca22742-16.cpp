static int mb86a20s_get_frontend(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int layer, rc;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	/* Reset frontend cache to default values */
	mb86a20s_reset_frontend_cache(fe);

	/* Check for partial reception */
	rc = mb86a20s_writereg(state, 0x6d, 0x85);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x6e);
	if (rc < 0)
		return rc;
	c->isdbt_partial_reception = (rc & 0x10) ? 1 : 0;

	/* Get per-layer data */

	for (layer = 0; layer < NUM_LAYERS; layer++) {
		dev_dbg(&state->i2c->dev, "%s: getting data for layer %c.\n",
			__func__, 'A' + layer);

		rc = mb86a20s_get_segment_count(state, layer);
		if (rc < 0)
			goto noperlayer_error;
		if (rc >= 0 && rc < 14) {
			c->layer[layer].segment_count = rc;
		} else {
			c->layer[layer].segment_count = 0;
			state->estimated_rate[layer] = 0;
			continue;
		}
		c->isdbt_layer_enabled |= 1 << layer;
		rc = mb86a20s_get_modulation(state, layer);
		if (rc < 0)
			goto noperlayer_error;
		dev_dbg(&state->i2c->dev, "%s: modulation %d.\n",
			__func__, rc);
		c->layer[layer].modulation = rc;
		rc = mb86a20s_get_fec(state, layer);
		if (rc < 0)
			goto noperlayer_error;
		dev_dbg(&state->i2c->dev, "%s: FEC %d.\n",
			__func__, rc);
		c->layer[layer].fec = rc;
		rc = mb86a20s_get_interleaving(state, layer);
		if (rc < 0)
			goto noperlayer_error;
		dev_dbg(&state->i2c->dev, "%s: interleaving %d.\n",
			__func__, rc);
		c->layer[layer].interleaving = rc;
		mb86a20s_layer_bitrate(fe, layer, c->layer[layer].modulation,
				       c->layer[layer].fec,
				       c->guard_interval,
				       c->layer[layer].segment_count);
	}

	rc = mb86a20s_writereg(state, 0x6d, 0x84);
	if (rc < 0)
		return rc;
	if ((rc & 0x60) == 0x20) {
		c->isdbt_sb_mode = 1;
		/* At least, one segment should exist */
		if (!c->isdbt_sb_segment_count)
			c->isdbt_sb_segment_count = 1;
	}

	/* Get transmission mode and guard interval */
	rc = mb86a20s_readreg(state, 0x07);
	if (rc < 0)
		return rc;
	c->transmission_mode = TRANSMISSION_MODE_AUTO;
	if ((rc & 0x60) == 0x20) {
		/* Only modes 2 and 3 are supported */
		switch ((rc >> 2) & 0x03) {
		case 1:
			c->transmission_mode = TRANSMISSION_MODE_4K;
			break;
		case 2:
			c->transmission_mode = TRANSMISSION_MODE_8K;
			break;
		}
	}
	c->guard_interval = GUARD_INTERVAL_AUTO;
	if (!(rc & 0x10)) {
		/* Guard interval 1/32 is not supported */
		switch (rc & 0x3) {
		case 0:
			c->guard_interval = GUARD_INTERVAL_1_4;
			break;
		case 1:
			c->guard_interval = GUARD_INTERVAL_1_8;
			break;
		case 2:
			c->guard_interval = GUARD_INTERVAL_1_16;
			break;
		}
	}
	return 0;

noperlayer_error:

	/* per-layer info is incomplete; discard all per-layer */
	c->isdbt_layer_enabled = 0;

	return rc;
}