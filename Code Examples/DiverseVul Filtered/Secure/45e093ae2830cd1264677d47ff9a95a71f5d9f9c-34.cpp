static int tipc_sk_create(struct net *net, struct socket *sock,
			  int protocol, int kern)
{
	struct tipc_net *tn;
	const struct proto_ops *ops;
	socket_state state;
	struct sock *sk;
	struct tipc_sock *tsk;
	struct tipc_msg *msg;

	/* Validate arguments */
	if (unlikely(protocol != 0))
		return -EPROTONOSUPPORT;

	switch (sock->type) {
	case SOCK_STREAM:
		ops = &stream_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_SEQPACKET:
		ops = &packet_ops;
		state = SS_UNCONNECTED;
		break;
	case SOCK_DGRAM:
	case SOCK_RDM:
		ops = &msg_ops;
		state = SS_READY;
		break;
	default:
		return -EPROTOTYPE;
	}

	/* Allocate socket's protocol area */
	sk = sk_alloc(net, AF_TIPC, GFP_KERNEL, &tipc_proto, kern);
	if (sk == NULL)
		return -ENOMEM;

	tsk = tipc_sk(sk);
	tsk->max_pkt = MAX_PKT_DEFAULT;
	INIT_LIST_HEAD(&tsk->publications);
	msg = &tsk->phdr;
	tn = net_generic(sock_net(sk), tipc_net_id);
	tipc_msg_init(tn->own_addr, msg, TIPC_LOW_IMPORTANCE, TIPC_NAMED_MSG,
		      NAMED_H_SIZE, 0);

	/* Finish initializing socket data structures */
	sock->ops = ops;
	sock->state = state;
	sock_init_data(sock, sk);
	if (tipc_sk_insert(tsk)) {
		pr_warn("Socket create failed; port numbrer exhausted\n");
		return -EINVAL;
	}
	msg_set_origport(msg, tsk->portid);
	setup_timer(&sk->sk_timer, tipc_sk_timeout, (unsigned long)tsk);
	sk->sk_backlog_rcv = tipc_backlog_rcv;
	sk->sk_rcvbuf = sysctl_tipc_rmem[1];
	sk->sk_data_ready = tipc_data_ready;
	sk->sk_write_space = tipc_write_space;
	sk->sk_destruct = tipc_sock_destruct;
	tsk->conn_timeout = CONN_TIMEOUT_DEFAULT;
	atomic_set(&tsk->dupl_rcvcnt, 0);

	/* Start out with safe limits until we receive an advertised window */
	tsk->snd_win = tsk_adv_blocks(RCVBUF_MIN);
	tsk->rcv_win = tsk->snd_win;

	if (sock->state == SS_READY) {
		tsk_set_unreturnable(tsk, true);
		if (sock->type == SOCK_DGRAM)
			tsk_set_unreliable(tsk, true);
	}
	return 0;
}