static inline u8 llc_ui_header_len(struct sock *sk, struct sockaddr_llc *addr)
{
	u8 rc = LLC_PDU_LEN_U;

	if (addr->sllc_test || addr->sllc_xid)
		rc = LLC_PDU_LEN_U;
	else if (sk->sk_type == SOCK_STREAM)
		rc = LLC_PDU_LEN_I;
	return rc;
}