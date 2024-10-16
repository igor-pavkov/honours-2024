static int emulate_delivery_system(struct dvb_frontend *fe, u32 delsys)
{
	int i;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;

	c->delivery_system = delsys;

	/*
	 * If the call is for ISDB-T, put it into full-seg, auto mode, TV
	 */
	if (c->delivery_system == SYS_ISDBT) {
		dev_dbg(fe->dvb->device,
			"%s: Using defaults for SYS_ISDBT\n",
			__func__);

		if (!c->bandwidth_hz)
			c->bandwidth_hz = 6000000;

		c->isdbt_partial_reception = 0;
		c->isdbt_sb_mode = 0;
		c->isdbt_sb_subchannel = 0;
		c->isdbt_sb_segment_idx = 0;
		c->isdbt_sb_segment_count = 0;
		c->isdbt_layer_enabled = 7;
		for (i = 0; i < 3; i++) {
			c->layer[i].fec = FEC_AUTO;
			c->layer[i].modulation = QAM_AUTO;
			c->layer[i].interleaving = 0;
			c->layer[i].segment_count = 0;
		}
	}
	dev_dbg(fe->dvb->device, "%s: change delivery system on cache to %d\n",
		__func__, c->delivery_system);

	return 0;
}