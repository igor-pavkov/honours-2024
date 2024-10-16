static void sock_def_wakeup(struct sock *sk)
{
	read_lock(&sk->sk_callback_lock);
	if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
		wake_up_interruptible_all(sk->sk_sleep);
	read_unlock(&sk->sk_callback_lock);
}