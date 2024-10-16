static int llc_ui_wait_for_conn(struct sock *sk, long timeout)
{
	DEFINE_WAIT(wait);

	while (1) {
		prepare_to_wait(sk->sk_sleep, &wait, TASK_INTERRUPTIBLE);
		if (sk_wait_event(sk, &timeout, sk->sk_state != TCP_SYN_SENT))
			break;
		if (signal_pending(current) || !timeout)
			break;
	}
	finish_wait(sk->sk_sleep, &wait);
	return timeout;
}