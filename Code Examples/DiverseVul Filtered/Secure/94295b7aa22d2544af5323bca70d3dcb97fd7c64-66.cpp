static NTSTATUS cli_tree_connect_recv(struct tevent_req *req)
{
	return tevent_req_simple_recv_ntstatus(req);
}