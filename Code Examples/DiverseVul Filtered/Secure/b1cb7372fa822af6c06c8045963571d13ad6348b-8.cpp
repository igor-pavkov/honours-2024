static int dvb_frontend_thread(void *data)
{
	struct dvb_frontend *fe = data;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	enum fe_status s = FE_NONE;
	enum dvbfe_algo algo;
	bool re_tune = false;
	bool semheld = false;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	fepriv->check_wrapped = 0;
	fepriv->quality = 0;
	fepriv->delay = 3*HZ;
	fepriv->status = 0;
	fepriv->wakeup = 0;
	fepriv->reinitialise = 0;

	dvb_frontend_init(fe);

	set_freezable();
	while (1) {
		up(&fepriv->sem);	    /* is locked when we enter the thread... */
restart:
		wait_event_interruptible_timeout(fepriv->wait_queue,
			dvb_frontend_should_wakeup(fe) || kthread_should_stop()
				|| freezing(current),
			fepriv->delay);

		if (kthread_should_stop() || dvb_frontend_is_exiting(fe)) {
			/* got signal or quitting */
			if (!down_interruptible(&fepriv->sem))
				semheld = true;
			fe->exit = DVB_FE_NORMAL_EXIT;
			break;
		}

		if (try_to_freeze())
			goto restart;

		if (down_interruptible(&fepriv->sem))
			break;

		if (fepriv->reinitialise) {
			dvb_frontend_init(fe);
			if (fe->ops.set_tone && fepriv->tone != -1)
				fe->ops.set_tone(fe, fepriv->tone);
			if (fe->ops.set_voltage && fepriv->voltage != -1)
				fe->ops.set_voltage(fe, fepriv->voltage);
			fepriv->reinitialise = 0;
		}

		/* do an iteration of the tuning loop */
		if (fe->ops.get_frontend_algo) {
			algo = fe->ops.get_frontend_algo(fe);
			switch (algo) {
			case DVBFE_ALGO_HW:
				dev_dbg(fe->dvb->device, "%s: Frontend ALGO = DVBFE_ALGO_HW\n", __func__);

				if (fepriv->state & FESTATE_RETUNE) {
					dev_dbg(fe->dvb->device, "%s: Retune requested, FESTATE_RETUNE\n", __func__);
					re_tune = true;
					fepriv->state = FESTATE_TUNED;
				} else {
					re_tune = false;
				}

				if (fe->ops.tune)
					fe->ops.tune(fe, re_tune, fepriv->tune_mode_flags, &fepriv->delay, &s);

				if (s != fepriv->status && !(fepriv->tune_mode_flags & FE_TUNE_MODE_ONESHOT)) {
					dev_dbg(fe->dvb->device, "%s: state changed, adding current state\n", __func__);
					dvb_frontend_add_event(fe, s);
					fepriv->status = s;
				}
				break;
			case DVBFE_ALGO_SW:
				dev_dbg(fe->dvb->device, "%s: Frontend ALGO = DVBFE_ALGO_SW\n", __func__);
				dvb_frontend_swzigzag(fe);
				break;
			case DVBFE_ALGO_CUSTOM:
				dev_dbg(fe->dvb->device, "%s: Frontend ALGO = DVBFE_ALGO_CUSTOM, state=%d\n", __func__, fepriv->state);
				if (fepriv->state & FESTATE_RETUNE) {
					dev_dbg(fe->dvb->device, "%s: Retune requested, FESTAT_RETUNE\n", __func__);
					fepriv->state = FESTATE_TUNED;
				}
				/* Case where we are going to search for a carrier
				 * User asked us to retune again for some reason, possibly
				 * requesting a search with a new set of parameters
				 */
				if (fepriv->algo_status & DVBFE_ALGO_SEARCH_AGAIN) {
					if (fe->ops.search) {
						fepriv->algo_status = fe->ops.search(fe);
						/* We did do a search as was requested, the flags are
						 * now unset as well and has the flags wrt to search.
						 */
					} else {
						fepriv->algo_status &= ~DVBFE_ALGO_SEARCH_AGAIN;
					}
				}
				/* Track the carrier if the search was successful */
				if (fepriv->algo_status != DVBFE_ALGO_SEARCH_SUCCESS) {
					fepriv->algo_status |= DVBFE_ALGO_SEARCH_AGAIN;
					fepriv->delay = HZ / 2;
				}
				dtv_property_legacy_params_sync(fe, c, &fepriv->parameters_out);
				fe->ops.read_status(fe, &s);
				if (s != fepriv->status) {
					dvb_frontend_add_event(fe, s); /* update event list */
					fepriv->status = s;
					if (!(s & FE_HAS_LOCK)) {
						fepriv->delay = HZ / 10;
						fepriv->algo_status |= DVBFE_ALGO_SEARCH_AGAIN;
					} else {
						fepriv->delay = 60 * HZ;
					}
				}
				break;
			default:
				dev_dbg(fe->dvb->device, "%s: UNDEFINED ALGO !\n", __func__);
				break;
			}
		} else {
			dvb_frontend_swzigzag(fe);
		}
	}

	if (dvb_powerdown_on_sleep) {
		if (fe->ops.set_voltage)
			fe->ops.set_voltage(fe, SEC_VOLTAGE_OFF);
		if (fe->ops.tuner_ops.sleep) {
			if (fe->ops.i2c_gate_ctrl)
				fe->ops.i2c_gate_ctrl(fe, 1);
			fe->ops.tuner_ops.sleep(fe);
			if (fe->ops.i2c_gate_ctrl)
				fe->ops.i2c_gate_ctrl(fe, 0);
		}
		if (fe->ops.sleep)
			fe->ops.sleep(fe);
	}

	fepriv->thread = NULL;
	if (kthread_should_stop())
		fe->exit = DVB_FE_DEVICE_REMOVED;
	else
		fe->exit = DVB_FE_NO_EXIT;
	mb();

	if (semheld)
		up(&fepriv->sem);
	dvb_frontend_wakeup(fe);
	return 0;
}