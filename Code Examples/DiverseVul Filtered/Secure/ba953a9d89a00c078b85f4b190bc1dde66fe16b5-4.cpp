static int pfkey_create(struct net *net, struct socket *sock, int protocol,
			int kern)
{
	struct netns_pfkey *net_pfkey = net_generic(net, pfkey_net_id);
	struct sock *sk;
	struct pfkey_sock *pfk;

	if (!ns_capable(net->user_ns, CAP_NET_ADMIN))
		return -EPERM;
	if (sock->type != SOCK_RAW)
		return -ESOCKTNOSUPPORT;
	if (protocol != PF_KEY_V2)
		return -EPROTONOSUPPORT;

	sk = sk_alloc(net, PF_KEY, GFP_KERNEL, &key_proto, kern);
	if (sk == NULL)
		return -ENOMEM;

	pfk = pfkey_sk(sk);
	mutex_init(&pfk->dump_lock);

	sock->ops = &pfkey_ops;
	sock_init_data(sock, sk);

	sk->sk_family = PF_KEY;
	sk->sk_destruct = pfkey_sock_destruct;

	atomic_inc(&net_pfkey->socks_nr);

	pfkey_insert(sk);

	return 0;
}