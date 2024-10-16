static void dvb_frontend_swzigzag(struct dvb_frontend *fe)
{
	enum fe_status s = FE_NONE;
	int retval = 0;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache, tmp;

	/* if we've got no parameters, just keep idling */
	if (fepriv->state & FESTATE_IDLE) {
		fepriv->delay = 3*HZ;
		fepriv->quality = 0;
		return;
	}

	/* in SCAN mode, we just set the frontend when asked and leave it alone */
	if (fepriv->tune_mode_flags & FE_TUNE_MODE_ONESHOT) {
		if (fepriv->state & FESTATE_RETUNE) {
			tmp = *c;
			if (fe->ops.set_frontend)
				retval = fe->ops.set_frontend(fe);
			*c = tmp;
			if (retval < 0)
				fepriv->state = FESTATE_ERROR;
			else
				fepriv->state = FESTATE_TUNED;
		}
		fepriv->delay = 3*HZ;
		fepriv->quality = 0;
		return;
	}

	/* get the frontend status */
	if (fepriv->state & FESTATE_RETUNE) {
		s = 0;
	} else {
		if (fe->ops.read_status)
			fe->ops.read_status(fe, &s);
		if (s != fepriv->status) {
			dvb_frontend_add_event(fe, s);
			fepriv->status = s;
		}
	}

	/* if we're not tuned, and we have a lock, move to the TUNED state */
	if ((fepriv->state & FESTATE_WAITFORLOCK) && (s & FE_HAS_LOCK)) {
		dvb_frontend_swzigzag_update_delay(fepriv, s & FE_HAS_LOCK);
		fepriv->state = FESTATE_TUNED;

		/* if we're tuned, then we have determined the correct inversion */
		if ((!(fe->ops.info.caps & FE_CAN_INVERSION_AUTO)) &&
		    (c->inversion == INVERSION_AUTO)) {
			c->inversion = fepriv->inversion;
		}
		return;
	}

	/* if we are tuned already, check we're still locked */
	if (fepriv->state & FESTATE_TUNED) {
		dvb_frontend_swzigzag_update_delay(fepriv, s & FE_HAS_LOCK);

		/* we're tuned, and the lock is still good... */
		if (s & FE_HAS_LOCK) {
			return;
		} else { /* if we _WERE_ tuned, but now don't have a lock */
			fepriv->state = FESTATE_ZIGZAG_FAST;
			fepriv->started_auto_step = fepriv->auto_step;
			fepriv->check_wrapped = 0;
		}
	}

	/* don't actually do anything if we're in the LOSTLOCK state,
	 * the frontend is set to FE_CAN_RECOVER, and the max_drift is 0 */
	if ((fepriv->state & FESTATE_LOSTLOCK) &&
	    (fe->ops.info.caps & FE_CAN_RECOVER) && (fepriv->max_drift == 0)) {
		dvb_frontend_swzigzag_update_delay(fepriv, s & FE_HAS_LOCK);
		return;
	}

	/* don't do anything if we're in the DISEQC state, since this
	 * might be someone with a motorized dish controlled by DISEQC.
	 * If its actually a re-tune, there will be a SET_FRONTEND soon enough.	*/
	if (fepriv->state & FESTATE_DISEQC) {
		dvb_frontend_swzigzag_update_delay(fepriv, s & FE_HAS_LOCK);
		return;
	}

	/* if we're in the RETUNE state, set everything up for a brand
	 * new scan, keeping the current inversion setting, as the next
	 * tune is _very_ likely to require the same */
	if (fepriv->state & FESTATE_RETUNE) {
		fepriv->lnb_drift = 0;
		fepriv->auto_step = 0;
		fepriv->auto_sub_step = 0;
		fepriv->started_auto_step = 0;
		fepriv->check_wrapped = 0;
	}

	/* fast zigzag. */
	if ((fepriv->state & FESTATE_SEARCHING_FAST) || (fepriv->state & FESTATE_RETUNE)) {
		fepriv->delay = fepriv->min_delay;

		/* perform a tune */
		retval = dvb_frontend_swzigzag_autotune(fe,
							fepriv->check_wrapped);
		if (retval < 0) {
			return;
		} else if (retval) {
			/* OK, if we've run out of trials at the fast speed.
			 * Drop back to slow for the _next_ attempt */
			fepriv->state = FESTATE_SEARCHING_SLOW;
			fepriv->started_auto_step = fepriv->auto_step;
			return;
		}
		fepriv->check_wrapped = 1;

		/* if we've just retuned, enter the ZIGZAG_FAST state.
		 * This ensures we cannot return from an
		 * FE_SET_FRONTEND ioctl before the first frontend tune
		 * occurs */
		if (fepriv->state & FESTATE_RETUNE) {
			fepriv->state = FESTATE_TUNING_FAST;
		}
	}

	/* slow zigzag */
	if (fepriv->state & FESTATE_SEARCHING_SLOW) {
		dvb_frontend_swzigzag_update_delay(fepriv, s & FE_HAS_LOCK);

		/* Note: don't bother checking for wrapping; we stay in this
		 * state until we get a lock */
		dvb_frontend_swzigzag_autotune(fe, 0);
	}
}