static int sco_sock_create(struct net *net, struct socket *sock, int protocol,
			   int kern)
{
	struct sock *sk;

	BT_DBG("sock %p", sock);

	sock->state = SS_UNCONNECTED;

	if (sock->type != SOCK_SEQPACKET)
		return -ESOCKTNOSUPPORT;

	sock->ops = &sco_sock_ops;

	sk = sco_sock_alloc(net, sock, protocol, GFP_ATOMIC, kern);
	if (!sk)
		return -ENOMEM;

	sco_sock_init(sk, NULL);
	return 0;
}