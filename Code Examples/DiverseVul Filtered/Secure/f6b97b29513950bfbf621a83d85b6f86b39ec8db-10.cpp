static int nr_create(struct net *net, struct socket *sock, int protocol)
{
	struct sock *sk;
	struct nr_sock *nr;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	if (sock->type != SOCK_SEQPACKET || protocol != 0)
		return -ESOCKTNOSUPPORT;

	sk = sk_alloc(net, PF_NETROM, GFP_ATOMIC, &nr_proto);
	if (sk  == NULL)
		return -ENOMEM;

	nr = nr_sk(sk);

	sock_init_data(sock, sk);

	sock->ops    = &nr_proto_ops;
	sk->sk_protocol = protocol;

	skb_queue_head_init(&nr->ack_queue);
	skb_queue_head_init(&nr->reseq_queue);
	skb_queue_head_init(&nr->frag_queue);

	nr_init_timers(sk);

	nr->t1     =
		msecs_to_jiffies(sysctl_netrom_transport_timeout);
	nr->t2     =
		msecs_to_jiffies(sysctl_netrom_transport_acknowledge_delay);
	nr->n2     =
		msecs_to_jiffies(sysctl_netrom_transport_maximum_tries);
	nr->t4     =
		msecs_to_jiffies(sysctl_netrom_transport_busy_delay);
	nr->idle   =
		msecs_to_jiffies(sysctl_netrom_transport_no_activity_timeout);
	nr->window = sysctl_netrom_transport_requested_window_size;

	nr->bpqext = 1;
	nr->state  = NR_STATE_0;

	return 0;
}