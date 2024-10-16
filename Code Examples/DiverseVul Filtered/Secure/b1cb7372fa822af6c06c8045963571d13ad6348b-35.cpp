static int dvb_frontend_is_exiting(struct dvb_frontend *fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;

	if (fe->exit != DVB_FE_NO_EXIT)
		return 1;

	if (fepriv->dvbdev->writers == 1)
		if (time_after_eq(jiffies, fepriv->release_jiffies +
				  dvb_shutdown_timeout * HZ))
			return 1;

	return 0;
}