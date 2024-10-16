static void dns_udp_call_sendto_done(struct tevent_req *subreq)
{
	struct dns_udp_call *call = tevent_req_callback_data(subreq,
				       struct dns_udp_call);
	int sys_errno;

	tdgram_sendto_queue_recv(subreq, &sys_errno);

	/* We don't care about errors */

	talloc_free(call);
}