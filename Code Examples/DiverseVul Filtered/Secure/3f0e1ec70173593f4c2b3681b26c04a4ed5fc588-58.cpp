static int h2c_frt_init(struct connection *conn)
{
	struct h2c *h2c;
	struct task *t = NULL;
	struct session *sess = conn->owner;

	h2c = pool_alloc(pool_head_h2c);
	if (!h2c)
		goto fail;


	h2c->shut_timeout = h2c->timeout = sess->fe->timeout.client;
	if (tick_isset(sess->fe->timeout.clientfin))
		h2c->shut_timeout = sess->fe->timeout.clientfin;

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

	h2c->ddht = hpack_dht_alloc(h2_settings_header_table_size);
	if (!h2c->ddht)
		goto fail;

	/* Initialise the context. */
	h2c->st0 = H2_CS_PREFACE;
	h2c->conn = conn;
	h2c->max_id = -1;
	h2c->errcode = H2_ERR_NO_ERROR;
	h2c->flags = H2_CF_NONE;
	h2c->rcvd_c = 0;
	h2c->rcvd_s = 0;
	h2c->nb_streams = 0;

	h2c->dbuf = &buf_empty;
	h2c->dsi = -1;
	h2c->msi = -1;
	h2c->last_sid = -1;

	h2c->mbuf = &buf_empty;
	h2c->miw = 65535; /* mux initial window size */
	h2c->mws = 65535; /* mux window size */
	h2c->mfs = 16384; /* initial max frame size */
	h2c->streams_by_id = EB_ROOT_UNIQUE;
	LIST_INIT(&h2c->send_list);
	LIST_INIT(&h2c->fctl_list);
	LIST_INIT(&h2c->buf_wait.list);
	conn->mux_ctx = h2c;

	if (t)
		task_queue(t);
	conn_xprt_want_recv(conn);

	/* mux->wake will be called soon to complete the operation */
	return 0;
 fail:
	if (t)
		task_free(t);
	pool_free(pool_head_h2c, h2c);
	return -1;
}