void dvb_frontend_reinitialise(struct dvb_frontend *fe)
{
	struct dvb_frontend_private *fepriv = fe->frontend_priv;

	fepriv->reinitialise = 1;
	dvb_frontend_wakeup(fe);
}