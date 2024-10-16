struct sk_buff *sock_omalloc(struct sock *sk, unsigned long size,
			     gfp_t priority)
{
	struct sk_buff *skb;

	/* small safe race: SKB_TRUESIZE may differ from final skb->truesize */
	if (atomic_read(&sk->sk_omem_alloc) + SKB_TRUESIZE(size) >
	    sysctl_optmem_max)
		return NULL;

	skb = alloc_skb(size, priority);
	if (!skb)
		return NULL;

	atomic_add(skb->truesize, &sk->sk_omem_alloc);
	skb->sk = sk;
	skb->destructor = sock_ofree;
	return skb;
}