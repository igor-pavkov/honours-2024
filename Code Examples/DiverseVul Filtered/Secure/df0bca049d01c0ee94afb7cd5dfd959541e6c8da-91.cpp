static void sock_def_error_report(struct sock *sk)
{
	read_lock(&sk->sk_callback_lock);
	if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
		wake_up_interruptible(sk->sk_sleep);
	sk_wake_async(sk, SOCK_WAKE_IO, POLL_ERR);
	read_unlock(&sk->sk_callback_lock);
}