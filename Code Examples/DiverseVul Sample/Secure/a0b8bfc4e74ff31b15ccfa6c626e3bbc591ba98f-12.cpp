static int pptp_out_call_rqst(struct pptp_conn_t *conn)
{
	struct pptp_out_call_rqst *msg = (struct pptp_out_call_rqst *)conn->in_buf;
	struct sockaddr_pppox src_addr, dst_addr;
  struct sockaddr_in addr;
	socklen_t addrlen;
	int pptp_sock;

	if (conf_verbose)
		log_ppp_info2("recv [PPTP Outgoing-Call-Request <Call-ID %x> <Call-Serial %x> <Min-BPS %i> <Max-BPS %i> <Bearer %x> <Framing %x> <Window-Size %i> <Delay %i>]\n", ntohs(msg->call_id), ntohs(msg->call_sernum), ntohl(msg->bps_min), ntohl(msg->bps_max), ntohl(msg->bearer), ntohl(msg->framing), ntohs(msg->recv_size), ntohs(msg->delay));

	if (conn->state != STATE_ESTB) {
		log_ppp_warn("unexpected PPTP_OUT_CALL_RQST\n");
		if (send_pptp_out_call_rply(conn, msg, 0, PPTP_CALL_RES_GE, PPTP_GE_NOCONN))
			return -1;
		return 0;
	}

	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.sa_family = AF_PPPOX;
	src_addr.sa_protocol = PX_PROTO_PPTP;
	src_addr.sa_addr.pptp.call_id = 0;
	addrlen = sizeof(addr);
	getsockname(conn->hnd.fd, (struct sockaddr*)&addr, &addrlen);
	src_addr.sa_addr.pptp.sin_addr = addr.sin_addr;

	memset(&dst_addr, 0, sizeof(dst_addr));
	dst_addr.sa_family = AF_PPPOX;
	dst_addr.sa_protocol = PX_PROTO_PPTP;
	dst_addr.sa_addr.pptp.call_id = htons(msg->call_id);
	addrlen = sizeof(addr);
	getpeername(conn->hnd.fd, (struct sockaddr*)&addr, &addrlen);
	dst_addr.sa_addr.pptp.sin_addr = addr.sin_addr;

	pptp_sock = socket(AF_PPPOX, SOCK_STREAM, PX_PROTO_PPTP);
	if (pptp_sock < 0) {
		log_ppp_error("failed to create PPTP socket (%s)\n", strerror(errno));
		return -1;
	}

	fcntl(pptp_sock, F_SETFD, fcntl(pptp_sock, F_GETFD) | FD_CLOEXEC);

	if (bind(pptp_sock, (struct sockaddr*)&src_addr, sizeof(src_addr))) {
		log_ppp_error("failed to bind PPTP socket (%s)\n", strerror(errno));
		close(pptp_sock);
		return -1;
	}
	addrlen = sizeof(src_addr);
	getsockname(pptp_sock, (struct sockaddr*)&src_addr, &addrlen);

	if (connect(pptp_sock, (struct sockaddr*)&dst_addr, sizeof(dst_addr))) {
		log_ppp_error("failed to connect PPTP socket (%s)\n", strerror(errno));
		close(pptp_sock);
		return -1;
	}

	if (send_pptp_out_call_rply(conn, msg, src_addr.sa_addr.pptp.call_id, PPTP_CALL_RES_OK, 0))
		return -1;

	conn->call_id = src_addr.sa_addr.pptp.call_id;
	conn->peer_call_id = msg->call_id;
	conn->ppp.fd = pptp_sock;
	conn->ppp.ses.chan_name = _strdup(inet_ntoa(dst_addr.sa_addr.pptp.sin_addr));

	triton_event_fire(EV_CTRL_STARTED, &conn->ppp.ses);

	if (establish_ppp(&conn->ppp)) {
		close(pptp_sock);
		//if (send_pptp_stop_ctrl_conn_rqst(conn, 0, 0))
		conn->state = STATE_FIN;
		return -1;
	}
	conn->state = STATE_PPP;
	__sync_sub_and_fetch(&stat_starting, 1);
	__sync_add_and_fetch(&stat_active, 1);

	if (conn->timeout_timer.tpd)
		triton_timer_del(&conn->timeout_timer);

	if (conf_echo_interval) {
		conn->echo_timer.period = conf_echo_interval * 1000;
		triton_timer_add(&conn->ctx, &conn->echo_timer, 0);
	}

	return 0;
}