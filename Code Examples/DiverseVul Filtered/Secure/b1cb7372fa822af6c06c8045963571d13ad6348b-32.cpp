int dvb_unregister_frontend(struct dvb_frontend* fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	mutex_lock(&frontend_mutex);
	dvb_frontend_stop(fe);
	dvb_remove_device(fepriv->dvbdev);

	/* fe is invalid now */
	mutex_unlock(&frontend_mutex);
	dvb_frontend_put(fe);
	return 0;
}