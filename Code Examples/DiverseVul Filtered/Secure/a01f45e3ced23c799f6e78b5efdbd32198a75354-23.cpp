static int h2_init(struct connection *conn, struct proxy *prx, struct session *sess)
{
	struct h2c *h2c;
	struct task *t = NULL;

	h2c = pool_alloc(pool_head_h2c);
	if (!h2c)
		goto fail_no_h2c;

	if (conn->ctx) {
		h2c->flags = H2_CF_IS_BACK;
		h2c->shut_timeout = h2c->timeout = prx->timeout.server;
		if (tick_isset(prx->timeout.serverfin))
			h2c->shut_timeout = prx->timeout.serverfin;
	} else {
		h2c->flags = H2_CF_NONE;
		h2c->shut_timeout = h2c->timeout = prx->timeout.client;
		if (tick_isset(prx->timeout.clientfin))
			h2c->shut_timeout = prx->timeout.clientfin;
	}

	h2c->proxy = prx;
	h2c->task = NULL;
	if (tick_isset(h2c->timeout)) {
		t = task_new(tid_bit);
		if (!t)
			goto fail;

		h2c->task = t;
		t->process = h2_timeout_task;
		t->context = h2c;
		t->expire = tick_add(now_ms, h2c->timeout);
	}

	h2c->wait_event.task = tasklet_new();
	if (!h2c->wait_event.task)
		goto fail;
	h2c->wait_event.task->process = h2_io_cb;
	h2c->wait_event.task->context = h2c;
	h2c->wait_event.events = 0;

	h2c->ddht = hpack_dht_alloc(h2_settings_header_table_size);
	if (!h2c->ddht)
		goto fail;

	/* Initialise the context. */
	h2c->st0 = H2_CS_PREFACE;
	h2c->conn = conn;
	h2c->max_id = -1;
	h2c->errcode = H2_ERR_NO_ERROR;
	h2c->rcvd_c = 0;
	h2c->rcvd_s = 0;
	h2c->nb_streams = 0;
	h2c->nb_cs = 0;

	h2c->dbuf = BUF_NULL;
	h2c->dsi = -1;
	h2c->msi = -1;
	h2c->last_sid = -1;

	h2c->mbuf = BUF_NULL;
	h2c->miw = 65535; /* mux initial window size */
	h2c->mws = 65535; /* mux window size */
	h2c->mfs = 16384; /* initial max frame size */
	h2c->streams_by_id = EB_ROOT;
	LIST_INIT(&h2c->send_list);
	LIST_INIT(&h2c->fctl_list);
	LIST_INIT(&h2c->sending_list);
	LIST_INIT(&h2c->buf_wait.list);

	if (t)
		task_queue(t);

	if (h2c->flags & H2_CF_IS_BACK) {
		/* FIXME: this is temporary, for outgoing connections we need
		 * to immediately allocate a stream until the code is modified
		 * so that the caller calls ->attach(). For now the outgoing cs
		 * is stored as conn->ctx by the caller.
		 */
		struct h2s *h2s;

		h2s = h2c_bck_stream_new(h2c, conn->ctx, sess);
		if (!h2s)
			goto fail_stream;
	}

	conn->ctx = h2c;

	/* prepare to read something */
	h2c_restart_reading(h2c);
	return 0;
  fail_stream:
	hpack_dht_free(h2c->ddht);
  fail:
	if (t)
		task_free(t);
	if (h2c->wait_event.task)
		tasklet_free(h2c->wait_event.task);
	pool_free(pool_head_h2c, h2c);
  fail_no_h2c:
	return -1;
}