static int dvb_frontend_start(struct dvb_frontend *fe)
{
	int ret;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	struct task_struct *fe_thread;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if (fepriv->thread) {
		if (fe->exit == DVB_FE_NO_EXIT)
			return 0;
		else
			dvb_frontend_stop (fe);
	}

	if (signal_pending(current))
		return -EINTR;
	if (down_interruptible (&fepriv->sem))
		return -EINTR;

	fepriv->state = FESTATE_IDLE;
	fe->exit = DVB_FE_NO_EXIT;
	fepriv->thread = NULL;
	mb();

	fe_thread = kthread_run(dvb_frontend_thread, fe,
		"kdvb-ad-%i-fe-%i", fe->dvb->num,fe->id);
	if (IS_ERR(fe_thread)) {
		ret = PTR_ERR(fe_thread);
		dev_warn(fe->dvb->device,
				"dvb_frontend_start: failed to start kthread (%d)\n",
				ret);
		up(&fepriv->sem);
		return ret;
	}
	fepriv->thread = fe_thread;
	return 0;
}