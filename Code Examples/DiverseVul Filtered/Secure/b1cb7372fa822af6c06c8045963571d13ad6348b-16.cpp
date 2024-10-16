static int dvb_frontend_check_parameters(struct dvb_frontend *fe)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	u32 freq_min;
	u32 freq_max;

	/* range check: frequency */
	dvb_frontend_get_frequency_limits(fe, &freq_min, &freq_max);
	if ((freq_min && c->frequency < freq_min) ||
	    (freq_max && c->frequency > freq_max)) {
		dev_warn(fe->dvb->device, "DVB: adapter %i frontend %i frequency %u out of range (%u..%u)\n",
				fe->dvb->num, fe->id, c->frequency,
				freq_min, freq_max);
		return -EINVAL;
	}

	/* range check: symbol rate */
	switch (c->delivery_system) {
	case SYS_DVBS:
	case SYS_DVBS2:
	case SYS_TURBO:
	case SYS_DVBC_ANNEX_A:
	case SYS_DVBC_ANNEX_C:
		if ((fe->ops.info.symbol_rate_min &&
		     c->symbol_rate < fe->ops.info.symbol_rate_min) ||
		    (fe->ops.info.symbol_rate_max &&
		     c->symbol_rate > fe->ops.info.symbol_rate_max)) {
			dev_warn(fe->dvb->device, "DVB: adapter %i frontend %i symbol rate %u out of range (%u..%u)\n",
					fe->dvb->num, fe->id, c->symbol_rate,
					fe->ops.info.symbol_rate_min,
					fe->ops.info.symbol_rate_max);
			return -EINVAL;
		}
	default:
		break;
	}

	return 0;
}