static void dvb_frontend_stop(struct dvb_frontend *fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if (fe->exit != DVB_FE_DEVICE_REMOVED)
		fe->exit = DVB_FE_NORMAL_EXIT;
	mb();

	if (!fepriv->thread)
		return;

	kthread_stop(fepriv->thread);

	sema_init(&fepriv->sem, 1);
	fepriv->state = FESTATE_IDLE;

	/* paranoia check in case a signal arrived */
	if (fepriv->thread)
		dev_warn(fe->dvb->device,
				"dvb_frontend_stop: warning: thread %p won't exit\n",
				fepriv->thread);
}