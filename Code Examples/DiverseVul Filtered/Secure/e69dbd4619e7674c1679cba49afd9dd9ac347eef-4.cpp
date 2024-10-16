static int ieee802154_sock_sendmsg(struct socket *sock, struct msghdr *msg,
				   size_t len)
{
	struct sock *sk = sock->sk;

	return sk->sk_prot->sendmsg(sk, msg, len);
}