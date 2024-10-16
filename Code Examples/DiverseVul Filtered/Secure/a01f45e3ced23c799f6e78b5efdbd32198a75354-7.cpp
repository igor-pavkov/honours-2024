static void h2_shutr(struct conn_stream *cs, enum cs_shr_mode mode)
{
	struct h2s *h2s = cs->ctx;

	if (!mode)
		return;

	h2_do_shutr(h2s);
}