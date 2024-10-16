static inline __maybe_unused void h2s_error(struct h2s *h2s, enum h2_err err)
{
	if (h2s->st > H2_SS_IDLE && h2s->st < H2_SS_ERROR) {
		h2s->errcode = err;
		h2s->st = H2_SS_ERROR;
		if (h2s->cs)
			h2s->cs->flags |= CS_FL_ERROR;
	}
}