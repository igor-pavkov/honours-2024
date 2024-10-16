static void h2_release(struct connection *conn)
{
	struct h2c *h2c = conn->ctx;

	LIST_DEL(&conn->list);

	if (h2c) {
		hpack_dht_free(h2c->ddht);

		HA_SPIN_LOCK(BUF_WQ_LOCK, &buffer_wq_lock);
		LIST_DEL(&h2c->buf_wait.list);
		HA_SPIN_UNLOCK(BUF_WQ_LOCK, &buffer_wq_lock);

		h2_release_buf(h2c, &h2c->dbuf);
		h2_release_buf(h2c, &h2c->mbuf);

		if (h2c->task) {
			h2c->task->context = NULL;
			task_wakeup(h2c->task, TASK_WOKEN_OTHER);
			h2c->task = NULL;
		}
		if (h2c->wait_event.task)
			tasklet_free(h2c->wait_event.task);
		if (h2c->wait_event.events != 0)
			conn->xprt->unsubscribe(conn, h2c->wait_event.events,
			    &h2c->wait_event);

		pool_free(pool_head_h2c, h2c);
	}

	conn->mux = NULL;
	conn->ctx = NULL;

	conn_stop_tracking(conn);
	conn_full_close(conn);
	if (conn->destroy_cb)
		conn->destroy_cb(conn);
	conn_free(conn);
}