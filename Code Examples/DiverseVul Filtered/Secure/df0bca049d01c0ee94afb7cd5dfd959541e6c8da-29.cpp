static void sock_def_write_space(struct sock *sk)
{
	read_lock(&sk->sk_callback_lock);

	/* Do not wake up a writer until he can make "significant"
	 * progress.  --DaveM
	 */
	if ((atomic_read(&sk->sk_wmem_alloc) << 1) <= sk->sk_sndbuf) {
		if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
			wake_up_interruptible_sync(sk->sk_sleep);

		/* Should agree with poll, otherwise some programs break */
		if (sock_writeable(sk))
			sk_wake_async(sk, SOCK_WAKE_SPACE, POLL_OUT);
	}

	read_unlock(&sk->sk_callback_lock);
}