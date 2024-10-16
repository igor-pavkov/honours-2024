static int pptp_stop_ctrl_conn_rqst(struct pptp_conn_t *conn)
{
	struct pptp_stop_ctrl_conn *msg = (struct pptp_stop_ctrl_conn *)conn->in_buf;
	if (conf_verbose)
		log_ppp_info2("recv [PPTP Stop-Ctrl-Conn-Request <Reason %i>]\n", msg->reason_result);

	send_pptp_stop_ctrl_conn_rply(conn, PPTP_CONN_STOP_OK, 0);

	return -1;
}