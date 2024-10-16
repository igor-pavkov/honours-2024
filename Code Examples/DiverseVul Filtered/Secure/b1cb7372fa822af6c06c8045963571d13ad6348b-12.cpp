static void dvb_frontend_put(struct dvb_frontend *fe)
{
	/*
	 * Check if the frontend was registered, as otherwise
	 * kref was not initialized yet.
	 */
	if (fe->frontend_priv)
		kref_put(&fe->refcount, dvb_frontend_free);
	else
		__dvb_frontend_free(fe);
}