static int sctp_getname(struct socket *sock, struct sockaddr *uaddr,
			int *uaddr_len, int peer)
{
	int rc;

	rc = inet6_getname(sock, uaddr, uaddr_len, peer);

	if (rc != 0)
		return rc;

	*uaddr_len = sctp_v6_addr_to_user(sctp_sk(sock->sk),
					  (union sctp_addr *)uaddr);

	return rc;
}