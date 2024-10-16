static int pptp_connect(struct triton_md_handler_t *h)
{
  struct sockaddr_in addr;
	socklen_t size = sizeof(addr);
	int sock;
	struct pptp_conn_t *conn;

	while(1) {
		sock = accept(h->fd, (struct sockaddr *)&addr, &size);
		if (sock < 0) {
			if (errno == EAGAIN)
				return 0;
			log_error("pptp: accept failed: %s\n", strerror(errno));
			continue;
		}

		if (ap_shutdown) {
			close(sock);
			continue;
		}

		if (conf_max_starting && ap_session_stat.starting >= conf_max_starting) {
			close(sock);
			continue;
		}

		if (conf_max_sessions && ap_session_stat.active + ap_session_stat.starting >= conf_max_sessions) {
			close(sock);
			continue;
		}

		if (triton_module_loaded("connlimit") && connlimit_check(cl_key_from_ipv4(addr.sin_addr.s_addr))) {
			close(sock);
			continue;
		}

		log_info2("pptp: new connection from %s\n", inet_ntoa(addr.sin_addr));

		if (iprange_client_check(addr.sin_addr.s_addr)) {
			log_warn("pptp: IP is out of client-ip-range, droping connection...\n");
			close(sock);
			continue;
		}

		if (fcntl(sock, F_SETFL, O_NONBLOCK)) {
			log_error("pptp: failed to set nonblocking mode: %s, closing connection...\n", strerror(errno));
			close(sock);
			continue;
		}

		conn = mempool_alloc(conn_pool);
		memset(conn, 0, sizeof(*conn));
		conn->hnd.fd = sock;
		conn->hnd.read = pptp_read;
		conn->hnd.write = pptp_write;
		conn->ctx.close = pptp_close;
		conn->ctx.before_switch = pptp_ctx_switch;
		conn->in_buf = _malloc(PPTP_CTRL_SIZE_MAX);
		conn->out_buf = _malloc(PPTP_CTRL_SIZE_MAX);
		conn->timeout_timer.expire = pptp_timeout;
		conn->timeout_timer.period = conf_timeout * 1000;
		conn->echo_timer.expire = pptp_send_echo;
		conn->ctrl.ctx = &conn->ctx;
		conn->ctrl.started = ppp_started;
		conn->ctrl.finished = ppp_finished;
		conn->ctrl.terminate = ppp_terminate;
		conn->ctrl.max_mtu = conf_ppp_max_mtu;
		conn->ctrl.type = CTRL_TYPE_PPTP;
		conn->ctrl.ppp = 1;
		conn->ctrl.name = "pptp";
		conn->ctrl.ifname = "";
		conn->ctrl.mppe = conf_mppe;

		conn->ctrl.calling_station_id = _malloc(17);
		conn->ctrl.called_station_id = _malloc(17);
		u_inet_ntoa(addr.sin_addr.s_addr, conn->ctrl.calling_station_id);
		getsockname(sock, &addr, &size);
		u_inet_ntoa(addr.sin_addr.s_addr, conn->ctrl.called_station_id);

		ppp_init(&conn->ppp);
		conn->ppp.ses.ctrl = &conn->ctrl;

		if (conf_ip_pool)
			conn->ppp.ses.ipv4_pool_name = _strdup(conf_ip_pool);
		if (conf_ipv6_pool)
			conn->ppp.ses.ipv6_pool_name = _strdup(conf_ipv6_pool);
		if (conf_dpv6_pool)
			conn->ppp.ses.dpv6_pool_name = _strdup(conf_dpv6_pool);
		if (conf_ifname)
			conn->ppp.ses.ifname_rename = _strdup(conf_ifname);

		triton_context_register(&conn->ctx, &conn->ppp.ses);
		triton_md_register_handler(&conn->ctx, &conn->hnd);
		triton_md_enable_handler(&conn->hnd,MD_MODE_READ);
		triton_timer_add(&conn->ctx, &conn->timeout_timer, 0);
		triton_context_wakeup(&conn->ctx);

		triton_event_fire(EV_CTRL_STARTING, &conn->ppp.ses);

		__sync_add_and_fetch(&stat_starting, 1);
	}
	return 0;
}