static int tipc_sk_insert(struct tipc_sock *tsk)
{
	struct sock *sk = &tsk->sk;
	struct net *net = sock_net(sk);
	struct tipc_net *tn = net_generic(net, tipc_net_id);
	u32 remaining = (TIPC_MAX_PORT - TIPC_MIN_PORT) + 1;
	u32 portid = prandom_u32() % remaining + TIPC_MIN_PORT;

	while (remaining--) {
		portid++;
		if ((portid < TIPC_MIN_PORT) || (portid > TIPC_MAX_PORT))
			portid = TIPC_MIN_PORT;
		tsk->portid = portid;
		sock_hold(&tsk->sk);
		if (!rhashtable_lookup_insert_fast(&tn->sk_rht, &tsk->node,
						   tsk_rht_params))
			return 0;
		sock_put(&tsk->sk);
	}

	return -1;
}