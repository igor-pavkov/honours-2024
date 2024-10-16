static void __lock_sock(struct sock *sk)
{
	DEFINE_WAIT(wait);

	for (;;) {
		prepare_to_wait_exclusive(&sk->sk_lock.wq, &wait,
					TASK_UNINTERRUPTIBLE);
		spin_unlock_bh(&sk->sk_lock.slock);
		schedule();
		spin_lock_bh(&sk->sk_lock.slock);
		if (!sock_owned_by_user(sk))
			break;
	}
	finish_wait(&sk->sk_lock.wq, &wait);
}