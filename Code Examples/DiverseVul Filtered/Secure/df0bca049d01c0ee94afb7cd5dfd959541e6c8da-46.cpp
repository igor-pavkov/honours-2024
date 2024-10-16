void sk_free(struct sock *sk)
{
	struct sk_filter *filter;

	if (sk->sk_destruct)
		sk->sk_destruct(sk);

	filter = rcu_dereference(sk->sk_filter);
	if (filter) {
		sk_filter_uncharge(sk, filter);
		rcu_assign_pointer(sk->sk_filter, NULL);
	}

	sock_disable_timestamp(sk);

	if (atomic_read(&sk->sk_omem_alloc))
		printk(KERN_DEBUG "%s: optmem leakage (%d bytes) detected.\n",
		       __func__, atomic_read(&sk->sk_omem_alloc));

	put_net(sock_net(sk));
	sk_prot_free(sk->sk_prot_creator, sk);
}