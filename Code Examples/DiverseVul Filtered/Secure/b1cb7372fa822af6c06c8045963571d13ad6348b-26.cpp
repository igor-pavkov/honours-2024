static void dvb_frontend_swzigzag_update_delay(struct dvb_frontend_private *fepriv, int locked)
{
	int q2;
	struct dvb_frontend *fe = fepriv->dvbdev->priv;

	dev_dbg(fe->dvb->device, "%s:\n", __func__);

	if (locked)
		(fepriv->quality) = (fepriv->quality * 220 + 36*256) / 256;
	else
		(fepriv->quality) = (fepriv->quality * 220 + 0) / 256;

	q2 = fepriv->quality - 128;
	q2 *= q2;

	fepriv->delay = fepriv->min_delay + q2 * HZ / (128*128);
}