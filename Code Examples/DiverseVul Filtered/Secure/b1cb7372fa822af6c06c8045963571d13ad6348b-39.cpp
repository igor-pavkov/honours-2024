static unsigned int dvb_frontend_poll(struct file *file, struct poll_table_struct *wait)
{
	struct dvb_device *dvbdev = file->private_data;
	struct dvb_frontend *fe = dvbdev->priv;
	struct dvb_frontend_private *fepriv = fe->frontend_priv;

	dev_dbg_ratelimited(fe->dvb->device, "%s:\n", __func__);

	poll_wait (file, &fepriv->events.wait_queue, wait);

	if (fepriv->events.eventw != fepriv->events.eventr)
		return (POLLIN | POLLRDNORM | POLLPRI);

	return 0;
}