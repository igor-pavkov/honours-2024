static void dvb_frontend_add_event(struct dvb_frontend *fe,
				   enum fe_status status)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;
	struct dvb_fe_events *events = &fepriv->events;
	struct dvb_frontend_event *e;
	int wp;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if ((status & FE_HAS_LOCK) && has_get_frontend(fe))
		dtv_get_frontend(fe, c, &fepriv->parameters_out);

	mutex_lock(&events->mtx);

	wp = (events->eventw + 1) % MAX_EVENT;
	if (wp == events->eventr) {
		events->overflow = 1;
		events->eventr = (events->eventr + 1) % MAX_EVENT;
	}

	e = &events->events[events->eventw];
	e->status = status;
	e->parameters = fepriv->parameters_out;

	events->eventw = wp;

	mutex_unlock(&events->mtx);

	wake_up_interruptible (&events->wait_queue);
}