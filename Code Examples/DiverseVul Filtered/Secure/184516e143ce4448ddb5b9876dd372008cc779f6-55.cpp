static int validate_nonce(
	const char *		pnonce,
	struct recvbuf *	rbufp
	)
{
	u_int	ts_i;
	u_int	ts_f;
	l_fp	ts;
	l_fp	now_delta;
	u_int	supposed;
	u_int	derived;

	if (3 != sscanf(pnonce, "%08x%08x%08x", &ts_i, &ts_f, &supposed))
		return FALSE;

	ts.l_ui = (u_int32)ts_i;
	ts.l_uf = (u_int32)ts_f;
	derived = derive_nonce(&rbufp->recv_srcadr, ts.l_ui, ts.l_uf);
	get_systime(&now_delta);
	L_SUB(&now_delta, &ts);

	return (supposed == derived && now_delta.l_ui < 16);
}