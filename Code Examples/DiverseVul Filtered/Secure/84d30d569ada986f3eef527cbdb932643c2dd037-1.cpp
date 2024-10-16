int git_smart__store_refs(transport_smart *t, int flushes)
{
	gitno_buffer *buf = &t->buffer;
	git_vector *refs = &t->refs;
	int error, flush = 0, recvd;
	const char *line_end = NULL;
	git_pkt *pkt = NULL;
	size_t i;

	/* Clear existing refs in case git_remote_connect() is called again
	 * after git_remote_disconnect().
	 */
	git_vector_foreach(refs, i, pkt) {
		git_pkt_free(pkt);
	}
	git_vector_clear(refs);
	pkt = NULL;

	do {
		if (buf->offset > 0)
			error = git_pkt_parse_line(&pkt, buf->data, &line_end, buf->offset);
		else
			error = GIT_EBUFS;

		if (error < 0 && error != GIT_EBUFS)
			return error;

		if (error == GIT_EBUFS) {
			if ((recvd = gitno_recv(buf)) < 0)
				return recvd;

			if (recvd == 0 && !flush) {
				giterr_set(GITERR_NET, "early EOF");
				return GIT_EEOF;
			}

			continue;
		}

		gitno_consume(buf, line_end);
		if (pkt->type == GIT_PKT_ERR) {
			giterr_set(GITERR_NET, "Remote error: %s", ((git_pkt_err *)pkt)->error);
			git__free(pkt);
			return -1;
		}

		if (pkt->type != GIT_PKT_FLUSH && git_vector_insert(refs, pkt) < 0)
			return -1;

		if (pkt->type == GIT_PKT_FLUSH) {
			flush++;
			git_pkt_free(pkt);
		}
	} while (flush < flushes);

	return flush;
}