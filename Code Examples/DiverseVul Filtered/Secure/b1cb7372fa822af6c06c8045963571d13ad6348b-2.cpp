static void dvb_frontend_invoke_release(struct dvb_frontend *fe,
					void (*release)(struct dvb_frontend *fe))
{
	if (release) {
		release(fe);
#ifdef CONFIG_MEDIA_ATTACH
		dvb_detach(release);
#endif
	}
}