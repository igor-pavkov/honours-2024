static int rose_create(struct net *net, struct socket *sock, int protocol)
{
	struct sock *sk;
	struct rose_sock *rose;

	if (net != &init_net)
		return -EAFNOSUPPORT;

	if (sock->type != SOCK_SEQPACKET || protocol != 0)
		return -ESOCKTNOSUPPORT;

	sk = sk_alloc(net, PF_ROSE, GFP_ATOMIC, &rose_proto);
	if (sk == NULL)
		return -ENOMEM;

	rose = rose_sk(sk);

	sock_init_data(sock, sk);

	skb_queue_head_init(&rose->ack_queue);
#ifdef M_BIT
	skb_queue_head_init(&rose->frag_queue);
	rose->fraglen    = 0;
#endif

	sock->ops    = &rose_proto_ops;
	sk->sk_protocol = protocol;

	init_timer(&rose->timer);
	init_timer(&rose->idletimer);

	rose->t1   = msecs_to_jiffies(sysctl_rose_call_request_timeout);
	rose->t2   = msecs_to_jiffies(sysctl_rose_reset_request_timeout);
	rose->t3   = msecs_to_jiffies(sysctl_rose_clear_request_timeout);
	rose->hb   = msecs_to_jiffies(sysctl_rose_ack_hold_back_timeout);
	rose->idle = msecs_to_jiffies(sysctl_rose_no_activity_timeout);

	rose->state = ROSE_STATE_0;

	return 0;
}