static int pep_sock_enable(struct sock *sk, struct sockaddr *addr, int len)
{
	int err;

	err = pipe_handler_request(sk, PNS_PEP_ENABLE_REQ, PAD,
				NULL, 0);
	if (err)
		return err;

	sk->sk_state = TCP_SYN_SENT;

	return 0;
}