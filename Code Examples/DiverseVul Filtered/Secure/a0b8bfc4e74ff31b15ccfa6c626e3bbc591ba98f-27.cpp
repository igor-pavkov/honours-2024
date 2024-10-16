static int pptp_echo_rqst(struct pptp_conn_t *conn)
{
	struct pptp_echo_rqst *in_msg = (struct pptp_echo_rqst *)conn->in_buf;
	struct pptp_echo_rply out_msg = {
		.header = PPTP_HEADER_CTRL(PPTP_ECHO_RPLY),
		.identifier = in_msg->identifier,
		.result_code = 1,
	};

	if (conf_verbose) {
		log_ppp_debug("recv [PPTP Echo-Request <Identifier %x>]\n", in_msg->identifier);
		log_ppp_debug("send [PPTP Echo-Reply <Identifier %x>]\n", out_msg.identifier);
	}

	if (conn->echo_timer.tpd)
		triton_timer_mod(&conn->echo_timer, 0);

	return post_msg(conn, &out_msg, sizeof(out_msg));
}