static struct sock *sctp_v6_create_accept_sk(struct sock *sk,
					     struct sctp_association *asoc,
					     bool kern)
{
	struct sock *newsk;
	struct ipv6_pinfo *newnp, *np = inet6_sk(sk);
	struct sctp6_sock *newsctp6sk;
	struct ipv6_txoptions *opt;

	newsk = sk_alloc(sock_net(sk), PF_INET6, GFP_KERNEL, sk->sk_prot, kern);
	if (!newsk)
		goto out;

	sock_init_data(NULL, newsk);

	sctp_copy_sock(newsk, sk, asoc);
	sock_reset_flag(sk, SOCK_ZAPPED);

	newsctp6sk = (struct sctp6_sock *)newsk;
	inet_sk(newsk)->pinet6 = &newsctp6sk->inet6;

	sctp_sk(newsk)->v4mapped = sctp_sk(sk)->v4mapped;

	newnp = inet6_sk(newsk);

	memcpy(newnp, np, sizeof(struct ipv6_pinfo));
	newnp->ipv6_mc_list = NULL;
	newnp->ipv6_ac_list = NULL;
	newnp->ipv6_fl_list = NULL;

	rcu_read_lock();
	opt = rcu_dereference(np->opt);
	if (opt)
		opt = ipv6_dup_options(newsk, opt);
	RCU_INIT_POINTER(newnp->opt, opt);
	rcu_read_unlock();

	/* Initialize sk's sport, dport, rcv_saddr and daddr for getsockname()
	 * and getpeername().
	 */
	sctp_v6_to_sk_daddr(&asoc->peer.primary_addr, newsk);

	newsk->sk_v6_rcv_saddr = sk->sk_v6_rcv_saddr;

	sk_refcnt_debug_inc(newsk);

	if (newsk->sk_prot->init(newsk)) {
		sk_common_release(newsk);
		newsk = NULL;
	}

out:
	return newsk;
}