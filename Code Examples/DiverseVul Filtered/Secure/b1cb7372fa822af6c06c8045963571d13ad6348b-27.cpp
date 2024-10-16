static bool has_get_frontend(struct dvb_frontend *fe)
{
	return fe->ops.get_frontend != NULL;
}