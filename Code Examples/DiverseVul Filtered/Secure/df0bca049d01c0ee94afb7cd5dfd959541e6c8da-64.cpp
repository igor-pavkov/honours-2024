static void sock_def_readable(struct sock *sk, int len)
{
	read_lock(&sk->sk_callback_lock);
	if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
		wake_up_interruptible_sync(sk->sk_sleep);
	sk_wake_async(sk, SOCK_WAKE_WAITD, POLL_IN);
	read_unlock(&sk->sk_callback_lock);
}