static int send_pptp_out_call_rply(struct pptp_conn_t *conn, struct pptp_out_call_rqst *rqst, int call_id, int res_code, int err_code)
{
	struct pptp_out_call_rply msg = {
		.header = PPTP_HEADER_CTRL(PPTP_OUT_CALL_RPLY),
		.call_id = htons(call_id),
		.call_id_peer = rqst->call_id,
		.result_code = res_code,
		.error_code = err_code,
		.cause_code = 0,
		.speed = rqst->bps_max,
		.recv_size = rqst->recv_size,
		.delay = 0,
		.channel = 0,
	};

	if (conf_verbose)
		log_ppp_info2("send [PPTP Outgoing-Call-Reply <Call-ID %x> <Peer-Call-ID %x> <Result %i> <Error %i> <Cause %i> <Speed %i> <Window-Size %i> <Delay %i> <Channel %x>]\n", ntohs(msg.call_id), ntohs(msg.call_id_peer), msg.result_code, msg.error_code, ntohs(msg.cause_code), ntohl(msg.speed), ntohs(msg.recv_size), ntohs(msg.delay), ntohl(msg.channel));

	return post_msg(conn, &msg, sizeof(msg));
}