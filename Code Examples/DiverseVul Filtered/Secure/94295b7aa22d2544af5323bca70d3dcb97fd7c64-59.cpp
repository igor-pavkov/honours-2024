NTSTATUS cli_session_setup_recv(struct tevent_req *req)
{
	return tevent_req_simple_recv_ntstatus(req);
}