static void setup_new_vc_session(void)
{
	char addr[INET6_ADDRSTRLEN];

	DEBUG(2,("setup_new_vc_session: New VC == 0, if NT4.x "
		"compatible we would close all old resources.\n"));
#if 0
	conn_close_all();
	invalidate_all_vuids();
#endif
	if (lp_reset_on_zero_vc()) {
		connections_forall(shutdown_other_smbds,
			CONST_DISCARD(void *,
			client_addr(get_client_fd(),addr,sizeof(addr))));
	}
}