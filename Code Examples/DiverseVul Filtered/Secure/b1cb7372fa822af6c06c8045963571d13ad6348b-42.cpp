static void dvb_frontend_init(struct dvb_frontend *fe)
{
	dev_dbg(fe->dvb->device,
			"%s: initialising adapter %i frontend %i (%s)...\n",
			__func__, fe->dvb->num, fe->id, fe->ops.info.name);

	if (fe->ops.init)
		fe->ops.init(fe);
	if (fe->ops.tuner_ops.init) {
		if (fe->ops.i2c_gate_ctrl)
			fe->ops.i2c_gate_ctrl(fe, 1);
		fe->ops.tuner_ops.init(fe);
		if (fe->ops.i2c_gate_ctrl)
			fe->ops.i2c_gate_ctrl(fe, 0);
	}
}