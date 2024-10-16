static int dtv_property_cache_sync(struct dvb_frontend *fe,
				   struct dtv_frontend_properties *c,
				   const struct dvb_frontend_parameters *p)
{
	c->frequency = p->frequency;
	c->inversion = p->inversion;

	switch (dvbv3_type(c->delivery_system)) {
	case DVBV3_QPSK:
		dev_dbg(fe->dvb->device, "%s: Preparing QPSK req\n", __func__);
		c->symbol_rate = p->u.qpsk.symbol_rate;
		c->fec_inner = p->u.qpsk.fec_inner;
		break;
	case DVBV3_QAM:
		dev_dbg(fe->dvb->device, "%s: Preparing QAM req\n", __func__);
		c->symbol_rate = p->u.qam.symbol_rate;
		c->fec_inner = p->u.qam.fec_inner;
		c->modulation = p->u.qam.modulation;
		break;
	case DVBV3_OFDM:
		dev_dbg(fe->dvb->device, "%s: Preparing OFDM req\n", __func__);

		switch (p->u.ofdm.bandwidth) {
		case BANDWIDTH_10_MHZ:
			c->bandwidth_hz = 10000000;
			break;
		case BANDWIDTH_8_MHZ:
			c->bandwidth_hz = 8000000;
			break;
		case BANDWIDTH_7_MHZ:
			c->bandwidth_hz = 7000000;
			break;
		case BANDWIDTH_6_MHZ:
			c->bandwidth_hz = 6000000;
			break;
		case BANDWIDTH_5_MHZ:
			c->bandwidth_hz = 5000000;
			break;
		case BANDWIDTH_1_712_MHZ:
			c->bandwidth_hz = 1712000;
			break;
		case BANDWIDTH_AUTO:
			c->bandwidth_hz = 0;
		}

		c->code_rate_HP = p->u.ofdm.code_rate_HP;
		c->code_rate_LP = p->u.ofdm.code_rate_LP;
		c->modulation = p->u.ofdm.constellation;
		c->transmission_mode = p->u.ofdm.transmission_mode;
		c->guard_interval = p->u.ofdm.guard_interval;
		c->hierarchy = p->u.ofdm.hierarchy_information;
		break;
	case DVBV3_ATSC:
		dev_dbg(fe->dvb->device, "%s: Preparing ATSC req\n", __func__);
		c->modulation = p->u.vsb.modulation;
		if (c->delivery_system == SYS_ATSCMH)
			break;
		if ((c->modulation == VSB_8) || (c->modulation == VSB_16))
			c->delivery_system = SYS_ATSC;
		else
			c->delivery_system = SYS_DVBC_ANNEX_B;
		break;
	case DVBV3_UNKNOWN:
		dev_err(fe->dvb->device,
				"%s: doesn't know how to handle a DVBv3 call to delivery system %i\n",
				__func__, c->delivery_system);
		return -EINVAL;
	}

	return 0;
}