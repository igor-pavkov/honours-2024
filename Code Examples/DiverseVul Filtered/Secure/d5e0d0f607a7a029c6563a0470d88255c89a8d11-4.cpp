static int vsock_stream_connect(struct socket *sock, struct sockaddr *addr,
				int addr_len, int flags)
{
	int err;
	struct sock *sk;
	struct vsock_sock *vsk;
	struct sockaddr_vm *remote_addr;
	long timeout;
	DEFINE_WAIT(wait);

	err = 0;
	sk = sock->sk;
	vsk = vsock_sk(sk);

	lock_sock(sk);

	/* XXX AF_UNSPEC should make us disconnect like AF_INET. */
	switch (sock->state) {
	case SS_CONNECTED:
		err = -EISCONN;
		goto out;
	case SS_DISCONNECTING:
		err = -EINVAL;
		goto out;
	case SS_CONNECTING:
		/* This continues on so we can move sock into the SS_CONNECTED
		 * state once the connection has completed (at which point err
		 * will be set to zero also).  Otherwise, we will either wait
		 * for the connection or return -EALREADY should this be a
		 * non-blocking call.
		 */
		err = -EALREADY;
		break;
	default:
		if ((sk->sk_state == SS_LISTEN) ||
		    vsock_addr_cast(addr, addr_len, &remote_addr) != 0) {
			err = -EINVAL;
			goto out;
		}

		/* The hypervisor and well-known contexts do not have socket
		 * endpoints.
		 */
		if (!transport->stream_allow(remote_addr->svm_cid,
					     remote_addr->svm_port)) {
			err = -ENETUNREACH;
			goto out;
		}

		/* Set the remote address that we are connecting to. */
		memcpy(&vsk->remote_addr, remote_addr,
		       sizeof(vsk->remote_addr));

		/* Autobind this socket to the local address if necessary. */
		if (!vsock_addr_bound(&vsk->local_addr)) {
			struct sockaddr_vm local_addr;

			vsock_addr_init(&local_addr, VMADDR_CID_ANY,
					VMADDR_PORT_ANY);
			err = __vsock_bind(sk, &local_addr);
			if (err != 0)
				goto out;

		}

		sk->sk_state = SS_CONNECTING;

		err = transport->connect(vsk);
		if (err < 0)
			goto out;

		/* Mark sock as connecting and set the error code to in
		 * progress in case this is a non-blocking connect.
		 */
		sock->state = SS_CONNECTING;
		err = -EINPROGRESS;
	}

	/* The receive path will handle all communication until we are able to
	 * enter the connected state.  Here we wait for the connection to be
	 * completed or a notification of an error.
	 */
	timeout = vsk->connect_timeout;
	prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);

	while (sk->sk_state != SS_CONNECTED && sk->sk_err == 0) {
		if (flags & O_NONBLOCK) {
			/* If we're not going to block, we schedule a timeout
			 * function to generate a timeout on the connection
			 * attempt, in case the peer doesn't respond in a
			 * timely manner. We hold on to the socket until the
			 * timeout fires.
			 */
			sock_hold(sk);
			INIT_DELAYED_WORK(&vsk->dwork,
					  vsock_connect_timeout);
			schedule_delayed_work(&vsk->dwork, timeout);

			/* Skip ahead to preserve error code set above. */
			goto out_wait;
		}

		release_sock(sk);
		timeout = schedule_timeout(timeout);
		lock_sock(sk);

		if (signal_pending(current)) {
			err = sock_intr_errno(timeout);
			goto out_wait_error;
		} else if (timeout == 0) {
			err = -ETIMEDOUT;
			goto out_wait_error;
		}

		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
	}

	if (sk->sk_err) {
		err = -sk->sk_err;
		goto out_wait_error;
	} else
		err = 0;

out_wait:
	finish_wait(sk_sleep(sk), &wait);
out:
	release_sock(sk);
	return err;

out_wait_error:
	sk->sk_state = SS_UNCONNECTED;
	sock->state = SS_UNCONNECTED;
	goto out_wait;
}