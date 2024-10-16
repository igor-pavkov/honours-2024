static int pptp_stop_ctrl_conn_rply(struct pptp_conn_t *conn)
{
	struct pptp_stop_ctrl_conn *msg = (struct pptp_stop_ctrl_conn*)conn->in_buf;
	if (conf_verbose)
		log_ppp_info2("recv [PPTP Stop-Ctrl-Conn-Reply <Result %i> <Error %i>]\n", msg->reason_result, msg->error_code);
	return -1;
}