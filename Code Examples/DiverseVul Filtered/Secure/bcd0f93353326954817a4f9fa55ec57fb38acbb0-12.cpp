static int pep_sendmsg(struct sock *sk, struct msghdr *msg, size_t len)
{
	struct pep_sock *pn = pep_sk(sk);
	struct sk_buff *skb;
	long timeo;
	int flags = msg->msg_flags;
	int err, done;

	if (len > USHRT_MAX)
		return -EMSGSIZE;

	if ((msg->msg_flags & ~(MSG_DONTWAIT|MSG_EOR|MSG_NOSIGNAL|
				MSG_CMSG_COMPAT)) ||
			!(msg->msg_flags & MSG_EOR))
		return -EOPNOTSUPP;

	skb = sock_alloc_send_skb(sk, MAX_PNPIPE_HEADER + len,
					flags & MSG_DONTWAIT, &err);
	if (!skb)
		return err;

	skb_reserve(skb, MAX_PHONET_HEADER + 3 + pn->aligned);
	err = memcpy_from_msg(skb_put(skb, len), msg, len);
	if (err < 0)
		goto outfree;

	lock_sock(sk);
	timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);
	if ((1 << sk->sk_state) & (TCPF_LISTEN|TCPF_CLOSE)) {
		err = -ENOTCONN;
		goto out;
	}
	if (sk->sk_state != TCP_ESTABLISHED) {
		/* Wait until the pipe gets to enabled state */
disabled:
		err = sk_stream_wait_connect(sk, &timeo);
		if (err)
			goto out;

		if (sk->sk_state == TCP_CLOSE_WAIT) {
			err = -ECONNRESET;
			goto out;
		}
	}
	BUG_ON(sk->sk_state != TCP_ESTABLISHED);

	/* Wait until flow control allows TX */
	done = atomic_read(&pn->tx_credits);
	while (!done) {
		DEFINE_WAIT_FUNC(wait, woken_wake_function);

		if (!timeo) {
			err = -EAGAIN;
			goto out;
		}
		if (signal_pending(current)) {
			err = sock_intr_errno(timeo);
			goto out;
		}

		add_wait_queue(sk_sleep(sk), &wait);
		done = sk_wait_event(sk, &timeo, atomic_read(&pn->tx_credits), &wait);
		remove_wait_queue(sk_sleep(sk), &wait);

		if (sk->sk_state != TCP_ESTABLISHED)
			goto disabled;
	}

	err = pipe_skb_send(sk, skb);
	if (err >= 0)
		err = len; /* success! */
	skb = NULL;
out:
	release_sock(sk);
outfree:
	kfree_skb(skb);
	return err;
}