static int mb86a20s_get_stats(struct dvb_frontend *fe, int status_nr)
{
	struct mb86a20s_state *state = fe->demodulator_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int rc = 0, layer;
	u32 bit_error = 0, bit_count = 0;
	u32 t_pre_bit_error = 0, t_pre_bit_count = 0;
	u32 t_post_bit_error = 0, t_post_bit_count = 0;
	u32 block_error = 0, block_count = 0;
	u32 t_block_error = 0, t_block_count = 0;
	int active_layers = 0, pre_ber_layers = 0, post_ber_layers = 0;
	int per_layers = 0;

	dev_dbg(&state->i2c->dev, "%s called.\n", __func__);

	mb86a20s_get_main_CNR(fe);

	/* Get per-layer stats */
	mb86a20s_get_blk_error_layer_CNR(fe);

	/*
	 * At state 7, only CNR is available
	 * For BER measures, state=9 is required
	 * FIXME: we may get MER measures with state=8
	 */
	if (status_nr < 9)
		return 0;

	for (layer = 0; layer < NUM_LAYERS; layer++) {
		if (c->isdbt_layer_enabled & (1 << layer)) {
			/* Layer is active and has rc segments */
			active_layers++;

			/* Handle BER before vterbi */
			rc = mb86a20s_get_pre_ber(fe, layer,
						  &bit_error, &bit_count);
			if (rc >= 0) {
				c->pre_bit_error.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->pre_bit_error.stat[1 + layer].uvalue += bit_error;
				c->pre_bit_count.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->pre_bit_count.stat[1 + layer].uvalue += bit_count;
			} else if (rc != -EBUSY) {
				/*
					* If an I/O error happened,
					* measures are now unavailable
					*/
				c->pre_bit_error.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				c->pre_bit_count.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				dev_err(&state->i2c->dev,
					"%s: Can't get BER for layer %c (error %d).\n",
					__func__, 'A' + layer, rc);
			}
			if (c->block_error.stat[1 + layer].scale != FE_SCALE_NOT_AVAILABLE)
				pre_ber_layers++;

			/* Handle BER post vterbi */
			rc = mb86a20s_get_post_ber(fe, layer,
						   &bit_error, &bit_count);
			if (rc >= 0) {
				c->post_bit_error.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->post_bit_error.stat[1 + layer].uvalue += bit_error;
				c->post_bit_count.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->post_bit_count.stat[1 + layer].uvalue += bit_count;
			} else if (rc != -EBUSY) {
				/*
					* If an I/O error happened,
					* measures are now unavailable
					*/
				c->post_bit_error.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				c->post_bit_count.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				dev_err(&state->i2c->dev,
					"%s: Can't get BER for layer %c (error %d).\n",
					__func__, 'A' + layer, rc);
			}
			if (c->block_error.stat[1 + layer].scale != FE_SCALE_NOT_AVAILABLE)
				post_ber_layers++;

			/* Handle Block errors for PER/UCB reports */
			rc = mb86a20s_get_blk_error(fe, layer,
						&block_error,
						&block_count);
			if (rc >= 0) {
				c->block_error.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->block_error.stat[1 + layer].uvalue += block_error;
				c->block_count.stat[1 + layer].scale = FE_SCALE_COUNTER;
				c->block_count.stat[1 + layer].uvalue += block_count;
			} else if (rc != -EBUSY) {
				/*
					* If an I/O error happened,
					* measures are now unavailable
					*/
				c->block_error.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				c->block_count.stat[1 + layer].scale = FE_SCALE_NOT_AVAILABLE;
				dev_err(&state->i2c->dev,
					"%s: Can't get PER for layer %c (error %d).\n",
					__func__, 'A' + layer, rc);

			}
			if (c->block_error.stat[1 + layer].scale != FE_SCALE_NOT_AVAILABLE)
				per_layers++;

			/* Update total preBER */
			t_pre_bit_error += c->pre_bit_error.stat[1 + layer].uvalue;
			t_pre_bit_count += c->pre_bit_count.stat[1 + layer].uvalue;

			/* Update total postBER */
			t_post_bit_error += c->post_bit_error.stat[1 + layer].uvalue;
			t_post_bit_count += c->post_bit_count.stat[1 + layer].uvalue;

			/* Update total PER */
			t_block_error += c->block_error.stat[1 + layer].uvalue;
			t_block_count += c->block_count.stat[1 + layer].uvalue;
		}
	}

	/*
	 * Start showing global count if at least one error count is
	 * available.
	 */
	if (pre_ber_layers) {
		/*
		 * At least one per-layer BER measure was read. We can now
		 * calculate the total BER
		 *
		 * Total Bit Error/Count is calculated as the sum of the
		 * bit errors on all active layers.
		 */
		c->pre_bit_error.stat[0].scale = FE_SCALE_COUNTER;
		c->pre_bit_error.stat[0].uvalue = t_pre_bit_error;
		c->pre_bit_count.stat[0].scale = FE_SCALE_COUNTER;
		c->pre_bit_count.stat[0].uvalue = t_pre_bit_count;
	} else {
		c->pre_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->pre_bit_count.stat[0].scale = FE_SCALE_COUNTER;
	}

	/*
	 * Start showing global count if at least one error count is
	 * available.
	 */
	if (post_ber_layers) {
		/*
		 * At least one per-layer BER measure was read. We can now
		 * calculate the total BER
		 *
		 * Total Bit Error/Count is calculated as the sum of the
		 * bit errors on all active layers.
		 */
		c->post_bit_error.stat[0].scale = FE_SCALE_COUNTER;
		c->post_bit_error.stat[0].uvalue = t_post_bit_error;
		c->post_bit_count.stat[0].scale = FE_SCALE_COUNTER;
		c->post_bit_count.stat[0].uvalue = t_post_bit_count;
	} else {
		c->post_bit_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->post_bit_count.stat[0].scale = FE_SCALE_COUNTER;
	}

	if (per_layers) {
		/*
		 * At least one per-layer UCB measure was read. We can now
		 * calculate the total UCB
		 *
		 * Total block Error/Count is calculated as the sum of the
		 * block errors on all active layers.
		 */
		c->block_error.stat[0].scale = FE_SCALE_COUNTER;
		c->block_error.stat[0].uvalue = t_block_error;
		c->block_count.stat[0].scale = FE_SCALE_COUNTER;
		c->block_count.stat[0].uvalue = t_block_count;
	} else {
		c->block_error.stat[0].scale = FE_SCALE_NOT_AVAILABLE;
		c->block_count.stat[0].scale = FE_SCALE_COUNTER;
	}

	return rc;
}