static void dvb_frontend_get(struct dvb_frontend *fe)
{
	kref_get(&fe->refcount);
}