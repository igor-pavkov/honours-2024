static int process_packet(struct pptp_conn_t *conn)
{
	struct pptp_header *hdr = (struct pptp_header *)conn->in_buf;
	switch(ntohs(hdr->ctrl_type))
	{
		case PPTP_START_CTRL_CONN_RQST:
			return pptp_start_ctrl_conn_rqst(conn);
		case PPTP_STOP_CTRL_CONN_RQST:
			return pptp_stop_ctrl_conn_rqst(conn);
		case PPTP_STOP_CTRL_CONN_RPLY:
			return pptp_stop_ctrl_conn_rply(conn);
		case PPTP_OUT_CALL_RQST:
			return pptp_out_call_rqst(conn);
		case PPTP_ECHO_RQST:
			return pptp_echo_rqst(conn);
		case PPTP_ECHO_RPLY:
			return pptp_echo_rply(conn);
		case PPTP_CALL_CLEAR_RQST:
			return pptp_call_clear_rqst(conn);
		case PPTP_SET_LINK_INFO:
			if (conf_verbose)
				log_ppp_info2("recv [PPTP Set-Link-Info]\n");
			return 0;
		default:
			log_ppp_warn("recv [PPTP Unknown (%x)]\n", ntohs(hdr->ctrl_type));
	}
	return 0;
}