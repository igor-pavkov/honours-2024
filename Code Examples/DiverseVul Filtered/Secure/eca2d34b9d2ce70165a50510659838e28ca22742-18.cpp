static int mb86a20s_read_signal_strength_from_cache(struct dvb_frontend *fe,
						    u16 *strength)
{
	struct dtv_frontend_properties *c = &fe->dtv_property_cache;


	*strength = c->strength.stat[0].uvalue;

	return 0;
}