void x25_limit_facilities(struct x25_facilities *facilities,
			  struct x25_neigh *nb)
{

	if (!nb->extended) {
		if (facilities->winsize_in  > 7) {
			pr_debug("incoming winsize limited to 7\n");
			facilities->winsize_in = 7;
		}
		if (facilities->winsize_out > 7) {
			facilities->winsize_out = 7;
			pr_debug("outgoing winsize limited to 7\n");
		}
	}
}