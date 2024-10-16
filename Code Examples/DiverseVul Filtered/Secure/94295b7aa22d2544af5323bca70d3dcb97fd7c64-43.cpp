static void cli_tree_connect_smb2_done(struct tevent_req *subreq)
{
	tevent_req_simple_finish_ntstatus(
		subreq, smb2cli_tcon_recv(subreq));
}