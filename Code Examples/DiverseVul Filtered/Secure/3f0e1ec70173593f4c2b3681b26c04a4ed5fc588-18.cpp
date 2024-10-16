static int h2_rcv_buf(struct conn_stream *cs, struct buffer *buf, int count)
{
	struct h2s *h2s = cs->ctx;
	struct h2c *h2c = h2s->h2c;
	int ret = 0;

	if (h2c->st0 != H2_CS_FRAME_P)
		return 0; // no pre-parsed frame yet

	if (h2c->dsi != h2s->id)
		return 0; // not for us

	if (!h2c->dbuf->size)
		return 0; // empty buffer

	switch (h2c->dft) {
	case H2_FT_HEADERS:
		ret = h2_frt_decode_headers(h2s, buf, count);
		break;

	case H2_FT_DATA:
		ret = h2_frt_transfer_data(h2s, buf, count);
		break;

	default:
		ret = 0;
	}
	return ret;
}