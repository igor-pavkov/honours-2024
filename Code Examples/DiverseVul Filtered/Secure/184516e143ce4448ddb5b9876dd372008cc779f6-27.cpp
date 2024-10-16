static void generate_nonce(
	struct recvbuf *	rbufp,
	char *			nonce,
	size_t			nonce_octets
	)
{
	u_int32 derived;

	derived = derive_nonce(&rbufp->recv_srcadr,
			       rbufp->recv_time.l_ui,
			       rbufp->recv_time.l_uf);
	snprintf(nonce, nonce_octets, "%08x%08x%08x",
		 rbufp->recv_time.l_ui, rbufp->recv_time.l_uf, derived);
}