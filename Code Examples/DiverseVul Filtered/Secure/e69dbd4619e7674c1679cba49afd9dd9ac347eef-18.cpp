static int ieee802154_sock_bind(struct socket *sock, struct sockaddr *uaddr,
				int addr_len)
{
	struct sock *sk = sock->sk;

	if (sk->sk_prot->bind)
		return sk->sk_prot->bind(sk, uaddr, addr_len);

	return sock_no_bind(sock, uaddr, addr_len);
}