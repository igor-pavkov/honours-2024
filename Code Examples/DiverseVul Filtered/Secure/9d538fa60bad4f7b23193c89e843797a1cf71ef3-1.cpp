int __sk_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
	int ret;
	unsigned int noreclaim_flag;

	/* these should have been dropped before queueing */
	BUG_ON(!sock_flag(sk, SOCK_MEMALLOC));

	noreclaim_flag = memalloc_noreclaim_save();
	ret = sk->sk_backlog_rcv(sk, skb);
	memalloc_noreclaim_restore(noreclaim_flag);

	return ret;
}