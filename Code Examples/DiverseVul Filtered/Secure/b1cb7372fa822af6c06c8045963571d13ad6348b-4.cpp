int dvb_frontend_resume(struct dvb_frontend *fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;
	int ret = 0;

	dev_dbg(fe->dvb->device, "%s: adap=%d fe=%d\n", __func__, fe->dvb->num,
			fe->id);

	fe->exit = DVB_FE_DEVICE_RESUME;
	if (fe->ops.init)
		ret = fe->ops.init(fe);

	if (fe->ops.tuner_ops.resume)
		ret = fe->ops.tuner_ops.resume(fe);
	else if (fe->ops.tuner_ops.init)
		ret = fe->ops.tuner_ops.init(fe);

	if (fe->ops.set_tone && fepriv->tone != -1)
		fe->ops.set_tone(fe, fepriv->tone);
	if (fe->ops.set_voltage && fepriv->voltage != -1)
		fe->ops.set_voltage(fe, fepriv->voltage);

	fe->exit = DVB_FE_NO_EXIT;
	fepriv->state = FESTATE_RETUNE;
	dvb_frontend_wakeup(fe);

	return ret;
}