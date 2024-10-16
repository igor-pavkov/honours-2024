static int pep_sock_connect(struct sock *sk, struct sockaddr *addr, int len)
{
	struct pep_sock *pn = pep_sk(sk);
	int err;
	u8 data[4] = { 0 /* sub-blocks */, PAD, PAD, PAD };

	if (pn->pipe_handle == PN_PIPE_INVALID_HANDLE)
		pn->pipe_handle = 1; /* anything but INVALID_HANDLE */

	err = pipe_handler_request(sk, PNS_PEP_CONNECT_REQ,
				pn->init_enable, data, 4);
	if (err) {
		pn->pipe_handle = PN_PIPE_INVALID_HANDLE;
		return err;
	}

	sk->sk_state = TCP_SYN_SENT;

	return 0;
}