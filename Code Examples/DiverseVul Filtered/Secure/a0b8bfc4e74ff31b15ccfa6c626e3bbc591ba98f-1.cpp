static int pptp_echo_rply(struct pptp_conn_t *conn)
{
	struct pptp_echo_rply *msg = (struct pptp_echo_rply *)conn->in_buf;

	if (conf_verbose)
		log_ppp_debug("recv [PPTP Echo-Reply <Identifier %x>]\n", msg->identifier);

	conn->echo_sent = 0;
	return 0;
}