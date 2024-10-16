static void h2_detach(struct conn_stream *cs)
{
	struct h2s *h2s = cs->ctx;
	struct h2c *h2c;
	struct session *sess;

	cs->ctx = NULL;
	if (!h2s)
		return;

	sess = h2s->sess;
	h2c = h2s->h2c;
	h2s->cs = NULL;
	h2c->nb_cs--;
	if (h2c->flags & H2_CF_DEM_TOOMANY &&
	    !h2_has_too_many_cs(h2c)) {
		h2c->flags &= ~H2_CF_DEM_TOOMANY;
		h2c_restart_reading(h2c);
	}

	/* this stream may be blocked waiting for some data to leave (possibly
	 * an ES or RST frame), so orphan it in this case.
	 */
	if (!(cs->conn->flags & CO_FL_ERROR) &&
	    (h2c->st0 < H2_CS_ERROR) &&
	    (h2s->flags & (H2_SF_BLK_MBUSY | H2_SF_BLK_MROOM | H2_SF_BLK_MFCTL)))
		return;

	if ((h2c->flags & H2_CF_DEM_BLOCK_ANY && h2s->id == h2c->dsi) ||
	    (h2c->flags & H2_CF_MUX_BLOCK_ANY && h2s->id == h2c->msi)) {
		/* unblock the connection if it was blocked on this
		 * stream.
		 */
		h2c->flags &= ~H2_CF_DEM_BLOCK_ANY;
		h2c->flags &= ~H2_CF_MUX_BLOCK_ANY;
		h2c_restart_reading(h2c);
	}

	h2s_destroy(h2s);

	if (h2c->flags & H2_CF_IS_BACK &&
	    (h2c->proxy->options2 & PR_O2_USE_HTX)) {
		if (!(h2c->conn->flags &
		    (CO_FL_ERROR | CO_FL_SOCK_RD_SH | CO_FL_SOCK_WR_SH))) {
			if (!h2c->conn->owner) {
				h2c->conn->owner = sess;
				if (!session_add_conn(sess, h2c->conn, h2c->conn->target)) {
					h2c->conn->owner = NULL;
					if (eb_is_empty(&h2c->streams_by_id)) {
						if (!srv_add_to_idle_list(objt_server(h2c->conn->target), h2c->conn))
							/* The server doesn't want it, let's kill the connection right away */
							h2c->conn->mux->destroy(h2c->conn);
						return;
					}
				}
			}
			if (eb_is_empty(&h2c->streams_by_id)) {
				if (session_check_idle_conn(h2c->conn->owner, h2c->conn) != 0)
					/* At this point either the connection is destroyed, or it's been added to the server idle list, just stop */
					return;
			}
			/* Never ever allow to reuse a connection from a non-reuse backend */
			if ((h2c->proxy->options & PR_O_REUSE_MASK) == PR_O_REUSE_NEVR)
				h2c->conn->flags |= CO_FL_PRIVATE;
			if (LIST_ISEMPTY(&h2c->conn->list) && h2c->nb_streams < h2_settings_max_concurrent_streams) {
				struct server *srv = objt_server(h2c->conn->target);

				if (srv) {
					if (h2c->conn->flags & CO_FL_PRIVATE)
						LIST_ADD(&srv->priv_conns[tid], &h2c->conn->list);
					else
						LIST_ADD(&srv->idle_conns[tid], &h2c->conn->list);
				}

			}
		}
	}

	/* We don't want to close right now unless we're removing the
	 * last stream, and either the connection is in error, or it
	 * reached the ID already specified in a GOAWAY frame received
	 * or sent (as seen by last_sid >= 0).
	 */
	if (eb_is_empty(&h2c->streams_by_id) &&     /* don't close if streams exist */
	    ((h2c->conn->flags & CO_FL_ERROR) ||    /* errors close immediately */
	     (h2c->st0 >= H2_CS_ERROR && !h2c->task) || /* a timeout stroke earlier */
	     (h2c->flags & (H2_CF_GOAWAY_FAILED | H2_CF_GOAWAY_SENT)) ||
	     (!(h2c->conn->owner)) || /* Nobody's left to take care of the connection, drop it now */
	     (!b_data(&h2c->mbuf) &&  /* mux buffer empty, also process clean events below */
	      (conn_xprt_read0_pending(h2c->conn) ||
	       (h2c->last_sid >= 0 && h2c->max_id >= h2c->last_sid))))) {
		/* no more stream will come, kill it now */
		h2_release(h2c->conn);
	}
	else if (h2c->task) {
		if (eb_is_empty(&h2c->streams_by_id) || b_data(&h2c->mbuf)) {
			h2c->task->expire = tick_add(now_ms, h2c->last_sid < 0 ? h2c->timeout : h2c->shut_timeout);
			task_queue(h2c->task);
		}
		else
			h2c->task->expire = TICK_ETERNITY;
	}
}