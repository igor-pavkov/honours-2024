static int mb86a20s_get_blk_error_layer_CNR(struct dvb_frontend *fe)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 mer, cnr;
	int rc, val, layer;
	const struct linear_segments *segs;
	unsigned segs_len;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	/* Check if the measures are already available */
	rc = mb86a20s_writereg(state, 0x50, 0x5b);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;

	/* Check if data is available */
	if (!(rc & 0x01)) {
		dev_dbg(&state->i2c->dev,
			"%s: MER measures aren't available yet.\n", __func__);
		return -EBUSY;
	}

	/* Read all layers */
	for (layer = 0; layer < NUM_LAYERS; layer++) {
		if (!(c->isdbt_layer_enabled & (1 << layer))) {
			c->cnr.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
			continue;
		}

		rc = mb86a20s_writereg(state, 0x50, 0x52 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_readreg(state, 0x51);
		if (rc < 0)
			return rc;
		mer = rc << 16;
		rc = mb86a20s_writereg(state, 0x50, 0x53 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_readreg(state, 0x51);
		if (rc < 0)
			return rc;
		mer |= rc << 8;
		rc = mb86a20s_writereg(state, 0x50, 0x54 + layer * 3);
		if (rc < 0)
			return rc;
		rc = mb86a20s_readreg(state, 0x51);
		if (rc < 0)
			return rc;
		mer |= rc;

		switch (c->layer[layer].modulation) {
		case DQPSK:
		case QPSK:
			segs = cnr_qpsk_table;
			segs_len = ARRAY_SIZE(cnr_qpsk_table);
			break;
		case QAM_16:
			segs = cnr_16qam_table;
			segs_len = ARRAY_SIZE(cnr_16qam_table);
			break;
		default:
		case QAM_64:
			segs = cnr_64qam_table;
			segs_len = ARRAY_SIZE(cnr_64qam_table);
			break;
		}
		cnr = interpolate_value(mer, segs, segs_len);

		c->cnr.stat[1 + layer].scale = FE_SCALE_DECIBEL;
		c->cnr.stat[1 + layer].svalue = cnr;

		dev_dbg(&state->i2c->dev,
			"%s: CNR for layer %c is %d.%03d dB (MER = %d).\n",
			__func__, 'A' + layer, cnr / 1000, cnr % 1000, mer);

	}

	/* Start a new MER measurement */
	/* MER counter reset */
	rc = mb86a20s_writereg(state, 0x50, 0x50);
	if (rc < 0)
		return rc;
	rc = mb86a20s_readreg(state, 0x51);
	if (rc < 0)
		return rc;
	val = rc;

	rc = mb86a20s_writereg(state, 0x51, val | 0x01);
	if (rc < 0)
		return rc;
	rc = mb86a20s_writereg(state, 0x51, val & 0x06);
	if (rc < 0)
		return rc;

	return 0;
}