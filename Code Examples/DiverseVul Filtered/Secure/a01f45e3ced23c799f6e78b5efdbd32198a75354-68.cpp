static struct task *h2_timeout_task(struct task *t, void *context, unsigned short state)
{
	struct h2c *h2c = context;
	int expired = tick_is_expired(t->expire, now_ms);

	if (!expired && h2c)
		return t;

	task_delete(t);
	task_free(t);

	if (!h2c) {
		/* resources were already deleted */
		return NULL;
	}

	h2c->task = NULL;
	h2c_error(h2c, H2_ERR_NO_ERROR);
	h2_wake_some_streams(h2c, 0, 0);

	if (b_data(&h2c->mbuf)) {
		/* don't even try to send a GOAWAY, the buffer is stuck */
		h2c->flags |= H2_CF_GOAWAY_FAILED;
	}

	/* try to send but no need to insist */
	h2c->last_sid = h2c->max_id;
	if (h2c_send_goaway_error(h2c, NULL) <= 0)
		h2c->flags |= H2_CF_GOAWAY_FAILED;

	if (b_data(&h2c->mbuf) && !(h2c->flags & H2_CF_GOAWAY_FAILED) && conn_xprt_ready(h2c->conn)) {
		int ret = h2c->conn->xprt->snd_buf(h2c->conn, &h2c->mbuf, b_data(&h2c->mbuf), 0);
		if (ret > 0) {
			b_del(&h2c->mbuf, ret);
			b_realign_if_empty(&h2c->mbuf);
		}
	}

	/* either we can release everything now or it will be done later once
	 * the last stream closes.
	 */
	if (eb_is_empty(&h2c->streams_by_id))
		h2_release(h2c->conn);

	return NULL;
}