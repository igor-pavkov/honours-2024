static int qrtr_connect(struct socket *sock, struct sockaddr *saddr,
			int len, int flags)
{
	DECLARE_SOCKADDR(struct sockaddr_qrtr *, addr, saddr);
	struct qrtr_sock *ipc = qrtr_sk(sock->sk);
	struct sock *sk = sock->sk;
	int rc;

	if (len < sizeof(*addr) || addr->sq_family != AF_QIPCRTR)
		return -EINVAL;

	lock_sock(sk);

	sk->sk_state = TCP_CLOSE;
	sock->state = SS_UNCONNECTED;

	rc = qrtr_autobind(sock);
	if (rc) {
		release_sock(sk);
		return rc;
	}

	ipc->peer = *addr;
	sock->state = SS_CONNECTED;
	sk->sk_state = TCP_ESTABLISHED;

	release_sock(sk);

	return 0;
}