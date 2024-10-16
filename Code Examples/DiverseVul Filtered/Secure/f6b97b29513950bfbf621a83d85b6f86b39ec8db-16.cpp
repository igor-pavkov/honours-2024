static struct sock *nr_make_new(struct sock *osk)
{
	struct sock *sk;
	struct nr_sock *nr, *onr;

	if (osk->sk_type != SOCK_SEQPACKET)
		return NULL;

	sk = sk_alloc(sock_net(osk), PF_NETROM, GFP_ATOMIC, osk->sk_prot);
	if (sk == NULL)
		return NULL;

	nr = nr_sk(sk);

	sock_init_data(NULL, sk);

	sk->sk_type     = osk->sk_type;
	sk->sk_priority = osk->sk_priority;
	sk->sk_protocol = osk->sk_protocol;
	sk->sk_rcvbuf   = osk->sk_rcvbuf;
	sk->sk_sndbuf   = osk->sk_sndbuf;
	sk->sk_state    = TCP_ESTABLISHED;
	sock_copy_flags(sk, osk);

	skb_queue_head_init(&nr->ack_queue);
	skb_queue_head_init(&nr->reseq_queue);
	skb_queue_head_init(&nr->frag_queue);

	nr_init_timers(sk);

	onr = nr_sk(osk);

	nr->t1      = onr->t1;
	nr->t2      = onr->t2;
	nr->n2      = onr->n2;
	nr->t4      = onr->t4;
	nr->idle    = onr->idle;
	nr->window  = onr->window;

	nr->device  = onr->device;
	nr->bpqext  = onr->bpqext;

	return sk;
}