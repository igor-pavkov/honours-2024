static int caif_create(struct net *net, struct socket *sock, int protocol,
			int kern)
{
	struct sock *sk = NULL;
	struct caifsock *cf_sk = NULL;
	static struct proto prot = {.name = "PF_CAIF",
		.owner = THIS_MODULE,
		.obj_size = sizeof(struct caifsock),
	};

	if (!capable(CAP_SYS_ADMIN) && !capable(CAP_NET_ADMIN))
		return -EPERM;
	/*
	 * The sock->type specifies the socket type to use.
	 * The CAIF socket is a packet stream in the sense
	 * that it is packet based. CAIF trusts the reliability
	 * of the link, no resending is implemented.
	 */
	if (sock->type == SOCK_SEQPACKET)
		sock->ops = &caif_seqpacket_ops;
	else if (sock->type == SOCK_STREAM)
		sock->ops = &caif_stream_ops;
	else
		return -ESOCKTNOSUPPORT;

	if (protocol < 0 || protocol >= CAIFPROTO_MAX)
		return -EPROTONOSUPPORT;
	/*
	 * Set the socket state to unconnected.	 The socket state
	 * is really not used at all in the net/core or socket.c but the
	 * initialization makes sure that sock->state is not uninitialized.
	 */
	sk = sk_alloc(net, PF_CAIF, GFP_KERNEL, &prot);
	if (!sk)
		return -ENOMEM;

	cf_sk = container_of(sk, struct caifsock, sk);

	/* Store the protocol */
	sk->sk_protocol = (unsigned char) protocol;

	/* Initialize default priority for well-known cases */
	switch (protocol) {
	case CAIFPROTO_AT:
		sk->sk_priority = TC_PRIO_CONTROL;
		break;
	case CAIFPROTO_RFM:
		sk->sk_priority = TC_PRIO_INTERACTIVE_BULK;
		break;
	default:
		sk->sk_priority = TC_PRIO_BESTEFFORT;
	}

	/*
	 * Lock in order to try to stop someone from opening the socket
	 * too early.
	 */
	lock_sock(&(cf_sk->sk));

	/* Initialize the nozero default sock structure data. */
	sock_init_data(sock, sk);
	sk->sk_destruct = caif_sock_destructor;

	mutex_init(&cf_sk->readlock); /* single task reading lock */
	cf_sk->layer.ctrlcmd = caif_ctrl_cb;
	cf_sk->sk.sk_socket->state = SS_UNCONNECTED;
	cf_sk->sk.sk_state = CAIF_DISCONNECTED;

	set_tx_flow_off(cf_sk);
	set_rx_flow_on(cf_sk);

	/* Set default options on configuration */
	cf_sk->conn_req.link_selector = CAIF_LINK_LOW_LATENCY;
	cf_sk->conn_req.protocol = protocol;
	release_sock(&cf_sk->sk);
	return 0;
}