static struct sock *rose_make_new(struct sock *osk)
{
	struct sock *sk;
	struct rose_sock *rose, *orose;

	if (osk->sk_type != SOCK_SEQPACKET)
		return NULL;

	sk = sk_alloc(sock_net(osk), PF_ROSE, GFP_ATOMIC, &rose_proto);
	if (sk == NULL)
		return NULL;

	rose = rose_sk(sk);

	sock_init_data(NULL, sk);

	skb_queue_head_init(&rose->ack_queue);
#ifdef M_BIT
	skb_queue_head_init(&rose->frag_queue);
	rose->fraglen  = 0;
#endif

	sk->sk_type     = osk->sk_type;
	sk->sk_priority = osk->sk_priority;
	sk->sk_protocol = osk->sk_protocol;
	sk->sk_rcvbuf   = osk->sk_rcvbuf;
	sk->sk_sndbuf   = osk->sk_sndbuf;
	sk->sk_state    = TCP_ESTABLISHED;
	sock_copy_flags(sk, osk);

	init_timer(&rose->timer);
	init_timer(&rose->idletimer);

	orose		= rose_sk(osk);
	rose->t1	= orose->t1;
	rose->t2	= orose->t2;
	rose->t3	= orose->t3;
	rose->hb	= orose->hb;
	rose->idle	= orose->idle;
	rose->defer	= orose->defer;
	rose->device	= orose->device;
	rose->qbitincl	= orose->qbitincl;

	return sk;
}