static inline void h2s_close(struct h2s *h2s)
{
	if (h2s->st != H2_SS_CLOSED)
		h2s->h2c->nb_streams--;
	h2s->st = H2_SS_CLOSED;
}