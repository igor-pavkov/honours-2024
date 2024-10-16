int dvb_frontend_suspend(struct dvb_frontend *fe)
{
	int ret = 0;

	dev_dbg(fe->dvb->device, "%s: adap=%d fe=%d\n", __func__, fe->dvb->num,
			fe->id);

	if (fe->ops.tuner_ops.suspend)
		ret = fe->ops.tuner_ops.suspend(fe);
	else if (fe->ops.tuner_ops.sleep)
		ret = fe->ops.tuner_ops.sleep(fe);

	if (fe->ops.sleep)
		ret = fe->ops.sleep(fe);

	return ret;
}