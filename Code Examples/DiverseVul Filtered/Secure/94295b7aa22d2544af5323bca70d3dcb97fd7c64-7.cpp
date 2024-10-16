NTSTATUS cli_tcon_andx_recv(struct tevent_req *req)
{
	return tevent_req_simple_recv_ntstatus(req);
}