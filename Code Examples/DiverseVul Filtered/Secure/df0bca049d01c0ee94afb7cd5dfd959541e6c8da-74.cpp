void sock_wfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;

	/* In case it might be waiting for more memory. */
	atomic_sub(skb->truesize, &sk->sk_wmem_alloc);
	if (!sock_flag(sk, SOCK_USE_WRITE_QUEUE))
		sk->sk_write_space(sk);
	sock_put(sk);
}