static int ieee802154_sock_connect(struct socket *sock, struct sockaddr *uaddr,
				   int addr_len, int flags)
{
	struct sock *sk = sock->sk;

	if (addr_len < sizeof(uaddr->sa_family))
		return -EINVAL;

	if (uaddr->sa_family == AF_UNSPEC)
		return sk->sk_prot->disconnect(sk, flags);

	return sk->sk_prot->connect(sk, uaddr, addr_len);
}