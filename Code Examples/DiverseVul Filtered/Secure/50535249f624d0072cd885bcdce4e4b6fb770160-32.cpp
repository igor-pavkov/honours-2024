static int qrtr_bind(struct socket *sock, struct sockaddr *saddr, int len)
{
	DECLARE_SOCKADDR(struct sockaddr_qrtr *, addr, saddr);
	struct qrtr_sock *ipc = qrtr_sk(sock->sk);
	struct sock *sk = sock->sk;
	int rc;

	if (len < sizeof(*addr) || addr->sq_family != AF_QIPCRTR)
		return -EINVAL;

	if (addr->sq_node != ipc->us.sq_node)
		return -EINVAL;

	lock_sock(sk);
	rc = __qrtr_bind(sock, addr, sock_flag(sk, SOCK_ZAPPED));
	release_sock(sk);

	return rc;
}