static int dtv_get_frontend(struct dvb_frontend *fe,
			    struct dtv_frontend_properties *c,
			    struct dvb_frontend_parameters *p_out)
{
	int r;

	if (fe->ops.get_frontend) {
		r = fe->ops.get_frontend(fe, c);
		if (unlikely(r < 0))
			return r;
		if (p_out)
			dtv_property_legacy_params_sync(fe, c, p_out);
		return 0;
	}

	/* As everything is in cache, get_frontend fops are always supported */
	return 0;
}