int nr_rx_frame(struct sk_buff *skb, struct net_device *dev)
{
	struct sock *sk;
	struct sock *make;
	struct nr_sock *nr_make;
	ax25_address *src, *dest, *user;
	unsigned short circuit_index, circuit_id;
	unsigned short peer_circuit_index, peer_circuit_id;
	unsigned short frametype, flags, window, timeout;
	int ret;

	skb->sk = NULL;		/* Initially we don't know who it's for */

	/*
	 *	skb->data points to the netrom frame start
	 */

	src  = (ax25_address *)(skb->data + 0);
	dest = (ax25_address *)(skb->data + 7);

	circuit_index      = skb->data[15];
	circuit_id         = skb->data[16];
	peer_circuit_index = skb->data[17];
	peer_circuit_id    = skb->data[18];
	frametype          = skb->data[19] & 0x0F;
	flags              = skb->data[19] & 0xF0;

	/*
	 * Check for an incoming IP over NET/ROM frame.
	 */
	if (frametype == NR_PROTOEXT &&
	    circuit_index == NR_PROTO_IP && circuit_id == NR_PROTO_IP) {
		skb_pull(skb, NR_NETWORK_LEN + NR_TRANSPORT_LEN);
		skb_reset_transport_header(skb);

		return nr_rx_ip(skb, dev);
	}

	/*
	 * Find an existing socket connection, based on circuit ID, if it's
	 * a Connect Request base it on their circuit ID.
	 *
	 * Circuit ID 0/0 is not valid but it could still be a "reset" for a
	 * circuit that no longer exists at the other end ...
	 */

	sk = NULL;

	if (circuit_index == 0 && circuit_id == 0) {
		if (frametype == NR_CONNACK && flags == NR_CHOKE_FLAG)
			sk = nr_find_peer(peer_circuit_index, peer_circuit_id, src);
	} else {
		if (frametype == NR_CONNREQ)
			sk = nr_find_peer(circuit_index, circuit_id, src);
		else
			sk = nr_find_socket(circuit_index, circuit_id);
	}

	if (sk != NULL) {
		skb_reset_transport_header(skb);

		if (frametype == NR_CONNACK && skb->len == 22)
			nr_sk(sk)->bpqext = 1;
		else
			nr_sk(sk)->bpqext = 0;

		ret = nr_process_rx_frame(sk, skb);
		bh_unlock_sock(sk);
		return ret;
	}

	/*
	 * Now it should be a CONNREQ.
	 */
	if (frametype != NR_CONNREQ) {
		/*
		 * Here it would be nice to be able to send a reset but
		 * NET/ROM doesn't have one.  We've tried to extend the protocol
		 * by sending NR_CONNACK | NR_CHOKE_FLAGS replies but that
		 * apparently kills BPQ boxes... :-(
		 * So now we try to follow the established behaviour of
		 * G8PZT's Xrouter which is sending packets with command type 7
		 * as an extension of the protocol.
		 */
		if (sysctl_netrom_reset_circuit &&
		    (frametype != NR_RESET || flags != 0))
			nr_transmit_reset(skb, 1);

		return 0;
	}

	sk = nr_find_listener(dest);

	user = (ax25_address *)(skb->data + 21);

	if (sk == NULL || sk_acceptq_is_full(sk) ||
	    (make = nr_make_new(sk)) == NULL) {
		nr_transmit_refusal(skb, 0);
		if (sk)
			bh_unlock_sock(sk);
		return 0;
	}

	window = skb->data[20];

	skb->sk             = make;
	make->sk_state	    = TCP_ESTABLISHED;

	/* Fill in his circuit details */
	nr_make = nr_sk(make);
	nr_make->source_addr = *dest;
	nr_make->dest_addr   = *src;
	nr_make->user_addr   = *user;

	nr_make->your_index  = circuit_index;
	nr_make->your_id     = circuit_id;

	bh_unlock_sock(sk);
	circuit = nr_find_next_circuit();
	bh_lock_sock(sk);

	nr_make->my_index    = circuit / 256;
	nr_make->my_id       = circuit % 256;

	circuit++;

	/* Window negotiation */
	if (window < nr_make->window)
		nr_make->window = window;

	/* L4 timeout negotiation */
	if (skb->len == 37) {
		timeout = skb->data[36] * 256 + skb->data[35];
		if (timeout * HZ < nr_make->t1)
			nr_make->t1 = timeout * HZ;
		nr_make->bpqext = 1;
	} else {
		nr_make->bpqext = 0;
	}

	nr_write_internal(make, NR_CONNACK);

	nr_make->condition = 0x00;
	nr_make->vs        = 0;
	nr_make->va        = 0;
	nr_make->vr        = 0;
	nr_make->vl        = 0;
	nr_make->state     = NR_STATE_3;
	sk_acceptq_added(sk);
	skb_queue_head(&sk->sk_receive_queue, skb);

	if (!sock_flag(sk, SOCK_DEAD))
		sk->sk_data_ready(sk, skb->len);

	bh_unlock_sock(sk);

	nr_insert_socket(make);

	nr_start_heartbeat(make);
	nr_start_idletimer(make);

	return 1;
}