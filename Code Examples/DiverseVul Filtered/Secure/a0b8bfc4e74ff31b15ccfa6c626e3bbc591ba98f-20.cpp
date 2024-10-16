static int pptp_start_ctrl_conn_rqst(struct pptp_conn_t *conn)
{
	struct pptp_start_ctrl_conn *msg = (struct pptp_start_ctrl_conn *)conn->in_buf;

	if (conf_verbose)
		log_ppp_info2("recv [PPTP Start-Ctrl-Conn-Request <Version %i> <Framing %x> <Bearer %x> <Max-Chan %i>]\n", msg->version, ntohl(msg->framing_cap), ntohl(msg->bearer_cap), ntohs(msg->max_channels));

	if (conn->state != STATE_IDLE) {
		log_ppp_warn("unexpected PPTP_START_CTRL_CONN_RQST\n");
		if (send_pptp_start_ctrl_conn_rply(conn, PPTP_CONN_RES_EXISTS, 0))
			return -1;
		return 0;
	}

	if (msg->version != htons(PPTP_VERSION)) {
		log_ppp_warn("PPTP version mismatch: expecting %x, received %" PRIu32 "\n", PPTP_VERSION, msg->version);
		if (send_pptp_start_ctrl_conn_rply(conn, PPTP_CONN_RES_PROTOCOL, 0))
			return -1;
		return 0;
	}
	/*if (!(ntohl(msg->framing_cap) & PPTP_FRAME_SYNC)) {
		log_ppp_warn("connection does not supports sync mode\n");
		if (send_pptp_start_ctrl_conn_rply(conn, PPTP_CONN_RES_GE, 0))
			return -1;
		return 0;
	}*/
	if (send_pptp_start_ctrl_conn_rply(conn, PPTP_CONN_RES_SUCCESS, 0))
		return -1;

	if (conn->timeout_timer.tpd)
		triton_timer_mod(&conn->timeout_timer, 0);

	conn->state = STATE_ESTB;

	return 0;
}