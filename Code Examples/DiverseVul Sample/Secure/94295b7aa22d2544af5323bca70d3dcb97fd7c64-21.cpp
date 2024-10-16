static void cli_tree_connect_andx_done(struct tevent_req *subreq)
{
	tevent_req_simple_finish_ntstatus(
		subreq, cli_tcon_andx_recv(subreq));
}