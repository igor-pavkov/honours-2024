static int nr_connect(struct socket *sock, struct sockaddr *uaddr,
	int addr_len, int flags)
{
	struct sock *sk = sock->sk;
	struct nr_sock *nr = nr_sk(sk);
	struct sockaddr_ax25 *addr = (struct sockaddr_ax25 *)uaddr;
	ax25_address *source = NULL;
	ax25_uid_assoc *user;
	struct net_device *dev;
	int err = 0;

	lock_sock(sk);
	if (sk->sk_state == TCP_ESTABLISHED && sock->state == SS_CONNECTING) {
		sock->state = SS_CONNECTED;
		goto out_release;	/* Connect completed during a ERESTARTSYS event */
	}

	if (sk->sk_state == TCP_CLOSE && sock->state == SS_CONNECTING) {
		sock->state = SS_UNCONNECTED;
		err = -ECONNREFUSED;
		goto out_release;
	}

	if (sk->sk_state == TCP_ESTABLISHED) {
		err = -EISCONN;	/* No reconnect on a seqpacket socket */
		goto out_release;
	}

	sk->sk_state   = TCP_CLOSE;
	sock->state = SS_UNCONNECTED;

	if (addr_len != sizeof(struct sockaddr_ax25) && addr_len != sizeof(struct full_sockaddr_ax25)) {
		err = -EINVAL;
		goto out_release;
	}
	if (addr->sax25_family != AF_NETROM) {
		err = -EINVAL;
		goto out_release;
	}
	if (sock_flag(sk, SOCK_ZAPPED)) {	/* Must bind first - autobinding in this may or may not work */
		sock_reset_flag(sk, SOCK_ZAPPED);

		if ((dev = nr_dev_first()) == NULL) {
			err = -ENETUNREACH;
			goto out_release;
		}
		source = (ax25_address *)dev->dev_addr;

		user = ax25_findbyuid(current_euid());
		if (user) {
			nr->user_addr   = user->call;
			ax25_uid_put(user);
		} else {
			if (ax25_uid_policy && !capable(CAP_NET_ADMIN)) {
				dev_put(dev);
				err = -EPERM;
				goto out_release;
			}
			nr->user_addr   = *source;
		}

		nr->source_addr = *source;
		nr->device      = dev;

		dev_put(dev);
		nr_insert_socket(sk);		/* Finish the bind */
	}

	nr->dest_addr = addr->sax25_call;

	release_sock(sk);
	circuit = nr_find_next_circuit();
	lock_sock(sk);

	nr->my_index = circuit / 256;
	nr->my_id    = circuit % 256;

	circuit++;

	/* Move to connecting socket, start sending Connect Requests */
	sock->state  = SS_CONNECTING;
	sk->sk_state = TCP_SYN_SENT;

	nr_establish_data_link(sk);

	nr->state = NR_STATE_1;

	nr_start_heartbeat(sk);

	/* Now the loop */
	if (sk->sk_state != TCP_ESTABLISHED && (flags & O_NONBLOCK)) {
		err = -EINPROGRESS;
		goto out_release;
	}

	/*
	 * A Connect Ack with Choke or timeout or failed routing will go to
	 * closed.
	 */
	if (sk->sk_state == TCP_SYN_SENT) {
		DEFINE_WAIT(wait);

		for (;;) {
			prepare_to_wait(sk->sk_sleep, &wait,
					TASK_INTERRUPTIBLE);
			if (sk->sk_state != TCP_SYN_SENT)
				break;
			if (!signal_pending(current)) {
				release_sock(sk);
				schedule();
				lock_sock(sk);
				continue;
			}
			err = -ERESTARTSYS;
			break;
		}
		finish_wait(sk->sk_sleep, &wait);
		if (err)
			goto out_release;
	}

	if (sk->sk_state != TCP_ESTABLISHED) {
		sock->state = SS_UNCONNECTED;
		err = sock_error(sk);	/* Always set at this point */
		goto out_release;
	}

	sock->state = SS_CONNECTED;

out_release:
	release_sock(sk);

	return err;
}