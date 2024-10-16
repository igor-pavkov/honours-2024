void dvb_frontend_detach(struct dvb_frontend* fe)
{
	dvb_frontend_invoke_release(fe, fe->ops.release_sec);
	dvb_frontend_invoke_release(fe, fe->ops.tuner_ops.release);
	dvb_frontend_invoke_release(fe, fe->ops.analog_ops.release);
	dvb_frontend_invoke_release(fe, fe->ops.detach);
	dvb_frontend_put(fe);
}