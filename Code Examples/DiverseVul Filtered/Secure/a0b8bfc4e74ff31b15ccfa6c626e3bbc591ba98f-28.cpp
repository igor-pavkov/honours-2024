static void pptp_send_echo(struct triton_timer_t *t)
{
	struct pptp_conn_t *conn = container_of(t, typeof(*conn), echo_timer);
	struct pptp_echo_rqst msg = {
		.header = PPTP_HEADER_CTRL(PPTP_ECHO_RQST),
	};

	if (++conn->echo_sent == conf_echo_failure) {
		log_ppp_warn("pptp: no echo reply\n");
		disconnect(conn);
		return;
	}

	msg.identifier = random();

	if (conf_verbose)
		log_ppp_debug("send [PPTP Echo-Request <Identifier %x>]\n", msg.identifier);

	if (post_msg(conn, &msg, sizeof(msg)))
		disconnect(conn);
}