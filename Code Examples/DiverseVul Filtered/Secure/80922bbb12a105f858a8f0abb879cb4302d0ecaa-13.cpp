static int econet_create(struct net *net, struct socket *sock, int protocol)
{
	struct sock *sk;
	struct econet_sock *eo;
	int err;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	/* Econet only provides datagram services. */
	if (sock->type != SOCK_DGRAM)
		return -ESOCKTNOSUPPORT;

	sock->state = SS_UNCONNECTED;

	err = -ENOBUFS;
	sk = sk_alloc(net, PF_ECONET, GFP_KERNEL, &econet_proto);
	if (sk == NULL)
		goto out;

	sk->sk_reuse = 1;
	sock->ops = &econet_ops;
	sock_init_data(sock, sk);

	eo = ec_sk(sk);
	sock_reset_flag(sk, SOCK_ZAPPED);
	sk->sk_family = PF_ECONET;
	eo->num = protocol;

	econet_insert_socket(&econet_sklist, sk);
	return(0);
out:
	return err;
}