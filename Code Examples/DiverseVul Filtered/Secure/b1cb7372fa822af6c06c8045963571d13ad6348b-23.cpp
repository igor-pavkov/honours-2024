static void dvb_frontend_free(struct kref *ref)
{
	struct dvb_frontend *fe =
		container_of(ref, struct dvb_frontend, refcount);

	__dvb_frontend_free(fe);
}