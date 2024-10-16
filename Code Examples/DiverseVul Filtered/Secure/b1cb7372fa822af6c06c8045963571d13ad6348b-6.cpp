static int dvb_frontend_swzigzag_autotune(struct dvb_frontend *fe, int check_wrapped)
{
	int autoinversion;
	int ready = 0;
	int fe_set_err = 0;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache, tmp;
	int original_inversion = c->inversion;
	u32 original_frequency = c->frequency;

	/* are we using autoinversion? */
	autoinversion = ((!(fe->ops.info.caps & FE_CAN_INVERSION_AUTO)) &&
			 (c->inversion == INVERSION_AUTO));

	/* setup parameters correctly */
	while(!ready) {
		/* calculate the lnb_drift */
		fepriv->lnb_drift = fepriv->auto_step * fepriv->step_size;

		/* wrap the auto_step if we've exceeded the maximum drift */
		if (fepriv->lnb_drift > fepriv->max_drift) {
			fepriv->auto_step = 0;
			fepriv->auto_sub_step = 0;
			fepriv->lnb_drift = 0;
		}

		/* perform inversion and +/- zigzag */
		switch(fepriv->auto_sub_step) {
		case 0:
			/* try with the current inversion and current drift setting */
			ready = 1;
			break;

		case 1:
			if (!autoinversion) break;

			fepriv->inversion = (fepriv->inversion == INVERSION_OFF) ? INVERSION_ON : INVERSION_OFF;
			ready = 1;
			break;

		case 2:
			if (fepriv->lnb_drift == 0) break;

			fepriv->lnb_drift = -fepriv->lnb_drift;
			ready = 1;
			break;

		case 3:
			if (fepriv->lnb_drift == 0) break;
			if (!autoinversion) break;

			fepriv->inversion = (fepriv->inversion == INVERSION_OFF) ? INVERSION_ON : INVERSION_OFF;
			fepriv->lnb_drift = -fepriv->lnb_drift;
			ready = 1;
			break;

		default:
			fepriv->auto_step++;
			fepriv->auto_sub_step = -1; /* it'll be incremented to 0 in a moment */
			break;
		}

		if (!ready) fepriv->auto_sub_step++;
	}

	/* if this attempt would hit where we started, indicate a complete
	 * iteration has occurred */
	if ((fepriv->auto_step == fepriv->started_auto_step) &&
	    (fepriv->auto_sub_step == 0) && check_wrapped) {
		return 1;
	}

	dev_dbg(fe->dvb->device, "%s: drift:%i inversion:%i auto_step:%i " \
			"auto_sub_step:%i started_auto_step:%i\n",
			__func__, fepriv->lnb_drift, fepriv->inversion,
			fepriv->auto_step, fepriv->auto_sub_step,
			fepriv->started_auto_step);

	/* set the frontend itself */
	c->frequency += fepriv->lnb_drift;
	if (autoinversion)
		c->inversion = fepriv->inversion;
	tmp = *c;
	if (fe->ops.set_frontend)
		fe_set_err = fe->ops.set_frontend(fe);
	*c = tmp;
	if (fe_set_err < 0) {
		fepriv->state = FESTATE_ERROR;
		return fe_set_err;
	}

	c->frequency = original_frequency;
	c->inversion = original_inversion;

	fepriv->auto_sub_step++;
	return 0;
}