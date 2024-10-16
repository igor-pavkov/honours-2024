static int dtv_set_frontend(struct dvb_frontend *fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct dvb_frontend_tune_settings fetunesettings;
	u32 rolloff = 0;

	if (dvb_frontend_check_parameters(fe) < 0)
		return -EINVAL;

	/*
	 * Initialize output parameters to match the values given by
	 * the user. FE_SET_FRONTEND triggers an initial frontend event
	 * with status = 0, which copies output parameters to userspace.
	 */
	dtv_property_legacy_params_sync(fe, c, &fepriv->parameters_out);

	/*
	 * Be sure that the bandwidth will be filled for all
	 * non-satellite systems, as tuners need to know what
	 * low pass/Nyquist half filter should be applied, in
	 * order to avoid inter-channel noise.
	 *
	 * ISDB-T and DVB-T/T2 already sets bandwidth.
	 * ATSC and DVB-C don't set, so, the core should fill it.
	 *
	 * On DVB-C Annex A and C, the bandwidth is a function of
	 * the roll-off and symbol rate. Annex B defines different
	 * roll-off factors depending on the modulation. Fortunately,
	 * Annex B is only used with 6MHz, so there's no need to
	 * calculate it.
	 *
	 * While not officially supported, a side effect of handling it at
	 * the cache level is that a program could retrieve the bandwidth
	 * via DTV_BANDWIDTH_HZ, which may be useful for test programs.
	 */
	switch (c->delivery_system) {
	case SYS_ATSC:
	case SYS_DVBC_ANNEX_B:
		c->bandwidth_hz = 6000000;
		break;
	case SYS_DVBC_ANNEX_A:
		rolloff = 115;
		break;
	case SYS_DVBC_ANNEX_C:
		rolloff = 113;
		break;
	case SYS_DVBS:
	case SYS_TURBO:
	case SYS_ISDBS:
		rolloff = 135;
		break;
	case SYS_DVBS2:
		switch (c->rolloff) {
		case ROLLOFF_20:
			rolloff = 120;
			break;
		case ROLLOFF_25:
			rolloff = 125;
			break;
		default:
		case ROLLOFF_35:
			rolloff = 135;
		}
		break;
	default:
		break;
	}
	if (rolloff)
		c->bandwidth_hz = mult_frac(c->symbol_rate, rolloff, 100);

	/* force auto frequency inversion if requested */
	if (dvb_force_auto_inversion)
		c->inversion = INVERSION_AUTO;

	/*
	 * without hierarchical coding code_rate_LP is irrelevant,
	 * so we tolerate the otherwise invalid FEC_NONE setting
	 */
	if (c->hierarchy == HIERARCHY_NONE && c->code_rate_LP == FEC_NONE)
		c->code_rate_LP = FEC_AUTO;

	/* get frontend-specific tuning settings */
	memset(&fetunesettings, 0, sizeof(struct dvb_frontend_tune_settings));
	if (fe->ops.get_tune_settings && (fe->ops.get_tune_settings(fe, &fetunesettings) == 0)) {
		fepriv->min_delay = (fetunesettings.min_delay_ms * HZ) / 1000;
		fepriv->max_drift = fetunesettings.max_drift;
		fepriv->step_size = fetunesettings.step_size;
	} else {
		/* default values */
		switch (c->delivery_system) {
		case SYS_DVBS:
		case SYS_DVBS2:
		case SYS_ISDBS:
		case SYS_TURBO:
		case SYS_DVBC_ANNEX_A:
		case SYS_DVBC_ANNEX_C:
			fepriv->min_delay = HZ / 20;
			fepriv->step_size = c->symbol_rate / 16000;
			fepriv->max_drift = c->symbol_rate / 2000;
			break;
		case SYS_DVBT:
		case SYS_DVBT2:
		case SYS_ISDBT:
		case SYS_DTMB:
			fepriv->min_delay = HZ / 20;
			fepriv->step_size = fe->ops.info.frequency_stepsize * 2;
			fepriv->max_drift = (fe->ops.info.frequency_stepsize * 2) + 1;
			break;
		default:
			/*
			 * FIXME: This sounds wrong! if freqency_stepsize is
			 * defined by the frontend, why not use it???
			 */
			fepriv->min_delay = HZ / 20;
			fepriv->step_size = 0; /* no zigzag */
			fepriv->max_drift = 0;
			break;
		}
	}
	if (dvb_override_tune_delay > 0)
		fepriv->min_delay = (dvb_override_tune_delay * HZ) / 1000;

	fepriv->state = FESTATE_RETUNE;

	/* Request the search algorithm to search */
	fepriv->algo_status |= DVBFE_ALGO_SEARCH_AGAIN;

	dvb_frontend_clear_events(fe);
	dvb_frontend_add_event(fe, 0);
	dvb_frontend_wakeup(fe);
	fepriv->status = 0;

	return 0;
}