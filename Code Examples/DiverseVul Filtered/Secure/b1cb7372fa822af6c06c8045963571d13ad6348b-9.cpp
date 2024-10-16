static int dvb_frontend_clear_cache(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	int i;
	u32 delsys;

	delsys = c->delivery_system;
	memset(c, 0, offsetof(struct dtv_frontend_properties, strength));
	c->delivery_system = delsys;

	dev_dbg(fe->dvb->device, "%s: Clearing cache for delivery system %d\n",
			__func__, c->delivery_system);

	c->transmission_mode = TRANSMISSION_MODE_AUTO;
	c->bandwidth_hz = 0;	/* AUTO */
	c->guard_interval = GUARD_INTERVAL_AUTO;
	c->hierarchy = HIERARCHY_AUTO;
	c->symbol_rate = 0;
	c->code_rate_HP = FEC_AUTO;
	c->code_rate_LP = FEC_AUTO;
	c->fec_inner = FEC_AUTO;
	c->rolloff = ROLLOFF_AUTO;
	c->voltage = SEC_VOLTAGE_OFF;
	c->sectone = SEC_TONE_OFF;
	c->pilot = PILOT_AUTO;

	c->isdbt_partial_reception = 0;
	c->isdbt_sb_mode = 0;
	c->isdbt_sb_subchannel = 0;
	c->isdbt_sb_segment_idx = 0;
	c->isdbt_sb_segment_count = 0;
	c->isdbt_layer_enabled = 0;
	for (i = 0; i < 3; i++) {
		c->layer[i].fec = FEC_AUTO;
		c->layer[i].modulation = QAM_AUTO;
		c->layer[i].interleaving = 0;
		c->layer[i].segment_count = 0;
	}

	c->stream_id = NO_STREAM_ID_FILTER;

	switch (c->delivery_system) {
	case SYS_DVBS:
	case SYS_DVBS2:
	case SYS_TURBO:
		c->modulation = QPSK;   /* implied for DVB-S in legacy API */
		c->rolloff = ROLLOFF_35;/* implied for DVB-S */
		break;
	case SYS_ATSC:
		c->modulation = VSB_8;
		break;
	case SYS_ISDBS:
		c->symbol_rate = 28860000;
		c->rolloff = ROLLOFF_35;
		c->bandwidth_hz = c->symbol_rate / 100 * 135;
		break;
	default:
		c->modulation = QAM_AUTO;
		break;
	}

	c->lna = LNA_AUTO;

	return 0;
}