static int econet_release(struct socket *sock)
{
	struct sock *sk;

	mutex_lock(&econet_mutex);

	sk = sock->sk;
	if (!sk)
		goto out_unlock;

	econet_remove_socket(&econet_sklist, sk);

	/*
	 *	Now the socket is dead. No more input will appear.
	 */

	sk->sk_state_change(sk);	/* It is useless. Just for sanity. */

	sock_orphan(sk);

	/* Purge queues */

	skb_queue_purge(&sk->sk_receive_queue);

	if (sk_has_allocations(sk)) {
		sk->sk_timer.data     = (unsigned long)sk;
		sk->sk_timer.expires  = jiffies + HZ;
		sk->sk_timer.function = econet_destroy_timer;
		add_timer(&sk->sk_timer);

		goto out_unlock;
	}

	sk_free(sk);

out_unlock:
	mutex_unlock(&econet_mutex);
	return 0;
}